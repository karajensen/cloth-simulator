#include "diagnostic.h"
#include "shader.h"
#include "text.h"
#include <algorithm>

namespace
{
    const int TEXT_BORDERX = 60;            ///< Border between diagnostic text and edge of screen
    const int TEXT_BORDERY = 10;            ///< Border between diagnostic text and edge of screen
    const int TEXT_SIZE = 16;               ///< Text Character size
    const int TEXT_WEIGHT = 600;            ///< Boldness of text
    const int MESH_SEGMENTS = 8;            ///< Quality of the diagnostic mesh
    const int VARIADIC_BUFFER_SIZE = 32;    ///< Size of the character buffer for variadic text calls
    const float CYLINDER_SIZE = 0.05f;      ///< Radius of the cylinder
    D3DXVECTOR3 ZAXIS(0.0f, 0.0f, 1.0f);    ///< World z axis
}

bool Diagnostic::sm_showDiagnostics = false;
bool Diagnostic::sm_showText = false;
std::shared_ptr<Shader> Diagnostic::sm_shader = nullptr;
std::shared_ptr<Diagnostic> Diagnostic::sm_diag = nullptr;

Diagnostic::Diagnostic(LPDIRECT3DDEVICE9 d3ddev) :
    m_sphere(nullptr),
    m_cylinder(nullptr),
    m_d3ddev(d3ddev)
{
    D3DXCreateSphere(d3ddev, 1.0f, MESH_SEGMENTS, MESH_SEGMENTS, &m_sphere, NULL);
    D3DXCreateCylinder(d3ddev, CYLINDER_SIZE, CYLINDER_SIZE, 1.0f, MESH_SEGMENTS, 1, &m_cylinder, NULL);

    m_colourmap.insert(ColorMap::value_type(RED, D3DXVECTOR3(1.0f, 0.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(GREEN, D3DXVECTOR3(0.0f, 1.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(BLUE, D3DXVECTOR3(0.0f, 0.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(WHITE, D3DXVECTOR3(1.0f, 1.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(YELLOW, D3DXVECTOR3(1.0f, 1.0f, 0.0f)));

    int border = 10;
    m_text.reset(new Text());
    if(!m_text->Load(d3ddev, false, TEXT_WEIGHT, TEXT_SIZE, DT_LEFT, 
        border, border, WINDOW_WIDTH-border, WINDOW_HEIGHT-border))
    {
        m_text = nullptr;
    }
}

Diagnostic::~Diagnostic()
{
    if(m_sphere)
    { 
        m_sphere->Release();
    }
    if(m_cylinder)
    {
        m_cylinder->Release();
    }
}

void Diagnostic::ToggleText()
{
    sm_showText = !sm_showText;
}

void Diagnostic::ToggleDiagnostics()
{
    sm_showDiagnostics = !sm_showDiagnostics;
}

bool Diagnostic::AllowDiagnostics()
{
    return sm_showDiagnostics;
}

bool Diagnostic::AllowText()
{
    return sm_showText;
}

void Diagnostic::ShowMessage(const std::string& message)
{
    MessageBox(NULL, message.c_str(), TEXT("ERROR"), MB_OK);
}

void Diagnostic::Initialise(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader)
{
    sm_shader = boundsShader;
    sm_diag.reset(new Diagnostic(d3ddev));
}

void Diagnostic::DrawAllText()
{
    if(sm_showText)
    {
        int counter = 0;
        for(TextMap::iterator it = m_textmap.begin(); it != m_textmap.end(); ++it)
        {
            m_text->SetText(it->second.text);
            m_text->SetColour(it->second.color);
            m_text->SetPosition(TEXT_BORDERX, TEXT_BORDERY+(TEXT_SIZE*(counter++)));
            m_text->Draw();
        }
    }
}

void Diagnostic::DrawAllObjects(const Transform& projection, const Transform& view)
{
    if(sm_showDiagnostics)
    {
        LPD3DXEFFECT pEffect(sm_shader->GetEffect());
        pEffect->SetTechnique(DxConstant::DefaultTechnique);

        auto renderObject = [&](LPD3DXMESH mesh, const D3DXVECTOR3& color, const Transform& world)
        {
            D3DXMATRIX wvp = world.Matrix * view.Matrix * projection.Matrix;
            pEffect->SetMatrix(DxConstant::WordViewProjection, &wvp);
            pEffect->SetFloatArray(DxConstant::VertexColor, &color.x, 3);

            UINT nPasses = 0;
            pEffect->Begin(&nPasses, 0);
            for(UINT iPass = 0; iPass < nPasses; iPass++)
            {
                pEffect->BeginPass(iPass);
                mesh->DrawSubset(0);
                pEffect->EndPass();
            }
            pEffect->End();
        };

        std::for_each(m_spheremap.begin(), m_spheremap.end(), [&](const SphereMap::value_type& sphere)
        {
            renderObject(m_sphere, sphere.second.color, sphere.second.world);
        });

        std::for_each(m_linemap.begin(), m_linemap.end(), [&](const LineMap::value_type& line)
        {
            renderObject(m_cylinder, line.second.color, line.second.world);
        });
    }
}

void Diagnostic::UpdateSphere(const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& position, float radius)
{
    if(m_spheremap.find(id) == m_spheremap.end())
    {
        m_spheremap.insert(SphereMap::value_type(id,DiagSphere())); 
    }
    m_spheremap[id].color = m_colourmap[color];
    m_spheremap[id].world.MakeIdentity();
    m_spheremap[id].world.SetScale(radius);
    m_spheremap[id].world.SetPosition(position);
}

void Diagnostic::UpdateLine(const std::string& id, Diagnostic::Colour color, 
    const D3DXVECTOR3& start, const D3DXVECTOR3& end)
{
    if(m_linemap.find(id) == m_linemap.end())
    {
        m_linemap.insert(LineMap::value_type(id,DiagLine())); 
    }
    m_linemap[id].color = m_colourmap[color];

    D3DXVECTOR3 forward = end-start;
    D3DXVECTOR3 middle = start + (forward*0.5f);
    float size = D3DXVec3Length(&forward);
    forward /= size;

    m_linemap[id].world.MakeIdentity();
    if(fabs(std::acos(D3DXVec3Dot(&ZAXIS, &forward))) > 0)
    {
        D3DXVECTOR3 up;
        D3DXVec3Cross(&up, &ZAXIS, &forward);
        D3DXVec3Normalize(&up, &up);

        D3DXVECTOR3 right;
        D3DXVec3Cross(&right, &ZAXIS, &forward);
        D3DXVec3Normalize(&right, &right);

        forward *= size;
        m_linemap[id].world.SetAxis(up, forward, right);
    }
    m_linemap[id].world.SetPosition(middle);
}

void Diagnostic::UpdateText(const std::string& id, Diagnostic::Colour color, const std::string& text)
{
    if(m_textmap.find(id) == m_textmap.end())
    {
        m_textmap.insert(TextMap::value_type(id,DiagText())); 
    }
    m_textmap[id].color = m_colourmap[color];
    m_textmap[id].text = id + ": " + text;
}

void Diagnostic::UpdateText(const std::string& id, Diagnostic::Colour color, bool increaseCounter)
{
    if(m_textmap.find(id) == m_textmap.end())
    {
        m_textmap.insert(TextMap::value_type(id,DiagText())); 
        m_textmap[id].counter = 0;
    }
    else if(increaseCounter)
    {
        ++m_textmap[id].counter;
    }
    m_textmap[id].color = m_colourmap[color];
    m_textmap[id].text = id + ": " + StringCast(m_textmap[id].counter);
}
