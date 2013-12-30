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
    const float lengthSqr = D3DXVec3LengthSq(&particleToParticle);
    const float combinedRadius = particleA.GetRadius() + particleB.GetRadius();

    if (lengthSqr < (combinedRadius*combinedRadius))
    {
        const float length = std::sqrt(lengthSqr);
        particleToParticle /= std::sqrt(length);
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
    const float lengthSqr = D3DXVec3LengthSq(&sphereToParticle);
    const float extendedParticleRadius = particle.GetRadius() * 2.0f;
    const float combinedRadius = hull.GetRadius() + extendedParticleRadius;

    if (lengthSqr < (combinedRadius*combinedRadius))
    {
        Simplex simplex;
        if(AreConvexHullsColliding(particle, hull, simplex))
        {
            simplex.GenerateFaces();
            const D3DXVECTOR3 penetration = GetConvexHullPenetration(particle, hull, simplex);
            particle.ResolveCollision(penetration, hull.GetVelocity(), hull.GetShape());
        }
    }
}

bool CollisionSolver::AreConvexHullsColliding(const CollisionMesh& particle, 
                                              const CollisionMesh& hull, 
                                              Simplex& simplex)
{
    // If two convex hulls have collided, the Minkowski Sum A + (-B) of both 
    // hulls will contain the origin. Reference from 'Proximity Queries and 
    // Penetration Depth Computation on 3D Game Objects' by Gino van den Bergen
    // http://graphics.stanford.edu/courses/cs468-01-fall/Papers/van-den-bergen.pdf

    const std::vector<D3DXVECTOR3>& particleVertices = particle.GetVertices();
    const std::vector<D3DXVECTOR3>& hullVertices = hull.GetVertices();

    // Determine an initial point for the simplex
    const int initialIndex = 0;
    D3DXVECTOR3 direction = particleVertices[initialIndex] - hullVertices[initialIndex];
    D3DXVECTOR3 lastEdgePoint = GetMinkowskiSumEdgePoint(direction, particle, hull);
    simplex.AddPoint(lastEdgePoint);
        
    direction = -direction;
    int iteration = 0;
    bool collisionFound = false;
    bool collisionPossible = true;
    const int maxIterations = 20;

    // Iteratively create a simplex within the Minkowski Sum Hull
    while(iteration < maxIterations && !collisionFound && collisionPossible)
    {
        ++iteration;
        lastEdgePoint = GetMinkowskiSumEdgePoint(direction, particle, hull);
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
    D3DXVECTOR3 furthestPoint;
    D3DXVECTOR3 penetrationDirection;
    float penetrationDistance = 0.0f;
    bool penetrationFound = false;
    const float minDistance = 0.1f;
    const int maxIterations = 10;
    int iteration = 0;

    while(!penetrationFound && iteration < maxIterations)
    {
        ++iteration;
        const Face& face = simplex.GetClosestFaceToOrigin();
        penetrationDirection = face.normal;
        penetrationDistance = face.distanceToOrigin;
        penetrationFound = penetrationDistance == 0.0f;

        if(!penetrationFound)
        {
            // Check if there are any edge points beyond the closest face
            furthestPoint = GetMinkowskiSumEdgePoint(face.normal, particle, hull);
            const D3DXVECTOR3 faceToPoint = furthestPoint - simplex.GetPoint(face.indices[0]);
            const float distance = fabs(D3DXVec3Dot(&faceToPoint, &face.normal));
            penetrationFound = distance < minDistance;

            if(!penetrationFound)
            {
                // Add the new point and extend the convex hull
                simplex.ExtendFace(furthestPoint);
            }
        }
    }

    if(!penetrationFound)
    {
        // Fallback on the initial closest face
        const Face& face = simplex.GetClosestFaceToOrigin();
        penetrationDirection = face.normal;
        penetrationDistance = face.distanceToOrigin;
    }

    if(particle.RenderSolverDiagnostics())
    {
        UpdateDiagnostics(simplex, furthestPoint);
    }

    return -(penetrationDirection * penetrationDistance);
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

D3DXVECTOR3 CollisionSolver::GetMinkowskiSumEdgePoint(const D3DXVECTOR3& direction,
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
    const float lengthSqr = D3DXVec3LengthSq(&sphereToParticle);
    const float combinedRadius = sphere.GetRadius() + particle.GetRadius();

    if (lengthSqr < (combinedRadius*combinedRadius))
    {
        const float length = std::sqrt(lengthSqr);
        sphereToParticle /= length;

        particle.ResolveCollision(sphereToParticle * fabs(combinedRadius-length), 
            sphere.GetVelocity(), sphere.GetShape());
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
        if(object.GetShape() == Geometry::SPHERE)
        {
            SolveParticleSphereCollision(particle, object);
        }
        else
        {
            SolveParticleHullCollision(particle, object);
        }
    }
}

void CollisionSolver::UpdateDiagnostics(const Simplex& simplex, 
                                        const D3DXVECTOR3& furthestPoint)
{
    if(m_engine->diagnostic()->AllowDiagnostics(Diagnostic::COLLISION))
    {
        const float radius = 0.1f;
        const float normalLength = 1.5f;
        D3DXVECTOR3 origin(0.0, 0.0, 0.0);

        m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION,
            "OriginPoint", Diagnostic::WHITE, origin, radius);

        m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION, 
            "FurthestPoint", Diagnostic::MAGENTA, furthestPoint, radius);

        const auto& borders = simplex.GetBorderEdges();
        for(unsigned int i = 0; i < borders.size(); ++i)
        {
            m_engine->diagnostic()->UpdateLine(Diagnostic::COLLISION,
                "BorderEdge" + StringCast(i), Diagnostic::RED, 
                simplex.GetPoint(borders[i].indices[0]), 
                simplex.GetPoint(borders[i].indices[1]));                    
        }

        const auto& faces = simplex.GetFaces();
        for(unsigned int i = 0; i < faces.size(); ++i)
        {
            if(faces[i].alive)
            {
                std::string id = StringCast(i);
                const Face& face = faces[i];

                const D3DXVECTOR3 center = simplex.GetFaceCenter(i);
                const D3DXVECTOR3& normal = face.normal * normalLength;
                const D3DXVECTOR3& pointA = simplex.GetPoint(face.indices[0]);
                const D3DXVECTOR3& pointB = simplex.GetPoint(face.indices[1]);
                const D3DXVECTOR3& pointC = simplex.GetPoint(face.indices[2]);

                m_engine->diagnostic()->UpdateSphere(Diagnostic::COLLISION, 
                    "sCenter" + id, Diagnostic::BLUE, center, radius);

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