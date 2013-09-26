////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - diagnostic.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "diagnostic.h"
#include "shader.h"
#include "text.h"
#include <algorithm>

namespace
{
    const int TEXT_BORDERX = 60;          ///< Border between diagnostic text and edge of screen
    const int TEXT_BORDERY = 10;          ///< Border between diagnostic text and edge of screen
    const int TEXT_SIZE = 16;             ///< Text Character size
    const int TEXT_WEIGHT = 600;          ///< Boldness of text
    const int MESH_SEGMENTS = 8;          ///< Quality of the diagnostic mesh
    const int VARIADIC_BUFFER_SIZE = 32;  ///< Size of the character buffer for variadic text calls
    const float CYLINDER_SIZE = 0.05f;    ///< Radius of the cylinder
    D3DXVECTOR3 ZAXIS(0.0f, 0.0f, 1.0f);  ///< World z axis
}

Diagnostic::Diagnostic() :
    m_d3ddev(nullptr),
    m_sphere(nullptr),
    m_cylinder(nullptr),
    m_showText(false)
{
}

void Diagnostic::Initialise(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<Shader> boundsShader)
{
    m_shader = boundsShader;
    m_d3ddev = d3ddev;

    D3DXCreateSphere(d3ddev, 1.0f, MESH_SEGMENTS, MESH_SEGMENTS, &m_sphere, NULL);

    D3DXCreateCylinder(d3ddev, CYLINDER_SIZE, CYLINDER_SIZE,
        1.0f, MESH_SEGMENTS, 1, &m_cylinder, NULL);

    m_colourmap.insert(ColorMap::value_type(RED, D3DXVECTOR3(1.0f, 0.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(GREEN, D3DXVECTOR3(0.0f, 1.0f, 0.0f)));
    m_colourmap.insert(ColorMap::value_type(BLUE, D3DXVECTOR3(0.0f, 0.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(WHITE, D3DXVECTOR3(1.0f, 1.0f, 1.0f)));
    m_colourmap.insert(ColorMap::value_type(YELLOW, D3DXVECTOR3(1.0f, 1.0f, 0.0f)));

    m_groupvector.resize(MAX_GROUPS);

    const int border = 10;
    m_text.reset(new Text());
    if(!m_text->Load(d3ddev, false, TEXT_WEIGHT, TEXT_SIZE, DT_LEFT, 
        border, border, WINDOW_WIDTH-border, WINDOW_HEIGHT-border))
    {
        m_text = nullptr;
    }
}

Diagnostic::DiagGroup::DiagGroup() :
    render(false)
{
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
    m_showText = !m_showText;
}

void Diagnostic::ToggleDiagnostics(Group group)
{
    m_groupvector[group].render = !m_groupvector[group].render;
}

bool Diagnostic::AllowDiagnostics(Group group)
{
    return m_groupvector[group].render;
}

bool Diagnostic::AllowText()
{
    return m_showText;
}

void Diagnostic::DrawAllText()
{
    if(m_showText)
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

void Diagnostic::DrawAllObjects(const Matrix& projection, const Matrix& view)
{
    for(GroupVector::iterator gitr = m_groupvector.begin(); 
        gitr != m_groupvector.end(); ++gitr)
    {
        if(gitr->render)
        {
            LPD3DXEFFECT pEffect(m_shader->GetEffect());
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

            for(SphereMap::iterator sitr = gitr->spheremap.begin(); sitr != gitr->spheremap.end(); ++sitr)
            {
                if(sitr->second.draw)
                {
                    renderObject(m_sphere, sitr->second.color, sitr->second.world);
                    sitr->second.draw = false;
                }
            }

            for(LineMap::iterator litr = gitr->linemap.begin(); litr != gitr->linemap.end(); ++litr)
            {
                if(litr->second.draw)
                {
                    renderObject(m_cylinder, litr->second.color, litr->second.world);
                    litr->second.draw = false;
                }
            }
        }
    }
}

void Diagnostic::UpdateSphere(Group group, const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& position, float radius)
{
    SphereMap& spheremap = m_groupvector[group].spheremap;

    if(spheremap.find(id) == spheremap.end())
    {
        spheremap.insert(SphereMap::value_type(id,DiagSphere())); 
    }
    spheremap[id].color = m_colourmap[color];
    spheremap[id].draw = true;
    spheremap[id].world.MakeIdentity();
    spheremap[id].world.SetScale(radius);
    spheremap[id].world.SetPosition(position);
}

void Diagnostic::UpdateLine(Group group, const std::string& id, Diagnostic::Colour color, 
    const D3DXVECTOR3& start, const D3DXVECTOR3& end)
{
    LineMap& linemap = m_groupvector[group].linemap;

    if(linemap.find(id) == linemap.end())
    {
        linemap.insert(LineMap::value_type(id,DiagLine())); 
    }
    linemap[id].color = m_colourmap[color];

    D3DXVECTOR3 forward = end-start;
    D3DXVECTOR3 middle = start + (forward*0.5f);
    float size = D3DXVec3Length(&forward);
    forward /= size;

    linemap[id].world.MakeIdentity();
    if(fabs(std::acos(D3DXVec3Dot(&ZAXIS, &forward))) > 0)
    {
        D3DXVECTOR3 up;
        D3DXVec3Cross(&up, &ZAXIS, &forward);
        D3DXVec3Normalize(&up, &up);

        D3DXVECTOR3 right;
        D3DXVec3Cross(&right, &up, &forward);
        D3DXVec3Normalize(&right, &right);

        forward *= size;
        linemap[id].world.SetAxis(up, forward, right);
    }
    linemap[id].world.SetPosition(middle);
    linemap[id].draw = true;
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
