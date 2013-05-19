/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Direct Input and mouse picking wrapper
* Note: Some keys do not work with continuous keypress/
* mouse click combination (ie. Space/Tab)
*****************************************************************/
#pragma once

#define DIRECTINPUT_VERSION 0x0800 //for direct input

#include "common.h"
#include "picking.h"
#include <dinput.h>
#include <unordered_map>

class Mesh;

class Input
{
public:

    typedef std::function<void(void)> KeyFn;

    /**
    * Constructor
    * @param handle to the instance of the application
    * @param handle to the window of the application
    */
    Input(HINSTANCE hInstance, HWND hWnd);

    /**
    * Destroy directinput
    */
    ~Input();

    /**
    * Updates the input from directX
    */
    void UpdateInput();

    /**
    * @return whether the mouse has been clicked for this tick.
    */
    bool IsMouseClicked() const;

    /**
    * @return whether the mouse is being held down
    */
    bool IsMousePressed();

    /**
    * Set the function to call upon toggle of the input key
    * @param the DirectInput keyboard scan code
    * @param whether to call on key press or key press continously
    * @param the function to call
    * @param an optional function to call if key is no longer pressed
    */
    void SetKeyCallback(unsigned int key, bool onContinous, KeyFn onKeyFn, KeyFn offKeyFn = nullptr);

    /**
    * Set the x/y mouse coordinates
    */
    void SetMouseCoord(int x, int y);

    /**
    * @return mouse picking information
    */
    Picking& GetMousePicking() { return m_picking; }

    /**
    * Send a ray into the scene to determine if mouse clicked an object
    * @param the camera projection matrix
    * @param the camera view matrix
    */
    void UpdatePicking(Transform& projection, Transform& view);

    /**
    * If a mesh was picked, call the associated picking function
    */
    void SolvePicking();

    /**
    * @return the mouse click position
    */
    int GetMouseX() const { return m_x; }
    int GetMouseY() const { return m_y; }

    /**
    * @return the direction the mouse has moved since last tick
    */
    const D3DXVECTOR2& GetMouseDirection() const { return m_mouseDirection; }
    
private:

    /**
    * Whether a key is currently being pressed or not
    * @param the state of the key
    * @param whether the key is being pressed
    */
    bool IsKeyDownContinous(unsigned int& state);

    /**
    * Whether a key was pressed. Once queried will return false until key is released
    * @param the state of the key
    * @param whether the key was pressed
    */
    bool IsKeyDown(unsigned int& state);

    /**
    * Recieves states of keys from directInput
    */
    void GetKeys(BYTE* KeyState);

    /**
    * Recieves state of mouse from directInput
    * @param the state of the mouse
    */
    void GetMouse(DIMOUSESTATE* MouseState);

    /**
    * Updates the cached key array from the direct input state
    * @param whether the key is currently being pressed
    * @param the state of the key
    */
    void UpdateKey(BYTE pressed, unsigned int& state);

    /**
    * Masks for state of input key
    */
    enum InputMask
    {
        KEY_DOWN = 1,    ///< Whether the key is currently being pressed
        KEY_QUERIED = 2  ///< Whether the key has been queried since pressed
    };

    /**
    * Key object structure
    */
    struct Key
    {
        unsigned int state;     ///< Current state of the key
        KeyFn onKeyFn;          ///< Function to call if key is down
        KeyFn offKeyFn;         ///< Function to call if key is up
        bool continuous;        ///< Whether key should look at continous or not
    };

    typedef std::unordered_map<unsigned int, Key> KeyMap;

    KeyMap m_keys;                         ///< Cached keys
    bool m_mouseClicked;                   ///< Whether mouse was clicked this tick
    unsigned int m_mouse;                  ///< Cached mouse state
    int m_x, m_y;                          ///< Mouse clicked screen coordinates
    Picking m_picking;                     ///< Mouse picking
    D3DXVECTOR2 m_mouseDirection;          ///< Direction mouse has moved between ticks
    LPDIRECTINPUT8 m_directInput;          ///< DirectX input
    LPDIRECTINPUTDEVICE8 m_keyboardInput;  ///< DirectX keyboard device
    LPDIRECTINPUTDEVICE8 m_mouseInput;     ///< DirectX mouse device
};