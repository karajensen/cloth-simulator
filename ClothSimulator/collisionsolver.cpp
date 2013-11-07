////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionsolver.h"
#include "particle.h"
#include "cloth.h"
#include <assert.h>

CollisionSolver::CollisionSolver(std::shared_ptr<Engine> engine, 
                                 std::shared_ptr<Cloth> cloth) :
    m_cloth(cloth),
    m_engine(engine)
{
}

CollisionSolver::~CollisionSolver()
{
}

void CollisionSolver::SolveParticleCollision(CollisionMesh& particleA, 
                                             CollisionMesh& particleB)
{
    D3DXVECTOR3 particleToParticle = particleB.GetPosition() - particleA.GetPosition();
    const float length = D3DXVec3Length(&particleToParticle);
    const float combinedRadius = particleA.GetRadius() + particleB.GetRadius();

    if (length < combinedRadius)
    {
        particleToParticle /= length;
        particleA.ResolveCollision(-particleToParticle*fabs(combinedRadius-length));
        particleB.ResolveCollision(particleToParticle*fabs(combinedRadius-length)); 
    }
}

void CollisionSolver::SolveParticleHullCollision(CollisionMesh& particle, 
                                                 const CollisionMesh& hull)
{
    // Determine if within a rough radius of the convex hull
    D3DXVECTOR3 sphereToParticle = particle.GetPosition() - hull.GetPosition();
    const float length = D3DXVec3Length(&sphereToParticle);
    const float combinedRadius = hull.GetRadius() + particle.GetRadius();

    if (length < combinedRadius)
    {
        // GJK involves creating a new shape from the Minkowski Difference of two hulls.
        // If the new shape includes the origin then two objects intersect. 
        // Reference: http://physics2d.com/content/gjk-algorithm

        const int maxIterations = 10;
        const std::vector<D3DXVECTOR3>& hullVertices = hull.GetVertices();
        const std::vector<D3DXVECTOR3>& particleVertices = particle.GetVertices();
        D3DXVECTOR3 initialDirection = particleVertices[0] - hullVertices[0];





    }
}

const D3DXVECTOR3& CollisionSolver::GetFurthestPoint(const std::vector<D3DXVECTOR3>& points,
                                                     const D3DXVECTOR3& direction) const
{
    int maximumIndex = 0;
    float maximumDot = D3DXVec3Dot(&points[maximumIndex], &direction);
    for(unsigned int i = 1; i < points.size(); ++i)
    {
        float dot = D3DXVec3Dot(&points[i], &direction);
        if(dot < maximumDot)
        {
            maximumDot = dot;
            maximumIndex = i;
        }
    }
    return points[maximumIndex];
}

void CollisionSolver::SolveParticleSphereCollision(CollisionMesh& particle,
                                                   const CollisionMesh& sphere)
{
    D3DXVECTOR3 sphereToParticle = particle.GetPosition() - sphere.GetPosition();
    const float length = D3DXVec3Length(&sphereToParticle);
    const float combinedRadius = sphere.GetRadius() + particle.GetRadius();

    if (length < combinedRadius)
    {
        sphereToParticle /= length;
        particle.ResolveCollision(sphereToParticle *
            fabs(combinedRadius-length), CollisionMesh::SPHERE); 
    }
}

void CollisionSolver::SolveClothCollision(const D3DXVECTOR3& minBounds, 
                                          const D3DXVECTOR3& maxBounds)
{
    D3DPERF_BeginEvent(D3DCOLOR(), L"CollisionSolver::SolveClothCollision");

    assert(!m_cloth.expired());
    auto cloth = m_cloth.lock();
    auto& particles = cloth->GetParticles();

    for(unsigned int i = 0; i < particles.size(); ++i)
    {
        // Solve the particles against themselves
        for(unsigned int j = i+1; j < particles.size(); ++j)
        {
            SolveParticleCollision(particles[i]->GetCollisionMesh(), 
                particles[j]->GetCollisionMesh());
        }

        // Solve the particle against the eight scene walls
        const D3DXVECTOR3& particlePosition = particles[i]->GetPosition();
        D3DXVECTOR3 position(0.0, 0.0, 0.0);

        // Check for ground and roof collisions
        if(particlePosition.y <= maxBounds.y)
        {
            position.y = maxBounds.y-particlePosition.y;
        }
        else if(particlePosition.y >= minBounds.y)
        {
            position.y = minBounds.y-particlePosition.y;
        }

        // Check for left and right wall collisions
        if(particlePosition.x >= maxBounds.x)
        {
            position.x = maxBounds.x-particlePosition.x;
        }
        else if(particlePosition.x <= minBounds.x)
        {
            position.x = minBounds.x-particlePosition.x;
        }

        // Check for forward and backward wall collisions
        if(particlePosition.z >= maxBounds.z)
        {
            position.z = maxBounds.z-particlePosition.z;
        }
        else if(particlePosition.z <= minBounds.z)
        {
            position.z = minBounds.z-particlePosition.z;
        }

        particles[i]->MovePosition(position);
    }

    D3DPERF_EndEvent();
}

void CollisionSolver::SolveObjectCollision(CollisionMesh& particle,
                                           const CollisionMesh& object)
{
    if(particle.IsDynamic())
    {
        if(object.GetShape() == CollisionMesh::SPHERE)
        {
            SolveParticleSphereCollision(particle, object);
        }
        else
        {
            SolveParticleHullCollision(particle, object);
        }
    }
}