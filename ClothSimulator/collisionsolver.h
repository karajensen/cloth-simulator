////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - clothsolver.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "collisionmesh.h"

class Cloth;

/**
* Class for solving cloth-object and cloth-cloth CollisionMeshs
*/
class CollisionSolver
{
public:

    /**
    * Constructor
    * @param cloth The cloth to solve CollisionMeshs for
    */
    explicit CollisionSolver(std::shared_ptr<Cloth> cloth);

    /**
    * Solves the CollisionMeshs between the cloth particles
    */
    void SolveSelfCollisionMesh();

    /**
    * Solves a CollisionMesh between a sphere and the cloth
    * @param sphere The sphere CollisionMesh geometry
    */
    void SolveSphereCollisionMesh(const CollisionMesh& sphere);

    /**
    * Solves a CollisionMesh between a box and the cloth
    * @param box The box CollisionMesh geometry
    */
    void SolveBoxCollisionMesh(const CollisionMesh& box);

    /**
    * Solves a CollisionMesh between a cylinder and the cloth
    * @param cylinder The cylinder CollisionMesh geometry
    */
    void SolveCylinderCollisionMesh(const CollisionMesh& cylinder);

    /**
    * Simplified box-cloth CollisionMesh for the ground plane
    * @param ground The ground CollisionMesh geometry
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