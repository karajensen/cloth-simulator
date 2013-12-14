////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionsolver.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "collisionsolver.h"
#include "particle.h"
#include "cloth.h"
#include "simplex.h"
#include <assert.h>

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
        const D3DXVECTOR3 translation = particleToParticle*fabs(combinedRadius-length);
        particleA.ResolveCollision(-translation);
        particleB.ResolveCollision(translation);
    }
}

void CollisionSolver::SolveParticleHullCollision(CollisionMesh& particle, 
                                                 const CollisionMesh& hull)
{
    // Determine if within a rough radius of the convex hull
    const D3DXVECTOR3 sphereToParticle = particle.GetPosition() - hull.GetPosition();
    const float length = D3DXVec3Length(&sphereToParticle);
    const float combinedRadius = hull.GetRadius() + particle.GetRadius();

    if (length < combinedRadius)
    {
        Simplex simplex;
        if(AreConvexHullsColliding(particle, hull, simplex))
        {
            const D3DXVECTOR3 penetration = GetConvexHullPenetration(particle, hull, simplex);
            particle.ResolveCollision(penetration, hull.GetShape());
        }
    }
}

bool CollisionSolver::AreConvexHullsColliding(const CollisionMesh& particle, 
                                              const CollisionMesh& hull, 
                                              Simplex& simplex)
{
    // If two convex hulls have collided, the Minkowski Difference of both 
    // hulls will contain the origin. Reference from 'Proximity Queries and 
    // Penetration Depth Computation on 3D Game Objects' by Gino van den Bergen.

    const std::vector<D3DXVECTOR3>& particleVertices = particle.GetVertices();
    const std::vector<D3DXVECTOR3>& hullVertices = hull.GetVertices();

    // Determine an initial point for the simplex
    const int initialIndex = 0;
    D3DXVECTOR3 direction = particleVertices[initialIndex] - hullVertices[initialIndex];
    D3DXVECTOR3 lastEdgePoint = GetMinkowskiDifferencePoint(direction, particle, hull);
    simplex.AddPoint(lastEdgePoint);

    direction = -direction;
    int iteration = 0;
    bool collisionFound = false;
    bool collisionPossible = true;
    const int maxIterations = 30;

    // Iteratively create a simplex within the Minkowski Difference hull
    while(iteration < maxIterations && !collisionFound && collisionPossible)
    {
        ++iteration;
        lastEdgePoint = GetMinkowskiDifferencePoint(direction, particle, hull);
        simplex.AddPoint(lastEdgePoint);

        if(D3DXVec3Dot(&lastEdgePoint, &direction) <= 0)
        {
            // New edge point of simplex is not past the origin.
            collisionPossible = false;
        }
        else if(simplex.IsLine())
        {
            SolveLineSimplex(simplex, direction);
        }
        else if(simplex.IsTriPlane())
        {
            SolvePlaneSimplex(simplex, direction);
        }
        else if(simplex.IsTetrahedron())
        {
            collisionFound = SolveTetrahedronSimplex(simplex, direction);
        }
    }
    return collisionFound;
}

D3DXVECTOR3 CollisionSolver::GetConvexHullPenetration(const CollisionMesh& particle, 
                                                      const CollisionMesh& hull, 
                                                      Simplex& simplex)
{
    simplex.GenerateFaces();
    D3DXVECTOR3 projectedOrigin;
    bool penetrationFound = false;
    const float epsilon = 0.1f;
    const int maxIterations = 1;
    int currentIteration = 0;

    //////////////////////////////////////////////////////
    //if(particle.RenderCollisionDiagnostics())
    //{
    //    const auto& points = simplex.GetPoints();
    //    const auto& faces = simplex.GetFaces();
    //
    //    for(unsigned int i = 0; i < faces.size(); ++i)
    //    {
    //        std::string id = StringCast(i);
    //        const Face& face = faces[i];
    //
    //        const D3DXVECTOR3 center = simplex.GetFaceCenter(i);
    //        const D3DXVECTOR3& normal = face.normal;
    //        const D3DXVECTOR3& pointA = points[face.indices[0]];
    //        const D3DXVECTOR3& pointB = points[face.indices[1]];
    //        const D3DXVECTOR3& pointC = points[face.indices[2]];
    //
    //       //m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
    //       //    "sNormal1" + id, Diagnostic::BLUE, center, center + normal);
    //
    //        m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
    //            "sLine11" + id, Diagnostic::RED, pointA, pointB);
    //
    //        m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
    //            "sLine21" + id, Diagnostic::RED, pointA, pointC);
    //
    //        m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
    //            "sLine31" + id, Diagnostic::RED, pointC, pointB);
    //    }
    //}
    //////////////////////////////////////////////////////

    while(!penetrationFound && currentIteration < maxIterations)
    {
        const Face& face = GetClosestFace(simplex);
        penetrationFound = face.distanceToOrigin == 0.0f;

        if(!penetrationFound)
        {
            projectedOrigin = face.distanceToOrigin * face.normal;

            // Ensure projected point of origin lies within the face triangle
            // TO DO
 

            // Check if there are any edge points beyond the closest face
            const D3DXVECTOR3 point = GetMinkowskiDifferencePoint(face.normal, particle, hull);
            const D3DXVECTOR3 faceToPoint = point - simplex.GetPoint(face.indices[0]);
            const float distance = fabs(D3DXVec3Dot(&faceToPoint, &face.normal));
            penetrationFound = distance < epsilon;

            if(particle.RenderCollisionDiagnostics())
            {
                m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION, 
                    "point", Diagnostic::MAGENTA, point, 0.2f);
            }

            if(!penetrationFound)
            {
                // Add the new point and extend the convex hull
                simplex.ExtendFace(point);
            }
        }
        ++currentIteration;
    }

    if(!penetrationFound)
    {
        // Fallback on the initial closest face
        const Face& face = GetClosestFace(simplex);
        projectedOrigin = face.distanceToOrigin * face.normal;
    }

    if(particle.RenderCollisionDiagnostics())
    {
        UpdateDiagnostics(simplex);
    }

    // Penetration vector is from origin to closest face
    //return -projectedOrigin;
    return D3DXVECTOR3(0,0,0);
}

const Face& CollisionSolver::GetClosestFace(Simplex& simplex)
{
    int closest = 0;
    const auto& faces = simplex.GetFaces();
    for(unsigned int i = 0; i < faces.size(); ++i)
    {
        if(faces[i].alive && faces[i].distanceToOrigin
            < faces[closest].distanceToOrigin)
        {
            closest = i;
        }
    }
    return faces[closest];
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

D3DXVECTOR3 CollisionSolver::GetMinkowskiDifferencePoint(const D3DXVECTOR3& direction,
                                                         const CollisionMesh& particle, 
                                                         const CollisionMesh& hull)
{
    return FindFurthestPoint(particle.GetVertices(), direction) - 
        FindFurthestPoint(hull.GetVertices(), -direction);
}

void CollisionSolver::SolveLineSimplex(const Simplex& simplex, D3DXVECTOR3& direction)
{
    const D3DXVECTOR3& pointA = simplex.GetPoint(1);
    const D3DXVECTOR3& pointB = simplex.GetPoint(0);
    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AO = -pointA;

    // Generate a new direction for the next point 
    // perpendicular to the line using triple product
    D3DXVECTOR3 ABcrossAO;
    D3DXVec3Cross(&ABcrossAO, &AB, &AO);
    D3DXVec3Cross(&direction, &ABcrossAO, &AB);
}

void CollisionSolver::SolvePlaneSimplex(Simplex& simplex, D3DXVECTOR3& direction)
{
    const D3DXVECTOR3& pointA = simplex.GetPoint(2);
    const D3DXVECTOR3& pointB = simplex.GetPoint(0);
    const D3DXVECTOR3& pointC = simplex.GetPoint(1);

    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AC = pointC - pointA;
    const D3DXVECTOR3 AO = -pointA;
                
    // Determine which side of the plane the origin is on
    D3DXVECTOR3 planeNormal;
    D3DXVec3Cross(&planeNormal, &AB, &AC);

    // Determine the new search direction towards the origin
    const float distanceToPlane = D3DXVec3Dot(&planeNormal, &AO);
    direction = (distanceToPlane < 0.0f) ? -planeNormal : planeNormal;
}

bool CollisionSolver::SolveTetrahedronSimplex(Simplex& simplex, D3DXVECTOR3& direction)
{
    const D3DXVECTOR3& pointA = simplex.GetPoint(3);
    const D3DXVECTOR3& pointB = simplex.GetPoint(0);
    const D3DXVECTOR3& pointC = simplex.GetPoint(1);
    const D3DXVECTOR3& pointD = simplex.GetPoint(2);

    const D3DXVECTOR3 AB = pointB - pointA;
    const D3DXVECTOR3 AC = pointC - pointA;
    const D3DXVECTOR3 AD = pointD - pointA;
    const D3DXVECTOR3 AO = -pointA;

    // Check if within the three surrounding planes
    // The forth plane has been previously tested with the plane simplex
    // All normals will point to the center of the tetrahedron
    D3DXVECTOR3 CBnormal, BDnormal, DCnormal;
    D3DXVec3Cross(&CBnormal, &AC, &AB);
    D3DXVec3Cross(&BDnormal, &AB, &AD);
    D3DXVec3Cross(&DCnormal, &AD, &AC);

    const float CBdistance = D3DXVec3Dot(&CBnormal, &AO);
    const float BDdistance = D3DXVec3Dot(&BDnormal, &AO);
    const float DCdistance = D3DXVec3Dot(&DCnormal, &AO);

    bool originInsideSimplex = true;
    if(CBdistance < 0.0f)
    {
        // Origin is outside of the CB plane
        // D is furthest point, remove it and search towards the origin
        simplex.RemovePoint(pointD);
        direction = -CBnormal;
        originInsideSimplex = false;
    }
    else if(BDdistance < 0.0f)
    {
        // Origin is outside of the BD plane
        // C is furthest point, remove it and search towards the origin
        simplex.RemovePoint(pointC);
        direction = -BDnormal;
        originInsideSimplex = false;
    }
    else if(DCdistance < 0.0f)
    {
        // Origin is outside of the DC plane
        // C is furthest point, remove it and search towards the origin
        simplex.RemovePoint(pointB);
        direction = -DCnormal;
        originInsideSimplex = false;
    }
    return originInsideSimplex;
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

void CollisionSolver::UpdateDiagnostics(const Simplex& simplex)
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::COLLISION))
    {
        const float radius = 0.1f;
        m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
            "origin", Diagnostic::WHITE, D3DXVECTOR3(0.0, 0.0, 0.0), radius);

        const auto& points = simplex.GetPoints();
        const auto& faces = simplex.GetFaces();

        for(unsigned int i = 0; i < faces.size(); ++i)
        {
            if(faces[i].alive)
            {
                std::string id = StringCast(i);
                const Face& face = faces[i];

                const D3DXVECTOR3 center = simplex.GetFaceCenter(i);
                const D3DXVECTOR3& normal = face.normal;
                const D3DXVECTOR3& pointA = points[face.indices[0]];
                const D3DXVECTOR3& pointB = points[face.indices[1]];
                const D3DXVECTOR3& pointC = points[face.indices[2]];

                m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
                    "sNormal" + id, Diagnostic::BLUE, center, center + normal);

                m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
                    "sLine1" + id, Diagnostic::YELLOW, pointA, pointB);

                m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION, 
                    "sLine2" + id, Diagnostic::YELLOW, pointA, pointC);

                m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
                    "sLine3" + id, Diagnostic::YELLOW, pointC, pointB);

            }
        }
    }
}