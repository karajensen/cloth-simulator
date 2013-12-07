////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simplex.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "simplex.h"
#include <algorithm>
#include <assert.h>

namespace
{
    const int LINE_SIMPLEX = 2;         ///< Number of points in a line simplex
    const int PLANE_SIMPLEX = 3;        ///< Number of points in a tri-plane simplex
    const int TETRAHEDRON_SIMPLEX = 4;  ///< Number of points in tetrahedron simplex
}

Simplex::Simplex()
{
}

Face::Face() :
    normal(0.0f, 0.0f, 0.0f),
    distanceToOrigin(0.0f),
    index(0)
{
    indices.assign(0);
}

bool Simplex::IsLine() const
{
    return m_simplex.size() == LINE_SIMPLEX;
}

bool Simplex::IsTetrahedron() const
{
    return m_simplex.size() == TETRAHEDRON_SIMPLEX;
}

bool Simplex::IsTriPlane() const
{
    return m_simplex.size() == PLANE_SIMPLEX;
}

void Simplex::RemovePoint(const D3DXVECTOR3& point)
{
    m_simplex.erase(std::remove(m_simplex.begin(), m_simplex.end(), point), m_simplex.end()); 
}

void Simplex::AddPoint(const D3DXVECTOR3& point)
{
    m_simplex.push_back(point);
}

const D3DXVECTOR3& Simplex::GetPoint(int index) const
{
    return m_simplex[index];
}

const std::deque<D3DXVECTOR3>& Simplex::GetPoints() const
{
    return m_simplex;
}

void Simplex::GenerateFaces()
{
    assert(m_simplex.size() <= TETRAHEDRON_SIMPLEX);
    m_faces.resize(TETRAHEDRON_SIMPLEX);

    const int A = 3;
    const int B = 0;
    const int C = 1;
    const int D = 2;
    const int ACD = 0;
    const int ADB = 1;
    const int ABC = 2;
    const int BDC = 3;

    const D3DXVECTOR3& pointA = GetPoint(A);
    const D3DXVECTOR3& pointB = GetPoint(B);
    const D3DXVECTOR3& pointC = GetPoint(C);
    const D3DXVECTOR3& pointD = GetPoint(D);

    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AC = pointC - pointA; 
    const D3DXVECTOR3 AD = pointD - pointA;
    const D3DXVECTOR3 BC = pointC - pointB;
    const D3DXVECTOR3 BD = pointD - pointB;

    // Save face indices
    m_faces[ACD].index = ACD;
    m_faces[ADB].index = ADB;
    m_faces[ABC].index = ABC;
    m_faces[BDC].index = BDC;

    // All normals pointing outwards
    D3DXVec3Cross(&m_faces[ACD].normal, &AC, &AD);
    D3DXVec3Cross(&m_faces[ADB].normal, &AD, &AB);
    D3DXVec3Cross(&m_faces[ABC].normal, &AB, &AC);
    D3DXVec3Cross(&m_faces[BDC].normal, &BD, &BC);

    D3DXVec3Normalize(&m_faces[ACD].normal, &m_faces[ACD].normal);
    D3DXVec3Normalize(&m_faces[ADB].normal, &m_faces[ADB].normal);
    D3DXVec3Normalize(&m_faces[ABC].normal, &m_faces[ABC].normal);
    D3DXVec3Normalize(&m_faces[BDC].normal, &m_faces[BDC].normal);

    // All indices clockwise
    m_faces[ACD].indices[0] = A;
    m_faces[ACD].indices[1] = D;
    m_faces[ACD].indices[2] = C;

    m_faces[ADB].indices[0] = A;
    m_faces[ADB].indices[1] = D;
    m_faces[ADB].indices[2] = B;

    m_faces[ABC].indices[0] = A;
    m_faces[ABC].indices[1] = B;
    m_faces[ABC].indices[2] = C;

    m_faces[BDC].indices[0] = B;
    m_faces[BDC].indices[1] = C;
    m_faces[BDC].indices[2] = D;

    // Find distance to origin
    m_faces[ACD].distanceToOrigin = GetDistanceToOrigin(m_faces[ACD]);
    m_faces[ADB].distanceToOrigin = GetDistanceToOrigin(m_faces[ADB]);
    m_faces[ABC].distanceToOrigin = GetDistanceToOrigin(m_faces[ABC]);
    m_faces[BDC].distanceToOrigin = GetDistanceToOrigin(m_faces[BDC]);
}

void Simplex::ExtendFace(int faceindex, const D3DXVECTOR3& point)
{
    // Add new vertex to the points list
    m_simplex.push_back(point);

    // Add three new faces connected to the three vertices of the face
    const int newFaceAmount = 2;
    const Face face = m_faces[faceindex];
    m_faces.resize(m_faces.size() + newFaceAmount);

    const int A = static_cast<int>(m_simplex.size())-1;
    const int B = face.indices[0];
    const int D = face.indices[1];
    const int C = face.indices[2];

    const int ACD = static_cast<int>(m_faces.size())-1;
    const int ABC = static_cast<int>(m_faces.size())-2;
    const int ADB = faceindex; // overwrite old entry

    const D3DXVECTOR3& pointA = point;
    const D3DXVECTOR3& pointB = GetPoint(B);
    const D3DXVECTOR3& pointC = GetPoint(C);
    const D3DXVECTOR3& pointD = GetPoint(D);

    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AC = pointC - pointA;
    const D3DXVECTOR3 AD = pointD - pointA;

    // Save face indices
    m_faces[ACD].index = ACD;
    m_faces[ABC].index = ABC;
    m_faces[ADB].index = ADB;

    // All normals pointing outwards
    D3DXVec3Cross(&m_faces[ACD].normal, &AC, &AD);
    D3DXVec3Cross(&m_faces[ADB].normal, &AD, &AB);
    D3DXVec3Cross(&m_faces[ABC].normal, &AB, &AC);

    D3DXVec3Normalize(&m_faces[ACD].normal, &m_faces[ACD].normal);
    D3DXVec3Normalize(&m_faces[ADB].normal, &m_faces[ADB].normal);
    D3DXVec3Normalize(&m_faces[ABC].normal, &m_faces[ABC].normal);

    // All indices clockwise
    m_faces[ACD].indices[0] = A;
    m_faces[ACD].indices[1] = D;
    m_faces[ACD].indices[2] = C;

    m_faces[ADB].indices[0] = A;
    m_faces[ADB].indices[1] = D;
    m_faces[ADB].indices[2] = B;

    m_faces[ABC].indices[0] = A;
    m_faces[ABC].indices[1] = B;
    m_faces[ABC].indices[2] = C;

    // Find distance to origin
    m_faces[ACD].distanceToOrigin = GetDistanceToOrigin(m_faces[ACD]);
    m_faces[ADB].distanceToOrigin = GetDistanceToOrigin(m_faces[ADB]);
    m_faces[ABC].distanceToOrigin = GetDistanceToOrigin(m_faces[ABC]);
}

float Simplex::GetDistanceToOrigin(const Face& face) const
{
    const D3DXVECTOR3 normalToOrigin = -face.normal;
    const D3DXVECTOR3 faceToOrigin = -GetPoint(face.indices[0]);
    const float distance = D3DXVec3Dot(&normalToOrigin, &faceToOrigin);
    assert(distance >= 0.0f);
    return distance;
}