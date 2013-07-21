/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* List of callback/window information for the gui
*****************************************************************/
#pragma once
#include <functional>
#include <Windows.h>

namespace GUI
{
    typedef std::function<void(void)> VoidFn;
    typedef std::function<void(bool)> SetFlag;

    struct GuiCallbacks
    {
        SetFlag setGravity; ///< Whether gravity is on/off
        SetFlag setHandleMode; ///< Whether handle mode is on/off
        SetFlag setVertsVisible; ///< Whether vertices are visible or not
        VoidFn resetCamera; ///< Resets the camera to the default position 
        VoidFn resetCloth; ///< Resets the cloth to the default position
        VoidFn unpinCloth; ///< Removes any pinned verts 
        VoidFn clearScene; ///< Removes any scene objects
        VoidFn quitFn; ///< Function called to quit the simulation
    };

    struct WindowHandle
    {
        HWND handle;
        HINSTANCE instance;
    };
}