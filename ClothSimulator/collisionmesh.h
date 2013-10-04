////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - CollisionMesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

class Shader;

/**
* Basic CollisionMesh geometry class
*/
class CollisionMesh
{
public:

    /**
    * Available shapes for CollisionMesh geometry
    */
    enum Shape
    {
        NONE,
        BOX,
        SPHERE,
        CYLINDER
    };

    /**
    * Instantable Geometry for the CollisionMesh
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

        Shape shape;     ///< Type of shape of the CollisionMesh geometry
        LPD3DXMESH mesh; ///< CollisionMesh geometry mesh
    };

    /**
    * Shape data for the CollisionMesh
    */
    struct Data
    {
        /**
        * Constructor
        */
        Data();

        D3DXVECTOR3 localMinBounds; ///< Untransformed min bounds for box geometry
        D3DXVECTOR3 localMaxBounds; ///< Untransformed max bounds for box geometry
        D3DXVECTOR3 minBounds;      ///< Transformed world min bounds for box geometry
        D3DXVECTOR3 maxBounds;      ///< Transformed world max bounds for box geometry
        Transform localWorld;       ///< Local transform before any calculations of the geometry
        float radius;               ///< Transformed radius for all geometry
    };

    /**
    * Constructor
    * @param parent The transform of the mesh parent
    * @param engine Callbacks from the rendering engine
    */
    CollisionMesh(const Transform& parent, EnginePtr engine);

    /**
    * Creates a sphere CollisionMesh model
    * @param d3ddev The directX device
    * @param radius The initial radius of the sphere
    * @param divisions The amount of divisions of the mesh
    */
    void LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions);

    /**
    * Creates a box CollisionMesh model
    * @param d3ddev The directX device
    * @param width The initial width of the box
    * @param height The initial height of the box
    * @param depth The initial depth of the box
    */
    void LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

    /**
    * Creates a cylinder CollisionMesh model
    * @param d3ddev The directX device
    * @param radius The initial radius of the cylinder
    * @param length The length of the cylinder
    * @param divisions The amount of divisions of the mesh
    */
    void LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions);

    /**
    * Loads the CollisionMesh as an instance of another
    * @param data The data to load
    * @param geometry Mesh instance
    */
    void LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry);

    /**
    * @return the shape the CollisionMesh mesh has
    */
    Shape GetShape() const;

    /**
    * Sets the colour the CollisionMesh mesh appears
    * @param color The colour to set in rgb from 0->1.0
    */
    void SetColor(const D3DXVECTOR3& color);

    /**
    * @return the center in world coordinates of the CollisionMesh geometry
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
    * @return the world matrix of the CollisionMesh geometry
    */
    const Matrix& CollisionMeshMatrix() const;

    /**
    * @return the CollisionMesh geometry mesh
    */
    LPD3DXMESH GetMesh();
    
    /**
    * Draw the CollisionMesh geometry. Assumes Update() has been called as needed
    * @param projection The projection matrix
    * @param view The view matrix
    * @param diagnostics whether to draw the CollisionMesh diagnostics or not
    */
    void Draw(const Matrix& projection, const Matrix& view, bool diagnostics = true);

    /**
    * @param draw Set whether the CollisionMesh mesh is drawn
    */
    void SetDraw(bool draw);

    /**
    * Set a function to call upon when the world transform updates
    * These functions should not change the transform they are observing
    * @param update The function to be called on full and partial updates
    */
    void SetObserver(Transform::UpdateFn update);

    /**
    * Updates the CollisionMesh geometry upon scale/rotate/translate
    */
    void FullUpdate();

    /**
    * Updates the CollisionMesh geometry upon translate
    */
    void PositionalUpdate();

    /**
    * @return the CollisionMesh mesh 
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
    * Draw the CollisionMesh geometry with a specific radius.
    * @param projection The projection matrix
    * @param view The view matrix
    * @param radius The radius to override
    */
    void DrawWithRadius(const Matrix& projection, 
        const Matrix& view, float radius);

    /**
    * @return whether the CollisionMesh has geometry attached to it or not
    */
    bool HasGeometry() const;

private:

    /**
    * Prevent copying
    */
    CollisionMesh(const CollisionMesh&);
    CollisionMesh& operator=(const CollisionMesh&);

    EnginePtr m_engine;         ///< Callbacks for the rendering engine
    bool m_draw;                ///< Whether to draw the geometry
    const Transform& m_parent;  ///< Parent transform of the CollisionMesh geometry
    Transform m_world;          ///< World transform of the CollisionMesh geometry
    D3DXVECTOR3 m_colour;       ///< Colour to render
    Data m_data;                ///< Data for the CollisionMesh geometry

    std::shared_ptr<Geometry> m_geometry; ///< CollisionMesh geometry mesh shared accross instances
    std::shared_ptr<Shader> m_shader;     ///< Shader for the CollisionMesh geometry
};
