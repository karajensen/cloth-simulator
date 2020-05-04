////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - diagnostic.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "diagnostic.h"
#include "shader.h"
#include "text.h"
#include "utils.h"

#include <algorithm>
#include <assert.h>

namespace
{
    const int TEXT_BORDERX = 60;          ///< Border between diagnostic text and edge of screen
    const int TEXT_BORDERY = 10;          ///< Border between diagnostic text and edge of screen
    const int TEXT_SIZE = 16;             ///< Text Character size
    const int TEXT_WEIGHT = 600;          ///< Boldness of text
    const int MESH_SEGMENTS = 8;          ///< Quality of the diagnostic mesh
    const float CYLINDER_SIZE = 0.05f;    ///< Radius of the cylinder
}

Diagnostic::Diagnostic()
    : m_wireframe(false)
    , m_d3ddev(nullptr)
    , m_sphere(nullptr)
    , m_cylinder(nullptr)
{
}

void Diagnostic::Initialise(LPDIRECT3DDEVICE9 d3ddev, LPD3DXEFFECT boundsShader)
{
    m_shader = boundsShader;
    m_d3ddev = d3ddev;

    D3DXCreateSphere(d3ddev, 1.0f, MESH_SEGMENTS, MESH_SEGMENTS, &m_sphere, NULL);

    D3DXCreateCylinder(d3ddev, CYLINDER_SIZE, CYLINDER_SIZE,
        1.0f, MESH_SEGMENTS, 1, &m_cylinder, NULL);

    m_colours.resize(MAX_COLORS);
    m_colours[RED] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    m_colours[GREEN] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    m_colours[BLUE] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    m_colours[WHITE] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    m_colours[YELLOW] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
    m_colours[CYAN] = D3DXVECTOR3(0.0f, 1.0f, 1.0f);
    m_colours[MAGENTA] = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
    m_colours[BLACK] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_colours[PURPLE] = D3DXVECTOR3(1.0f, 0.0f, 0.5f);
    m_groupvector.resize(MAX_GROUPS);

    const int border = 10;
    m_text.reset(new Text());
    if(!m_text->Load(d3ddev, false, TEXT_WEIGHT, TEXT_SIZE, DT_LEFT, 
        border, border, WINDOW_WIDTH-border, WINDOW_HEIGHT-border))
    {
        ShowMessageBox("Text object failed creation");
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

bool Diagnostic::AllowWireframe() const
{
    return m_wireframe;
}

void Diagnostic::SetWireframe(bool wireframe)
{
    m_wireframe = wireframe;
    m_d3ddev->SetRenderState(D3DRS_FILLMODE, 
        m_wireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID); 
}

void Diagnostic::ToggleDiagnostics(Group group)
{
    m_groupvector[group].render = !m_groupvector[group].render;
}

bool Diagnostic::AllowDiagnostics(Group group) const
{
    return m_groupvector[group].render;
}

void Diagnostic::DrawAllText()
{
    int counter = 0;
    auto renderText = [this, &counter](const TextMap::value_type& text)
    {
        m_text->SetText(text.second.text);
        m_text->SetColour(m_colours[text.second.color]);
        m_text->SetPosition(TEXT_BORDERX, TEXT_BORDERY+(TEXT_SIZE*(counter++)));
        m_text->Draw();
    };

    for(auto& group : m_groupvector)
    {
        if(group.render)
        {
            for(auto& text : group.textmap)
            {
                if(text.second.draw)
                {
                    renderText(text);
                    if(text.second.cleardraw)
                    {
                        text.second.draw = false;
                    }
                }
            }
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
            m_shader->SetTechnique(DxConstant::DefaultTechnique);

            for(auto& line : group.linemap)
            {
                if(line.second.draw)
                {
                    RenderObject(m_shader, m_cylinder, m_colours[line.second.color], 
                        line.second.world, projection, view);
                    line.second.draw = false;
                }
            }

            for(auto& sphere : group.spheremap)
            {
                if(sphere.second.draw)
                {
                    RenderObject(m_shader, m_sphere, m_colours[sphere.second.color],
                        sphere.second.world, projection, view);
                    sphere.second.draw = false;
                }
            }
        }
    }
}

void Diagnostic::UpdateSphere(Group group, const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& position, float radius)
{
    assert(AllowDiagnostics(group));
    SphereMap& spheremap = m_groupvector[group].spheremap;

    if(spheremap.find(id) == spheremap.end())
    {
        spheremap.insert(SphereMap::value_type(id,DiagSphere())); 
    }
    spheremap[id].color = color;
    spheremap[id].draw = true;
    spheremap[id].world.MakeIdentity();
    spheremap[id].world.SetScale(radius);
    spheremap[id].world.SetPosition(position);
}

void Diagnostic::UpdateLine(Group group, const std::string& id, 
    Diagnostic::Colour color, const D3DXVECTOR3& start, const D3DXVECTOR3& end)
{
    assert(AllowDiagnostics(group));
    LineMap& linemap = m_groupvector[group].linemap;

    if(linemap.find(id) == linemap.end())
    {
        linemap.insert(LineMap::value_type(id, DiagLine())); 
    }
    linemap[id].color = color;

    D3DXVECTOR3 forward = end-start;
    D3DXVECTOR3 middle = start + (forward * 0.5f);
    float size = D3DXVec3Length(&forward);
    forward /= size;

    linemap[id].world.MakeIdentity();
    D3DXVECTOR3 up = linemap[id].world.Up();
    D3DXVECTOR3 right = linemap[id].world.Right();
    D3DXVECTOR3 zAxis(0.0f, 0.0f, 1.0f);
    
    const float threshold = 0.96f;
    if(fabs(D3DXVec3Dot(&zAxis, &forward)) < threshold)
    {
        D3DXVec3Cross(&up, &zAxis, &forward);
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
    Diagnostic::Colour color, const std::string& text, bool cleardraw)
{
    assert(AllowDiagnostics(group));
    TextMap& textmap = m_groupvector[group].textmap;

    if(textmap.find(id) == textmap.end())
    {
        textmap.insert(TextMap::value_type(id,DiagText())); 
    }
    textmap[id].color = color;
    textmap[id].text = id + ": " + text;
    textmap[id].draw = true;
    textmap[id].cleardraw = cleardraw;
}

void Diagnostic::UpdateText(Group group, const std::string& id,
    Diagnostic::Colour color, bool increaseCounter, bool cleardraw)
{
    assert(AllowDiagnostics(group));
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
    textmap[id].color = color;
    textmap[id].text = id + ": " + StringCast(textmap[id].counter);
    textmap[id].draw = true;
    textmap[id].cleardraw = cleardraw;
}

const D3DXVECTOR3& Diagnostic::GetColor(Colour color)
{
    return m_colours[color];
}