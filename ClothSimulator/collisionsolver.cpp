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

void CollisionSolver::SolveParticleCollision(CollisionMesh& particleA, 
                                             CollisionMesh& particleB)
{
    const float combinedRadius = particleA.GetRadius() + particleB.GetRadius();
    D3DXVECTOR3 particleToParticle = particleB.GetPosition() - particleA.GetPosition();
    const float length = D3DXVec3Length(&particleToParticle);

    if (length < combinedRadius)
    {
        particleToParticle /= length;
        particleA.ResolveCollision(-particleToParticle*fabs(combinedRadius-length));
        particleB.ResolveCollision(particleToParticle*fabs(combinedRadius-length)); 
    }
}

void CollisionSolver::SolveParticleBoxCollision(CollisionMesh& particle, 
                                                const CollisionMesh& box)
{

}

void CollisionSolver::SolveParticleCylinderCollision(CollisionMesh& particle, 
                                                     const CollisionMesh& cylinder)
{


}

void CollisionSolver::SolveParticleSphereCollision(CollisionMesh& particle,
                                                   const CollisionMesh& sphere)
{
    D3DXVECTOR3 sphereToParticle = particle.GetPosition() - sphere.GetPosition();
    float length = D3DXVec3Length(&sphereToParticle);
    float halfRadius = particle.GetRadius() * 0.5f;
    float radius = sphere.GetRadius() + halfRadius;

    if (length < radius)
    {
        sphereToParticle /= length;
        particle.ResolveCollision(sphereToParticle *
            fabs(radius-length), CollisionMesh::SPHERE); 
    }
}

void CollisionSolver::SolveClothCollision(const D3DXVECTOR3& minBounds, 
                                          const D3DXVECTOR3& maxBounds)
{
    D3DPERF_BeginEvent(D3DCOLOR(), L"CollisionSolver::SolveClothCollision");

    auto cloth = GetCloth();
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
        switch(object.GetShape())
        {
        case CollisionMesh::SPHERE:
            SolveParticleSphereCollision(particle, object);
            break;
        case CollisionMesh::BOX:
            SolveParticleBoxCollision(particle, object);
            break;
        case CollisionMesh::CYLINDER:
            SolveParticleCylinderCollision(particle, object);
            break;
        }
    }
}

std::shared_ptr<Cloth> CollisionSolver::GetCloth()
{
    assert(!m_cloth.expired());
    return m_cloth.lock();
}