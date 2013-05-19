/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Cloth Spring objects
*****************************************************************/

#pragma once

#include "common.h"

class Particle;

struct Spring
{
    /**
    * Constructor
    * @param the two particles connected by the spring
    */
    Spring(Particle& p1, Particle& p2);

    /**
    * Update the spring
    */
    void SolveSpring();

    Particle& P1; ///< connected particle
    Particle& P2; ///< connected particle
    float RestDistance; ///< distance for spring at rest
};
