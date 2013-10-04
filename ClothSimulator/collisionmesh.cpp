////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionmesh.h"
#include "shader.h"

CollisionMesh::CollisionMesh(const Transform& parent, EnginePtr engine) :
    m_draw(false),
    m_parent(parent),
    m_colour(0.0f, 0.0f, 1.0f),
    m_geometry(nullptr),
    m_shader(engine->getShader(ShaderManager::BOUNDS_SHADER)),
    m_engine(engine)
{
}

CollisionMesh::Geometry::Geometry() :
    shape(NONE),
    mesh(nullptr)
{
}

CollisionMesh::Data::Data() :
    localMinBounds(0.0f, 0.0f, 0.0f),
    localMaxBounds(0.0f, 0.0f, 0.0f),
    minBounds(0.0f, 0.0f, 0.0f),
    maxBounds(0.0f, 0.0f, 0.0f),
    radius(0.0f)
{
}

CollisionMesh::Geometry::~Geometry()
{ 
    if(mesh != nullptr)
    { 
        mesh->Release(); 
    } 
}

void CollisionMesh::LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    if(d3ddev)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = BOX;
        D3DXCreateBox(d3ddev, 1.0f, 1.0f, 1.0f, &m_geometry->mesh, NULL);
    }

    m_data.localWorld.SetScale(width, height, depth);
    m_data.localMinBounds = -m_data.localWorld.GetScale() * 0.5f;
    m_data.localMaxBounds = m_data.localWorld.GetScale() * 0.5f;
    FullUpdate();
}

void CollisionMesh::LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions)
{
    if(d3ddev)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = SPHERE;
        D3DXCreateSphere(d3ddev, 1.0f, divisions, divisions, &m_geometry->mesh, NULL);
    }
    
    //radius of sphere is uniform across x/y/z axis
    m_data.localWorld.SetScale(radius);
    FullUpdate();
}

void CollisionMesh::LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions)
{
    if(d3ddev)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = CYLINDER;
        D3DXCreateCylinder(d3ddev, 1.0f, 1.0f, 1.0f, divisions, 1, &m_geometry->mesh, NULL);
    }

    //length of cylinder is along the z axis, radius is scaled uniformly across the x/y axis
    m_data.localWorld.SetScale(radius, radius, length);
    FullUpdate();
}

void CollisionMesh::LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry)
{
    m_geometry = geometry;
    const D3DXVECTOR3 scale = data.localWorld.GetScale();
    switch(geometry->shape)
    {
    case SPHERE:
        LoadSphere(nullptr, scale.x, 0);
        break;
    case BOX:
        LoadBox(nullptr, scale.x, scale.y, scale.z);
        break;
    case CYLINDER:
        LoadCylinder(nullptr, scale.x, scale.z, 0);
        break;
    }
}

void CollisionMesh::SetObserver(Transform::UpdateFn update)
{
    m_world.SetObserver(update, update);
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
    return m_data.radius;
}

const D3DXVECTOR3& CollisionMesh::GetMinBounds() const
{
    return m_data.minBounds;
}

const D3DXVECTOR3& CollisionMesh::GetMaxBounds() const
{
    return m_data.maxBounds;
}

D3DXVECTOR3 CollisionMesh::GetPosition() const
{
    return m_world.Position();
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

void CollisionMesh::Draw(const Matrix& projection, const Matrix& view, bool diagnostics)
{
    if(m_draw && m_geometry && m_geometry->mesh)
    {
        if(diagnostics && m_engine->diagnostic()->AllowDiagnostics(Diagnostic::GENERAL))
        {
            const float radius = 0.2f;
            if(m_geometry->shape == BOX)
            {
                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                    StringCast(this) + "MinBounds", Diagnostic::GREEN,
                    m_data.minBounds, radius);
                
                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                    StringCast(this) + "MaxBounds", Diagnostic::GREEN,
                    m_data.maxBounds, radius);
            }
            else if(m_geometry->shape == SPHERE)
            {
                D3DXVECTOR3 centerToRadius = GetPosition();
                centerToRadius.y += m_data.localWorld.GetScale().x;

                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                    StringCast(this) + "Radius", Diagnostic::GREEN, 
                    centerToRadius, radius);
            }
            else if(m_geometry->shape == CYLINDER)
            {
                const D3DXVECTOR3& scale = m_data.localWorld.GetScale();
                float halflength = scale.z*0.5f;

                D3DXVECTOR3 end1 = GetPosition();
                end1 += m_parent.Forward()*halflength;
                end1 += m_parent.Right()*scale.y;

                D3DXVECTOR3 end2 = GetPosition();
                end2 -= m_parent.Forward()*halflength;
                end2 -= m_parent.Right()*scale.x;

                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL, 
                    StringCast(this) + "End1", Diagnostic::GREEN, end1, radius);

                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL, 
                    StringCast(this) + "End2", Diagnostic::GREEN, end2, radius);
            }
        }

        LPD3DXEFFECT pEffect(m_shader->GetEffect());
        pEffect->SetTechnique(DxConstant::DefaultTechnique);

        D3DXMATRIX wvp = m_world.GetMatrix() * view.GetMatrix() * projection.GetMatrix();
        pEffect->SetMatrix(DxConstant::WordViewProjection, &wvp);

        pEffect->SetFloatArray(DxConstant::VertexColor, &(m_colour.x), 3);

        UINT nPasses = 0;
        pEffect->Begin(&nPasses, 0);
        for( UINT iPass = 0; iPass<nPasses; iPass++)
        {
            pEffect->BeginPass(iPass);
            m_geometry->mesh->DrawSubset(0);
            pEffect->EndPass();
        }
        pEffect->End();
    }
}

const CollisionMesh::Data& CollisionMesh::GetData() const
{
    return m_data;
}

CollisionMesh::Data& CollisionMesh::GetData()
{
    return m_data;
}

void CollisionMesh::PositionalUpdate()
{
    if(m_geometry)
    {
        if(m_geometry->shape == BOX)
        {
            D3DXVECTOR3 difference(m_parent.Position()-m_world.Position());
            m_data.minBounds += difference;
            m_data.maxBounds += difference;
        }

        //DirectX: World = LocalWorld * ParentWorld
        m_world.Set(m_data.localWorld.GetMatrix()*m_parent.GetMatrix());
    }
}

void CollisionMesh::FullUpdate()
{
    if(m_geometry)
    {
        //DirectX: World = LocalWorld * ParentWorld
        m_world.Set(m_data.localWorld.GetMatrix()*m_parent.GetMatrix());

        //z component for local world is largest for cylinder and uniform
        //with other components for box/sphere
        m_data.radius = m_parent.GetScale().z * m_data.localWorld.GetScale().z;

        if(m_geometry->shape == BOX)
        {
            D3DXVec3TransformCoord(&m_data.minBounds, &m_data.localMinBounds, &m_parent.GetMatrix());
            D3DXVec3TransformCoord(&m_data.maxBounds, &m_data.localMaxBounds, &m_parent.GetMatrix());
        }
    }
}

void CollisionMesh::DrawWithRadius(const Matrix& projection, const Matrix& view, float radius)
{
    //assumes the mesh is a sphere with no scaling from parent
    float scale = m_data.localWorld.GetScale().x;
    m_world.MatrixPtr()->_11 = radius;
    m_world.MatrixPtr()->_22 = radius;
    m_world.MatrixPtr()->_33 = radius;
    Draw(projection, view, false);
    m_world.MatrixPtr()->_11 = scale;
    m_world.MatrixPtr()->_22 = scale;
    m_world.MatrixPtr()->_33 = scale;
}