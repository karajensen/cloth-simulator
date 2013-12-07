////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - particle.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "collisionmesh.h"
#include "callbacks.h"

class Shader;

/**
* A sphere representing a vertex on the cloth
*/
class Particle
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit Particle(EnginePtr engine);

    /**
    * Create the particle
    * @param position The intial position of the particle
    * @param uv The uvs for the particle
    * @param index The internal index of the particle
    * @param geometry The collision geometry for the particle
    * @param data The collision data for the particle
    */
    void Initialise(const D3DXVECTOR3& position, const D3DXVECTOR2& uv, unsigned int index,
         std::shared_ptr<CollisionMesh::Geometry> geometry, const CollisionMesh::Data& data);

    /**
    * Draws the particle visual mesh
    * @param projection The projection matrix
    * @param view The view matrix
    * @param position The position to set the visual mesh
    */
    void DrawVisualMesh(const Matrix& projection, 
        const Matrix& view, const D3DXVECTOR3& position);

    /**
    * Draws the particle collision mesh
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void DrawCollisionMesh(const Matrix& projection, const Matrix& view);

    /**
    * @return the particle collision mesh object
    */
    CollisionMesh& GetCollisionMesh();

    /**
    * Resets the particle back to its initial position
    */
    void ResetPosition();

    /**
    * Adds force to the particle
    * @param force The force to add
    */
    void AddForce(const D3DXVECTOR3& force);

    /**
    * @return whether particle is pinned
    */
    bool IsPinned() const { return m_pinned; };

    /**
    * @param pin Set whether the particle is pinned
    */
    void PinParticle(bool pin);

    /**
    * @return whether particle is selected
    */
    bool IsSelected() const { return m_selected; };

    /**
    * @param select Set whether particle is selected
    */
    void SelectParticle(bool select);

    /**
    * Move a particle explicitly
    * @param position The position to move to
    */
    void MovePosition(const D3DXVECTOR3& position);

    /**
    * @return whether the particle should undergo smoothing or not
    */
    bool RequiresSmoothing() const;

    /**
    * @return the internal index of the particle
    */
    unsigned int GetIndex() const { return m_index; }

    /**
    * @return the position of the particle in world coordinates
    */
    const D3DXVECTOR3& GetPosition() const { return m_position; }

    /**
    * @return the uvs for the particle
    */
    const D3DXVECTOR2& GetUVs() const { return m_uvs; }

    /**
    * Update the particles position
    * @param damping The damping to apply to the movement
    * @param timestepSqr Delta time squared
    */
    void PreCollisionUpdate(float damping, float timestepSqr);

    /**
    * Sets the colour of the visual particle mesh
    * @param colour The colour to set to in rgb
    */
    void SetColor(const D3DXVECTOR3& colour);

    /**
    * Resets the acceleration to zero
    */
    void ResetAcceleration();

    /**
    * Updates the required values post collision resolution
    */
    void PostCollisionUpdate();

private:

    /**
    * Updates the particle's transform with the cached position
    */
    void UpdateTransform();

    /**
    * Prevent copying
    */
    Particle(const Particle&);
    Particle& operator=(const Particle&);
               
    D3DXVECTOR3 m_previousPosition;              ///< Current previous position this tick
    D3DXVECTOR3 m_position;                      ///< Current position in world coordinates of particle
    D3DXVECTOR3 m_acceleration;                  ///< Current acceleration of particle
    D3DXVECTOR3 m_initialPosition;               ///< Initial position of particle 
    D3DXVECTOR2 m_uvs;                           ///< Texture uvs for the particle
    Transform m_transform;                       ///< Current transform of particle
    bool m_selected;                             ///< Whether particle is selected or not
    bool m_pinned;                               ///< Whether particle is pinned or not
    unsigned int m_index;                        ///< Internal index of the particle
    D3DXVECTOR3 m_color;                         ///< Color of the particle
    std::shared_ptr<CollisionMesh> m_collision;  ///< collision geometry for particle
};