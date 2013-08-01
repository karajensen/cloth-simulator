/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Cloth Spring objects
*****************************************************************/

#pragma once

#include "common.h"

class Particle;

class Spring
{
public:

    enum Type
    {
        STRETCH,
        SHEAR,
        BEND
    };

    /**
    * Constructor
    */
    Spring();

    /**
    * Creates the spring
    * @param the two particles connected by the spring
    */
    void Initialise(Particle& p1, Particle& p2, int id, Type type);

    /**
    * Update the spring
    */
    void SolveSpring();

    /**
    * Updates the line diagnostic for the spring
    */
    void UpdateDiagnostic() const;

private:

    Type m_type; ///< type of spring
    int m_id; ///< ID for the spring
    int m_color; ///< Color of spring depending on how it affects the cloth
    Particle* m_particle1; ///< connected particle
    Particle* m_particle2; ///< connected particle
    float m_restDistance; ///< distance for spring at rest
};
