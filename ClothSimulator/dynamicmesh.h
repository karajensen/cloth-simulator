////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - dynamicmesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "collisionmesh.h"

/**
* Non-parental sphere mesh whose positional movement is explicitly set by the owner
*/
class DynamicMesh : public CollisionMesh
{
public:

    /**
    * Function signature for resolving a collision
    */
    typedef std::function<void(const D3DXVECTOR3&)> MotionFn;

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    * @param resolveFn A function to call to resolve any collision
    */
    DynamicMesh(EnginePtr engine, MotionFn resolveFn);

    /**
    * Loads the collision as an instance of another
    * @param mesh The collision mesh to base the instance off
    */
    virtual void LoadInstance(const CollisionMesh& mesh) override;

    /**
    * Draw the collision geometry and diagnostics
    * @param projection The projection matrix
    * @param view The view matrix
    */
    virtual void DrawMesh(const Matrix& projection, const Matrix& view) override;

    /**
    * Updates the partition and any cached values the require it
    */
    virtual void UpdateCollision() override;

    /**
    * Draw the collision geometry with a specific visual representation
    * @param projection The projection matrix
    * @param view The view matrix
    * @param radius The radius to override
    * @param color The color to render with
    * @param position The position to render at
    */
    void DrawRepresentation(const Matrix& projection, const Matrix& view, 
        float radius, const D3DXVECTOR3& color, const D3DXVECTOR3& position);

    /**
    * Updates the collision geometry upon translate for non-parented meshes
    * @param position The position to be set to
    */
    void PositionalNonParentalUpdate(const D3DXVECTOR3& position);

    /**
    * Moves the owner of the collision mesh to resolve a collision
    * @param translation The amount to move the owner by
    */
    virtual void ResolveCollision(const D3DXVECTOR3& translation) override;

    /**
    * Moves the owner of the collision mesh to resolve a collision
    * @param translation The amount to move the owner by
    * @param velocity The velocity of the colliding mesh
    * @param shape The interacting body causing the movement
    */
    virtual void ResolveCollision(const D3DXVECTOR3& translation, 
        const D3DXVECTOR3& velocity, Geometry::Shape shape) override;

    /**
    * @return whether the collision mesh is dynamic or kinematic
    */
    virtual bool IsDynamic() const override;

    /**
    * @param shape The shape to query for interaction
    * @return whether the mesh is colliding with the given shape
    */
    bool IsCollidingWith(Geometry::Shape shape) const;

    /**
    * @return the velocity for the interacting collision meshes
    */
    virtual const D3DXVECTOR3& GetInteractingVelocity() const override;

private:

    /**
    * Prevent copying
    */
    DynamicMesh(const DynamicMesh&);
    DynamicMesh& operator=(const DynamicMesh&);

    /**
    * @param shape The shape to find the collision type for
    * @return the type of collision from the given shape
    */
    unsigned int GetCollisionType(Geometry::Shape shape) const;

    D3DXVECTOR3 m_resolveVelocity;             ///< Combined resolution velocity
    D3DXVECTOR3 m_previousResolveVelocity;     ///< Combined previous resolution velocity
    MotionFn m_resolveFn;                      ///< Translate the collision in response to a collision
    unsigned int m_collisionType;              ///< Interacting collision bodies this tick
    unsigned int m_cachedCollisionType;        ///< Interacting collision bodies last tick 
};                                             
