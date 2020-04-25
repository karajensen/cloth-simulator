////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - callbacks.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "diagnostic.h"
#include "GUIcallbacks.h"
#include "octree_interface.h"

/**
* Functions required for mesh rendering/diagnostics
*/
struct Engine
{
    /**
    * Retrieves the shader effect from the shader manager
    */
    std::function<LPD3DXEFFECT(int shader)> getShader;

    /**
    * Sends all lighting information to the shader
    */
    std::function<void(LPD3DXEFFECT)> sendLightsToShader;   

    /**
    * Retrieves the diagnostic renderer
    */
    std::function<Diagnostic*(void)> diagnostic;

    /**
    * Retrives the directx device
    */
    std::function<LPDIRECT3DDEVICE9(void)> device;

    /**
    * Retrieves the octree interface
    */
    std::function<IOctree*(void)> octree;
};
typedef std::shared_ptr<Engine> EnginePtr;
