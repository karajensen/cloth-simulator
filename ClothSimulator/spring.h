////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - spring.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

class Particle;
class Diagnostic;

/**
* Spring between particles for the cloth
*/
class Spring
{
public:

    /**
    * Types of springs available
    */
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
    * @param p1/p2 The two particles connected by the spring
    * @param id The ID of the particle
    * @param type The type of spring created
    */
    void Initialise(Particle& p1, Particle& p2, int id, Type type);

    /**
    * Update the spring
    * @param timestep multiplier solved spring positions
    */
    void SolveSpring(float timestep);

    /**
    * Updates the line diagnostic for the spring
    * @param diagnostic The diagnostic renderer
    */
    void UpdateDiagnostic(Diagnostic& diagnostic) const;

private:

    Type m_type;            ///< type of spring
    int m_id;               ///< ID for the spring
    int m_color;            ///< Color of spring depending on how it affects the cloth
    Particle* m_particle1;  ///< connected particle
    Particle* m_particle2;  ///< connected particle
    float m_restDistance;   ///< distance for spring at rest
};
