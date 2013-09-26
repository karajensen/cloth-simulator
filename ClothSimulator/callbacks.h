////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - callbacks.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"
#include "shader.h"
#include "diagnostic.h"
#include "GUIcallbacks.h"

/**
* Functions required for mesh rendering/diagnostics
*/
struct Engine
{
    /**
    * Retrieves the shader effect from the shader manager
    */
    std::function<ShaderManager::ShaderPtr(ShaderManager::SceneShader)> getShader;

    /**
    * Sends all lighting information to the shader
    */
    std::function<void(LPD3DXEFFECT)> sendLightingToEffect;   

    /**
    * Retrieves the diagnostic renderer
    */
    std::function<Diagnostic*(void)> diagnostic;

    /**
    * Retrives the directx device
    */
    std::function<LPDIRECT3DDEVICE9(void)> device;
};
typedef std::shared_ptr<Engine> EnginePtr;
