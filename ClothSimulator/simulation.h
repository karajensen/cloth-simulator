/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Main Simulation Class
*****************************************************************/

#pragma once

#include "common.h"
#include "guicallbacks.h"

class Camera;
class Cloth;
class Text;
class Sprite;
class Input;
class Mesh;
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
    void LoadGuiCallbacks(GUI::GuiCallback* callback);

    /**
    * Renders the simulation
    */
    void Render();

    /**
    * Updates the simulation
    */
    void Update();

    /**
    * Update the input mouse coordinates
    * @param the coordinates in screen space
    */
    void SetMouseCoord(int x, int y);

private:

    /**
    * Generates a new cloth object
    */
    void CreateCloth();

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

    typedef std::shared_ptr<Text> TextPtr;
    typedef std::shared_ptr<Sprite> SpritePtr;
    typedef std::shared_ptr<Mesh> MeshPtr;

    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<Cloth> m_cloth;     ///< Simulation cloth object
    std::shared_ptr<Input> m_input;     ///< Simulation input object
    std::shared_ptr<Camera> m_camera;   ///< Main camera
    std::vector<MeshPtr> m_meshes;      ///< Container of meshes

    float m_clothSize;             ///< Current size of the cloth
    int m_clothDimensions;         ///< Current dimensions of the cloth
    bool m_handleMode;             ///< Whether the simulation is in handle mode
    LPDIRECT3DDEVICE9 m_d3ddev;    ///< DirectX device
    static bool sm_drawCollisions; ///< Whether to display collision models
};
