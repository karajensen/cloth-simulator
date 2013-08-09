/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Basic collision geometry class
*****************************************************************/

#pragma once

#include "common.h"
class Shader;

class Collision
{
public:

    enum Shape
    {
        NONE,
        BOX,
        SPHERE,
        CYLINDER
    };

    struct Geometry
    {
        Geometry();
        ~Geometry();

        Shape shape;     ///< Type of shape of the collision geometry
        LPD3DXMESH mesh; ///< Collision geometry mesh
    };

    struct Data
    {
        Data();
        D3DXVECTOR3 localMinBounds; ///< Untransformed min bounds for box geometry
        D3DXVECTOR3 localMaxBounds; ///< Untransformed max bounds for cube geometry
        D3DXVECTOR3 minBounds;      ///< Transformed world min bounds of the cube
        D3DXVECTOR3 maxBounds;      ///< Transformed world max bounds of the cube
        Transform localWorld;       ///< Local transform before any calculations of the geometry
    };

    /**
    * Constructor
    * @param the transform of the mesh parent
    */
    Collision(const Transform& parent);

    /**
    * Creates a sphere collision model
    * @param the directX device
    * @param the initial radius of the sphere
    * @param the amount of divisions of the mesh
    */
    void LoadSphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions);

    /**
    * Creates a box collision model
    * @param the directX device
    * @param the initial width of the box
    * @param the initial height of the box
    * @param the initial depth of the box
    * @param whether to load the box as an instance
    */
    void LoadBox(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

    /**
    * Creates a cylinder collision model
    * @param the directX device
    * @param the initial radius of the cylinder
    * @param the length of the cylinder
    * @param the amount of divisions of the mesh
    */
    void LoadCylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions);

    /**
    * Loads the collision as an instance of another
    * @param the data to load
    * @param geometry mesh instance
    */
    void LoadInstance(const Data& data, std::shared_ptr<Geometry> geometry);

    /**
    * @return the shape the collision mesh has
    */
    Shape GetShape() const;

    /**
    * Sets the colour the collision mesh appears
    * @param the colour to set in rgb from 0->1.0
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
    * @param the projection matrix
    * @param the view matrix
    */
    void Draw(const Transform& projection, const Transform& view);

    /**
    * Set visiblity of the collision geometry
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
    * @return the geometry data
    */
    const Data& GetData() const;
    Data& GetData();

    /**
    * Draw the collision geometry with a specific radius.
    * @param the projection matrix
    * @param the view matrix
    * @param the radius to override
    */
    void DrawWithRadius(const Transform& projection, 
        const Transform& view, float radius);

    /**
    * @return whether the collision has geometry attached to it or not
    */
    bool HasGeometry() const;

    /**
    * Initialise the use of collisions 
    * @param the shader to apply to collision meshes
    */
    static void Initialise(std::shared_ptr<Shader> boundsShader);

private:

    bool m_draw;                ///< Whether to draw the geometry
    const Transform& m_parent;  ///< Parent transform of the collision geometry
    Transform m_world;          ///< World transform of the collision geometry
    D3DXVECTOR3 m_colour;       ///< Colour to render
    Data m_data;                ///< Data for the collision geometry

    std::shared_ptr<Geometry> m_geometry; ///< Collision geometry mesh shared accross instances
    static std::shared_ptr<Shader> sm_shader; ///< Shared shader of all collision geometry
};
