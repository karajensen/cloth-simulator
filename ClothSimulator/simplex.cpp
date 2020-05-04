////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simplex.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "simplex.h"

#include <algorithm>
#include <assert.h>

Face::Face()
    : normal(0.0f, 0.0f, 0.0f)
    , distanceToOrigin(0.0f)
    , index(0)
    , alive(true)
{
    indices.assign(0);
}

Edge::Edge()
{
    indices.assign(0);
}

Simplex::Simplex()
{
    const int maximumExpectedEdges = 20;
    m_edges.reserve(maximumExpectedEdges);
}

bool Simplex::IsLine() const
{
    return m_simplex.size() == POINTS_IN_EDGE;
}

bool Simplex::IsTetrahedron() const
{
    return m_simplex.size() == POINTS_IN_TETRAHEDRON;
}

bool Simplex::IsTriPlane() const
{
    return m_simplex.size() == POINTS_IN_FACE;
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
    auto createFace = [this](int face, int i0, int i1, int i2, 
        const D3DXVECTOR3& u, const D3DXVECTOR3& v)
    {
        m_faces[face].index = face;
        m_faces[face].indices[0] = i0;
        m_faces[face].indices[1] = i1;
        m_faces[face].indices[2] = i2;

        m_faces[face].edges[0].indices[0] = i0;
        m_faces[face].edges[0].indices[1] = i1;
        m_faces[face].edges[1].indices[0] = i0;
        m_faces[face].edges[1].indices[1] = i2;
        m_faces[face].edges[2].indices[0] = i1;
        m_faces[face].edges[2].indices[1] = i2;

        D3DXVec3Cross(&m_faces[face].normal, &u, &v);
        D3DXVec3Normalize(&m_faces[face].normal, &m_faces[face].normal);

        m_faces[face].distanceToOrigin = GetDistanceToOrigin(m_faces[face]);
        assert(m_faces[face].distanceToOrigin >= 0.0f);
    };

    assert(m_simplex.size() <= POINTS_IN_TETRAHEDRON);
    m_faces.resize(POINTS_IN_TETRAHEDRON);

    const int A = 3;
    const int B = 0;
    const int C = 1;
    const int D = 2;

    const D3DXVECTOR3& pointA = GetPoint(A);
    const D3DXVECTOR3& pointB = GetPoint(B);
    const D3DXVECTOR3& pointC = GetPoint(C);
    const D3DXVECTOR3& pointD = GetPoint(D);

    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AC = pointC - pointA; 
    const D3DXVECTOR3 AD = pointD - pointA;
    const D3DXVECTOR3 BC = pointC - pointB;
    const D3DXVECTOR3 BD = pointD - pointB;

    createFace(0, A, C, D, AC, AD);
    createFace(1, A, D, B, AD, AB);
    createFace(2, A, B, C, AB, AC);
    createFace(3, B, D, C, BD, BC);
}

void Simplex::ExtendFace(const D3DXVECTOR3& point)
{
    // Incremental Convex hull algorithm removes all faces 
    // that are facing the new point and generates new faces 
    // on the border of the highlighted ones. Reference:
    // http://www.eecs.tufts.edu/~mhorn01/comp163/algorithm.html

    const int pointIndex = static_cast<int>(m_simplex.size());
    m_simplex.push_back(point);
    m_edges.clear();

    // Determine faces that the point is in front of
    std::vector<int> visibleFaces;
    for(const Face& face : m_faces)
    {
        if(face.alive)
        {
            D3DXVECTOR3 faceToPoint = point - GetPoint(face.indices[0]);
            if(D3DXVec3Dot(&face.normal, &faceToPoint) > 0.0f)
            {
                visibleFaces.push_back(face.index);
            }
        }
    }
    assert(!visibleFaces.empty());

    // Find all border edges from the visible faces
    const int minimumFaces = 1;
    if(visibleFaces.size() == minimumFaces)
    {
        // For a single face, all edges are on the border
        for(const Edge& edge : m_faces[visibleFaces[0]].edges)
        {
            m_edges.push_back(edge);
        }
    }
    else
    {
        // For multiple faces, determine the border edges
        for(int index : visibleFaces)
        {
            FindBorderEdges(m_faces[index], visibleFaces);
        }
    }
    assert(!m_edges.empty());

    // Mark all visible faces as dead
    for(int index : visibleFaces)
    {
        m_faces[index].alive = false;
    }
    
    // Connect up new faces from the edges to the point
    unsigned int visibleIndex = 0;
    bool hasDeadFaces = true;
    
    for(const Edge& edge : m_edges)
    {
        // Determine a new face to overwrite/create
        int faceIndex = -1;
        if(visibleIndex < visibleFaces.size())
        {
            faceIndex = visibleFaces[visibleIndex];
            ++visibleIndex;
        }
        else
        {
            if(hasDeadFaces)
            {
                faceIndex = GetDeadFaceIndex();
                hasDeadFaces = faceIndex != -1;
            }
        
            if(!hasDeadFaces)
            {
                faceIndex = m_faces.size();
                m_faces.emplace_back();
            }
        }
    
        // Create the new face
        Face& face = m_faces[faceIndex];
        face.alive = true;
        face.index = faceIndex;
        face.indices[0] = edge.indices[0];
        face.indices[1] = edge.indices[1];
        face.indices[2] = pointIndex;
    
        face.edges[0].indices[0] = pointIndex;
        face.edges[0].indices[1] = edge.indices[0];
        face.edges[1].indices[0] = pointIndex;
        face.edges[1].indices[1] = edge.indices[1];
        face.edges[2].indices[0] = edge.indices[0];
        face.edges[2].indices[1] = edge.indices[1];
    
        const D3DXVECTOR3 u = GetPoint(edge.indices[0]) - point;
        const D3DXVECTOR3 v = GetPoint(edge.indices[1]) - point;
    
        D3DXVec3Cross(&face.normal, &u, &v);
        D3DXVec3Normalize(&face.normal, &face.normal);
    
        face.distanceToOrigin = GetDistanceToOrigin(face);
        if(face.distanceToOrigin < 0.0f)
        {
            face.distanceToOrigin = fabs(face.distanceToOrigin);
            face.normal = -face.normal;
        }
    }
}

int Simplex::GetDeadFaceIndex() const
{
    auto itr = std::find_if(m_faces.begin(), m_faces.end(), 
        [](const Face& face){ return !face.alive; });

    return itr == m_faces.end() ? -1 : itr->index;
}

void Simplex::FindBorderEdges(const Face& face, 
                              const std::vector<int>& faces)
{
    // Can have a maximum of 2 border edges per face
    int borderCounter = 0;
    const int maxBorders = 2;

    for(const Edge& edge : face.edges)
    {
        if(!IsSharedEdge(face.index, edge, faces))
        {
            ++borderCounter;
            m_edges.push_back(edge);
            if(borderCounter >= maxBorders)
            {
                return;
            }
        }
    }
}

bool Simplex::IsSharedEdge(int index, const Edge& edge, const std::vector<int>& faces) const
{
    for(int faceindex : faces)
    {
        const Face& face = m_faces[faceindex];
        if(face.index == index || !face.alive)
        {
            continue;
        }

        for(const Edge& connectedEdge : face.edges)
        {
            if(AreEdgesEqual(edge, connectedEdge))
            {
                return true;
            }
        }
    }
    return false;
}

bool Simplex::AreEdgesEqual(const Edge& edge1, const Edge& edge2) const
{
    return ((edge1.indices[0] == edge2.indices[0] ||
             edge1.indices[0] == edge2.indices[1]) && 
            (edge1.indices[1] == edge2.indices[0] || 
             edge1.indices[1] == edge2.indices[1]));
}

float Simplex::GetDistanceToOrigin(const Face& face) const
{
    const D3DXVECTOR3 normalToOrigin = -face.normal;
    const D3DXVECTOR3 faceToOrigin = -GetPoint(face.indices[0]);
    return D3DXVec3Dot(&normalToOrigin, &faceToOrigin);
}

D3DXVECTOR3 Simplex::GetFaceCenter(int faceindex) const
{
    const Face& face = m_faces[faceindex];
    const D3DXVECTOR3& p0 = m_simplex[face.indices[0]];
    const D3DXVECTOR3& p1 = m_simplex[face.indices[1]];
    const D3DXVECTOR3& p2 = m_simplex[face.indices[2]];

    D3DXVECTOR3 center;
    center.x = (p0.x + p1.x + p2.x) / 3.0f;
    center.y = (p0.y + p1.y + p2.y) / 3.0f;
    center.z = (p0.z + p1.z + p2.z) / 3.0f;
    return center;
}

const Face& Simplex::GetClosestFaceToOrigin()
{
    int closest = 0;
    for(unsigned int i = 0; i < m_faces.size(); ++i)
    {
        if(m_faces[i].alive && m_faces[i].distanceToOrigin
            < m_faces[closest].distanceToOrigin)
        {
            closest = i;
        }
    }
    return m_faces[closest];
}