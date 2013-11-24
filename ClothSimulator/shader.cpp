////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - shader.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <algorithm>
#include "shader.h"
#include "light.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
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
    if(FAILED(D3DXCreateEffectFromFile(d3ddev,filename.c_str(), 0, 0,
        D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 0, &m_effect, &errorlog)))
    {
        std::string errorMessage("Shader " + filename + " has failed!");
        if(errorlog)
        {
            errorMessage += (char*)errorlog->GetBufferPointer();
        }
        ShowMessageBox(errorMessage);
        return false;
    }
    return true;
}

bool ShaderManager::Inititalise(LPDIRECT3DDEVICE9 d3ddev)
{
    const std::string ShaderFolder(".\\Resources\\Shaders\\");

    m_shaders.resize(MAX_SHADERS);
    std::generate(m_shaders.begin(), m_shaders.end(), 
        [&](){ return std::unique_ptr<Shader>(new Shader()); });
    
    bool success = true;
    success = (success ? m_shaders[MAIN_SHADER]->Load(d3ddev,ShaderFolder+"main.fx") : false);
    success = (success ? m_shaders[CLOTH_SHADER]->Load(d3ddev,ShaderFolder+"cloth.fx") : false);
    success = (success ? m_shaders[BOUNDS_SHADER]->Load(d3ddev,ShaderFolder+"bounds.fx") : false);
    success = (success ? m_shaders[TOOL_SHADER]->Load(d3ddev,ShaderFolder+"tool.fx") : false);
    success = (success ? m_shaders[GROUND_SHADER]->Load(d3ddev,ShaderFolder+"ground.fx") : false);
    return success;
}

LPD3DXEFFECT ShaderManager::GetShader(int shader)
{
    return m_shaders[shader]->GetEffect();
}