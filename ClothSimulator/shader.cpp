////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - shader.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <algorithm>
#include "shader.h"
#include "light.h"

ShaderManager::ShaderManager() :
    m_worldShader(ShaderManager::MAIN_SHADER),
    m_useWorldShader(false)
{
}

Shader::Shader():
    m_effect(nullptr)
{
}

Shader::~Shader()
{
    if(m_effect != nullptr)
    {
        m_effect->Release();
    }
}

bool Shader::Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename)
{
    ID3DXBuffer* errorlog = nullptr;
    if(FAILED(D3DXCreateEffectFromFile(d3ddev,filename.c_str(),0,0,
        D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY,0, &m_effect, &errorlog)))
    {
        std::string errorMessage("Shader " + filename + " has failed!");
        if(errorlog)
        {
            errorMessage += (char*)errorlog->GetBufferPointer();
        }
        Diagnostic::ShowMessage(errorMessage);
        return false;
    }
    return true;
}

bool ShaderManager::Inititalise(LPDIRECT3DDEVICE9 d3ddev)
{
    const std::string ShaderFolder(".\\Resources\\Shaders\\");

    m_shaders.resize(MAX_SHADERS);
    std::generate(m_shaders.begin(), m_shaders.end(), 
        [&](){ return ShaderManager::ShaderPtr(new Shader()); });
    
    bool success = true;
    success = (success ? m_shaders[MAIN_SHADER]->Load(d3ddev,ShaderFolder+"main.fx") : false);
    success = (success ? m_shaders[CLOTH_SHADER]->Load(d3ddev,ShaderFolder+"cloth.fx") : false);
    success = (success ? m_shaders[BOUNDS_SHADER]->Load(d3ddev,ShaderFolder+"bounds.fx") : false);
    success = (success ? m_shaders[TOOL_SHADER]->Load(d3ddev,ShaderFolder+"tool.fx") : false);
    success = (success ? m_shaders[GROUND_SHADER]->Load(d3ddev,ShaderFolder+"ground.fx") : false);
    return success;
}

std::shared_ptr<Shader> ShaderManager::GetShader(SceneShader shader)
{
    return m_shaders[shader];
}

LPD3DXEFFECT ShaderManager::GetWorldEffect()
{ 
    return m_shaders[m_worldShader]->GetEffect(); 
}

bool ShaderManager::UseWorldShader()
{ 
    return m_useWorldShader; 
}

void ShaderManager::SetUseWorldShader(bool use)
{ 
    m_useWorldShader = use; 
}

void ShaderManager::SetWorldShader(SceneShader shader)
{ 
    m_worldShader = shader;
}