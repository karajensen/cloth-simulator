
#include "collision.h"
#include "shader.h"
#include "diagnostic.h"

std::shared_ptr<Shader> Collision::sm_shader = nullptr;

Collision::Collision(const Transform& parent) :
    m_draw(false),
    m_colour(0.0f, 0.0f, 1.0f),
    m_parent(parent),
    m_geometry(nullptr)
{
}

Collision::Geometry::Geometry() :
    shape(NONE),
    mesh(nullptr)
{
}

Collision::Data::Data() :
    localMinBounds(0.0f, 0.0f, 0.0f),
    localMaxBounds(0.0f, 0.0f, 0.0f),
    minBounds(0.0f, 0.0f, 0.0f),
    maxBounds(0.0f, 0.0f, 0.0f)
{
}

Collision::Geometry::~Geometry()
{ 
    if(mesh != nullptr)
    { 
        mesh->Release(); 
    } 
}

void Collision::LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth)
{
    if(d3ddev)
    {
        m_geometry.reset(new Geometry());
        m_geometry->shape = BOX;
        D3DXCreateBox(d3ddev, 1.0f, 1.0f, 1.0f, &m_geometry->mesh, NULL);
    }
    m_data.localWorld.SetScale(width, height, depth);
    m_data.localMinBounds = -m_data.localWorld.GetScaleFactor() * 0.5f;
    m_data.localMaxBounds = m_data.localWorld.GetScaleFactor() * 0.5f;
    FullUpdate();
}

void Collision::LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions)
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

void Collision::LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions)
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

void Collision::LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry)
{
    m_geometry = geometry;
    const D3DXVECTOR3 scale = data.localWorld.GetScaleFactor();
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

bool Collision::HasGeometry() const
{
    return m_geometry != nullptr;
}

void Collision::Initialise(std::shared_ptr<Shader> boundsShader)
{
    sm_shader = boundsShader;
}

LPD3DXMESH Collision::GetMesh()
{
    return m_geometry->mesh;
}

void Collision::SetDraw(bool draw) 
{ 
    m_draw = draw;
}

float Collision::GetRadius() const
{
    return m_data.localWorld.GetScaleFactor().x;
}

D3DXVECTOR3 Collision::GetPosition() const
{
    return m_world.Position();
}

const Transform& Collision::GetTransform() const
{
    return m_world;
}

std::shared_ptr<Collision::Geometry> Collision::GetGeometry() const
{
    return m_geometry;
}

Collision::Shape Collision::GetShape() const
{ 
    return m_geometry->shape;
}

void Collision::SetColor(const D3DXVECTOR3& color)
{ 
    m_colour = color;
}

void Collision::Draw(const Transform& projection, const Transform& view)
{
    if(m_draw && m_geometry && m_geometry->mesh)
    {
        if(Diagnostic::AllowDiagnostics())
        {
            const float radius = 0.2f;
            if(m_geometry->shape == BOX)
            {
                Diagnostic::Get().UpdateSphere(StringCast(this) + "MinBounds", 
                    Diagnostic::GREEN, m_data.minBounds, radius);
                
                Diagnostic::Get().UpdateSphere(StringCast(this) + "MaxBounds", 
                    Diagnostic::GREEN, m_data.maxBounds, radius);
            }
            else if(m_geometry->shape == SPHERE)
            {
                D3DXVECTOR3 centerToRadius = GetPosition();
                centerToRadius.y += m_data.localWorld.GetScaleFactor().x;
                Diagnostic::Get().UpdateSphere(StringCast(this) + "Radius", 
                    Diagnostic::GREEN, centerToRadius, radius);
            }
            else if(m_geometry->shape == CYLINDER)
            {
                const D3DXVECTOR3& scale = m_data.localWorld.GetScaleFactor();
                float halflength = scale.z*0.5f;

                D3DXVECTOR3 end1 = GetPosition();
                end1 += m_parent.Forward()*halflength;
                end1 += m_parent.Right()*scale.y;

                D3DXVECTOR3 end2 = GetPosition();
                end2 -= m_parent.Forward()*halflength;
                end2 -= m_parent.Right()*scale.x;

                Diagnostic::Get().UpdateSphere(StringCast(this) + "End1", 
                    Diagnostic::GREEN, end1, radius);
                Diagnostic::Get().UpdateSphere(StringCast(this) + "End2", 
                    Diagnostic::GREEN, end2, radius);
            }
        }

        LPD3DXEFFECT pEffect(sm_shader->GetEffect());
        pEffect->SetTechnique(DxConstant::DefaultTechnique);

        D3DXMATRIX wvp = m_world.Matrix() * view.Matrix() * projection.Matrix();
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

const Collision::Data& Collision::GetData() const
{
    return m_data;
}

Collision::Data& Collision::GetData()
{
    return m_data;
}

void Collision::PositionalUpdate()
{
    if(m_geometry)
    {
        if(m_geometry->shape == BOX)
        {
            D3DXVECTOR3 difference(m_parent.Position()-m_world.Position());
            m_world.Translate(difference);
            m_data.minBounds += difference;
            m_data.maxBounds += difference;
        }
        else
        {
            m_world.SetPosition(m_parent.Position());
        }
    }
}

void Collision::FullUpdate()
{
    if(m_geometry)
    {
        //DirectX: World = LocalWorld * ParentWorld
        m_world.Equals(m_data.localWorld.Matrix()*m_parent.Matrix(), 
            MultiplyVector(m_data.localWorld.GetScaleFactor(), m_parent.GetScaleFactor()));

        if(m_geometry->shape == BOX)
        {
            D3DXVec3TransformCoord(&m_data.minBounds, &m_data.localMinBounds, &m_parent.Matrix());
            D3DXVec3TransformCoord(&m_data.maxBounds, &m_data.localMaxBounds, &m_parent.Matrix());
        }
    }
}

void Collision::DrawWithRadius(const Transform& projection, const Transform& view, float radius)
{
    //assumes collision is a sphere with no scaling from parent
    m_world.MatrixPtr()->_11 = radius;
    m_world.MatrixPtr()->_22 = radius;
    m_world.MatrixPtr()->_33 = radius;
    Draw(projection, view);
    m_world.MatrixPtr()->_11 = m_data.localWorld.GetScaleFactor().x;
    m_world.MatrixPtr()->_22 = m_data.localWorld.GetScaleFactor().x;
    m_world.MatrixPtr()->_33 = m_data.localWorld.GetScaleFactor().x;
}