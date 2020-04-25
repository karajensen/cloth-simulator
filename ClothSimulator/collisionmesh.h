////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - collisionmesh.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "callbacks.h"
#include "geometry.h"

#include <deque>

class Shader;
class Partition;

/**
* Attaches to a parent mesh and supports partitioning and collision resolution
* If no parent is given, only translation is supported
*/
class CollisionMesh
{
public:

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    * @param parent The transform of the mesh parent
    * @note If no parent mesh is given, only translation is supported
    */
    CollisionMesh(EnginePtr engine, const Transform* parent = nullptr);

    /**
    * Destructor
    */
    virtual ~CollisionMesh(){}

    /**
    * Creates a collision model
    * @param createmesh Whether to create a mesh model or not
    * @param shape The shape of the collisin mesh
    * @param minScale Minimum allowed scale of the collision mesh
    * @param maxScale Maximum allowed scale of the collision mesh
    * @param divisions The amount of divisions of the mesh if required
    */
    void Initialise(bool createmesh, 
                    Geometry::Shape shape,
                    const D3DXVECTOR3& minScale, 
                    const D3DXVECTOR3& maxScale, 
                    int divisions = 0);

    /**
    * Creates a collision model
    * @param createmesh Whether to create a mesh model or not
    * @param shape The shape of the collisin mesh
    * @param scale Scale of the collision mesh
    * @param divisions The amount of divisions of the mesh if required
    */
    void Initialise(bool createmesh, 
                    Geometry::Shape shape,
                    const D3DXVECTOR3& scale, 
                    int divisions = 0);

    /**
    * Loads the collision as an instance of another
    * @param mesh The collision mesh to base the instance off
    */
    virtual void LoadInstance(const CollisionMesh& mesh);

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
    virtual void DrawMesh(const Matrix& projection, const Matrix& view);

    /**
    * @param draw Set whether the collision mesh is drawn
    */
    void SetDraw(bool draw);

    /**
    * Updates the collision geometry upon scale/rotate/translate
    * @note requires a parent for this update
    */
    void FullUpdate();

    /**
    * Updates the collision geometry upon translate
    * @note requires a parent for this update
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
    * Updates the partition and any cached values the require it
    */
    virtual void UpdateCollision();

    /**
    * @return the vertices of the mesh in world coordinates
    * @note will update the vertices only once per tick if called
    */
    const std::vector<D3DXVECTOR3>& GetVertices() const;

    /**
    * @return the velocity for the collision mesh
    */
    const D3DXVECTOR3& GetVelocity() const;

    /**
    * @return the velocity for the interacting collision meshes
    * @throw only updated for dynamic meshes
    */
    virtual const D3DXVECTOR3& GetInteractingVelocity() const;

    /**
    * @return whether the hull should render any collision solver diagnostics
    */
    bool RenderSolverDiagnostics() const;

    /**
    * @param render Whether the hull should render any collision solver diagnostics
    */
    void SetRenderSolverDiagnostics(bool render);

    /**
    * Sets the local scale of the mesh
    */
    void SetLocalScale(float scale);

    /**
    * @return the local scale of the mesh
    */
    D3DXVECTOR3 GetLocalScale() const;

    /**
    * @return whether the collision mesh has a shape or not
    */
    bool HasShape() const;

    /**
    * Explicitly set the position
    * @param position The position to set
    */
    void SetPosition(const D3DXVECTOR3& position);

    /**
    * Moves the owner of the collision mesh to resolve a collision
    * @param translation The amount to move the owner by
    * @throw will only work for dynamic collision meshes
    */
    virtual void ResolveCollision(const D3DXVECTOR3& translation);

    /**
    * Moves the owner of the collision mesh to resolve a collision
    * @param translation The amount to move the owner by
    * @param velocity The velocity of the colliding mesh
    * @param shape The interacting body causing the movement
    * @throw will only work for dynamic collision meshes
    */
    virtual void ResolveCollision(const D3DXVECTOR3& translation, 
                                  const D3DXVECTOR3& velocity, 
                                  Geometry::Shape shape);

    /**
    * @return whether the collision mesh is dynamic or kinematic
    */
    virtual bool IsDynamic() const;

protected:

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
    * Creates the local points of the OABB
    * @param width/height/depth The dimensions of the geometry
    */
    void CreateLocalBounds(float width, float height, float depth);

    /**
    * Creates a collision model
    * @param scale The scale to load the mesh at
    */
    void LoadCollisionModel(const D3DXVECTOR3& scale);

    /**
    * Determines the correct local scale depending on the parent scale
    */
    D3DXVECTOR3 FindLocalScale();

protected:

    EnginePtr m_engine;                        ///< Callbacks for the rendering engine
    const Transform* m_parent;                 ///< Parent transform of the collision geometry
    Transform m_localWorld;                    ///< Local World transform of the collision geometry
    Transform m_world;                         ///< World transform of the collision geometry
    Partition* m_partition;                    ///< Partition collision currently in
    D3DXVECTOR3 m_positionDelta;               ///< Change in position this tick
    D3DXVECTOR3 m_velocity;                    ///< Velocity for the collision mesh
    D3DXVECTOR3 m_colour;                      ///< Colour to render
    D3DXVECTOR3 m_position;                    ///< Cached position of collision geometry
    std::vector<D3DXVECTOR3> m_localBounds;    ///< Local AABB points
    std::vector<D3DXVECTOR3> m_oabb;           ///< Bounds of the world coord OABB
    std::vector<D3DXVECTOR3> m_worldVertices;  ///< Transformed vertices of the mesh
    std::shared_ptr<Geometry> m_geometry;      ///< collision geometry mesh shared accross instances
    bool m_draw;                               ///< Whether to draw the geometry
    bool m_requiresFullUpdate;                 ///< Whether the collision mesh requires a full update
    bool m_requiresPositionalUpdate;           ///< Whether the collision mesh requires a positional update
    bool m_renderSolverDiagnostics;            ///< Whether to render any collision solver diagnostics
    float m_radius;                            ///< Transformed radius that encases geometry
    D3DXVECTOR3 m_minLocalScale;               ///< Minimum allowed scale of the collision mesh
    D3DXVECTOR3 m_maxLocalScale;               ///< Maximum allowed scale of the collision mesh
};                                             
