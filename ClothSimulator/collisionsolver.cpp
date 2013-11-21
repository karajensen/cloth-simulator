////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionsolver.h"
#include "particle.h"
#include "cloth.h"
#include <assert.h>
#include <deque>

CollisionSolver::CollisionSolver(std::shared_ptr<Engine> engine, 
                                 std::shared_ptr<Cloth> cloth) :
    m_cloth(cloth),
    m_engine(engine)
{
}

CollisionSolver::~CollisionSolver()
{
}

void CollisionSolver::SolveParticleCollision(CollisionMesh& particleA, 
                                             CollisionMesh& particleB)
{
    D3DXVECTOR3 particleToParticle = particleB.GetPosition() - particleA.GetPosition();
    const float length = D3DXVec3Length(&particleToParticle);
    const float combinedRadius = particleA.GetRadius() + particleB.GetRadius();

    if (length < combinedRadius)
    {
        particleToParticle /= length;
        particleA.ResolveCollision(-particleToParticle*fabs(combinedRadius-length));
        particleB.ResolveCollision(particleToParticle*fabs(combinedRadius-length)); 
    }
}

void CollisionSolver::SolveParticleHullCollision(CollisionMesh& particle, 
                                                 const CollisionMesh& hull)
{
    // Determine if within a rough radius of the convex hull
    D3DXVECTOR3 sphereToParticle = particle.GetPosition() - hull.GetPosition();
    const float length = D3DXVec3Length(&sphereToParticle);
    const float combinedRadius = hull.GetRadius() + particle.GetRadius();

    if (length < combinedRadius)
    {
        // If two convex hulls have collided, the Minkowski Difference 
        // of their hulls will contain the origin. GJK generates a simplex
        // that encases the Minkowski Difference points to test for this.
        // Reference: http://physics2d.com/content/gjk-algorithm

        std::deque<D3DXVECTOR3> simplex;
        const std::vector<D3DXVECTOR3>& particleVertices = particle.GetVertices();
        const std::vector<D3DXVECTOR3>& hullVertices = hull.GetVertices();

        // Determine an initial point for the simplex
        const int initialIndex = 0;
        D3DXVECTOR3 direction = particleVertices[initialIndex] - hullVertices[initialIndex];
        D3DXVECTOR3 lastEdgePoint = GetMinkowskiDifferencePoint(direction, particle, hull);
        simplex.push_back(lastEdgePoint);
        direction = -direction;

        // Iteratively create the simplex
        int iteration = 0;
        const int maxIterations = 30;
        bool collisionFound = false;
        while(iteration < maxIterations && !collisionFound)
        {
            ++iteration;
            assert(simplex.size() <= 4);

            lastEdgePoint = GetMinkowskiDifferencePoint(direction, particle, hull);
            simplex.push_back(lastEdgePoint);

            if(D3DXVec3Dot(&lastEdgePoint, &direction) <= 0)
            {
                // New edge point of simplex is not past the origin.
                // No collision is possible
                iteration = maxIterations;
            }
            else
            {
                const D3DXVECTOR3& pointA = lastEdgePoint;
                const D3DXVECTOR3 AO = -pointA;
                if(simplex.size() == 2)
                {
                    // Simplex is a line
                    const D3DXVECTOR3& pointB = simplex[0];
                    const D3DXVECTOR3 AB = pointB - pointA;

                    // Generate a new direction for the next point 
                    // perpendicular to the line using triple product
                    D3DXVECTOR3 ABcrossAO;
                    D3DXVec3Cross(&ABcrossAO, &AB, &AO);
                    D3DXVec3Cross(&direction, &ABcrossAO, &AB);
                }
                else if(simplex.size() == 3)
                {
                    // Simplex is a triangle plane
                    const D3DXVECTOR3& pointB = simplex[0];
                    const D3DXVECTOR3& pointC = simplex[1];
                    const D3DXVECTOR3 AB = pointB - pointA;
                    const D3DXVECTOR3 AC = pointC - pointA;
                
                    // Determine which side of the plane the origin is on
                    D3DXVECTOR3 planeNormal;
                    D3DXVec3Cross(&planeNormal, &AB, &AC);
                    const float distanceToPlane = D3DXVec3Dot(&planeNormal, &AO);

                    // Determine the new search direction towards the origin
                    direction = (distanceToPlane < 0.0f) ? -planeNormal : planeNormal;
                }
                else if(simplex.size() == 4)
                {
                    // Simplex is a tetrahedron
                    const D3DXVECTOR3& pointB = simplex[0];
                    const D3DXVECTOR3& pointC = simplex[1];
                    const D3DXVECTOR3& pointD = simplex[2];

                    const D3DXVECTOR3 AB = pointB - pointA;
                    const D3DXVECTOR3 AC = pointC - pointA;
                    const D3DXVECTOR3 AD = pointD - pointA;

                    // Check if within the three surrounding planes
                    // The forth plane has been previously tested with the plane simplex
                    // All normals will point to the center of the tetrahedron
                    D3DXVECTOR3 CBnormal, BDnormal, DCnormal;
                    D3DXVec3Cross(&CBnormal, &AC, &AB);
                    D3DXVec3Cross(&BDnormal, &AB, &AD);
                    D3DXVec3Cross(&DCnormal, &AD, &AC);

                    D3DXVec3Normalize(&CBnormal, &CBnormal);
                    D3DXVec3Normalize(&BDnormal, &BDnormal);
                    D3DXVec3Normalize(&DCnormal, &DCnormal);

                    const float CBdistance = D3DXVec3Dot(&CBnormal, &AO);
                    const float BDdistance = D3DXVec3Dot(&BDnormal, &AO);
                    const float DCdistance = D3DXVec3Dot(&DCnormal, &AO);

                    if(CBdistance < 0.0f)
                    {
                        // Origin is outside of the CB plane
                        // D is furthest point, remove it and search towards the origin
                        simplex.erase(std::remove(simplex.begin(), simplex.end(), pointD), simplex.end()); 
                        direction = -CBnormal;
                    }
                    else if(BDdistance < 0.0f)
                    {
                        // Origin is outside of the BD plane
                        // C is furthest point, remove it and search towards the origin
                        simplex.erase(std::remove(simplex.begin(), simplex.end(), pointC), simplex.end()); 
                        direction = -BDnormal;
                    }
                    else if(DCdistance < 0.0f)
                    {
                        // Origin is outside of the DC plane
                        // C is furthest point, remove it and search towards the origin
                        simplex.erase(std::remove(simplex.begin(), simplex.end(), pointB), simplex.end()); 
                        direction = -DCnormal;
                    }
                    else
                    {
                        // Origin is within the tetrahedron
                        collisionFound = true;
                    }
                }
            }
        }

        if(collisionFound)
        {
            particle.ResetMotion(hull.GetShape());
        }
    }
}

D3DXVECTOR3 CollisionSolver::GetMinkowskiDifferencePoint(const D3DXVECTOR3& direction,
                                                         const CollisionMesh& particle, 
                                                         const CollisionMesh& hull)
{
    return FindFurthestPoint(particle.GetVertices(), direction) - 
        FindFurthestPoint(hull.GetVertices(), -direction);
}

const D3DXVECTOR3& CollisionSolver::FindFurthestPoint(const std::vector<D3DXVECTOR3>& points,
                                                      const D3DXVECTOR3& direction) const
{
    int furthestIndex = 0;
    float furthestDot = D3DXVec3Dot(&points[furthestIndex], &direction);
    for(unsigned int i = 1; i < points.size(); ++i)
    {
        float dot = D3DXVec3Dot(&points[i], &direction);
        if(dot > furthestDot)
        {
            furthestDot = dot;
            furthestIndex = i;
        }
    }
    return points[furthestIndex];
}

void CollisionSolver::SolveParticleSphereCollision(CollisionMesh& particle,
                                                   const CollisionMesh& sphere)
{
    D3DXVECTOR3 sphereToParticle = particle.GetPosition() - sphere.GetPosition();
    const float length = D3DXVec3Length(&sphereToParticle);
    const float combinedRadius = sphere.GetRadius() + particle.GetRadius();

    if (length < combinedRadius)
    {
        sphereToParticle /= length;
        particle.ResolveCollision(sphereToParticle *
            fabs(combinedRadius-length), sphere.GetShape());
    }
}

void CollisionSolver::SolveClothCollision(const D3DXVECTOR3& minBounds, 
                                          const D3DXVECTOR3& maxBounds)
{
    D3DPERF_BeginEvent(D3DCOLOR(), L"CollisionSolver::SolveClothCollision");

    assert(!m_cloth.expired());
    auto cloth = m_cloth.lock();
    auto& particles = cloth->GetParticles();

    for(unsigned int i = 0; i < particles.size(); ++i)
    {
        // Solve the particles against themselves
        for(unsigned int j = i+1; j < particles.size(); ++j)
        {
            SolveParticleCollision(particles[i]->GetCollisionMesh(), 
                particles[j]->GetCollisionMesh());
        }

        // Solve the particle against the eight scene walls
        const D3DXVECTOR3& particlePosition = particles[i]->GetPosition();
        D3DXVECTOR3 position(0.0, 0.0, 0.0);

        // Check for ground and roof collisions
        if(particlePosition.y <= maxBounds.y)
        {
            position.y = maxBounds.y-particlePosition.y;
        }
        else if(particlePosition.y >= minBounds.y)
        {
            position.y = minBounds.y-particlePosition.y;
        }

        // Check for left and right wall collisions
        if(particlePosition.x >= maxBounds.x)
        {
            position.x = maxBounds.x-particlePosition.x;
        }
        else if(particlePosition.x <= minBounds.x)
        {
            position.x = minBounds.x-particlePosition.x;
        }

        // Check for forward and backward wall collisions
        if(particlePosition.z >= maxBounds.z)
        {
            position.z = maxBounds.z-particlePosition.z;
        }
        else if(particlePosition.z <= minBounds.z)
        {
            position.z = minBounds.z-particlePosition.z;
        }

        particles[i]->MovePosition(position);
    }

    D3DPERF_EndEvent();
}

void CollisionSolver::SolveObjectCollision(CollisionMesh& particle,
                                           const CollisionMesh& object)
{
    if(particle.IsDynamic())
    {
        if(object.GetShape() == CollisionMesh::SPHERE)
        {
            SolveParticleSphereCollision(particle, object);
        }
        else
        {
            SolveParticleHullCollision(particle, object);
        }
    }
}