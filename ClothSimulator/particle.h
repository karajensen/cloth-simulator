/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Cloth particle objects
*****************************************************************/

#pragma once

#include "common.h"

class Collision;
class CollisionSphere;

class Particle
{
public:

    /**
    * Constructor
    * @param the directx device
    * @param the actual radius of the particle collision mesh
    */
    Particle(LPDIRECT3DDEVICE9 d3ddev, float radius);

    /**
    * Create the particle
    * @param the intial position of the particle
    * @param the internal index of the particle
    */
    void Initialise(const D3DXVECTOR3& position, unsigned int index);

    /**
    * Draws the particle visual mesh
    * @param the projection matrix
    * @param the view matrix
    */
    void DrawVisualMesh(const Transform& projection, const Transform& view);

    /**
    * Draws the particle collision mesh
    * @param the projection matrix
    * @param the view matrix
    */
    void DrawCollisionMesh(const Transform& projection, const Transform& view);

    /**
    * @return the particle collision mesh object
    */
    Collision* GetCollision();

    /**
    * Resets the particle back to its initial position
    */
    void ResetPosition();

    /**
    * Adds force to the particle
    * @param the force to add
    */
    void AddForce(const FLOAT3& force);

    /**
    * @return whether particle is pinned
    */
    bool IsPinned() const { return m_pinned; };

    /**
    * @param set whether the particle is pinned
    */
    void PinParticle(bool pin);

    /**
    * @return whether particle is selected
    */
    bool IsSelected() const { return m_selected; };

    /**
    * @param set whether particle is selected
    */
    void SelectParticle(bool select);

    /**
    * Move a particle explicitly
    * @param the position to move to
    */
    void MovePosition(const FLOAT3& v);

    /**
    * @return the internal index of the particle
    */
    unsigned int GetIndex() const { return m_index; }

    /**
    * @return the position of the particle in world coordinates
    */
    const FLOAT3& GetPosition() const { return m_position; }

    /**
    * Update the particles position
    * @param the damping to apply to the movement
    * @param delta time squared
    */
    void Update(float damping, float timestepSqr);

    /**
    * Sets the colour of the visual particle mesh
    * @param the colour to set to in rgb
    */
    void SetColor(const D3DXVECTOR3& colour);

    /**
    * Resets the acceleration to zero
    */
    void ResetAcceleration();

private:

    FLOAT3 m_acceleration;      ///< Current acceleration of particle
    FLOAT3 m_oldPosition;       ///< Save position from last update
    FLOAT3 m_initialPosition;   ///< Initial position of particle 
    FLOAT3 m_position;          ///< Current position in world coordinates of particle
    Transform m_transform;      ///< Current transform of particle
    bool m_selected;            ///< Whether particle is selected or not
    bool m_pinned;              ///< Whether particle is pinned or not
    unsigned int m_index;       ///< Internal index of the particle

    std::shared_ptr<CollisionSphere> m_collision; ///< Collision geometry for particle
};