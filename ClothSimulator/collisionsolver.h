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
    * @param minBounds The minimum point inside the walls
    * @param maxBounds The maximum point inside the walls
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
    * Solves a collision between a convex hull and a particle using GJK detection
    * @param particle The collision mesh for the particle
    * @param hull The collision mesh for the convex hull
    */
    void SolveParticleHullCollision(CollisionMesh& particle, const CollisionMesh& hull);

    /**
    * Solves a collision between a sphere and particle
    * @param particle The collision mesh for the particle
    * @param sphere The collision mesh for the sphere
    */
    void SolveParticleSphereCollision(CollisionMesh& particle, const CollisionMesh& sphere);

    std::weak_ptr<Cloth> m_cloth;     ///< Cloth object to work on
    std::shared_ptr<Engine> m_engine; ///< Callbacks for the rendering engine

};