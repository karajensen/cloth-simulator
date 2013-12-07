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

    /**
    * D3DX Mesh Primitive Vertex structure
    */
    struct D3DXVertex
    {
        D3DXVECTOR3 position;   ///< Vertex position
        D3DXVECTOR3 normal;     ///< Vertex normal
    };

    /**
    * Collision Type bit flags
    */
    enum
    {
        NO_COLLISION = 0,
        BOX_COLLISION = 1,
        SPHERE_COLLISION = 2,
        CYLINDER_COLLISION = 4
    };
}

CollisionMesh::CollisionMesh(const Transform& parent, EnginePtr engine) :
    m_engine(engine),
    m_parent(parent),
    m_partition(nullptr),
    m_positionDelta(0.0f, 0.0f, 0.0f),
    m_colour(1.0f, 1.0f, 1.0f),
    m_inCollisionColor(0.0f, 0.0f, 0.0f),
    m_shader(engine->getShader(ShaderManager::BOUNDS_SHADER)),
    m_geometry(nullptr),
    m_resolveFn(nullptr),
    m_interactingGeometry(NONE),
    m_draw(false),
    m_requiresFullUpdate(false),
    m_requiresPositionalUpdate(false),
    m_radius(0.0f)
{
    m_oabb.resize(CORNERS);
}

CollisionMesh::Geometry::Geometry() :
    shape(NONE),
    mesh(nullptr)
{
}

CollisionMesh::Data::Data()
{
    localBounds.resize(CORNERS);
}

CollisionMesh::Geometry::~Geometry()
{ 
    if(mesh != nullptr)
    { 
        mesh->Release(); 
    } 
}

void CollisionMesh::MakeDynamic(CollisionMesh::MotionFn resolveFn)
{
    m_resolveFn = resolveFn;
}

void CollisionMesh::CreateLocalBounds(float width, float height, float depth)
{
    const D3DXVECTOR3 minBounds = -D3DXVECTOR3(width, height, depth) * 0.5f;

    m_data.localBounds[0] = minBounds;
    m_data.localBounds[1] = minBounds + D3DXVECTOR3(width, 0, 0);
    m_data.localBounds[2] = minBounds + D3DXVECTOR3(width, height, 0);
    m_data.localBounds[3] = minBounds + D3DXVECTOR3(0, height, 0);
    m_data.localBounds[4] = minBounds + D3DXVECTOR3(0, 0, depth);
    m_data.localBounds[5] = minBounds + D3DXVECTOR3(width, 0, depth);
    m_data.localBounds[6] = minBounds + D3DXVECTOR3(width, height, depth);
    m_data.localBounds[7] = minBounds + D3DXVECTOR3(0, height, depth);
}

void CollisionMesh::LoadBox(bool createmesh, float width, float height, float depth)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = BOX;
        D3DXCreateBox(m_engine->device(), 1.0f,
            1.0f, 1.0f, &m_geometry->mesh, nullptr);
        SaveVertices();
    }

    CreateLocalBounds(width, height, depth);
    m_data.localWorld.SetScale(width, height, depth);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->vertices.size());;
    FullUpdate();
    UpdateCollision();
}


void CollisionMesh::LoadSphere(bool createmesh, float radius, int divisions)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = SPHERE;
        D3DXCreateSphere(m_engine->device(), 1.0f, divisions, 
            divisions, &m_geometry->mesh, nullptr);
        SaveVertices();
    }
    
    //radius of sphere is uniform across x/y/z axis
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, boundsRadius);
    m_data.localWorld.SetScale(radius);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->vertices.size());
    FullUpdate();
    UpdateCollision();
}

void CollisionMesh::LoadCylinder(bool createmesh, float radius, float length, int divisions)
{
    if(createmesh)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = CYLINDER;
        D3DXCreateCylinder(m_engine->device(), 1.0f, 1.0f, 1.0f, 
            divisions, 1, &m_geometry->mesh, nullptr);
        SaveVertices();
    }

    //length of cylinder is along the z axis, radius is scaled uniformly across the x/y axis
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, length);
    m_data.localWorld.SetScale(radius, radius, length);
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->vertices.size());
    FullUpdate();
    UpdateCollision();
}

void CollisionMesh::SaveVertices()
{
    void* buffer = nullptr;
    if(FAILED(m_geometry->mesh->LockVertexBuffer(0, &buffer)))
    {
        ShowMessageBox("Vertex buffer lock failed");
    }
    D3DXVertex* vertexBuffer = static_cast<D3DXVertex*>(buffer);

    DWORD vertexNumber = m_geometry->mesh->GetNumVertices();
    for(DWORD i = 0; i < vertexNumber; ++i)
    {
        // Remove any duplicates as directx creates 3 vertices for every triangle
        if(std::find(m_geometry->vertices.begin(), m_geometry->vertices.end(), 
            vertexBuffer[i].position) == m_geometry->vertices.end())
        {
            m_geometry->vertices.push_back(vertexBuffer[i].position);
        }
    }
    m_geometry->mesh->UnlockVertexBuffer();
}

void CollisionMesh::LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry)
{
    m_geometry = geometry;
    m_worldVertices.clear();
    m_worldVertices.resize(m_geometry->vertices.size());

    const D3DXVECTOR3 scale = data.localWorld.GetScale();
    switch(geometry->shape)
    {
    case SPHERE:
        LoadSphere(false, scale.x, 0);
        break;
    case BOX:
        LoadBox(false, scale.x, scale.y, scale.z);
        break;
    case CYLINDER:
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
    return m_geometry->mesh;
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

std::shared_ptr<CollisionMesh::Geometry> CollisionMesh::GetGeometry() const
{
    return m_geometry;
}

CollisionMesh::Shape CollisionMesh::GetShape() const
{ 
    return m_geometry->shape;
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
    if(m_draw && m_geometry && m_geometry->mesh &&
        m_engine->diagnostic()->AllowDiagnostics(Diagnostic::COLLISION))
    {
        // Render vertices of diagnostic mesh
        const std::string id = StringCast(this);
        const float vertexRadius = 0.1f;
        const auto& vertices = GetVertices();
        for(unsigned int i = 0; i < vertices.size(); ++i)
        {
            m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
                id + StringCast(i), Diagnostic::RED, 
                vertices[i], vertexRadius);
        }

        // Render OABB for diagnostic mesh
        auto getPointColor = [=](int index) -> Diagnostic::Colour
        {
            return index == MINBOUND || index == MAXBOUND ?
                Diagnostic::BLUE : Diagnostic::CYAN;
        };

        const float radius = 0.2f;
        std::string corner;
        for(unsigned int i = 0; i < CORNERS/2; ++i)
        {
            corner = StringCast(i);
            
            m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
                id + "cA" + corner, getPointColor(i), m_oabb[i], radius);

            m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
                id + "cB" + corner, getPointColor(i+4), m_oabb[i+4], radius);

            m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
                id + "lA" + corner, Diagnostic::CYAN, 
                m_oabb[i], m_oabb[i+1 >= 4 ? 0 : i+1]);
            
            m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
                id + "lB" + corner, Diagnostic::CYAN, 
                m_oabb[i+4], m_oabb[i+5 >= CORNERS ? 4 : i+5]);
                
            m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
                id + "lC" + corner, Diagnostic::CYAN, 
                m_oabb[i], m_oabb[i+4]);
        }

        // Render radius of diagnostic mesh in wireframe
        m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
            id + "radius", Diagnostic::WHITE, GetPosition(), GetRadius());
    }
}

void CollisionMesh::DrawMesh(const Matrix& projection, const Matrix& view, const D3DXVECTOR3& color)
{
    if(m_draw && m_geometry && m_geometry->mesh)
    {
        D3DXMATRIX wvp = m_world.GetMatrix() * view.GetMatrix() * projection.GetMatrix();
        m_shader->SetMatrix(DxConstant::WordViewProjection, &wvp);
        m_shader->SetTechnique(DxConstant::DefaultTechnique);
        m_shader->SetFloatArray(DxConstant::VertexColor, &(color.x), 3);

        UINT nPasses = 0;
        m_shader->Begin(&nPasses, 0);
        for(UINT pass = 0; pass < nPasses; ++pass)
        {
            m_shader->BeginPass(pass);
            m_geometry->mesh->DrawSubset(0);
            m_shader->EndPass();
        }
        m_shader->End();

        m_interactingGeometry = NO_COLLISION;
    }
}

void CollisionMesh::DrawMesh(const Matrix& projection, const Matrix& view)
{
    D3DXVECTOR3 color = m_colour;
    if(!IsCollidingWith(NONE))
    {
        color = m_inCollisionColor;
    }
    else if(m_partition)
    {
        color = m_engine->diagnostic()->GetColor(m_partition->GetColor());
    }
    DrawMesh(projection, view, color);
}

const CollisionMesh::Data& CollisionMesh::GetData() const
{
    return m_data;
}

CollisionMesh::Data& CollisionMesh::GetData()
{
    return m_data;
}

void CollisionMesh::FullUpdate()
{
    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent.Position() - m_world.Position();
    m_world.Set(m_data.localWorld.GetMatrix()*m_parent.GetMatrix());
    m_position = m_world.Position();
    m_requiresFullUpdate = true;
}

void CollisionMesh::PositionalUpdate()
{
    //DirectX: World = LocalWorld * ParentWorld
    m_positionDelta += m_parent.Position() - m_world.Position();
    m_world.Set(m_data.localWorld.GetMatrix()*m_parent.GetMatrix());
    m_position = m_world.Position();
    m_requiresPositionalUpdate = true;
}

void CollisionMesh::UpdateCollision()
{
    if(m_geometry && (m_requiresPositionalUpdate || m_requiresFullUpdate))
    {
        // Update the mesh vertices
        for(unsigned int i = 0; i < m_geometry->vertices.size(); ++i)
        {
            if(m_requiresFullUpdate)
            {
                D3DXVec3TransformCoord(&m_worldVertices[i], 
                    &m_geometry->vertices[i], &m_world.GetMatrix());
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
                    &m_data.localBounds[i], &m_parent.GetMatrix());  
            }
            else
            {
                m_oabb[i] += m_positionDelta;
            }
        }

        // Update the radius
        if(m_requiresFullUpdate)
        {
            if(m_geometry->shape == SPHERE)
            {
                m_radius = m_parent.GetScale().x * m_data.localWorld.GetScale().x;
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
    const float savedscale = m_data.localWorld.GetScale().x;
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

unsigned int CollisionMesh::GetCollisionType(CollisionMesh::Shape shape)
{
    switch(shape)
    {
    case BOX:
        return BOX_COLLISION;
    case SPHERE:
        return SPHERE_COLLISION;
    case CYLINDER:
        return CYLINDER_COLLISION;
    case NONE:
    default:
        return NO_COLLISION;
    }
}

void CollisionMesh::ResolveCollision(const D3DXVECTOR3& translation, Shape shape)
{
    if(IsDynamic())
    {
        if(shape != NONE)
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

bool CollisionMesh::IsCollidingWith(CollisionMesh::Shape shape)
{
    unsigned int collisionType = GetCollisionType(shape);
    return (m_interactingGeometry & collisionType) == collisionType;
}

const D3DXVECTOR3& CollisionMesh::GetVelocity() const
{
    return m_positionDelta;
}