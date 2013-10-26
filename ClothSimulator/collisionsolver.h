////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

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
    * Wall-cloth collision detection and resolution
    * @note iterates over all particles as quicker than adding to octree
    * @param minimumBounds The minimum point inside the walls
    * @param maximumBounds The maximum point inside the walls
    */
    void SolveClothWallCollision(const D3DXVECTOR3& minBounds, 
        const D3DXVECTOR3& maxBounds);

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