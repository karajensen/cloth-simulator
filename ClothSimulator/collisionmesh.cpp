////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionmesh.h"
#include "partition.h"
#include "shader.h"
#include <assert.h>

namespace
{
    const int MINBOUND = 0; ///< Index for the minbound entry in the AABB
    const int MAXBOUND = 6; ///< Index for the maxbound entry in the AABB
    const int CORNERS = 8;  ///< Number of corners in a cube
}

CollisionMesh::CollisionMesh(EnginePtr engine, const Transform* parent) :
    m_engine(engine),
    m_parent(parent),
    m_partition(nullptr),
    m_positionDelta(0.0f, 0.0f, 0.0f),
    m_velocity(0.0f, 0.0f, 0.0f),
    m_colour(1.0f, 1.0f, 1.0f),
    m_geometry(nullptr),
    m_draw(false),
    m_requiresFullUpdate(false),
    m_requiresPositionalUpdate(false),
    m_radius(0.0f),
    m_renderSolverDiagnostics(false)
{
    m_localBounds.resize(CORNERS);
    m_oabb.resize(CORNERS);
}

void CollisionMesh::CreateLocalBounds(float width, float height, float depth)
{
    const D3DXVECTOR3 minBounds = -D3DXVECTOR3(width, height, depth) * 0.5f;

    m_localBounds[0] = minBounds;
    m_localBounds[1] = minBounds + D3DXVECTOR3(width, 0, 0);
    m_localBounds[2] = minBounds + D3DXVECTOR3(width, height, 0);
    m_localBounds[3] = minBounds + D3DXVECTOR3(0, height, 0);
    m_localBounds[4] = minBounds + D3DXVECTOR3(0, 0, depth);
    m_localBounds[5] = minBounds + D3DXVECTOR3(width, 0, depth);
    m_localBounds[6] = minBounds + D3DXVECTOR3(width, height, depth);
    m_localBounds[7] = minBounds + D3DXVECTOR3(0, height, depth);
}

void CollisionMesh::LoadCollisionModel(const D3DXVECTOR3& scale)
{
    // Increase the radius to the circumference for the oabb
    D3DXVECTOR3 bounds(scale);
    switch(m_geometry->GetShape())
    {
    case Geometry::SPHERE:
        bounds *= 2.0f;
        break;
    case Geometry::CYLINDER:
        bounds.x *= 2.0f;
        bounds.y *= 2.0f;
        break;
    }
    CreateLocalBounds(bounds.x, bounds.y, bounds.z);

    m_localWorld.SetScale(scale.x, scale.y, scale.z);
    if(m_parent)
    {
        m_world.Set(m_localWorld.GetMatrix()*m_parent->GetMatrix());
    }
    else
    {
        m_world.SetScale(m_localWorld.GetScale());
    }
    m_position = m_world.Position();

    m_requiresFullUpdate = true;
    UpdateCollision();
}

D3DXVECTOR3 CollisionMesh::FindLocalScale()
{
    assert(m_parent);
    D3DXVECTOR3 scale = m_parent->GetScale();
    const D3DXVECTOR3& min = m_parent->GetMinimumScale();
    const D3DXVECTOR3& max = m_parent->GetMaximumScale();

    scale.x = ((scale.x-min.x)*((m_maxLocalScale.x-
        m_minLocalScale.x)/(max.x-min.x)))+m_minLocalScale.x;

    scale.y = ((scale.y-min.y)*((m_maxLocalScale.y-
        m_minLocalScale.y)/(max.y-min.y)))+m_minLocalScale.y;

    scale.z = ((scale.z-min.z)*((m_maxLocalScale.z-
        m_minLocalScale.z)/(max.z-min.z)))+m_minLocalScale.z;

    return scale;
}

void CollisionMesh::Initialise(bool createmesh, 
                               Geometry::Shape shape,
                               const D3DXVECTOR3& scale, 
                               int divisions)
{
    Initialise(createmesh, shape, scale, scale, divisions);
}

void CollisionMesh::Initialise(bool createmesh,
                               Geometry::Shape shape,
                               const D3DXVECTOR3& minScale, 
                               const D3DXVECTOR3& maxScale, 
                               int divisions)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry(m_engine->device(), 
            m_engine->getShader(ShaderManager::BOUNDS_SHADER),
            shape, divisions));    
    }

    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->GetVertices().size());

    D3DXVECTOR3 scale(minScale);
    if(m_parent)
    {
        // If parental, scale the mesh from min-max depending on the parent scale
        m_minLocalScale = minScale;
        m_maxLocalScale = maxScale;
        scale = FindLocalScale();
    }
    LoadCollisionModel(scale);
}

void CollisionMesh::LoadInstance(const CollisionMesh& mesh)
{
    m_geometry = mesh.GetGeometry();
    Initialise(false, m_geometry->GetShape(), 
        mesh.m_minLocalScale, mesh.m_maxLocalScale);
}

bool CollisionMesh::HasGeometry() const
{
    return m_geometry != nullptr;
}

LPD3DXMESH CollisionMesh::GetMesh()
{
    return m_geometry->GetMesh();
}

void CollisionMesh::SetDraw(bool draw) 
{ 
    m_draw = draw;
}

float CollisionMesh::GetRadius() const
{
    return m_radius;
}

const D3DXVECTOR3& CollisionMesh::GetMinBounds() const
{
    return m_oabb[MINBOUND];
}

const D3DXVECTOR3& CollisionMesh::GetMaxBounds() const
{
    return m_oabb[MAXBOUND];
}

const D3DXVECTOR3& CollisionMesh::GetPosition() const
{
    return m_position;
}

const Matrix& CollisionMesh::CollisionMatrix() const
{
    return m_world;
}

std::shared_ptr<Geometry> CollisionMesh::GetGeometry() const
{
    return m_geometry;
}

Geometry::Shape CollisionMesh::GetShape() const
{ 
    return m_geometry->GetShape();
}

void CollisionMesh::SetColor(const D3DXVECTOR3& color)
{ 
    m_colour = color;
}

const std::vector<D3DXVECTOR3>& CollisionMesh::GetVertices() const
{
    return m_worldVertices;
}

void CollisionMesh::DrawDiagnostics()
{
    if(m_draw && m_geometry &&
        m_engine->diagnostic()->AllowDiagnostics(Diagnostic::MESH))
    {
        // Render world vertices
        const std::string id = StringCast(this);
        const float vertexRadius = 0.1f;
        const auto& vertices = GetVertices();
        for(unsigned int i = 0; i < vertices.size(); ++i)
        {
            m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH,
                "0" + StringCast(i) + id, Diagnostic::RED, 
                vertices[i], vertexRadius);
        }

        // Render face normals
        m_geometry->UpdateDiagnostics(*m_engine->diagnostic(), m_world.GetMatrix());

        // Render OABB for diagnostic mesh
        auto getPointColor = [=](int index) -> Diagnostic::Colour
        {
            return index == MINBOUND || index == MAXBOUND ?
                Diagnostic::BLUE : Diagnostic::MAGENTA;
        };

        const float radius = 0.2f;
        std::string corner;
        for(unsigned int i = 0; i < CORNERS/2; ++i)
        {
            corner = StringCast(i);
            
            m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH,
                "CornerA" + corner + id, getPointColor(i), m_oabb[i], radius);

            m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH,
                "CornerB" + corner + id, getPointColor(i+4), m_oabb[i+4], radius);

            m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                "LineA" + corner + id, Diagnostic::MAGENTA, 
                m_oabb[i], m_oabb[i+1 >= 4 ? 0 : i+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                "LineB" + corner + id, Diagnostic::MAGENTA, 
                m_oabb[i+4], m_oabb[i+5 >= CORNERS ? 4 : i+5]);
                
            m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                "LineC" + corner + id, Diagnostic::MAGENTA, 
                m_oabb[i], m_oabb[i+4]);
        }

        // Render radius of diagnostic mesh in wireframe
        m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH,
            "Radius" + id, Diagnostic::WHITE, GetPosition(), GetRadius());
    }
}

void CollisionMesh::DrawMesh(const Matrix& projection, const Matrix& view, const D3DXVECTOR3& color)
{
    if(m_draw && m_geometry)
    {
        LPD3DXEFFECT shader = m_geometry->GetShader();
        D3DXMATRIX wvp = m_world.GetMatrix() * view.GetMatrix() * projection.GetMatrix();
        shader->SetMatrix(DxConstant::WordViewProjection, &wvp);
        shader->SetTechnique(DxConstant::DefaultTechnique);
        shader->SetFloatArray(DxConstant::VertexColor, &(color.x), 3);

        UINT nPasses = 0;
        shader->Begin(&nPasses, 0);
        for(UINT pass = 0; pass < nPasses; ++pass)
        {
            shader->BeginPass(pass);
            m_geometry->GetMesh()->DrawSubset(0);
            shader->EndPass();
        }
        shader->End();
    }
}

void CollisionMesh::DrawMesh(const Matrix& projection, const Matrix& view)
{
    D3DXVECTOR3 color = m_colour;
    if(m_partition)
    {
        color = m_engine->diagnostic()->GetColor(m_partition->GetColor());
    }
    DrawMesh(projection, view, color);
}

void CollisionMesh::FullUpdate()
{
    assert(m_parent);

    // Modify the scale of the collision mesh
    // depending on the parent scale
    D3DXVECTOR3 scale(FindLocalScale());
    D3DXVECTOR3 localScale(m_localWorld.GetScale());
    if(scale != localScale)
    {
        LoadCollisionModel(scale);
    }

    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent->Position() - m_world.Position();
    m_world.Set(m_localWorld.GetMatrix()*m_parent->GetMatrix());
    m_position = m_world.Position();
    m_requiresFullUpdate = true;
}

void CollisionMesh::PositionalUpdate()
{
    assert(m_parent);

    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent->Position() - m_world.Position();
    m_world.Set(m_localWorld.GetMatrix()*m_parent->GetMatrix());
    m_position = m_world.Position();
    m_requiresPositionalUpdate = true;
}

void CollisionMesh::UpdateCollision()
{
    if(m_geometry && (m_requiresPositionalUpdate || m_requiresFullUpdate))
    {
        // Update the mesh vertices
        const auto& vertices = m_geometry->GetVertices();
        for(unsigned int i = 0; i < vertices.size(); ++i)
        {
            if(m_requiresFullUpdate)
            {
                D3DXVec3TransformCoord(&m_worldVertices[i], 
                    &vertices[i], &m_world.GetMatrix());
            }
            else
            {
                m_worldVertices[i] += m_positionDelta;
            }
        }

        // Update the OABB Bounding box
        for(unsigned int i = 0; i < m_oabb.size(); ++i)
        {
            if(m_requiresFullUpdate)
            {
                if(m_parent)
                {
                    D3DXVec3TransformCoord(&m_oabb[i], 
                        &m_localBounds[i], &m_parent->GetMatrix());
                }
                else
                {
                    m_oabb[i] = m_localBounds[i] + m_position;
                }
            }
            else
            {
                m_oabb[i] += m_positionDelta;
            }
        }

        // Update the radius
        if(m_requiresFullUpdate)
        {
            if(GetShape() == Geometry::SPHERE)
            {
                m_radius = m_localWorld.GetScale().x;
                m_radius *= m_parent ? m_parent->GetScale().x : 1.0f;
            }
            else
            {
                m_radius = D3DXVec3Length(
                    &(m_oabb[MINBOUND]-m_oabb[MAXBOUND])) * 0.5f;
            }
        }

        // Update the partition
        if(m_partition)
        {
            m_engine->octree()->UpdateObject(*this);
        }

        m_requiresFullUpdate = false;
        m_requiresPositionalUpdate = false;
    }

    m_velocity = m_positionDelta;
    MakeZeroVector(m_positionDelta);
}

const std::vector<D3DXVECTOR3>& CollisionMesh::GetOABB() const
{
    return m_oabb;
}

void CollisionMesh::SetPartition(Partition* partition)
{
    m_partition = partition;
}

Partition* CollisionMesh::GetPartition() const
{
    return m_partition;
}

void CollisionMesh::ResolveCollision(const D3DXVECTOR3& translation)
{
    throw std::exception("CollisionMesh::ResolveCollision not implemented");
}

void CollisionMesh::ResolveCollision(const D3DXVECTOR3& translation, 
                                     const D3DXVECTOR3& velocity, 
                                     Geometry::Shape shape)
{
    throw std::exception("CollisionMesh::ResolveCollision not implemented");
}

bool CollisionMesh::IsDynamic() const
{
    return false;
}

const D3DXVECTOR3& CollisionMesh::GetVelocity() const
{
    return m_velocity;
}

const D3DXVECTOR3& CollisionMesh::GetInteractingVelocity() const
{
    throw std::exception("CollisionMesh::GetInteractingVelocity not implemented");
}

bool CollisionMesh::RenderSolverDiagnostics() const
{
    return m_renderSolverDiagnostics;
}

void CollisionMesh::SetRenderSolverDiagnostics(bool render)
{
    m_renderSolverDiagnostics = render;
}

void CollisionMesh::SetLocalScale(float scale)
{
    m_minLocalScale.x = scale;
    m_minLocalScale.y = scale;
    m_minLocalScale.z = scale;
    m_maxLocalScale = m_minLocalScale;
    m_localWorld.SetScale(scale);
}

D3DXVECTOR3 CollisionMesh::GetLocalScale() const
{
    return m_localWorld.GetScale();
}

bool CollisionMesh::HasShape() const
{
    return GetShape() != Geometry::NONE;
}

void CollisionMesh::SetPosition(const D3DXVECTOR3& position)
{
    m_position = position;
    m_world.SetPosition(position);
}