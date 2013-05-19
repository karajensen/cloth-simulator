/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Picking mesh and structures
*****************************************************************/
#pragma once

#include "common.h"
#include <functional>

/**
* Base class for any mesh that is able to be picked
*/
class PickableMesh
{
public:

    typedef std::function<void(void)> MeshPickFn;

    PickableMesh();
    virtual ~PickableMesh();

    /**
    * Set mesh pick function
    * @param the function to set
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

    Picking();

    /**
    * Send a ray into the scene to determine if mouse clicked an object
    * @param the camera projection matrix
    * @param the camera view matrix
    * @param the mouse click screen coordinates
    */
    void UpdatePicking(Transform& projection, Transform& view, int x, int y);

    /**
    * If a mesh was picked, call the associated picking function
    */
    void SolvePicking();

    /**
    * Set the picked mesh
    * @param the chosen mesh
    * @param the distance to the chosen mesh
    */
    void SetPickedMesh(PickableMesh* mesh, float distance);

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

private:

	D3DXVECTOR3 m_rayOrigin;     ///< World coordinates origin of picking ray
    D3DXVECTOR3 m_rayDirection;  ///< Direction vector from origin
	PickableMesh* m_mesh;        ///< Raw pointer to mesh that was clicked
	float m_distanceToMesh;      ///< Distance from origin to the mesh clicked
};
