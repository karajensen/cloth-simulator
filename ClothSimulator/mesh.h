////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - mesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "pickablemesh.h"
#include "callbacks.h"

class Picking;
class IOctree;
class CollisionMesh;
class Partition;

/**
* DirectX renderable, pickable and collidable mesh class
*/
class Mesh : public Transform, public PickableMesh
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    */
    explicit Mesh(EnginePtr engine);

    /**
    * Destructor
    */
    ~Mesh();

    /**
    * Load the mesh
    * @param d3ddev The directX device
    * @param filename The path to the mesh
    * @param shader The shader attached to the mesh
    * @param index A user defined index
    */
    void LoadMesh(LPDIRECT3DDEVICE9 d3ddev, 
                  const std::string& filename, 
                  LPD3DXEFFECT shader, 
                  int index = NO_INDEX);

    /**
    * Load the mesh as an instance of another mesh
    * @param d3ddev The directX device
    * @param mesh The mesh of the object to instance off
    * @param index A user defined index
    */
    bool LoadAsInstance(LPDIRECT3DDEVICE9 d3ddev, 
                        Mesh& mesh, 
                        int index = NO_INDEX);

    /**
    * Draw the visual model of the mesh
    * @param cameraPos the position of the camera in world coordinates
    * @param projection the projection matrix
    * @param view the view matrix
    */
    void DrawMesh(const D3DXVECTOR3& cameraPos,
                  const Matrix& projection, 
                  const Matrix& view);

    /**
    * Draw the collision model of the mesh
    * @param projection the projection matrix
    * @param view the view matrix
    */
    void DrawCollisionMesh(const Matrix& projection, const Matrix& view);

    /**
    * Renders diagnostics for the mesh
    */
    void DrawDiagnostics();

    /**
    * Updates the mesh collision
    */
    void UpdateCollision();

    /**
    * Tests whether mesh was clicked
    * @param input the picking input structure
    * @return whether this mesh was picked
    */
    bool MousePickingTest(Picking& input);

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
    std::shared_ptr<Geometry> Mesh::GetGeometry();

    /**
    * @return the collison mesh
    */
    CollisionMesh& GetCollisionMesh();

    /**
    * Creates a collision model
    * @param shape The shape of the collisin mesh
    * @param minScale Minimum allowed scale of the collision mesh
    * @param maxScale Maximum allowed scale of the collision mesh
    * @param divisions The amount of divisions of the mesh if required
    */
    void InitialiseCollision(Geometry::Shape shape,
                             const D3DXVECTOR3& minScale, 
                             const D3DXVECTOR3& maxScale, 
                             int divisions = 0);

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
    * @param filename the filename for the texture
    * @param dimensions the size of the texture
    * @param miplevels the number of mipmap levels to generate
    */
    void LoadTexture(const std::string& filename, int dimensions, int miplevels);

    /**
    * @return whether the mesh has collision geometry attached to it
    */
    bool HasCollisionMesh() const;

    /**
    * Resets the animation
    */
    void ResetAnimation();

    /**
    * Saves the mesh current position into the list of animation points
    */
    void SavePosition();

    /**
    * @return the list of saved animation points
    */
    const std::vector<D3DXVECTOR3>& GetAnimationPoints() const;

    /**
    * Animates the mesh through the list of animation points
    * @param deltatime The time passed since last frame in seconds
    */
    void Animate(float deltatime);

private:

    /**
    * Toggle whether this mesh is selected or not
    */
    void ToggleSelected();

    /**
    * Initialises the collison geometry
    */
    void InitializeCollision();

    /**
    * Prevent copying
    */
    Mesh(const Mesh&);
    Mesh& operator=(const Mesh&);

private:

    EnginePtr m_engine;                          ///< Callbacks for the rendering engine
    std::shared_ptr<CollisionMesh> m_collision;  ///< The collision geometry attached to the mesh
    std::shared_ptr<Geometry> m_geometry;        ///< Data for rendering/instancing the mesh
    D3DXVECTOR3 m_color;                         ///< Color for the mesh
    D3DXVECTOR3 m_selectedcolor;                 ///< Color for the selected mesh
    D3DXVECTOR3 m_initialcolor;                  ///< initial color for the mesh
    std::vector<D3DXVECTOR3> m_animation;        ///< Animation points for the mesh
    int m_index = NO_INDEX;                      ///< User defined index for the mesh
    bool m_pickable = false;                     ///< Whether the mesh can be mouse picked or not
    bool m_selected = false;                     ///< Whether the mesh is selected or not
    bool m_draw = false;                         ///< Whether the mesh is visible or not
    int m_target = NO_INDEX;                     ///< Animation index target
    bool m_animating = false;                    ///< Whether the mesh is animating or not
    bool m_reversing = false;                    ///< Whether animating in reverse or not
    float m_speed = 0.0f;                        ///< The speed the mesh will animate
};
