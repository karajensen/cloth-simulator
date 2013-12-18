////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

/**
* Base class for any mesh that is able to be picked
*/
class PickableMesh
{
public:

    typedef std::function<void(void)> MeshPickFn;

    /**
    * Constructor.
    */
    PickableMesh();

    /**
    * Destructor
    */
    virtual ~PickableMesh();

    /**
    * Set mesh pick function
    * @param fn the function to set
    */
    void SetMeshPickFunction(MeshPickFn fn);

    /**
    * Function to call upon a mesh pick
    */
    void OnPickMesh();

private:

    MeshPickFn m_meshPickFn; ///< Function to call if mesh was picked
};

/**
* Handles all mouse picking
*/
class Picking
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit Picking(EnginePtr engine);

    /**
    * Send a ray into the scene to determine if mouse clicked an object
    * @param projection The camera projection matrix
    * @param world The camera world matrix
    * @param x/y The mouse click screen coordinates
    */
    void UpdatePicking(const Matrix& projection, const Matrix& world, int x, int y);

    /**
    * If a mesh was picked, call the associated picking function
    */
    void SolvePicking();

    /**
    * Set the picked mesh
    * @param mesh The chosen mesh
    * @param distance The distance to the chosen mesh
    */
    void SetPickedMesh(PickableMesh* mesh, float distance);

    /**
    * Locks the currently set mesh as picked for this tick
    * @param lock Whether to lock the mesh or not
    */
    void LockMesh(bool lock);

    /**
    * @return the ray origin
    */
    const D3DXVECTOR3& GetRayOrigin() const { return m_rayOrigin; }

    /**
    * @return the ray direction
    */
    const D3DXVECTOR3& GetRayDirection() const { return m_rayDirection; }

    /**
    * @return the pickable mesh
    */
    const PickableMesh* GetMesh() const { return m_mesh; }

    /**
    * @return the distance to the chosen mesh
    */
    float GetDistanceToMesh() const { return m_distanceToMesh; }

    /**
    * @return whether picking can occur this tick
    */
    bool IsLocked() const;

    /**
    * Casts a ray to the mesh to determine if the mouse is colliding with it
    * @param worldInverse The mesh world inverse matrix
    * @param mesh The mesh to test
    * @param distanceToMesh The distance to the collision
    * @return whether the ray hit the mesh or not
    */
    bool RayCastMesh(const D3DXMATRIX& worldInverse, LPD3DXMESH mesh, float& distanceToMesh);

private:

    bool m_locked;               ///< Stops picking from overwriting current mesh
    D3DXVECTOR3 m_rayOrigin;     ///< World coordinates origin of picking ray
    D3DXVECTOR3 m_rayDirection;  ///< Direction vector from origin
    PickableMesh* m_mesh;        ///< Raw pointer to mesh that was clicked
    float m_distanceToMesh;      ///< Distance from origin to the mesh clicked
    EnginePtr m_engine;          ///< Callbacks for the rendering engine
};
