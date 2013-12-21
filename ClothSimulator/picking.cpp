////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "picking.h"
#include "pickablemesh.h"

Picking::Picking(EnginePtr engine):
    m_rayDirection(0.0f, 0.0f, 0.0f),
    m_rayOrigin(0.0f, 0.0f, 0.0f),
    m_mesh(nullptr),
    m_distanceToMesh(0.0f),
    m_locked(false),
    m_engine(engine)
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

    // Create screen space mouse ray
    D3DXVECTOR3 mouseRay;
    mouseRay.x =  (((2.0f*x)/WINDOW_WIDTH )-1) / projection.GetMatrix()._11;
    mouseRay.y = -(((2.0f*y)/WINDOW_HEIGHT)-1) / projection.GetMatrix()._22;
    mouseRay.z =  CAMERA_NEAR;

    // Convert to view space mouse ray
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

bool Picking::RayCastMesh(const D3DXMATRIX& worldInverse,
                          const Geometry& geometry, 
                          float& distanceToMesh)
{
    bool foundCollision = false;

    // Convert origin/direction into local mesh coordinates
    D3DXVECTOR3 origin, direction;
    D3DXVec3TransformCoord(&origin, &GetRayOrigin(), &worldInverse);
    D3DXVec3TransformNormal(&direction, &GetRayDirection(), &worldInverse);
    D3DXVec3Normalize(&direction, &direction);

   for(const MeshFace& face : geometry.GetFaces())
   {
       const float distance = D3DXVec3Dot(&face.normal, &m_rayDirection);
       if(distance >= 0.0f)
       {
           const D3DXVECTOR3 projectedpoint = m_rayOrigin - (face.normal * distance);
           const D3DXVECTOR3 d = projectedpoint - face.origin;
   
           // Convert world coordinates of p into barycentric coordinates
           // Plane equation: d = su + tv where d = p - p0
           // Dot by u/v to: d.v = (su + tv).v and d.u = (su + tv).u
           // Rearrange to: s = (dv - t(vv)) / (uv) and t = (du - s(uu)) / (uv)
           // Substitute each one into the other and find s,t
           const float uu = face.uu;
           const float vv = face.vv;
           const float uv = face.uv;
           const float dv = D3DXVec3Dot(&d, &face.v);
           const float du = D3DXVec3Dot(&d, &face.u);
           const float t = ((uv * du) - (dv * uu)) / ((uv * uv) - (vv * uu));
           const float s = ((dv * uv) - (du * vv)) / ((uv * uv) - (uu * vv));
   
           // Determine if inside half of the plane (triangle)
           if(t >= 0.0f && s >= 0.0f && t+s <= 1.0f)
           {
               distanceToMesh = distance;
               foundCollision = true;
               break;
           }
       }
   }

    return foundCollision;
}