////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "picking.h"

Picking::Picking(EnginePtr engine):
    m_rayDirection(0.0f, 0.0f, 0.0f),
    m_rayOrigin(0.0f, 0.0f, 0.0f),
    m_mesh(nullptr),
    m_distanceToMesh(0.0f),
    m_locked(false),
    m_engine(engine)
{ 
}

PickableMesh::PickableMesh() :
    m_meshPickFn(nullptr)
{
}

PickableMesh::~PickableMesh()
{
}

void Picking::UpdatePicking(const Matrix& projection, const Matrix& world, int x, int y)
{
    m_locked = false;
    m_rayOrigin = D3DXVECTOR3();
    m_rayDirection = D3DXVECTOR3();
    m_mesh = nullptr;
    m_distanceToMesh = FLT_MAX;
    m_rayOrigin = world.Position();

    // Screen space mouse ray
    D3DXVECTOR3 mouseRay;
    mouseRay.x =  (((2.0f*x)/WINDOW_WIDTH )-1) / projection.GetMatrix()._11;
    mouseRay.y = -(((2.0f*y)/WINDOW_HEIGHT)-1) / projection.GetMatrix()._22;
    mouseRay.z =  CAMERA_NEAR;

    // View space mouse ray
    D3DXVec3TransformNormal(&m_rayDirection, &mouseRay, &world.GetMatrix());
}

void Picking::LockMesh(bool lock)
{
    m_locked = lock;
}

void Picking::SolvePicking()
{
    if(m_mesh)
    {
        m_mesh->OnPickMesh();
    }
}

bool Picking::IsLocked() const
{
    return m_locked;
}

void Picking::SetPickedMesh(PickableMesh* mesh, float distance)
{
    m_mesh = mesh;
    m_distanceToMesh = distance;
}

bool Picking::RayCastMesh(const D3DXMATRIX& worldInverse, LPD3DXMESH mesh, float& distanceToMesh)
{
    D3DXVECTOR3 origin, direction;
    D3DXVec3TransformCoord(&origin, &GetRayOrigin(), &worldInverse);
    D3DXVec3TransformNormal(&direction, &GetRayDirection(), &worldInverse);
    D3DXVec3Normalize(&direction, &direction);

    void* vBuffer = nullptr;
    mesh->LockVertexBuffer(0, &vBuffer);
    Vertex* vertexBuffer = static_cast<Vertex*>(vBuffer);
    mesh->UnlockVertexBuffer();

    void* iBuffer = nullptr;
    mesh->LockIndexBuffer(0, &iBuffer);
    DWORD* indexBuffer = static_cast<DWORD*>(iBuffer);
    mesh->UnlockIndexBuffer();

    const DWORD indices = mesh->GetNumFaces() * 3;
    for(DWORD i = 0; i < indices; i += 3)
    {
        const Vertex& v1 = vertexBuffer[indexBuffer[i]];
        const Vertex& v2 = vertexBuffer[indexBuffer[i+1]];
        const Vertex& v3 = vertexBuffer[indexBuffer[i+2]];
        
        D3DXVECTOR3 normal = (v1.normal + v2.normal + v3.normal) / 3.0f;
        



    }

    return false;
}

void PickableMesh::SetMeshPickFunction(MeshPickFn fn)
{
    m_meshPickFn = fn;
}

void PickableMesh::OnPickMesh()
{
    if(m_meshPickFn != nullptr)
    {
        m_meshPickFn();
    }
}