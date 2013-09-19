////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - diagnostic.cpp
////////////////////////////////////////////////////////////////////////////////////////

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

std::shared_ptr<Diagnostic> Diagnostic::sm_diag = nullptr;

Diagnostic::Diagnostic(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader) :
    m_d3ddev(d3ddev),
    m_sphere(nullptr),
    m_cylinder(nullptr),
    m_shader(boundsShader),
    m_showText(false),
    m_showDiagnostics(false)
{
    D3DXCreateSphere(d3ddev, 1.0f, MESH_SEGMENTS, MESH_SEGMENTS, &m_sphere, NULL);
    D3DXCreateCylinder(d3ddev, CYLINDER_SIZE, CYLINDER_SIZE, 1.0f, MESH_SEGMENTS, 1, &m_cylinder, NULL);

    m_colourmap.insert(ColorMap::value_type(RED, D3DXVECTOR3(1.0f, 0.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(GREEN, D3DXVECTOR3(0.0f, 1.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(BLUE, D3DXVECTOR3(0.0f, 0.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(WHITE, D3DXVECTOR3(1.0f, 1.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(YELLOW, D3DXVECTOR3(1.0f, 1.0f, 0.0f)));

    const int border = 10;
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
    sm_diag->m_showText = !sm_diag->m_showText;
}

void Diagnostic::ToggleDiagnostics()
{
    sm_diag->m_showDiagnostics = !sm_diag->m_showDiagnostics;
}

bool Diagnostic::AllowDiagnostics()
{
    return sm_diag->m_showDiagnostics;
}

bool Diagnostic::AllowText()
{
    return sm_diag->m_showText;
}

void Diagnostic::ShowMessage(const std::string& message)
{
    MessageBox(NULL, message.c_str(), TEXT("ERROR"), MB_OK);
}

void Diagnostic::Initialise(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader)
{
    sm_diag.reset(new Diagnostic(d3ddev, boundsShader));
}

void Diagnostic::DrawAllText()
{
    if(sm_diag->m_showText)
    {
        int counter = 0;
        for(TextMap::iterator it = sm_diag->m_textmap.begin(); it != sm_diag->m_textmap.end(); ++it)
        {
            sm_diag->m_text->SetText(it->second.text);
            sm_diag->m_text->SetColour(it->second.color);
            sm_diag->m_text->SetPosition(TEXT_BORDERX, TEXT_BORDERY+(TEXT_SIZE*(counter++)));
            sm_diag->m_text->Draw();
        }
    }
}

void Diagnostic::DrawAllObjects(const Matrix& projection, const Matrix& view)
{
    if(sm_diag->m_showDiagnostics)
    {
        LPD3DXEFFECT pEffect(sm_diag->m_shader->GetEffect());
        pEffect->SetTechnique(DxConstant::DefaultTechnique);

        auto renderObject = [&](LPD3DXMESH mesh, const D3DXVECTOR3& color, const Matrix& world)
        {
            D3DXMATRIX wvp = world.GetMatrix() * view.GetMatrix() * projection.GetMatrix();
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

        for(SphereMap::iterator it = sm_diag->m_spheremap.begin(); it != sm_diag->m_spheremap.end(); ++it)
        {
            if(it->second.draw)
            {
                renderObject(sm_diag->m_sphere, it->second.color, it->second.world);
                it->second.draw = false;
            }
        }

        for(LineMap::iterator it = sm_diag->m_linemap.begin(); it != sm_diag->m_linemap.end(); ++it)
        {
            if(it->second.draw)
            {
                renderObject(sm_diag->m_cylinder, it->second.color, it->second.world);
                it->second.draw = false;
            }
        }
    }
}

void Diagnostic::UpdateSphere(const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& position, float radius)
{
    if(sm_diag->m_spheremap.find(id) == sm_diag->m_spheremap.end())
    {
        sm_diag->m_spheremap.insert(SphereMap::value_type(id,DiagSphere())); 
    }
    sm_diag->m_spheremap[id].color = sm_diag->m_colourmap[color];
    sm_diag->m_spheremap[id].draw = true;
    sm_diag->m_spheremap[id].world.MakeIdentity();
    sm_diag->m_spheremap[id].world.SetScale(radius);
    sm_diag->m_spheremap[id].world.SetPosition(position);
}

void Diagnostic::UpdateLine(const std::string& id, Diagnostic::Colour color, 
    const D3DXVECTOR3& start, const D3DXVECTOR3& end)
{
    if(sm_diag->m_linemap.find(id) == sm_diag->m_linemap.end())
    {
        sm_diag->m_linemap.insert(LineMap::value_type(id,DiagLine())); 
    }
    sm_diag->m_linemap[id].color = sm_diag->m_colourmap[color];

    D3DXVECTOR3 forward = end-start;
    D3DXVECTOR3 middle = start + (forward*0.5f);
    float size = D3DXVec3Length(&forward);
    forward /= size;

    sm_diag->m_linemap[id].world.MakeIdentity();
    if(fabs(std::acos(D3DXVec3Dot(&ZAXIS, &forward))) > 0)
    {
        D3DXVECTOR3 up;
        D3DXVec3Cross(&up, &ZAXIS, &forward);
        D3DXVec3Normalize(&up, &up);

        D3DXVECTOR3 right;
        D3DXVec3Cross(&right, &up, &forward);
        D3DXVec3Normalize(&right, &right);

        forward *= size;
        sm_diag->m_linemap[id].world.SetAxis(up, forward, right);
    }
    sm_diag->m_linemap[id].world.SetPosition(middle);
    sm_diag->m_linemap[id].draw = true;
}

void Diagnostic::UpdateText(const std::string& id, Diagnostic::Colour color, const std::string& text)
{
    if(sm_diag->m_textmap.find(id) == sm_diag->m_textmap.end())
    {
        sm_diag->m_textmap.insert(TextMap::value_type(id,DiagText())); 
    }
    sm_diag->m_textmap[id].color = sm_diag->m_colourmap[color];
    sm_diag->m_textmap[id].text = id + ": " + text;
}

void Diagnostic::UpdateText(const std::string& id, Diagnostic::Colour color, bool increaseCounter)
{
    if(sm_diag->m_textmap.find(id) == sm_diag->m_textmap.end())
    {
        sm_diag->m_textmap.insert(TextMap::value_type(id,DiagText())); 
        sm_diag->m_textmap[id].counter = 0;
    }
    else if(increaseCounter)
    {
        ++sm_diag->m_textmap[id].counter;
    }
    sm_diag->m_textmap[id].color = sm_diag->m_colourmap[color];
    sm_diag->m_textmap[id].text = id + ": " + StringCast(sm_diag->m_textmap[id].counter);
}
