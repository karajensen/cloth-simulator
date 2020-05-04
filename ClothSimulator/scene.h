////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - scene.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "callbacks.h"

#include <queue>

class IOctree;
class Shader;
class CollisionSolver;
class CollisionMesh;
class Mesh;
class Picking;
class Manipulator;
class Input;

/**
* Factory/Manager that creates and renders all objects in the scene
*/
class Scene
{
public:

    /**
    * Available objects to create
    */
    enum Object
    {
        BOX,
        SPHERE,
        CYLINDER,
        MAX_OBJECT
    };

    /**
    * Constructor
    * @param engine Callbacks from the rendering engine
    * @param solver The solver for collision resolution
    */
    Scene(EnginePtr engine, std::shared_ptr<CollisionSolver> solver);
     
    /**
    * Destructor
    */
    ~Scene();

    /**
    * Draws all scene meshes
    * @param position The camera position
    * @param projection The camera projection matrix
    * @param view The camera view matrix
    */
    void Draw(const D3DXVECTOR3& position,
              const Matrix& projection, 
              const Matrix& view);

    /**
    * Draws the scene manipulator tool
    * @param position The camera position
    * @param projection The camera projection matrix
    * @param view The camera view matrix
    */
    void DrawTools(const D3DXVECTOR3& position,
                   const Matrix& projection, 
                   const Matrix& view);

    /**
    * Draws all scene mesh collisions
    * @param projection The camera projection matrix
    * @param view The camera view matrix
    */
    void DrawCollisions(const Matrix& projection, const Matrix& view);

    /**
    * Removes the currently selected object if possible
    */
    void RemoveObject();

    /**
    * Removes all objects in the scene
    */
    void RemoveScene();

    /**
    * Adds an object to the scene
    * @param object the object to add
    */
    void AddObject(Object object);

    /**
    * Tests all scene objects for mouse picking
    * @param picking The mouse picking object
    */
    void ScenePickingTest(Picking& picking);

    /**
    * Tests all manipulator axis for mouse picking
    * @param picking The mouse picking object
    */
    void ManipulatorPickingTest(Picking& picking);

    /**
    * Updates the state of the scene pre collision solving
    * @param pressed Whether the mouse is currently being pressed
    * @param direction The mouse movement direction
    * @param world The camera world matrix
    * @param invProjection The camera inverse projection matrix
    * @param deltatime The time passed since last frame in seconds
    */
    void PreCollisionUpdate(bool pressed, 
                            const D3DXVECTOR2& direction,
                            const Matrix& world, 
                            const Matrix& invProjection, 
                            float deltatime);

    /**
    * Updates the state of the scene post collision solving
    */
    void PostCollisionUpdate();

    /**
    * Solves the collisions between objects
    */
    void SolveCollisions();

    /**
    * Set the visibility of the scene collision meshes
    * @param visible whether the collision meshes are visible or not
    */
    void SetCollisionVisibility(bool visible);

    /**
    * Toggles the visibility of the wall collision meshes
    */
    void ToggleWallVisibility();

    /**
    * Loads the gui callbacks
    * @param callbacks callbacks for the gui to fill in
    */
    void LoadGuiCallbacks(GuiCallbacks* callbacks);

private:

    typedef std::shared_ptr<Mesh> MeshPtr;
    typedef std::unique_ptr<CollisionMesh> CollisionPtr;

    /**
    * Removes a mesh from the scene
    * @param mesh The mesh to remove
    */
    void RemoveMesh(const MeshPtr& mesh);

    /**
    * Sets the selected mesh
    * @param mesh the selected mesh
    */
    void SetSelectedMesh(const Mesh* mesh);

    /**
    * Determines the correct method of resolution given the collision meshes
    * @param meshA The first collision mesh
    * @param meshB The second collision mesh
    */
    void SolveCollisions(CollisionMesh* meshA, CollisionMesh* meshB);

    /**
    * Prevent copying
    */
    Scene(const Scene&);
    Scene& operator=(const Scene&);

private:

    EnginePtr m_engine;                          ///< Callbacks for the rendering engine
    std::queue<unsigned int> m_open;             ///< Indices for the avaliable meshes
    std::vector<MeshPtr> m_meshes;               ///< Changable meshes in the scene
    std::vector<MeshPtr> m_templates;            ///< Mesh templates for creating mesh instances
    std::unique_ptr<Manipulator> m_manipulator;  ///< manipulator tool for changing objects
    std::shared_ptr<CollisionSolver> m_solver;   ///< The solver for collision resolution
    MeshPtr m_ground;                            ///< Ground grid mesh
    std::vector<CollisionPtr> m_walls;           ///< Wall collision meshes
    D3DXVECTOR3 m_wallMinBounds;                 ///< Minimum position in the wall enclosed space
    D3DXVECTOR3 m_wallMaxBounds;                 ///< Maximum position in the wall enclosed space
    int m_selectedMesh = 0;                      ///< Currently selected object
    int m_diagnosticMesh = 0;                    ///< Currently selected object for diagnostics
    bool m_drawCollisions = false;               ///< Whether to render the mesh collision models or not
    bool m_drawWalls = false;                    ///< Whether to render the wall mesh collision models or not
    SetFlag m_enableCreation;                    ///< Callback for enabled/disabling gui mesh creation
};
