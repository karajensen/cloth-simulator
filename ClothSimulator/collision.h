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

    /**
    * Geometry base
    */
    struct Geometry
    {
        /**
        * Destructor/constructor
        */
        Geometry();
        virtual ~Geometry();

        LPD3DXMESH mesh; ///< Collision geometry mesh
    };

    /**
    * Geometry for a sphere collsion model
    */
    struct Sphere : public Geometry
    {        
        /**
        * Creates a sphere collision model
        * @param the directX device
        * @param the initial radius of the sphere
        * @param the amount of divisions of the mesh
        */
        Sphere(LPDIRECT3DDEVICE9 d3ddev, float radius, int divisions);

        float radius;      ///< sphere radius with scaling applied
        float localRadius; ///< initial sphere radius
    };

    /**
    * Geometry for a cylinder collsion model
    */
    struct Cylinder : public Geometry
    {        
        /**
        * Creates a cylinder collision model
        * @param the directX device
        * @param the initial radius of the cylinder
        * @param the length of the cylinder
        * @param the amount of divisions of the mesh
        */
        Cylinder(LPDIRECT3DDEVICE9 d3ddev, float radius, float length, int divisions);
    };

    /**
    * Geometry for a box collsion model
    */
    struct Box : public Geometry
    {
        /**
        * Creates a cube collision model
        * @param the directX device
        * @param the initial width of the cube
        * @param the initial height of the cube
        * @param the initial depth of the cube
        */
        Box(LPDIRECT3DDEVICE9 d3ddev, float width, float height, float depth);

        D3DXVECTOR3 localMinBounds; ///< Untransformed min bounds of the cube
        D3DXVECTOR3 localMaxBounds; ///< Untransformed max bounds of the cube
        D3DXVECTOR3 minBounds;      ///< Transformed world min bounds of the cube
        D3DXVECTOR3 maxBounds;      ///< Transformed world max bounds of the cube
        D3DXVECTOR3 dimensions;     ///< The width, height and depth of the cube
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
    * Creates a cube collision model
    * @param the directX device
    * @param the initial width of the cube
    * @param the initial height of the cube
    * @param the initial depth of the cube
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
    * @param shape of the geometry
    * @param the data to load
    */
    void LoadInstance(Shape shape, std::shared_ptr<Geometry> data);

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
    const D3DXVECTOR3& GetPosition() const;

    /**
    * @return the world matrix of the collision geometry
    */
    const Transform& GetTransform() const;

    /**
    * @return the collision geometry mesh
    */
    LPD3DXMESH GetMesh();
    
    /**
    * Test the collision geometry against another collision geometry
    * @param the geometry to test against
    * @return whether collision occured
    */
    bool TestCollision(const Collision* obj) const;

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
    * @return the collision data 
    */
    std::shared_ptr<Geometry> GetData();

    /**
    * @return the data as a specific shape
    */
    Box& GetBoxData();
    Sphere& GetSphereData();
    Cylinder& GetCylinderData();

    /**
    * @return the const data as a specific shape
    */
    const Box& GetBoxData() const;
    const Sphere& GetSphereData() const;
    const Cylinder& GetCylinderData() const;

    /**
    * Draw the collision geometry with a specific radius.
    * @param the projection matrix
    * @param the view matrix
    * @param the radius to override
    */
    void DrawWithRadius(const Transform& projection, 
        const Transform& view, float radius);

    /**
    * Initialise the use of collisions 
    * @param the shader to apply to collision meshes
    */
    static void Initialise(std::shared_ptr<Shader> boundsShader);

private:

    /**
    * Sets the collision as a cylinder
    * @param the data for the cylinder
    */
    void MakeCylinder(Cylinder* cylinderdata);

    /**
    * Sets the collision as a sphere
    * @param the data for the sphere
    */
    void MakeSphere(Sphere* spheredata);

    /**
    * Sets the collision as a box
    * @param the data for the box
    */
    void MakeBox(Box* boxdata);

    Shape m_shape;              ///< Type of shape of the collision geometry
    bool m_draw;                ///< Whether to draw the geometry
    const Transform& m_parent;  ///< Parent transform of the collision geometry
    Transform m_world;          ///< World transform of the collision geometry
    Transform m_localWorld;     ///< Local transform before any calculations of the geometry
    D3DXVECTOR3 m_position;     ///< Position in world coordinates of the collision geometry
    D3DXVECTOR3 m_colour;       ///< Colour to render

    std::shared_ptr<Geometry> m_data;         ///< Data for the collision geometry
    static std::shared_ptr<Shader> sm_shader; ///< Shared shader of all collision geometry
};

class CollisionTester
{
public:

    /**
    * Tests Cube-Sphere collision
    * @param the cube/sphere to test
    * @return whether collision is occurring
    */
    static bool TestBoxSphereCollision(const Collision* box, const Collision* sphere);

    /**
    * Tests Sphere-Sphere collision
    * @param the sphere/sphere to test
    * @return whether collision is occurring
    */
    static bool TestSphereSphereCollision(const Collision* sphere1, const Collision* sphere2);

    /**
    * Tests Cube-Cube collision
    * @param the cube/cube to test
    * @return whether collision is occurring
    */
    static bool TestBoxBoxCollision(const Collision* box1, const Collision* box2);
};
