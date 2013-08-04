/****************************************************************
* Kara Jensen (mail@karajensen.com) 
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
    * Data for rendering a mesh
    */
    struct MeshData
    {
        LPD3DXMESH mesh;                      ///< The directX mesh
        LPDIRECT3DTEXTURE9 texture;           ///< The texture attached to the mesh
        std::shared_ptr<Shader> shader;       ///< The shader attached to the mesh
        std::shared_ptr<Collision> collision; ///< The collision geometry attached to the mesh

        MeshData();
        ~MeshData();
    };

    /**
    * Load the mesh
    * @param the directX device
    * @param the path to the mesh
    * @param the shader attached to the mesh
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, std::shared_ptr<Shader> shader);

    /**
    * Load the mesh as an instance of another mesh
    * @param the directX device
    * @param the meshdata from the parent mesh
    */
    bool LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, std::shared_ptr<MeshData> data);

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
    * Sets visibility of the mesh
    */
    void SetVisible(bool visible);

    /**
    * @return whether the mesh is visible
    */
    bool IsVisible() const;

    /**
    * @return the mesh data
    */
    std::shared_ptr<MeshData> GetData();

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

    /**
    * @param whether the mesh is pickable or not
    */
    void SetPickable(bool pickable);

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
        Vertex();
    };

    std::shared_ptr<MeshData> m_data;   ///< Data for the mesh

    bool m_pickable;   ///< Whether the mesh can be mouse picked or not
    bool m_selected;   ///< Whether the mesh is selected or not
    bool m_draw;       ///< Whether the mesh is visible or not
};
