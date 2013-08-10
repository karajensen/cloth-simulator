////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#include "picking.h"
#include "diagnostic.h"

Picking::Picking():
    m_rayDirection(0.0f, 0.0f, 0.0f),
    m_rayOrigin(0.0f, 0.0f, 0.0f),
    m_mesh(nullptr),
    m_distanceToMesh(0.0f)    
{ 
}

PickableMesh::PickableMesh() :
    m_meshPickFn(nullptr)
{
}

PickableMesh::~PickableMesh()
{
}

void Picking::UpdatePicking(Transform& projection, Transform& view, int x, int y)
{
    m_rayOrigin = D3DXVECTOR3();
    m_rayDirection = D3DXVECTOR3();
    m_mesh = nullptr;
    m_distanceToMesh = FLT_MAX;

    D3DXVECTOR3 mouseRay;
    mouseRay.x =  (((2.0f*x)/WINDOW_WIDTH )-1) / projection.Matrix()._11;
    mouseRay.y = -(((2.0f*y)/WINDOW_HEIGHT)-1) / projection.Matrix()._22;
    mouseRay.z =  1.0f;

    D3DXMATRIX ViewInverse;
    D3DXMatrixInverse(&ViewInverse, NULL, &view.Matrix());

    //Transform the screen space pick ray into 3D world space
    m_rayDirection.x  = mouseRay.x*ViewInverse._11 + mouseRay.y*ViewInverse._21 + mouseRay.z*ViewInverse._31;
    m_rayDirection.y  = mouseRay.x*ViewInverse._12 + mouseRay.y*ViewInverse._22 + mouseRay.z*ViewInverse._32;
    m_rayDirection.z  = mouseRay.x*ViewInverse._13 + mouseRay.y*ViewInverse._23 + mouseRay.z*ViewInverse._33;
    m_rayOrigin.x = ViewInverse._41;
    m_rayOrigin.y = ViewInverse._42;
    m_rayOrigin.z = ViewInverse._43;
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