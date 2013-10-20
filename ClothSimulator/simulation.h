////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simulation.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "callbacks.h"

class Diagnostic;
class LightManager;
class ShaderManager;
class CollisionSolver;
class Scene;
class Camera;
class Cloth;
class Input;
class Timer;
class Octree;

/**
* Main Simulation Class
*/
class Simulation
{
public:

    /**
    * Constructor
    */
    Simulation();

    /**
    * Destructor
    */
    ~Simulation();

    /**
    * Initialises the simulation
    * @param hInstance The instance to the window
    * @param hWnd The handle to the window
    * @param d3ddev The directX device
    * @return whether initialisation succeeded
    */
    bool CreateSimulation(HINSTANCE hInstance, HWND hWnd, LPDIRECT3DDEVICE9 d3ddev);

    /**
    * Loads the gui callbacks
    * @param callbacks Methods the gui can call for the simulation
    */
    void LoadGuiCallbacks(GuiCallbacks* callbacks);

    /**
    * Renders the simulation
    */
    void Render();

    /**
    * Updates the simulation
    */
    void Update();

private:

    /**
    * Loads all scene meshes
    * @return whether loading was successful
    */
    bool LoadMeshes();

    /**
    * Creates the input object and connects callbacks to input keys
    * @param hInstance The window instance
    * @param hWnd The handle to the window
    * @param engine Callbacks from the rendering engine
    */
    void LoadInput(HINSTANCE hInstance, HWND hWnd, EnginePtr engine);

    /**
    * Prevent copying
    */
    Simulation(const Simulation&);
    Simulation& operator=(const Simulation&);

    std::unique_ptr<LightManager> m_light;       ///< Manager for the simulation lights
    std::unique_ptr<ShaderManager> m_shader;     ///< Manager for the simulation shaders
    std::unique_ptr<CollisionSolver> m_solver;   ///< Collision solver for cloth
    std::unique_ptr<Timer> m_timer;              ///< Simulation timer object
    std::shared_ptr<Cloth> m_cloth;              ///< Simulation cloth object
    std::unique_ptr<Input> m_input;              ///< Simulation input object
    std::unique_ptr<Camera> m_camera;            ///< Main camera
    std::unique_ptr<Scene> m_scene;              ///< Mesh manager for the scene
    std::unique_ptr<Diagnostic> m_diagnostics;   ///< Diagnostic renderer
    std::unique_ptr<Octree> m_octree;            ///< Octree spatial partitining
    LPDIRECT3DDEVICE9 m_d3ddev;                  ///< DirectX device
    bool m_drawCollisions;                       ///< Whether to display collision models
};
