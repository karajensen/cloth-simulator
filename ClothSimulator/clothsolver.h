/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Class for solving cloth-object and cloth-cloth collisions
*****************************************************************/

#pragma once
#include "common.h"
#include "collision.h"

class Cloth;

class ClothSolver
{
public:

    /**
    * Constructor
    * @param the cloth to solve collisions for
    */
    ClothSolver(std::shared_ptr<Cloth> cloth);

    /**
    * Solves the collisions between the cloth particles
    */
    void SolveSelfCollision();

    /**
    * Solves a collision between a sphere and the cloth
    * @param the sphere collision geometry
    */
    void SolveSphereCollision(const Collision& sphere);

    /**
    * Solves a collision between a box and the cloth
    * @param the box collision geometry
    */
    void SolveBoxCollision(const Collision& box);

    /**
    * Solves a collision between a cylinder and the cloth
    * @param the cylinder collision geometry
    */
    void SolveCylinderCollision(const Collision& cylinder);

    /**
    * Simplified box-cloth collision for the ground plane
    * @param the ground collision geometry
    */
    void SolveGroundCollision(const Collision& box);

private:

    /**
    * @return the cloth from the weak pointer
    */
    std::shared_ptr<Cloth> GetCloth();

    std::weak_ptr<Cloth> m_cloth; ///< Cloth object to work on

};