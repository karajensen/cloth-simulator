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
    assert(m_simplex.size() <= TETRAHEDRON_SIMPLEX);
}

const D3DXVECTOR3& Simplex::GetPoint(SimplexPoint point) const
{
    return m_simplex[point == LAST ? m_simplex.size()-1 : point];
}