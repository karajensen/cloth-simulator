////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - simulation.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "GUICallbacks.h"

class LightManager;
class ShaderManager;
class CollisionSolver;
class Scene;
class Camera;
class Cloth;
class Input;
class Timer;

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
    void LoadGuiCallbacks(GUI::GuiCallbacks* callbacks);

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
    */
    void LoadInput(HINSTANCE hInstance, HWND hWnd);

    /**
    * Prevent copying
    */
    Simulation(const Simulation&);
    Simulation& operator=(const Simulation&);

    std::shared_ptr<LightManager> m_light;       ///< Manager for the simulation lights
    std::shared_ptr<ShaderManager> m_shader;     ///< Manager for the simulation shaders
    std::shared_ptr<CollisionSolver> m_solver;   ///< Collision solver for cloth
    std::shared_ptr<Timer> m_timer;              ///< Simulation timer object
    std::shared_ptr<Cloth> m_cloth;              ///< Simulation cloth object
    std::shared_ptr<Input> m_input;              ///< Simulation input object
    std::shared_ptr<Camera> m_camera;            ///< Main camera
    std::shared_ptr<Scene> m_scene;              ///< Mesh manager for the scene
    LPDIRECT3DDEVICE9 m_d3ddev;                  ///< DirectX device
    bool m_drawCollisions;                       ///< Whether to display collision models
};
