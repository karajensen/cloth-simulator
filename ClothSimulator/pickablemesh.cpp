////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - pickablemesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "pickablemesh.h"

PickableMesh::PickableMesh() :
    m_meshPickFn(nullptr)
{
}

PickableMesh::~PickableMesh()
{
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