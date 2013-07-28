/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Main Simulation Class
*****************************************************************/

#pragma once

#include "common.h"
#include "GUICallbacks.h"

class MeshManager;
class Camera;
class Cloth;
class Input;
class Timer;

class Simulation
{
public:

    Simulation();
    ~Simulation();

    /**
    * Initialises the simulation
    * @param the instance to the window
    * @param the handle to the window
    * @param the directX device
    * @return whether initialisation succeeded
    */
    bool CreateSimulation(HINSTANCE hInstance, HWND hWnd, LPDIRECT3DDEVICE9 d3ddev);

    /**
    * Loads the gui callbacks
    * @param callbacks for the gui to fill in
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
    * @param whether loading was successful
    */
    bool LoadMeshes();

    /**
    * Creates the input object and connects callbacks to input keys
    * @param the window instance
    * @param the handle to the window
    */
    void LoadInput(HINSTANCE hInstance, HWND hWnd);

    std::shared_ptr<Timer> m_timer;       ///< Simulation timer object
    std::shared_ptr<Cloth> m_cloth;       ///< Simulation cloth object
    std::shared_ptr<Input> m_input;       ///< Simulation input object
    std::shared_ptr<Camera> m_camera;     ///< Main camera
    std::shared_ptr<MeshManager> m_scene; ///< Mesh manager for the scene
    LPDIRECT3DDEVICE9 m_d3ddev;           ///< DirectX device
    static bool sm_drawCollisions;        ///< Whether to display collision models
};
