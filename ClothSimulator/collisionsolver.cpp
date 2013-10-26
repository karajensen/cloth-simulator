////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionsolver.h"
#include "particle.h"
#include "cloth.h"
#include <assert.h>

CollisionSolver::CollisionSolver(std::shared_ptr<Cloth> cloth) :
    m_cloth(cloth)
{
}

void CollisionSolver::SolveSelfCollision()
{
    D3DPERF_BeginEvent(D3DCOLOR(), L"CollisionSolver::SolveSelfCollision");

    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    for(unsigned int i = 0; i < particles.size(); ++i)
    {
        D3DXVECTOR3 centerToParticle;
        D3DXVECTOR3 center(particles[i]->GetPosition());
        const float radius = particles[i]->GetCollisionMesh()->GetRadius() * 2.0f;

        for(unsigned int j = i+1; j < particles.size(); ++j)
        {
            centerToParticle = particles[j]->GetPosition() - center;
            const float length = D3DXVec3Length(&centerToParticle);

            if (length < radius)
            {
                centerToParticle /= length;
                particles[i]->MovePosition(-centerToParticle*fabs(radius-length));
                particles[j]->MovePosition(centerToParticle*fabs(radius-length)); 
            }
        }
    }

    D3DPERF_EndEvent();
}

void CollisionSolver::SolveSphereCollisionMesh(const CollisionMesh& sphere)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    D3DXVECTOR3 centerToParticle;
    D3DXVECTOR3 sphereCenter(sphere.GetPosition());

    for(const Cloth::ParticlePtr& particle : particles)
    {
        centerToParticle = particle->GetPosition() - sphereCenter;
        float length = D3DXVec3Length(&centerToParticle);
        float halfClothRadius = particle->GetCollisionMesh()->GetRadius() * 0.5f;
        float radius = sphere.GetRadius() + halfClothRadius;

        if (length < radius)
        {
            centerToParticle /= length;
            particle->MovePosition(centerToParticle*fabs(radius-length)); 
        }
    }
}

void CollisionSolver::SolveBoxCollisionMesh(const CollisionMesh& box)
{

}

void CollisionSolver::SolveCylinderCollisionMesh(const CollisionMesh& cylinder)
{


}

void CollisionSolver::SolveClothWallCollision(const D3DXVECTOR3& minBounds, 
                                              const D3DXVECTOR3& maxBounds)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    for(const std::unique_ptr<Particle>& particle : particles)
    {
        const D3DXVECTOR3& particlePosition = particle->GetPosition();
        D3DXVECTOR3 position(0.0, 0.0, 0.0);

        // Check for ground and roof collisions
        if(particlePosition.y <= maxBounds.y)
        {
            position.y = fabs(maxBounds.y-particlePosition.y);
        }
        else if(particlePosition.y >= minBounds.y)
        {
            position.y = fabs(minBounds.y-particlePosition.y);
        }

        // Check for left and right wall collisions
        if(particlePosition.x >= maxBounds.x)
        {
            position.x = fabs(maxBounds.x-particlePosition.x);
        }
        else if(particlePosition.x <= minBounds.x)
        {
            position.x = fabs(minBounds.x-particlePosition.x);
        }

        // Check for forward and backward wall collisions
        if(particlePosition.z >= maxBounds.z)
        {
            position.z = fabs(maxBounds.z-particlePosition.z);
        }
        else if(particlePosition.z <= minBounds.z)
        {
            position.z = fabs(minBounds.z-particlePosition.z);
        }

        particle->MovePosition(position);
    }
}

std::shared_ptr<Cloth> CollisionSolver::GetCloth()
{
    assert(!m_cloth.expired());
    return m_cloth.lock();
}