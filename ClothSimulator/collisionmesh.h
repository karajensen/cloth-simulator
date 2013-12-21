////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"
#include "geometry.h"

class Shader;
class Partition;

/**
* Attaches to a parent mesh and supports partitioning and collision resolution
*/
class CollisionMesh
{
public:

    /**
    * Constructor
    * @param parent The transform of the mesh parent
    * @param engine Callbacks from the rendering engine
    */
    CollisionMesh(const Transform& parent, EnginePtr engine);

    /**
    * Function signature for resolving a collision
    */
    typedef std::function<void(const D3DXVECTOR3&)> MotionFn;

    /**
    * Makes the collision mesh dynamic
    * @param resolveFn A function to call to resolve any collision
    */
    void MakeDynamic(MotionFn resolveFn);

    /**
    * Creates a sphere collision model
    * @param createmesh Whether to create a mesh model or not
    * @param radius The initial radius of the sphere
    * @param divisions The amount of divisions of the mesh
    */
    void LoadSphere(bool createmesh, float radius, int divisions);

    /**
    * Creates a box collision model
    * @param createmesh Whether to create a mesh model or not
    * @param width The initial width of the box
    * @param height The initial height of the box
    * @param depth The initial depth of the box
    */
    void LoadBox(bool createmesh, float width, float height, float depth);

    /**
    * Creates a cylinder collision model
    * @param createmesh Whether to create a mesh model or not
    * @param radius The initial radius of the cylinder
    * @param length The length of the cylinder
    * @param divisions The amount of divisions of the mesh
    */
    void LoadCylinder(bool createmesh, float radius, float length, int divisions);

    /**
    * Loads the collision as an instance of another
    * @param scale The local scale of the new instance
    * @param geometry Mesh instance
    */
    void LoadInstance(const D3DXVECTOR3& scale, std::shared_ptr<Geometry> geometry);

    /**
    * @return the shape the collision mesh has
    */
    Geometry::Shape GetShape() const;

    /**
    * Sets the colour the collision mesh appears
    * @param color The colour to set in rgb from 0->1.0
    */
    void SetColor(const D3DXVECTOR3& color);

    /**
    * @return the center in world coordinates of the collision geometry
    */
    const D3DXVECTOR3& GetPosition() const;

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
    * Draw the collision geometry and diagnostics
    */
    void DrawDiagnostics();

    /**
    * Draw the collision geometry and diagnostics
    * @param projection The projection matrix
    * @param view The view matrix
    */
    void DrawMesh(const Matrix& projection, const Matrix& view);

    /**
    * Draw the collision geometry with a specific visual representation
    * @param projection The projection matrix
    * @param view The view matrix
    * @param radius The radius to override
    * @param color The color to render with
    * @param position The position to render at
    */
    void DrawRepresentation(const Matrix& projection, const Matrix& view, 
        float radius, const D3DXVECTOR3& color, const D3DXVECTOR3& position);

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
    * @param translation The amount to move the owner by
    * @param shape The interacting body causing the movement
    * @note will only work for dynamic collision meshes
    */
    void ResolveCollision(const D3DXVECTOR3& translation, 
        Geometry::Shape shape = Geometry::NONE);

    /**
    * @return whether the collision mesh is dynamic or kinematic
    */
    bool IsDynamic() const;

    /**
    * Updates the partition and any cached values the require it
    */
    void UpdateCollision();

    /**
    * @return the vertices of the mesh in world coordinates
    * @note will update the vertices only once per tick if called
    */
    const std::vector<D3DXVECTOR3>& GetVertices() const;

    /**
    * @param shape The shape to query for interaction
    * @return whether the mesh is colliding with the given shape
    */
    bool IsCollidingWith(Geometry::Shape shape);

    /**
    * @return the velocity for the collision mesh
    */
    const D3DXVECTOR3& GetVelocity() const;

    /**
    * @return whether the hull should render any collision solver diagnostics
    */
    bool RenderCollisionDiagnostics() const;

    /**
    * @param render Whether the hull should render any collision solver diagnostics
    */
    void SetRenderCollisionDiagnostics(bool render);

    /**
    * @return the local scale of the mesh
    */
    D3DXVECTOR3 GetLocalScale() const;

    /**
    * Sets the local scale of the mesh
    */
    void SetLocalScale(float scale);

    /**
    * @return whether the collision mesh has a shape or not
    */
    bool HasShape() const;

private:

    /**
    * Prevent copying
    */
    CollisionMesh(const CollisionMesh&);
    CollisionMesh& operator=(const CollisionMesh&);

    /**
    * Draw the collision geometry and diagnostics
    * @param projection The projection matrix
    * @param view The view matrix
    * @param color The color to render the mesh in
    */
    void DrawMesh(const Matrix& projection, const Matrix& view, const D3DXVECTOR3& color);

    /**
    * @param shape The shape to find the collision type for
    * @return the type of collision from the given shape
    */
    unsigned int GetCollisionType(Geometry::Shape shape);

    /**
    * Creates the local points of the OABB
    * @param width/height/depth The dimensions of the geometry
    */
    void CreateLocalBounds(float width, float height, float depth);

    EnginePtr m_engine;                        ///< Callbacks for the rendering engine
    const Transform& m_parent;                 ///< Parent transform of the collision geometry
    Transform m_localWorld;                    ///< Local World transform of the collision geometry
    Transform m_world;                         ///< World transform of the collision geometry
    Partition* m_partition;                    ///< Partition collision currently in
    D3DXVECTOR3 m_positionDelta;               ///< Change in position this tick
    D3DXVECTOR3 m_colour;                      ///< Colour to render
    D3DXVECTOR3 m_position;                    ///< Cached position of collision geometry
    std::vector<D3DXVECTOR3> m_localBounds;    ///< Local AABB points
    std::vector<D3DXVECTOR3> m_oabb;           ///< Bounds of the world coord OABB
    std::vector<D3DXVECTOR3> m_worldVertices;  ///< Transformed vertices of the mesh
    std::shared_ptr<Geometry> m_geometry;      ///< collision geometry mesh shared accross instances
    MotionFn m_resolveFn;                      ///< Translate the collision in response to a collision
    unsigned int m_interactingGeometry;        ///< Other collisin geometry shapes that are interacting 
    bool m_draw;                               ///< Whether to draw the geometry
    bool m_requiresFullUpdate;                 ///< Whether the collision mesh requires a full update
    bool m_requiresPositionalUpdate;           ///< Whether the collision mesh requires a positional update
    bool m_renderCollisionDiagnostics;         ///< Whether to render any collision solver diagnostics
    float m_radius;                            ///< Transformed radius that encases geometry
};                                             
