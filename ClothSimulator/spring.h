/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Cloth Spring objects
*****************************************************************/

#pragma once

#include "common.h"

class Particle;

class Spring
{
public:

    Spring();

    /**
    * Creates the spring
    * @param the two particles connected by the spring
    */
    void Initialise(Particle& p1, Particle& p2);

    /**
    * Update the spring
    */
    void SolveSpring();

private:

    Particle* m_particle1; ///< connected particle
    Particle* m_particle2; ///< connected particle
    float m_restDistance; ///< distance for spring at rest
};
