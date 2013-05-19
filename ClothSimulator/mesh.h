/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* DirectX mesh class
*****************************************************************/

#pragma once

#include "common.h"
#include "picking.h"

class Collision;
class Shader;

class Mesh : public Transform, public PickableMesh
{
public:

    Mesh();
    virtual ~Mesh();

    /**
    * Load the mesh
    * @param the directX device
    * @param the path to the mesh
    * @param the shader attached to the mesh
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, std::shared_ptr<Shader> shader);

    /**
    * Draw the visual model of the mesh
    * @param the position of the camera in world coordinates
    * @Param the projection matrix
    * @param the view matrix
    */
    virtual void DrawMesh(const D3DXVECTOR3& cameraPos, const Transform& projection, const Transform& view);

    /**
    * Draw the collision model of the mesh
    * @Param the projection matrix
    * @param the view matrix
    */
    virtual void DrawCollision(const Transform& projection, const Transform& view);

    /**
    * Tests whether mesh was clicked
    * @param the picking input structure
    */
    virtual void MousePickingTest(Picking& input);

    /**
    * Toggles visibility of the mesh
    */
    void ToggleVisibility();

    /**
    * @return the collison mesh
    */
    Collision* GetCollision();

    /**
    * Creates a collision cube for the mesh
    * @param the directX device
    * @param the dimensions of the collision box
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

    /**
    * Creates a collision sphere for the mesh
    * @param the directX device
    * @param the initial radius of the sphere. This will be multiplied with the mesh scale
    * @param the quality of the collision sphere. Higher is more polygons.
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality);

    /**
    * @param whether the collision mesh is visible
    */
    virtual void SetCollisionVisibility(bool draw);

protected:

    /**
    * Toggle whether this mesh is selected or not
    */
    void ToggleSelected();

    /**
    * Mesh vertex structure
    */
    struct Vertex
    {
        D3DXVECTOR3 position;
        D3DXVECTOR3 normal;
        D3DXVECTOR2 uvs;
    };

    bool m_selected;   ///< Whether the mesh is selected or not
    bool m_draw;       ///< Whether the mesh is visible or not
    LPD3DXMESH m_mesh; ///< The directX mesh

    LPDIRECT3DTEXTURE9 m_texture;           ///< The texture attached to the mesh
    std::shared_ptr<Shader> m_shader;       ///< The shader attached to the mesh
    std::shared_ptr<Collision> m_collision; ///< The collision geometry attached to the mesh
};
