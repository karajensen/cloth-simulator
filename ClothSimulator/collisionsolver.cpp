////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - clothsolver.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionsolver.h"
#include "particle.h"
#include "cloth.h"

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
        const float radius = particles[i]->GetCollision()->GetRadius() * 2.0f;

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

void CollisionSolver::SolveSphereCollision(const Collision& sphere)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    D3DXVECTOR3 centerToParticle;
    D3DXVECTOR3 sphereCenter(sphere.GetPosition());

    std::for_each(particles.begin(), particles.end(), [&](const Cloth::ParticlePtr& particle)
    {
        centerToParticle = particle->GetPosition() - sphereCenter;
        float length = D3DXVec3Length(&centerToParticle);
        float radius = sphere.GetRadius() + particle->GetCollision()->GetRadius();

        if (length < radius)
        {
            centerToParticle /= length;
            particle->MovePosition(centerToParticle*fabs(radius-length)); 
        }
    });
}

void CollisionSolver::SolveBoxCollision(const Collision& box)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    const D3DXVECTOR3& maxBounds = box.GetMaxBounds();
    const D3DXVECTOR3& minBounds = box.GetMinBounds();
    const float boxRadius = D3DXVec3Length(&(maxBounds-minBounds));

    std::for_each(particles.begin(), particles.end(), [&](const Cloth::ParticlePtr& particle)
    {
        D3DXVECTOR3 sphereToBox = box.GetPosition() - particle->GetPosition();
        float length = D3DXVec3Length(&sphereToBox);

        //Test whether inside box radius
        if (length < particle->GetCollision()->GetRadius() + boxRadius)
        {


        }
    });
}

void CollisionSolver::SolveCylinderCollision(const Collision& cylinder)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();
    const float cylinderRadius = cylinder.GetRadius();

    std::for_each(particles.begin(), particles.end(), [&](const Cloth::ParticlePtr& particle)
    {
        D3DXVECTOR3 sphereToCylinder = cylinder.GetPosition() - particle->GetPosition();
        float length = D3DXVec3Length(&sphereToCylinder);

        //Test whether inside cylinder radius
        if (length < particle->GetCollision()->GetRadius() + cylinderRadius)
        {







        }
    });
}

void CollisionSolver::SolveGroundCollision(const Collision& ground)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    std::for_each(particles.begin(), particles.end(), [&](const Cloth::ParticlePtr& particle)
    {
        if(particle->GetPosition().y <= ground.GetMaxBounds().y)
        {
            float distance = fabs(ground.GetMaxBounds().y-particle->GetPosition().y);
            particle->MovePosition(D3DXVECTOR3(0.0f, distance, 0.0f));
        }
    });
}

std::shared_ptr<Cloth> CollisionSolver::GetCloth()
{
    if(m_cloth.expired())
    {
        ShowMessageBox("cloth asked for is non-existant");
        return nullptr;
    }
    return m_cloth.lock();
}