////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "collision.h"

class Cloth;

/**
* Class for solving cloth-object and cloth-cloth collisions
*/
class ClothSolver
{
public:

    /**
    * Constructor
    * @param cloth The cloth to solve collisions for
    */
    explicit ClothSolver(std::shared_ptr<Cloth> cloth);

    /**
    * Solves the collisions between the cloth particles
    */
    void SolveSelfCollision();

    /**
    * Solves a collision between a sphere and the cloth
    * @param sphere The sphere collision geometry
    */
    void SolveSphereCollision(const Collision& sphere);

    /**
    * Solves a collision between a box and the cloth
    * @param box The box collision geometry
    */
    void SolveBoxCollision(const Collision& box);

    /**
    * Solves a collision between a cylinder and the cloth
    * @param cylinder The cylinder collision geometry
    */
    void SolveCylinderCollision(const Collision& cylinder);

    /**
    * Simplified box-cloth collision for the ground plane
    * @param ground The ground collision geometry
    */
    void SolveGroundCollision(const Collision& ground);

private:

    /**
    * Prevent copying
    */
    ClothSolver(const ClothSolver&);
    ClothSolver& operator=(const ClothSolver&);

    /**
    * @return the cloth from the weak pointer
    */
    std::shared_ptr<Cloth> GetCloth();

    std::weak_ptr<Cloth> m_cloth; ///< Cloth object to work on

};