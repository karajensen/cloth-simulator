////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - clothsolver.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "collisionmesh.h"

class Cloth;

/**
* Solves cloth-object and cloth-cloth collisions
*/
class CollisionSolver
{
public:

    /**
    * Constructor
    * @param cloth The cloth to solve collisions for
    */
    explicit CollisionSolver(std::shared_ptr<Cloth> cloth);

    /**
    * Solves the collisions between the cloth particles
    */
    void SolveSelfCollision();

    /**
    * Solves a collision between a sphere and the cloth
    * @param sphere The sphere collision geometry
    */
    void SolveSphereCollisionMesh(const CollisionMesh& sphere);

    /**
    * Solves a collision between a box and the cloth
    * @param box The box collision geometry
    */
    void SolveBoxCollisionMesh(const CollisionMesh& box);

    /**
    * Solves a collision between a cylinder and the cloth
    * @param cylinder The cylinder collision geometry
    */
    void SolveCylinderCollisionMesh(const CollisionMesh& cylinder);

    /**
    * Simplified box-cloth collision for the ground plane
    * @param ground The ground collision geometry
    */
    void SolveGroundCollisionMesh(const CollisionMesh& ground);

private:

    /**
    * Prevent copying
    */
    CollisionSolver(const CollisionSolver&);
    CollisionSolver& operator=(const CollisionSolver&);

    /**
    * @return the cloth from the weak pointer
    */
    std::shared_ptr<Cloth> GetCloth();

    std::weak_ptr<Cloth> m_cloth; ///< Cloth object to work on

};