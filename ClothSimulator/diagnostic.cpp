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
    m_cylinder(nullptr)
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

void Diagnostic::ToggleDiagnostics(Group group)
{
    m_groupvector[group].render = !m_groupvector[group].render;
}

bool Diagnostic::AllowDiagnostics(Group group)
{
    return m_groupvector[group].render;
}

void Diagnostic::DrawAllText()
{
    int counter = 0;
    auto renderText = [&](const TextMap::value_type& text)
    {
        m_text->SetText(text.second.text);
        m_text->SetColour(text.second.color);
        m_text->SetPosition(TEXT_BORDERX, TEXT_BORDERY+(TEXT_SIZE*(counter++)));
        m_text->Draw();
    };

    for(auto& group : m_groupvector)
    {
        if(group.render)
        {
            std::for_each(group.textmap.begin(), group.textmap.end(), renderText);
        }
    }
}

void Diagnostic::RenderObject(LPD3DXEFFECT effect, LPD3DXMESH mesh, const D3DXVECTOR3& color,
    const Matrix& world, const Matrix& projection, const Matrix& view)
{
    D3DXMATRIX wvp = world.GetMatrix() * view.GetMatrix() * projection.GetMatrix();
    effect->SetMatrix(DxConstant::WordViewProjection, &wvp);
    effect->SetFloatArray(DxConstant::VertexColor, &color.x, 3);

    UINT nPasses = 0;
    effect->Begin(&nPasses, 0);
    for(UINT iPass = 0; iPass < nPasses; iPass++)
    {
        effect->BeginPass(iPass);
        mesh->DrawSubset(0);
        effect->EndPass();
    }
    effect->End();
}

void Diagnostic::DrawAllObjects(const Matrix& projection, const Matrix& view)
{
    for(auto& group : m_groupvector)
    {
        if(group.render)
        {
            LPD3DXEFFECT effect(m_shader->GetEffect());
            effect->SetTechnique(DxConstant::DefaultTechnique);

            for(auto& sphere : group.spheremap)
            {
                if(sphere.second.draw)
                {
                    RenderObject(effect, m_sphere, sphere.second.color, 
                        sphere.second.world, projection, view);
                    sphere.second.draw = false;
                }
            }

            for(auto& line : group.linemap)
            {
                if(line.second.draw)
                {
                    RenderObject(effect, m_cylinder, line.second.color, 
                        line.second.world, projection, view);
                    line.second.draw = false;
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

void Diagnostic::UpdateLine(Group group, const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& start, const D3DXVECTOR3& end)
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
    D3DXVECTOR3 up = linemap[id].world.Up();
    D3DXVECTOR3 right = linemap[id].world.Right();
    
    if(fabs(std::acos(D3DXVec3Dot(&ZAXIS, &forward))) > 0)
    {
        D3DXVec3Cross(&up, &ZAXIS, &forward);
        D3DXVec3Normalize(&up, &up);

        D3DXVec3Cross(&right, &up, &forward);
        D3DXVec3Normalize(&right, &right);
    }

    forward *= size;
    linemap[id].world.SetAxis(up, forward, right);
    linemap[id].world.SetPosition(middle);
    linemap[id].draw = true;
}

void Diagnostic::UpdateText(Group group, const std::string& id, 
    Diagnostic::Colour color, const std::string& text)
{
    TextMap& textmap = m_groupvector[group].textmap;

    if(textmap.find(id) == textmap.end())
    {
        textmap.insert(TextMap::value_type(id,DiagText())); 
    }
    textmap[id].color = m_colourmap[color];
    textmap[id].text = id + ": " + text;
}

void Diagnostic::UpdateText(Group group, const std::string& id,
    Diagnostic::Colour color, bool increaseCounter)
{
    TextMap& textmap = m_groupvector[group].textmap;

    if(textmap.find(id) == textmap.end())
    {
        textmap.insert(TextMap::value_type(id,DiagText())); 
        textmap[id].counter = 0;
    }
    else if(increaseCounter)
    {
        ++textmap[id].counter;
    }
    textmap[id].color = m_colourmap[color];
    textmap[id].text = id + ": " + StringCast(textmap[id].counter);
}
