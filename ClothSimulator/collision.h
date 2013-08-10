////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

class Shader;

/**
* Basic collision geometry class
*/
class Collision
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
        LPD3DXMESH mesh; ///< Collision geometry mesh
    };

    /**
    * Shape data for the collision
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
    };

    /**
    * Constructor
    * @param parent The transform of the mesh parent
    * @param boundsShader The shader for the collision mesh
    */
    Collision(const Transform& parent, std::shared_ptr<Shader> boundsShader);

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
    * @return the length of the cylinder
    */
    float GetLength() const;

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
    const Transform& GetTransform() const;

    /**
    * @return the collision geometry mesh
    */
    LPD3DXMESH GetMesh();
    
    /**
    * Draw the collision geometry. Assumes Update() has been called as needed
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void Draw(const Transform& projection, const Transform& view);

    /**
    * @param draw Set whether the collision mesh is drawn
    */
    void SetDraw(bool draw);

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
    void DrawWithRadius(const Transform& projection, 
        const Transform& view, float radius);

    /**
    * @return whether the collision has geometry attached to it or not
    */
    bool HasGeometry() const;

private:

    /**
    * Prevent copying
    */
    Collision(const Collision&);
    Collision& operator=(const Collision&);

    bool m_draw;                ///< Whether to draw the geometry
    const Transform& m_parent;  ///< Parent transform of the collision geometry
    Transform m_world;          ///< World transform of the collision geometry
    D3DXVECTOR3 m_colour;       ///< Colour to render
    Data m_data;                ///< Data for the collision geometry

    std::shared_ptr<Geometry> m_geometry; ///< Collision geometry mesh shared accross instances
    std::shared_ptr<Shader> m_shader;     ///< Shader for the collision geometry
};
