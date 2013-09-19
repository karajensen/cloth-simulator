////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "picking.h"
#include "diagnostic.h"

Picking::Picking():
    m_rayDirection(0.0f, 0.0f, 0.0f),
    m_rayOrigin(0.0f, 0.0f, 0.0f),
    m_mesh(nullptr),
    m_distanceToMesh(0.0f),
    m_locked(false)
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

    D3DXVECTOR3 mouseRay;
    mouseRay.x =  (((2.0f*x)/WINDOW_WIDTH )-1) / projection.GetMatrix()._11;
    mouseRay.y = -(((2.0f*y)/WINDOW_HEIGHT)-1) / projection.GetMatrix()._22;
    mouseRay.z =  CAMERA_NEAR;

    //Transform the screen space pick ray into 3D world space
    m_rayDirection.x  = mouseRay.x * world.GetMatrix()._11 + 
        mouseRay.y * world.GetMatrix()._21 + mouseRay.z * world.GetMatrix()._31;

    m_rayDirection.y  = mouseRay.x * world.GetMatrix()._12 +
        mouseRay.y * world.GetMatrix()._22 + mouseRay.z * world.GetMatrix()._32;

    m_rayDirection.z  = mouseRay.x * world.GetMatrix()._13 + 
        mouseRay.y * world.GetMatrix()._23 + mouseRay.z * world.GetMatrix()._33;

    m_rayOrigin.x = world.GetMatrix()._41;
    m_rayOrigin.y = world.GetMatrix()._42;
    m_rayOrigin.z = world.GetMatrix()._43;
}

void Picking::LockMesh(bool lock)
{
    m_locked = lock;
}

void Picking::SolvePicking()
{
    if(Diagnostic::AllowText())
    {
        Diagnostic::UpdateText("DistanceToPick", Diagnostic::WHITE, 
            StringCast(m_distanceToMesh == FLT_MAX ? 0.0f : m_distanceToMesh));
    }

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