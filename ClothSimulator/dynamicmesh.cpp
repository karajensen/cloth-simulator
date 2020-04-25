////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - dynamicmesh.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "dynamicmesh.h"
#include "partition.h"
#include "shader.h"

#include <assert.h>

namespace
{
    /**
    * Collision Type bit flags
    */
    enum
    {
        NO_COLLISION = 1,
        BOX_COLLISION = 2,
        SPHERE_COLLISION = 4,
        CYLINDER_COLLISION = 8
    };
}

DynamicMesh::DynamicMesh(EnginePtr engine, DynamicMesh::MotionFn resolveFn)
    : CollisionMesh(engine, nullptr)
    , m_previousResolveVelocity(0.0f, 0.0f, 0.0f)
    , m_resolveVelocity(0.0f, 0.0f, 0.0f)
    , m_resolveFn(resolveFn)
    , m_collisionType(NO_COLLISION)
    , m_cachedCollisionType(NO_COLLISION)
{
    SetDraw(true);
}

void DynamicMesh::LoadInstance(const CollisionMesh& mesh)
{
    MakeZeroVector(m_resolveVelocity);
    MakeZeroVector(m_previousResolveVelocity);
    m_collisionType = NO_COLLISION;
    m_cachedCollisionType = NO_COLLISION;
    CollisionMesh::LoadInstance(mesh);
}

void DynamicMesh::DrawMesh(const Matrix& projection, const Matrix& view)
{
    D3DXVECTOR3 color = m_colour;
    if(!IsCollidingWith(Geometry::NONE))
    {
        color = m_engine->diagnostic()->GetColor(Diagnostic::BLACK);
    }
    else if(m_partition)
    {
        color = m_engine->diagnostic()->GetColor(m_partition->GetColor());
    }
    CollisionMesh::DrawMesh(projection, view, color);
}

void DynamicMesh::PositionalNonParentalUpdate(const D3DXVECTOR3& position)
{
    assert(!m_parent);

    m_positionDelta += position - m_position;
    m_world.SetPosition(position);
    m_position = position;
    m_requiresPositionalUpdate = true;
}

void DynamicMesh::UpdateCollision()
{
    CollisionMesh::UpdateCollision();
    m_previousResolveVelocity = m_resolveVelocity;
    MakeZeroVector(m_resolveVelocity);
    m_cachedCollisionType = m_collisionType;
    m_collisionType = NO_COLLISION;
}

void DynamicMesh::DrawRepresentation(const Matrix& projection, 
                                     const Matrix& view, 
                                     float scale,
                                     const D3DXVECTOR3& color,
                                     const D3DXVECTOR3& position)
{
    // Set the matrix explicitly to stop any observer updates
    const float savedscale = m_localWorld.GetScale().x;
    m_world.MatrixPtr()->_11 = scale;
    m_world.MatrixPtr()->_22 = scale;
    m_world.MatrixPtr()->_33 = scale;
    m_world.MatrixPtr()->_41 = position.x;
    m_world.MatrixPtr()->_42 = position.y;
    m_world.MatrixPtr()->_43 = position.z;
    CollisionMesh::DrawMesh(projection, view, color);
    m_world.MatrixPtr()->_11 = savedscale;
    m_world.MatrixPtr()->_22 = savedscale;
    m_world.MatrixPtr()->_33 = savedscale;
    m_world.MatrixPtr()->_41 = m_position.x;
    m_world.MatrixPtr()->_42 = m_position.y;
    m_world.MatrixPtr()->_43 = m_position.z;
}

unsigned int DynamicMesh::GetCollisionType(Geometry::Shape shape) const
{
    switch(shape)
    {
    case Geometry::BOX:
        return BOX_COLLISION;
    case Geometry::SPHERE:
        return SPHERE_COLLISION;
    case Geometry::CYLINDER:
        return CYLINDER_COLLISION;
    case Geometry::NONE:
    default:
        return NO_COLLISION;
    }
}

void DynamicMesh::ResolveCollision(const D3DXVECTOR3& translation)
{
    if(IsDynamic())
    {
        m_resolveFn(translation);
    }
}

void DynamicMesh::ResolveCollision(const D3DXVECTOR3& translation, 
                                     const D3DXVECTOR3& velocity, 
                                     Geometry::Shape shape)
{
    if(IsDynamic())
    {
        // Remove the side components of the interacting velocity
        // This prevents 'stickyness' of the cloth for sideways motion
        m_resolveVelocity.y *= 1.25f;
        m_resolveVelocity.x = 0.0f;
        m_resolveVelocity.z = 0.0f;

        m_resolveVelocity += velocity;
        m_collisionType &= ~NO_COLLISION;
        m_collisionType |= GetCollisionType(shape);
        m_resolveFn(translation + m_resolveVelocity);
    }
}

bool DynamicMesh::IsDynamic() const
{
    return m_resolveFn != nullptr;
}

bool DynamicMesh::IsCollidingWith(Geometry::Shape shape) const
{
    unsigned int collisionType = GetCollisionType(shape);
    return (m_cachedCollisionType & collisionType) == collisionType;
}

const D3DXVECTOR3& DynamicMesh::GetInteractingVelocity() const
{
    return m_previousResolveVelocity;
}