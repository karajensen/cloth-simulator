////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionmesh.h"
#include "shader.h"

namespace
{
    const int MINBOUND = 0; ///< Index for the minbound entry in the AABB
    const int MAXBOUND = 6; ///< Index for the maxbound entry in the AABB
    const int CORNERS = 8; ///< Number of corners in a cube
}

CollisionMesh::CollisionMesh(const Transform& parent, EnginePtr engine) :
    m_draw(false),
    m_parent(parent),
    m_colour(0.0f, 0.0f, 1.0f),
    m_geometry(nullptr),
    m_shader(engine->getShader(ShaderManager::BOUNDS_SHADER)),
    m_engine(engine),
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

void CollisionMesh::LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    if(d3ddev)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = BOX;
        D3DXCreateBox(d3ddev, 1.0f, 1.0f, 1.0f, &m_geometry->mesh, NULL);
    }

    CreateLocalBounds(width, height, depth);
    m_data.localWorld.SetScale(width, height, depth);
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
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, boundsRadius);
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
    const float boundsRadius = radius * 2.0f;
    CreateLocalBounds(boundsRadius, boundsRadius, length);
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
            D3DXVECTOR3 centerToRadius = GetPosition();
            centerToRadius.y += m_radius;
            const float drawradius = 0.2f;

            m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                StringCast(this) + "Radius", Diagnostic::RED, 
                centerToRadius, drawradius);

            auto getPointColor = [=](int index) -> Diagnostic::Colour
            {
                return index == MINBOUND || index == MAXBOUND ?
                    Diagnostic::GREEN : Diagnostic::YELLOW;
            };

            for(unsigned int i = 0; i < CORNERS/2; ++i)
            {
                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                    StringCast(this) + StringCast(i), getPointColor(i),
                    m_oabb[i], drawradius);

                m_engine->diagnostic()->UpdateSphere(Diagnostic::GENERAL,
                    StringCast(this) + StringCast(i+4), getPointColor(i+4),
                    m_oabb[i+4], drawradius);

                m_engine->diagnostic()->UpdateLine(Diagnostic::GENERAL,
                    StringCast(this) + StringCast(i) + "line1",
                    Diagnostic::YELLOW, m_oabb[i], m_oabb[i+1 >= 4 ? 0 : i+1]);
            
                m_engine->diagnostic()->UpdateLine(Diagnostic::GENERAL,
                    StringCast(this) + StringCast(i) + "line2", 
                    Diagnostic::YELLOW, m_oabb[i+4], m_oabb[i+5 >= CORNERS ? 4 : i+5]);
                
                m_engine->diagnostic()->UpdateLine(Diagnostic::GENERAL,
                    StringCast(this) + StringCast(i) + "line3",
                    Diagnostic::YELLOW, m_oabb[i], m_oabb[i+4]);
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
        D3DXVECTOR3 difference(m_parent.Position()-m_world.Position());
        for(D3DXVECTOR3& point : m_oabb)
        {
            point += difference;
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

        for(unsigned int i = 0; i < m_oabb.size(); ++i)
        {
            D3DXVec3TransformCoord(&m_oabb[i],
                &m_data.localBounds[i], &m_parent.GetMatrix());  
        }

        if(m_geometry->shape == SPHERE)
        {
            m_radius = m_parent.GetScale().x * m_data.localWorld.GetScale().x;
        }
        else
        {
            m_radius = D3DXVec3Length(&(m_oabb[MINBOUND]-m_oabb[MAXBOUND])) * 0.5f;
        }
    }
}

const std::vector<D3DXVECTOR3>& CollisionMesh::GetOABB() const
{
    return m_oabb;
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