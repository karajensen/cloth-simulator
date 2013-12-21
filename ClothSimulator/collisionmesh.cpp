////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionmesh.h"
#include "partition.h"
#include "shader.h"
#include <assert.h>
#include <set>

namespace
{
    const int MINBOUND = 0; ///< Index for the minbound entry in the AABB
    const int MAXBOUND = 6; ///< Index for the maxbound entry in the AABB
    const int CORNERS = 8;  ///< Number of corners in a cube

    /**
    * Collision Type bit flags
    */
    enum
    {
        NO_COLLISION = 1,
        BOX_COLLISION = 2,
        SPHERE_COLLISION = 4,
        CYLINDER_COLLISION = 8
    };
}

CollisionMesh::CollisionMesh(const Transform& parent, EnginePtr engine) :
    m_engine(engine),
    m_parent(parent),
    m_partition(nullptr),
    m_positionDelta(0.0f, 0.0f, 0.0f),
    m_colour(1.0f, 1.0f, 1.0f),
    m_geometry(nullptr),
    m_resolveFn(nullptr),
    m_interactingGeometry(Geometry::NONE),
    m_draw(false),
    m_requiresFullUpdate(false),
    m_requiresPositionalUpdate(false),
    m_radius(0.0f),
    m_renderCollisionDiagnostics(false)
{
    m_localBounds.resize(CORNERS);
    m_oabb.resize(CORNERS);
}

void CollisionMesh::MakeDynamic(CollisionMesh::MotionFn resolveFn)
{
    m_resolveFn = resolveFn;
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

void CollisionMesh::LoadBox(bool createmesh, float width, float height, float depth)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry(m_engine->device(), 
            m_engine->getShader(ShaderManager::BOUNDS_SHADER),
            Geometry::BOX));
    }

    CreateLocalBounds(width, height, depth);
    m_localWorld.SetScale(width, height, depth);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->GetVertices().size());
    FullUpdate();
    UpdateCollision();
}


void CollisionMesh::LoadSphere(bool createmesh, float radius, int divisions)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry(m_engine->device(), 
            m_engine->getShader(ShaderManager::BOUNDS_SHADER),
            Geometry::SPHERE, divisions));
    }
    
    //radius of sphere is uniform across x/y/z axis
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, boundsRadius);
    m_localWorld.SetScale(radius);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->GetVertices().size());
    FullUpdate();
    UpdateCollision();
}

void CollisionMesh::LoadCylinder(bool createmesh, float radius, float length, int divisions)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry(m_engine->device(), 
            m_engine->getShader(ShaderManager::BOUNDS_SHADER),
            Geometry::CYLINDER, divisions));
    }

    //length of cylinder is along the z axis, radius is scaled uniformly across the x/y axis
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, length);
    m_localWorld.SetScale(radius, radius, length);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->GetVertices().size());
    FullUpdate();
    UpdateCollision();
}

void CollisionMesh::LoadInstance(const D3DXVECTOR3& scale, std::shared_ptr<Geometry> geometry)
{
    m_geometry = geometry;
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->GetVertices().size());

    switch(geometry->GetShape())
    {
    case Geometry::SPHERE:
        LoadSphere(false, scale.x, 0);
        break;
    case Geometry::BOX:
        LoadBox(false, scale.x, scale.y, scale.z);
        break;
    case Geometry::CYLINDER:
        LoadCylinder(false, scale.x, scale.z, 0);
        break;
    }
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
        const std::string id = StringCast(this);

        // Render normals of diagnostic mesh
        const auto& polygons = m_geometry->GetFaces();
        const float normalsize = 0.6f;
        for(unsigned int i = 0; i < polygons.size(); ++i)
        {
            D3DXVECTOR3 center;
            D3DXVec3TransformCoord(&center,
                &polygons[i].center, &m_world.GetMatrix());
            
            D3DXVECTOR3 normal;
            D3DXVec3TransformNormal(&normal, 
                &polygons[i].normal, &m_world.GetMatrix());
            D3DXVec3Normalize(&normal, &normal);

            m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                "FaceNormal" + StringCast(i) + id, Diagnostic::CYAN, 
                center, center + (normal * normalsize));
        }

        // Render vertices of diagnostic mesh
        const float vertexRadius = 0.1f;
        const auto& vertices = GetVertices();
        for(unsigned int i = 0; i < vertices.size(); ++i)
        {
            m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH,
                StringCast(i) + id + "0", Diagnostic::RED, 
                vertices[i], vertexRadius);
        }

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

        m_interactingGeometry = NO_COLLISION;
    }
}

void CollisionMesh::DrawMesh(const Matrix& projection, const Matrix& view)
{
    D3DXVECTOR3 color = m_colour;
    if(!IsCollidingWith(Geometry::NONE))
    {
        color = m_engine->diagnostic()->GetColor(Diagnostic::BLACK);
    }
    else if(m_partition)
    {
        color = m_engine->diagnostic()->GetColor(m_partition->GetColor());
    }

    DrawMesh(projection, view, color);
}

void CollisionMesh::FullUpdate()
{
    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent.Position() - m_world.Position();
    m_world.Set(m_localWorld.GetMatrix()*m_parent.GetMatrix());
    m_position = m_world.Position();
    m_requiresFullUpdate = true;
}

void CollisionMesh::PositionalUpdate()
{
    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent.Position() - m_world.Position();
    m_world.Set(m_localWorld.GetMatrix()*m_parent.GetMatrix());
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
                D3DXVec3TransformCoord(&m_oabb[i],
                    &m_localBounds[i], &m_parent.GetMatrix());  
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
                m_radius = m_parent.GetScale().x * m_localWorld.GetScale().x;
            }
            else
            {
                m_radius = D3DXVec3Length(&(m_oabb[MINBOUND]-m_oabb[MAXBOUND])) * 0.5f;
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

    m_positionDelta.x = 0.0f;
    m_positionDelta.y = 0.0f;
    m_positionDelta.z = 0.0f;
}

const std::vector<D3DXVECTOR3>& CollisionMesh::GetOABB() const
{
    return m_oabb;
}

void CollisionMesh::DrawRepresentation(const Matrix& projection, 
                                       const Matrix& view, 
                                       float scale,
                                       const D3DXVECTOR3& color,
                                       const D3DXVECTOR3& position)
{
    // Set the matrix explicitly to stop any observer updates
    const float savedscale = m_localWorld.GetScale().x;
    m_world.MatrixPtr()->_11 = scale;
    m_world.MatrixPtr()->_22 = scale;
    m_world.MatrixPtr()->_33 = scale;
    m_world.MatrixPtr()->_41 = position.x;
    m_world.MatrixPtr()->_42 = position.y;
    m_world.MatrixPtr()->_43 = position.z;
    DrawMesh(projection, view, color);
    m_world.MatrixPtr()->_11 = savedscale;
    m_world.MatrixPtr()->_22 = savedscale;
    m_world.MatrixPtr()->_33 = savedscale;
    m_world.MatrixPtr()->_41 = m_position.x;
    m_world.MatrixPtr()->_42 = m_position.y;
    m_world.MatrixPtr()->_43 = m_position.z;
}

void CollisionMesh::SetPartition(Partition* partition)
{
    m_partition = partition;
}

Partition* CollisionMesh::GetPartition() const
{
    return m_partition;
}

unsigned int CollisionMesh::GetCollisionType(Geometry::Shape shape)
{
    switch(shape)
    {
    case Geometry::BOX:
        return BOX_COLLISION;
    case Geometry::SPHERE:
        return SPHERE_COLLISION;
    case Geometry::CYLINDER:
        return CYLINDER_COLLISION;
    case Geometry::NONE:
    default:
        return NO_COLLISION;
    }
}

void CollisionMesh::ResolveCollision(const D3DXVECTOR3& translation, Geometry::Shape shape)
{
    if(IsDynamic())
    {
        if(shape != Geometry::NONE)
        {
            m_interactingGeometry &= ~NO_COLLISION;
            m_interactingGeometry |= GetCollisionType(shape);
        }
        m_resolveFn(translation);
    }
}

bool CollisionMesh::IsDynamic() const
{
    return m_resolveFn != nullptr;
}

bool CollisionMesh::IsCollidingWith(Geometry::Shape shape)
{
    unsigned int collisionType = GetCollisionType(shape);
    return (m_interactingGeometry & collisionType) == collisionType;
}

const D3DXVECTOR3& CollisionMesh::GetVelocity() const
{
    return m_positionDelta;
}

bool CollisionMesh::RenderCollisionDiagnostics() const
{
    return m_renderCollisionDiagnostics;
}

void CollisionMesh::SetRenderCollisionDiagnostics(bool render)
{
    m_renderCollisionDiagnostics = render;
}

D3DXVECTOR3 CollisionMesh::GetLocalScale() const
{
    return m_localWorld.GetScale();
}

void CollisionMesh::SetLocalScale(float scale)
{
    m_localWorld.SetScale(scale);
}

bool CollisionMesh::HasShape() const
{
    return GetShape() != Geometry::NONE;
}