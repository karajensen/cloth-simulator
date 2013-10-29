////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

class Shader;
class Partition;

/**
* Holds data for collision geometry
*/
class CollisionMesh
{
public:

    /**
    * Available shapes for collision geometry
    */
    enum Shape
    {
        NONE,
        BOX,
        SPHERE,
        CYLINDER
    };

    /**
    * Instantable Geometry for the collision
    */
    struct Geometry
    {
        /**
        * Constructor
        */
        Geometry();

        /**
        * Destructor
        */
        ~Geometry();

        Shape shape;     ///< Type of shape of the collision geometry
        LPD3DXMESH mesh; ///< Directx geometry mesh
    };

    /**
    * Shape data for the collision mesh
    */
    struct Data
    {
        /**
        * Constructor
        */
        Data();

        std::vector<D3DXVECTOR3> localBounds; ///< Local AABB points
        Transform localWorld;                 ///< Local transform
    };

    /**
    * Constructor
    * @param parent The transform of the mesh parent
    * @param engine Callbacks from the rendering engine
    * @param resolveFn A function to call to resolve any collision
    */
    CollisionMesh(const Transform& parent, EnginePtr engine, 
        std::function<void(const D3DXVECTOR3&)> resolveFn = nullptr);

    /**
    * Creates a sphere collision model
    * @param d3ddev The directX device
    * @param radius The initial radius of the sphere
    * @param divisions The amount of divisions of the mesh
    */
    void LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions);

    /**
    * Creates a box collision model
    * @param d3ddev The directX device
    * @param width The initial width of the box
    * @param height The initial height of the box
    * @param depth The initial depth of the box
    */
    void LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

    /**
    * Creates a cylinder collision model
    * @param d3ddev The directX device
    * @param radius The initial radius of the cylinder
    * @param length The length of the cylinder
    * @param divisions The amount of divisions of the mesh
    */
    void LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions);

    /**
    * Loads the collision as an instance of another
    * @param data The data to load
    * @param geometry Mesh instance
    */
    void LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry);

    /**
    * @return the shape the collision mesh has
    */
    Shape GetShape() const;

    /**
    * Sets the colour the collision mesh appears
    * @param color The colour to set in rgb from 0->1.0
    */
    void SetColor(const D3DXVECTOR3& color);

    /**
    * @return the center in world coordinates of the collision geometry
    */
    D3DXVECTOR3 GetPosition() const;

    /**
    * @return the radius of the sphere/cylinder
    */
    float GetRadius() const;

    /**
    * @return the minbounds of the box
    */
    const D3DXVECTOR3& GetMinBounds() const;

    /**
    * @return the minbounds of the box
    */
    const D3DXVECTOR3& GetMaxBounds() const;

    /**
    * @return the world matrix of the collision geometry
    */
    const Matrix& CollisionMatrix() const;

    /**
    * @return the geometry mesh
    */
    LPD3DXMESH GetMesh();
    
    /**
    * Draw the collision geometry. Assumes Update() has been called as needed
    * @param projection The projection matrix
    * @param view The view matrix
    * @param diagnostics whether to draw the collision diagnostics or not
    */
    void Draw(const Matrix& projection, const Matrix& view, bool diagnostics = true);

    /**
    * @param draw Set whether the collision mesh is drawn
    */
    void SetDraw(bool draw);

    /**
    * Set a function to call upon when the world transform updates
    * These functions should not change the transform they are observing
    * @param update The function to be called on full and partial updates
    */
    void SetObserver(Transform::UpdateFn update);

    /**
    * Updates the collision geometry upon scale/rotate/translate
    */
    void FullUpdate();

    /**
    * Updates the collision geometry upon translate
    */
    void PositionalUpdate();

    /**
    * @return the collision mesh 
    */
    std::shared_ptr<Geometry> GetGeometry() const;

    /**
    * @return the const geometry data
    */
    const Data& GetData() const;

    /**
    * @return the geometry data
    */
    Data& GetData();

    /**
    * Draw the collision geometry with a specific radius.
    * @param projection The projection matrix
    * @param view The view matrix
    * @param radius The radius to override
    */
    void DrawWithRadius(const Matrix& projection, 
        const Matrix& view, float radius);

    /**
    * @return whether the collision has geometry attached to it or not
    */
    bool HasGeometry() const;

    /**
    * @return the OABB for the collision geometry
    */
    const std::vector<D3DXVECTOR3>& GetOABB() const;

    /**
    * Sets the partition for the mesh
    */
    void SetPartition(Partition* partition);

    /**
    * @return the partition for the mesh
    */
    Partition* GetPartition() const;

    /**
    * Moves the owner of the collision mesh to resolve a collision
    * @param translate The amount to move the owner by
    * @param shape The interacting body causing the movement
    */
    void ResolveCollision(const D3DXVECTOR3& translation, Shape shape = NONE);

    /**
    * @return whether the collision mesh is dynamic or kinematic
    */
    bool IsDynamic() const;

    
    bool m_renderAsResolved;              ///< Whether to render the mesh as resolved this tick

private:

    /**
    * Prevent copying
    */
    CollisionMesh(const CollisionMesh&);
    CollisionMesh& operator=(const CollisionMesh&);

    /**
    * Creates the local points of the OABB
    * @param width/height/depth The dimensions of the geometry
    */
    void CreateLocalBounds(float width, float height, float depth);

    /**
    * Updates the partition the collision mesh exists in
    */
    void UpdatePartition();

    EnginePtr m_engine;                   ///< Callbacks for the rendering engine
    bool m_draw;                          ///< Whether to draw the geometry
    const Transform& m_parent;            ///< Parent transform of the collision geometry
    Transform m_world;                    ///< World transform of the collision geometry
    D3DXVECTOR3 m_colour;                 ///< Colour to render
    Data m_data;                          ///< Local data for the collision geometry
    float m_radius;                       ///< Transformed radius that encases geometry
    std::vector<D3DXVECTOR3> m_oabb;      ///< Bounds of the world coord OABB
    std::shared_ptr<Geometry> m_geometry; ///< collision geometry mesh shared accross instances
    LPD3DXEFFECT m_shader;                ///< Shader for the collision geometry
    Partition* m_partition;               ///< Partition collision currently in

    
    D3DXVECTOR3 m_resolvedColour;         ///< The color to render when the collision is resolved
    std::function<void(const D3DXVECTOR3&)> m_resolveFn; ///< Collision resolution function
};
