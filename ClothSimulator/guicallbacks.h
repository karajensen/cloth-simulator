////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - GUICallback.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <functional>
#include <Windows.h>

namespace GUI
{
    typedef std::function<void(void)> VoidFn;
    typedef std::function<void(bool)> SetFlag;
    typedef std::function<void(double)> SetValue;
    typedef std::function<double(void)> GetValue;

    /**
    * Callbacks for use in the GUI
    */
    struct GuiCallbacks
    {
        SetFlag enableMeshCreation; ///< Enables/disables gui mesh creation
        SetFlag setGravity; ///< Whether gravity is on/off
        SetFlag setHandleMode; ///< Whether handle mode is on/off
        SetFlag setVertsVisible; ///< Whether vertices are visible or not
        SetFlag setWireframeMode; ///< Whether wireframe mode is on/off
        VoidFn setMoveTool; ///< Change the manipulator to translation
        VoidFn setRotateTool; ///< Change the manipulator to rotation
        VoidFn setScaleTool; ///< Change the manipulator to scaling
        VoidFn resetCamera; ///< Resets the camera to the default position 
        VoidFn resetCloth; ///< Resets the cloth to the default position
        VoidFn unpinCloth; ///< Removes any pinned verts 
        VoidFn clearScene; ///< Removes any scene objects
        VoidFn quitFn; ///< Function called to quit the simulation
        VoidFn createBox; ///< Function called to create a box
        VoidFn createCylinder; ///< Function called to create a cylinder
        VoidFn createSphere; ///< Function called to create a sphere
        SetValue setTimestep; ///< Function set timestep
        SetValue setIterations; ///< Function set iterations
        SetValue setVertexRows; ///< Function set vertex number
        SetValue setSpacing; ///< Function set spacing between vertices
        GetValue getTimestep; ///< Function get timestep
        GetValue getIterations; ///< Function get iterations
        GetValue getVertexRows; ///< Function get vertex number
        GetValue getSpacing; ///< Function get spacing between vertices
    };

    /**
    * GUI window information
    */
    struct WindowHandle
    {
        HWND handle;
        HINSTANCE instance;
    };
}