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
    * @param a user defined index
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, 
        std::shared_ptr<Shader> shader, int index = NO_INDEX);

    /**
    * Load the mesh as an instance of another mesh
    * @param the directX device
    * @param the meshdata from the parent mesh
    * @param a user defined index
    */
    bool LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, 
        std::shared_ptr<MeshData> data, int index = NO_INDEX);

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
    * @param the initial radius of the sphere
    * @param the quality of the collision sphere
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality);

    /**
    * Creates a collision cylinder for the mesh
    * @param the directX device
    * @param the initial radius of the cylinder
    * @param the length of the cylinder.
    * @param the quality of the cylinder
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int quality);

    /**
    * @param whether the collision mesh is visible
    */
    virtual void SetCollisionVisibility(bool draw);

    /**
    * @param whether the mesh is pickable or not
    */
    void SetPickable(bool pickable);

    /**
    * @return the user defined index for the mesh
    */
    int GetIndex() const;

    /**
    * Sets whether the mesh is selected or not
    */
    void SetSelected(bool selected);

    /**
    * Sets the mesh color
    * @param the rgb color components from 0->1
    */
    void SetColor(float r, float g, float b);

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

    std::shared_ptr<MeshData> m_data;  ///< Data for the mesh
    D3DXVECTOR3 m_color; ///< Color for the mesh
    D3DXVECTOR3 m_initialcolor; ///< initial color for the mesh
    int m_index; ///< User defined index for the mesh
    bool m_pickable; ///< Whether the mesh can be mouse picked or not
    bool m_selected; ///< Whether the mesh is selected or not
    bool m_draw;  ///< Whether the mesh is visible or not
};
