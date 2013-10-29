////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

class Particle;
class CollisionMesh;
class Cloth;

/**
* Solves cloth-object and cloth-cloth collisions
*/
class CollisionSolver
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    * @param cloth The cloth to solve collisions for
    */
    CollisionSolver(std::shared_ptr<Engine> engine, std::shared_ptr<Cloth> cloth);

    /**
    * Solves the cloth self and wall collisions
    * @param minimumBounds The minimum point inside the walls
    * @param maximumBounds The maximum point inside the walls
    */
    void SolveClothCollision(const D3DXVECTOR3& minBounds, const D3DXVECTOR3& maxBounds);

    /**
    * Solves the collisions between the cloth particles and scene objects
    * @param particle The particle to solve for
    * @param object The scene object to solve against
    */
    void SolveObjectCollision(CollisionMesh& particle, const CollisionMesh& object);

private:

    /**
    * Prevent copying
    */
    CollisionSolver(const CollisionSolver&);
    CollisionSolver& operator=(const CollisionSolver&);

    /**
    * Solves a collision between two particles
    * @param particleA The collision mesh for the first particle
    * @param particleB The collision mesh for the second particle
    */
    void SolveParticleCollision(CollisionMesh& particleA, CollisionMesh& particleB);

    /**
    * Solves a collision between a cylinder and particle
    * @param particle The collision mesh for the particle
    * @param cylinder The collision mesh for the cylinder
    */
    void SolveParticleCylinderCollision(CollisionMesh& particle, const CollisionMesh& cylinder);

    /**
    * Solves a collision between a sphere and particle
    * @param particle The collision mesh for the particle
    * @param sphere The collision mesh for the sphere
    */
    void SolveParticleSphereCollision(CollisionMesh& particle, const CollisionMesh& sphere);

    /**
    * Solves a collision between a box and particle
    * @param particle The collision mesh for the particle
    * @param box The collision mesh for the sphere
    */
    void SolveParticleBoxCollision(CollisionMesh& particle, const CollisionMesh& box);

    /**
    * @return the cloth from the weak pointer
    */
    std::shared_ptr<Cloth> GetCloth();

    std::weak_ptr<Cloth> m_cloth;     ///< Cloth object to work on
    std::shared_ptr<Engine> m_engine; ///< Callbacks for the rendering engine

};