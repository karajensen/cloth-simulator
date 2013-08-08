#include "clothsolver.h"
#include "diagnostic.h"
#include "particle.h"
#include "cloth.h"

ClothSolver::ClothSolver(std::shared_ptr<Cloth> cloth) :
    m_cloth(cloth)
{
}

void ClothSolver::SolveSelfCollision()
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();


}

void ClothSolver::SolveSphereCollision(const Collision& sphere)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    D3DXVECTOR3 centerToParticle;
    D3DXVECTOR3 sphereCenter(sphere.GetPosition());

    std::for_each(particles.begin(), particles.end(), [&](const Cloth::ParticlePtr& particle)
    {
        centerToParticle = particle->GetCollision()->GetPosition() - sphereCenter;
        float length = D3DXVec3Length(&centerToParticle);

        if (length < sphere.GetRadius())
        {
            centerToParticle /= length;
            particle->MovePosition(centerToParticle*(sphere.GetRadius()-length)); 
        }
    });
}

void ClothSolver::SolveBoxCollision(const Collision& box)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

    for(unsigned int i = 0; i < particles.size(); ++i)
    {
        D3DXVECTOR3 pos = particles[i]->GetPosition();
        if(pos.y <= box.GetData().maxBounds.y)
        {
            float distance = box.GetData().maxBounds.y-pos.y;
            distance *= (distance < 0) ? -1.0f : 1.0f;
            particles[i]->MovePosition(D3DXVECTOR3(0,distance,0));
        }
    }
}

void ClothSolver::SolveCylinderCollision(const Collision& cylinder)
{
    auto cloth = GetCloth();
    auto& particles = cloth->GetParticles();

}

std::shared_ptr<Cloth> ClothSolver::GetCloth()
{
    if(m_cloth.expired())
    {
        Diagnostic::ShowMessage("cloth asked for is non-existant");
        return nullptr;
    }
    return m_cloth.lock();
}