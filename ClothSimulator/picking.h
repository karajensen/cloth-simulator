////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - picking.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "callbacks.h"
#include "geometry.h"

class PickableMesh;

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
    * Locks the currently set mesh as picked for this tick
    * @param lock Whether to lock the mesh or not
    */
    void LockMesh(bool lock);

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
    * @param mesh The calling mesh
    * @param world The mesh world matrix
    * @param geometry The mesh to test
    * @return whether the mesh was selected or not
    */
    bool RayCastMesh(PickableMesh* mesh,
                     const D3DXMATRIX& world, 
                     const Geometry& geometry);

    /**
    * Updates mouse picking diagnostics
    */
    void UpdateDiagnostics();

    /**
    * Casts a ray to a sphere to determine if the mouse is colliding with it
    * @param center The center of the sphere
    * @param radius The radius of the sphere
    * @return whether the ray hit the sphere or not
    */
    bool RayCastSphere(const D3DXVECTOR3& center, float radius);

private:

    /**
    * Barycentric coordinates for the picked triangle
    */
    struct BarycentricCoords
    {
        /**
        * constructor
        */
        BarycentricCoords();

        float s; ///< S coordinate on the triangle
        float t; ///< T coordinate on the triangle
    };

private:

    const D3DXMATRIX* m_pickWorld;  ///< World matrix for picked mesh
    const MeshFace* m_pickFace;     ///< Picked triangle face;
    BarycentricCoords m_pickCoords; ///< Barycentric coordinates for the picked triangle
    bool m_locked;                  ///< Stops picking from overwriting current mesh
    D3DXVECTOR3 m_rayOrigin;        ///< World coordinates origin of picking ray
    D3DXVECTOR3 m_rayDirection;     ///< Direction vector from origin
    PickableMesh* m_mesh;           ///< Raw pointer to mesh that was clicked
    float m_distanceToMesh;         ///< Distance from origin to the mesh clicked
    EnginePtr m_engine;             ///< Callbacks for the rendering engine
};
