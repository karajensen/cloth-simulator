////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - mesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "shader.h"
#include "picking.h"

class Collision;

/**
* DirectX renderable, pickable and collidable mesh class
*/
class Mesh : public Transform, public PickableMesh
{
public:

    /**
    * Data for rendering and instancing a mesh
    */
    struct MeshData
    {
        LPD3DXMESH mesh;                 ///< The directX mesh
        LPDIRECT3DTEXTURE9 texture;      ///< The texture attached to the mesh
        std::shared_ptr<Shader> shader;  ///< The shader attached to the mesh

        /**
        * Constructor
        */
        MeshData();

        /**
        * Destructor
        */
        ~MeshData();
    };

    /**
    * Constructor
    * @param callbacks Methods to call when rendering the mesh
    */
    explicit Mesh(const RenderCallbacks& callbacks);

    /**
    * Destructor
    */
    virtual ~Mesh();

    /**
    * Load the mesh
    * @param d3ddev The directX device
    * @param filename The path to the mesh
    * @param shader The shader attached to the mesh
    * @param index A user defined index
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, 
        std::shared_ptr<Shader> shader, int index = NO_INDEX);

    /**
    * Load the mesh as an instance of another mesh
    * @param d3ddev The directX device
    * @param collision The collision of the other mesh or null if none
    * @param data The meshdata from the other mesh
    * @param index A user defined index
    */
    bool LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, const Collision* collision, 
        std::shared_ptr<MeshData> data, int index = NO_INDEX);

    /**
    * Draw the visual model of the mesh
    * @param cameraPos the position of the camera in world coordinates
    * @param projection the projection matrix
    * @param view the view matrix
    */
    virtual void DrawMesh(const D3DXVECTOR3& cameraPos, 
        const Transform& projection, const Transform& view);

    /**
    * Draw the collision model of the mesh
    * @param projection the projection matrix
    * @param view the view matrix
    */
    virtual void DrawCollision(const Transform& projection, const Transform& view);

    /**
    * Tests whether mesh was clicked
    * @param input the picking input structure
    * @return whether this mesh was picked
    */
    virtual bool MousePickingTest(Picking& input);

    /**
    * Sets visibility of the mesh
    * @param visible whether the mesh is visible or not
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
    * @param d3ddev The directX device
    * @param width/height/depth The dimensions of the collision box
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

    /**
    * Creates a collision sphere for the mesh
    * @param d3ddev The directX device
    * @param radius The initial radius of the sphere
    * @param quality The detail of the collision sphere
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, int quality);

    /**
    * Creates a collision cylinder for the mesh
    * @param d3ddev The directX device
    * @param radius The initial radius of the cylinder
    * @param length The length of the cylinder.
    * @param quality The detail of the cylinder
    */
    void CreateCollision(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int quality);

    /**
    * @param draw whether the collision mesh is visible
    */
    virtual void SetCollisionVisibility(bool draw);

    /**
    * @param pickable whether the mesh is pickable or not
    */
    void SetPickable(bool pickable);

    /**
    * @return the user defined index for the mesh
    */
    int GetIndex() const;

    /**
    * @param selected Whether the mesh is selected or not
    */
    void SetSelected(bool selected);

    /**
    * Sets the mesh color
    * @param r/g/b The rgb color components from 0->1
    */
    void SetColor(float r, float g, float b);

    /**
    * Sets the mesh selected color
    * @param r/g/b The rgb color components from 0->1
    */
    void SetSelectedColor(float r, float g, float b);

    /**
    * Loads a texture for the mesh
    * @param d3ddev the directX device
    * @param filename the filename for the texture
    * @param dimensions the size of the texture
    * @return whether loading was successful
    */
    bool LoadTexture(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename, int dimensions);

    /**
    * @return whether the mesh has collision geometry attached to it
    */
    bool HasCollision() const;

protected:

    /**
    * Toggle whether this mesh is selected or not
    */
    void ToggleSelected();

    /**
    * Prevent copying
    */
    Mesh(const Mesh&);
    Mesh& operator=(const Mesh&);

    /**
    * Mesh vertex structure
    */
    struct Vertex
    {
        D3DXVECTOR3 position;   ///< Vertex position
        D3DXVECTOR3 normal;     ///< Vertex normal
        D3DXVECTOR2 uvs;        ///< Vertex UV information

        /**
        * Constructor
        */
        Vertex();
    };

    RenderCallbacks m_callbacks;             ///< Callbacks for rendering the mesh
    std::shared_ptr<Collision> m_collision;  ///< The collision geometry attached to the mesh
    std::shared_ptr<MeshData> m_data;        ///< Data for rendering/instancing the mesh
    D3DXVECTOR3 m_color;                     ///< Color for the mesh
    D3DXVECTOR3 m_selectedcolor;             ///< Color for the selected mesh
    D3DXVECTOR3 m_initialcolor;              ///< initial color for the mesh
    int m_index;                             ///< User defined index for the mesh
    bool m_pickable;                         ///< Whether the mesh can be mouse picked or not
    bool m_selected;                         ///< Whether the mesh is selected or not
    bool m_draw;                             ///< Whether the mesh is visible or not
};
