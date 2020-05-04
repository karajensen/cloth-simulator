////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "callbacks.h"

struct Face;
class Simplex;
class Particle;
class Cloth;

/**
* Detects and solves cloth-object and cloth-cloth collisions
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
    * Destructor
    */
    ~CollisionSolver();

    /**
    * Detects and solves cloth particle-particle and particle-wall collisions
    * @param minBounds The minimum point inside the walls
    * @param maxBounds The maximum point inside the walls
    */
    void SolveClothCollision(const D3DXVECTOR3& minBounds, const D3DXVECTOR3& maxBounds);

    /**
    * Detects and solves cloth and scene object-particle collisions
    * @param particle The collision mesh for the particle
    * @param object The collision mesh for the scene object
    */
    void SolveObjectCollision(CollisionMesh& particle, const CollisionMesh& object);

private:

    /**
    * Prevent copying
    */
    CollisionSolver(const CollisionSolver&) = delete;
    CollisionSolver& operator=(const CollisionSolver&) = delete;

    /**
    * Detects and solves a collision between two particles
    * @param particleA The collision mesh for the first particle
    * @param particleB The collision mesh for the second particle
    */
    void SolveParticleCollision(CollisionMesh& particleA, CollisionMesh& particleB);

    /**
    * Detects and solves a collision between a convex hull and a particle
    * @param particle The collision mesh for the particle
    * @param hull The collision mesh for the convex hull
    */
    void SolveParticleHullCollision(CollisionMesh& particle, const CollisionMesh& hull);

    /**
    * Detects and solves a collision between a sphere and particle
    * @param particle The collision mesh for the particle
    * @param sphere The collision mesh for the sphere
    */
    void SolveParticleSphereCollision(CollisionMesh& particle, const CollisionMesh& sphere);

    /**
    * Generates the furthest point along a direction from a set of points
    * @param direction The direction to search along
    * @param points The set of points to search
    * @return The furthest point in the set along the given direction
    */
    const D3DXVECTOR3& FindFurthestPoint(const std::vector<D3DXVECTOR3>& points,
                                         const D3DXVECTOR3& direction) const;

    /**
    * Generates a point on the edge of the Minkowski Sum hull
    * using a chosen vertex from each collision mesh that is furthest
    * along the given direction. Known as a 'support' function.
    * @param direction The direction to search along
    * @param particle The collision mesh for the particle
    * @param hull The collision mesh for the convex hull
    * @return an edge point in the Minkowski Sum
    */
    D3DXVECTOR3 GetMinkowskiSumEdgePoint(const D3DXVECTOR3& direction,
                                         const CollisionMesh& particle, 
                                         const CollisionMesh& hull);

    /**
    * Determines the next search direction given a line simplex
    * @param simplex The line simplex of two points
    * @param direction The current search direction to modify
    */
    void SolveLineSimplex(const Simplex& simplex, D3DXVECTOR3& direction);

    /**
    * Determines the next search direction given a tri plane simplex
    * @param simplex The plane simplex of three points
    * @param direction The current search direction to modify
    */
    void SolvePlaneSimplex(Simplex& simplex, D3DXVECTOR3& direction);

    /**
    * Determines the next search direction given a tetrahedron simplex
    * @param simplex The tetrahedron simplex of four points
    * @param direction The current search direction to modify
    * @return whether the origin is inside the simplex 
    */
    bool SolveTetrahedronSimplex(Simplex& simplex, D3DXVECTOR3& direction);

    /**
    * Uses the GJK Algorithm to determine collision between two convex hulls
    * @param particle The collision mesh for the particle
    * @param hull The collision mesh for the convex hull
    * @param simplex An empty simplex to fill with at most four points
    * @return whether the two convex hulls are colliding
    */
    bool AreConvexHullsColliding(const CollisionMesh& particle, 
                                 const CollisionMesh& hull, 
                                 Simplex& simplex);

    /**
    * Uses the theory of EPA to determine penetration between two convex hulls
    * @param particle The collision mesh for the particle
    * @param hull The collision mesh for the convex hull
    * @param simplex The tetrahedron simplex encasing the origin
    * @return The direction and magnitude of penetration between the hulls
    */
    D3DXVECTOR3 GetConvexHullPenetration(const CollisionMesh& particle, 
                                         const CollisionMesh& hull, 
                                         Simplex& simplex);

    /**
    * Updates the diagnostics for a simplex
    * @param simplex The simplex to update diagnostics for
    * @param furthestPoint The last furthest point to find for the simplex
    */
    void UpdateDiagnostics(const Simplex& simplex, const D3DXVECTOR3& furthestPoint);

private:

    std::weak_ptr<Cloth> m_cloth;     ///< Cloth object holding all particles
    std::shared_ptr<Engine> m_engine; ///< Callbacks for the rendering engine
};