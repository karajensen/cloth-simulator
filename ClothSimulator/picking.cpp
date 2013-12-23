////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "picking.h"
#include "pickablemesh.h"
#include <assert.h>

Picking::Picking(EnginePtr engine):
    m_rayDirection(0.0f, 0.0f, 0.0f),
    m_rayOrigin(0.0f, 0.0f, 0.0f),
    m_mesh(nullptr),
    m_distanceToMesh(0.0f),
    m_locked(false),
    m_engine(engine),
    m_pickFace(nullptr)
{ 
}

Picking::BarycentricCoords::BarycentricCoords() :
    s(0.0f),
    t(0.0f)
{
}

void Picking::UpdatePicking(const Matrix& projection,
                            const Matrix& world,
                            int x, int y)
{
    m_pickCoords.s = 0.0f;
    m_pickCoords.t = 0.0f;
    m_locked = false;
    m_rayOrigin = D3DXVECTOR3();
    m_rayDirection = D3DXVECTOR3();
    m_mesh = nullptr;
    m_pickFace = nullptr;
    m_distanceToMesh = FLT_MAX;
    m_rayOrigin = world.Position();

    // Create screen space mouse ray
    D3DXVECTOR3 mouseRay;
    mouseRay.x =  (((2.0f*x)/WINDOW_WIDTH )-1) / projection.GetMatrix()._11;
    mouseRay.y = -(((2.0f*y)/WINDOW_HEIGHT)-1) / projection.GetMatrix()._22;
    mouseRay.z =  CAMERA_NEAR;

    // Convert to view space mouse ray (camera world is inverse view matrix)
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

bool Picking::RayCastMesh(PickableMesh* mesh,
                          const D3DXMATRIX& world, 
                          const Geometry& geometry)
{
    D3DXMATRIX worldInverse;
    D3DXMatrixInverse(&worldInverse, 0, &world);

    // Convert origin/direction into local mesh coordinates
    D3DXVECTOR3 origin, direction;
    D3DXVec3TransformCoord(&origin, &m_rayOrigin, &worldInverse);
    D3DXVec3TransformNormal(&direction, &m_rayDirection, &worldInverse);
    D3DXVec3Normalize(&direction, &direction);

    for(const MeshFace& face : geometry.GetFaces())
    {
        // Determine if normal is facing the pick direction
        const float faceDirection = D3DXVec3Dot(&face.normal, &direction);
        if(faceDirection < 0.0f)
        {
            // Find the line-plane intersection point
            // Plane: (P-P₀).n = 0, Line: L = L₀ + td
            // Intersection point when L = P
            // Expand and Substitute for t = (P₀ - L₀).n / d.n
            const float epsilon = 0.001f;
            const float t = D3DXVec3Dot(&(face.origin - origin), &face.normal) / faceDirection;
            D3DXVECTOR3 intersection = origin + (t * direction);

            // Ensure point of intersection is on the plane
            assert(fabs(D3DXVec3Dot(&(intersection - face.origin), &face.normal)) <= epsilon);
            
            // Ensure ray is in front of the plane
            // To restrict the test to line-plane, check if t <= 1.0f
            if(t >= 0.0f)
            {
                // Convert world coordinates of p into barycentric coordinates 
                // p is a point on the plane found through plane-ray intersection
                // Plane equation: planeToPoint = su + tv where planeToPoint = p - p0
                // Dot by u/v to: pp.v = (su + tv).v and pp.u = (su + tv).u
                // Rearrange to: s = (pp.v - t(v.v)) / (u.v) and t = (p.u - s(u.u)) / (u.v)
                // Substitute each one into the other and find s,t
                D3DXVECTOR3 planeToPoint = intersection - face.origin;
                const float uu = face.uu;
                const float vv = face.vv;
                const float uv = face.uv;
                const float pv = D3DXVec3Dot(&planeToPoint, &face.v); 
                const float pu = D3DXVec3Dot(&planeToPoint, &face.u);
                float denominator = (uv * uv) - (vv * uu);
                denominator = denominator == 0.0f ? epsilon : denominator;
                m_pickCoords.t = ((uv * pu) - (pv * uu)) / denominator;
                m_pickCoords.s = ((pv * uv) - (pu * vv)) / denominator;

                // Determine if inside half of the plane (triangle)
                if(m_pickCoords.t >= 0.0f && m_pickCoords.s >= 0.0f && 
                    m_pickCoords.s + m_pickCoords.t <= 1.0f)
                {
                    // Convert back to world coordinates to find the distance
                    D3DXVec3TransformCoord(&intersection, &intersection, &world);
                    const float distanceToMesh = D3DXVec3Length(&(intersection - m_rayOrigin));
                    if(distanceToMesh < m_distanceToMesh)
                    {
                        // Set the new mesh as selected
                        m_pickWorld = &world;
                        m_pickFace = &face;
                        m_distanceToMesh = distanceToMesh;
                        m_mesh = mesh;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Picking::UpdateDiagnostics()
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::MESH))
    {
        m_engine->diagnostic()->UpdateText(Diagnostic::MESH,
            "PickCoord", Diagnostic::WHITE, StringCast(m_pickCoords.s)
            + ", " + StringCast(m_pickCoords.t));

        if(m_mesh)
        {
            const float radius = 0.1f;
            m_engine->diagnostic()->UpdateSphere(Diagnostic::MESH, 
                "PickIntersection", Diagnostic::BLACK, m_rayOrigin + 
                (m_distanceToMesh * m_rayDirection), radius);

            if(m_pickFace)
            {
                D3DXVECTOR3 p0, p1, p2;
                D3DXVec3TransformCoord(&p1, &m_pickFace->P1, m_pickWorld);
                D3DXVec3TransformCoord(&p2, &m_pickFace->P2, m_pickWorld);
                D3DXVec3TransformCoord(&p0, &m_pickFace->origin, m_pickWorld);

                m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,  
                    "PickLine0", Diagnostic::BLACK, p0, p1);

                m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                    "PickLine1", Diagnostic::BLACK, p1, p2);

                m_engine->diagnostic()->UpdateLine(Diagnostic::MESH,
                    "PickLine2", Diagnostic::BLACK, p2, p0);
            }
        }
    }
}