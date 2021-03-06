////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - spring.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "spring.h"
#include "particle.h"
#include "diagnostic.h"
#include "utils.h"

Spring::Spring()
    : m_type(STRETCH)
    , m_id(-1)
    , m_color(0)
    , m_particle1(nullptr)
    , m_particle2(nullptr)
    , m_restDistance(0.0f)
{
}

void Spring::Initialise(Particle& p1, Particle& p2, int id, Type type)
{
    m_type = type;
    m_id = id;
    m_particle1 = &p1;
    m_particle2 = &p2;
   
    D3DXVECTOR3 difference = p1.GetPosition()-p2.GetPosition();
    m_restDistance = D3DXVec3Length(&difference);

    switch(type)
    {
    case STRETCH:
        m_color = Diagnostic::RED;
        break;
    case SHEAR:
        m_color = Diagnostic::GREEN;
        break;
    case BEND:
        m_color = Diagnostic::YELLOW;
        break;
    }
}

void Spring::SolveSpring(float timestep)
{
    const D3DXVECTOR3& v1 = m_particle1->GetCollisionMesh().GetInteractingVelocity();
    const D3DXVECTOR3& v2 = m_particle2->GetCollisionMesh().GetInteractingVelocity();

    D3DXVECTOR3 difference(m_particle2->GetPosition() - m_particle1->GetPosition());
    float distance = D3DXVec3Length(&difference);
    D3DXVECTOR3 error(difference-((difference/distance)*m_restDistance)); 

    if(v1 != v2 && (!IsZeroVector(v1) || !IsZeroVector(v2)))
    {
        // Move the particle with the smallest amount of interacting 
        // velocity towards the particle with the most amount
        const float v1Length = D3DXVec3LengthSq(&v1);
        const float v2Length = D3DXVec3LengthSq(&v2);
        m_particle2->MovePosition(-error * (v1Length > v2Length ? 0.9f : 0.1f));
        m_particle1->MovePosition(error * (v1Length > v2Length ? 0.1f : 0.9f));
    }
    else
    {
        D3DXVECTOR3 errorHalf(error * 0.5f);
        m_particle1->MovePosition(errorHalf);
        m_particle2->MovePosition(-errorHalf);
    }
}

void Spring::UpdateDiagnostic(Diagnostic& diagnostic) const
{
    if(m_type == BEND)
    {
        diagnostic.UpdateSphere(Diagnostic::CLOTH, "Spring"+StringCast(m_id), 
            static_cast<Diagnostic::Colour>(m_color),
            (m_particle1->GetPosition()*0.5f)+(m_particle2->GetPosition()*0.5f), 0.1f);
    }
    else
    {
        diagnostic.UpdateLine(Diagnostic::CLOTH, "Spring"+StringCast(m_id), 
            static_cast<Diagnostic::Colour>(m_color),
            m_particle1->GetPosition(), m_particle2->GetPosition());
    }
}
