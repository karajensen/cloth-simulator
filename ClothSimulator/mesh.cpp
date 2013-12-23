////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - mesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "mesh.h"
#include "collisionmesh.h"
#include "Shader.h"
#include "input.h"
#include "light.h"
#include "shader.h"
#include "picking.h"
#include <assert.h>

Mesh::Mesh(EnginePtr engine):
    m_engine(engine),
    m_color(1.0f, 1.0f, 1.0f),
    m_initialcolor(1.0f, 1.0f, 1.0f),
    m_selectedcolor(0.7f, 0.7f, 1.0f),
    m_index(NO_INDEX),
    m_pickable(true),
    m_selected(false),
    m_draw(true),
    m_target(1),
    m_animating(false),
    m_reversing(false),
    m_speed(10.0),
    m_collision(nullptr),
    m_geometry(nullptr)
{
}

Mesh::~Mesh()
{
}

void Mesh::LoadTexture(LPDIRECT3DDEVICE9 d3ddev, 
                       const std::string& filename, 
                       int dimensions, int miplevels)
{
    m_geometry->LoadTexture(d3ddev, filename, dimensions, miplevels);
}

void Mesh::LoadMesh(LPDIRECT3DDEVICE9 d3ddev, 
                    const std::string& filename,
                    LPD3DXEFFECT shader, int index)
{
    m_index = index;
    m_geometry.reset(new Geometry(d3ddev, filename, shader));
}

void Mesh::InitializeCollision()
{
    if(!m_collision)
    {
        m_collision.reset(new CollisionMesh(*this, m_engine));

        Transform::UpdateFn fullFn =
            std::bind(&CollisionMesh::FullUpdate, m_collision);

        Transform::UpdateFn positionalFn =
            std::bind(&CollisionMesh::PositionalUpdate, m_collision);

        SetObserver(fullFn, positionalFn);
    }
}

bool Mesh::LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, 
                          const CollisionMesh* collisionmesh,
                          std::shared_ptr<Geometry> geometry, 
                          int index)
{
    m_index = index;
    m_geometry = geometry;
    if(collisionmesh)
    {
        InitializeCollision();
        m_collision->LoadInstance(
            collisionmesh->GetLocalScale(),
            collisionmesh->GetGeometry());
    }
    return true;
}

void Mesh::UpdateCollision()
{
    if(HasCollisionMesh())
    {
        m_collision->UpdateCollision();
    }
}

void Mesh::DrawDiagnostics()
{
    if(HasCollisionMesh())
    {
        m_collision->DrawDiagnostics();
    }
}

void Mesh::DrawMesh(const D3DXVECTOR3& cameraPos,
    const Matrix& projection, const Matrix& view)
{
    if(m_geometry && m_draw)
    {
        const auto shader = m_geometry->GetShader();
        shader->SetTechnique(DxConstant::DefaultTechnique);
        shader->SetFloatArray(DxConstant::CameraPosition, &(cameraPos.x), 3);
        shader->SetFloatArray(DxConstant::VertexColor, &(m_color.x), 3);
        shader->SetTexture(DxConstant::DiffuseTexture, m_geometry->GetTexture());
        m_engine->sendLightsToShader(shader);

        D3DXMATRIX worldInvTrans;
        D3DXMATRIX worldViewProj = GetMatrix() * view.GetMatrix() * projection.GetMatrix();
        D3DXMatrixInverse(&worldInvTrans, 0, &GetMatrix());
        D3DXMatrixTranspose(&worldInvTrans, &worldInvTrans);

        shader->SetMatrix(DxConstant::WorldInverseTranspose, &worldInvTrans);
        shader->SetMatrix(DxConstant::WordViewProjection, &worldViewProj);
        shader->SetMatrix(DxConstant::World, &GetMatrix());

        UINT nPasses = 0;
        shader->Begin(&nPasses, 0);
        for(UINT iPass = 0; iPass < nPasses; ++iPass)
        {
            shader->BeginPass(iPass);
            m_geometry->GetMesh()->DrawSubset(0);
            shader->EndPass();
        }
        shader->End();
    }
}

void Mesh::DrawCollisionMesh(const Matrix& projection, const Matrix& view)
{
    if(m_collision && m_draw)
    {
        m_collision->DrawMesh(projection, view);
    }
}

CollisionMesh& Mesh::GetCollisionMesh()
{
    return *m_collision;
}

bool Mesh::MousePickingTest(Picking& input)
{
    if(m_pickable && m_geometry && m_draw && !input.IsLocked())
    {
        if(m_collision && m_collision->HasShape())
        {
            // Pre collision sphere-ray test against shape
            const float radius = m_collision->GetRadius();
            const D3DXVECTOR3& position = m_collision->GetPosition();







        }

        const Matrix& world =  m_collision ? m_collision->CollisionMatrix() : *this;
        const Geometry& mesh = m_collision ? *m_collision->GetGeometry() : *m_geometry;
        return input.RayCastMesh(this, world.GetMatrix(), mesh);
    }
    return false;
}

void Mesh::SetCollisionVisibility(bool draw)
{
    if(m_collision)
    {
        m_collision->SetDraw(draw);
    }
}

void Mesh::SetVisible(bool visible)
{
    m_draw = visible;
    m_animation.clear();
}

void Mesh::ToggleSelected()
{
    m_selected = !m_selected;
}

bool Mesh::HasCollisionMesh() const
{
    return m_collision && m_collision->HasGeometry();
}

void Mesh::CreateCollisionCylinder(float radius, float length, int quality)
{
    InitializeCollision();
    m_collision->LoadCylinder(true, radius, length, quality);
}

void Mesh::CreateCollisionBox(float width, float height, float depth)
{
    InitializeCollision();
    m_collision->LoadBox(true, width, height, depth);
}

void Mesh::CreateCollisionSphere(float radius, int quality)
{
    InitializeCollision();
    m_collision->LoadSphere(true, radius, quality);
}

bool Mesh::IsVisible() const
{
    return m_draw;
}

void Mesh::SetPickable(bool pickable)
{
    m_pickable = pickable;
}

std::shared_ptr<Geometry> Mesh::GetGeometry()
{
    return m_geometry;
}

int Mesh::GetIndex() const
{
    return m_index;
}

void Mesh::SetSelected(bool selected)
{
    m_selected = selected;
    m_color = selected ? m_selectedcolor : m_initialcolor;
    m_animating = !selected;

    if(selected && !m_animation.empty())
    {
        // snap mesh to end of animation
        SetPosition(m_animation[m_animation.size()-1]);
        m_target = m_animation.size()-1;
        m_reversing = false;
    }
}

void Mesh::SetColor(float r, float g, float b)
{
    m_initialcolor.x = r;
    m_initialcolor.y = g;
    m_initialcolor.z = b;
    m_color = m_initialcolor;
}

void Mesh::SetSelectedColor(float r, float g, float b)
{
    m_selectedcolor.x = r;
    m_selectedcolor.y = g;
    m_selectedcolor.z = b;
}

const std::vector<D3DXVECTOR3>& Mesh::GetAnimationPoints() const
{
    return m_animation;
}

void Mesh::ResetAnimation()
{
    m_animation.clear();
    m_target = 1;
}

void Mesh::SavePosition()
{
    D3DXVECTOR3 position = Position();
    if(m_animation.empty() || m_animation[m_animation.size()-1] != position)
    {
        m_animation.push_back(position);
        m_target = m_animation.size()-1;
    }
}

void Mesh::Animate(float deltatime)
{
    if(m_animating && m_animation.size() > 1)
    {
        const float threshold = 1.0f;
        const float length = D3DXVec3Length(&(m_animation[m_target]-Position()));

        if(length < threshold)
        {
            m_target += m_reversing ? -1 : 1;
            if(m_target < 0)
            {
                m_reversing = !m_reversing;
                m_target = 1;
            }
            else if(m_target >= static_cast<int>(m_animation.size()))
            {
                m_reversing = !m_reversing;
                m_target = static_cast<int>(m_animation.size()-2);
            }
        }

        D3DXVECTOR3 path = m_animation[m_target] - 
            m_animation[m_target+(m_reversing ? 1 : -1)];
        D3DXVec3Normalize(&path, &path);

        // Translate along the global axis to prevent rotation changing animation path
        TranslateGlobal(path * m_speed * deltatime);
    }
}