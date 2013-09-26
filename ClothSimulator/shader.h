////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - shader.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

/**
* DirectX Shader class
*/
class Shader
{
public:
    
    /**
    * Constructor
    */
    Shader();

    /**
    * Destructor
    */
    ~Shader();

    /**
    * Loads the shader
    * @param d3ddev The directX device
    * @param filename The filename of the shader
    * @return whether loading was successful
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename);

    /**
    * @return the DirectX Shader effect
    */
    LPD3DXEFFECT GetEffect(){ return m_effect; };

private:

    /**
    * Prevent copying
    */
    Shader(const Shader&);
    Shader& operator=(const Shader&);

    LPD3DXEFFECT m_effect; ///< DirectX shader effect
};

/**
* Manager for directX shaders
*/
class ShaderManager
{
public:

    typedef std::shared_ptr<Shader> ShaderPtr;

    /**
    * Avaliable shaders in the scene
    */
    enum SceneShader
    { 
        MAIN_SHADER, 
        CLOTH_SHADER, 
        BOUNDS_SHADER,
        TOOL_SHADER,
        GROUND_SHADER,
        MAX_SHADERS 
    };

    /**
    * Constructor
    */
    ShaderManager();

    /**
    * Initialise all shaders
    * @return whether initialisation succeeded
    */
    bool Inititalise(LPDIRECT3DDEVICE9 d3ddev);

    /**
    * @param the shader to get
    * @return a shared pointer to the required shader
    */
    std::shared_ptr<Shader> GetShader(SceneShader shader);

private:

    /**
    * Prevent copying
    */
    ShaderManager(const ShaderManager&);
    ShaderManager& operator=(const ShaderManager&);

    std::vector<ShaderPtr> m_shaders; ///< All shaders in scene
};

/**
* Constants shared between shader effect files
*/
namespace DxConstant
{
    static const D3DXHANDLE DefaultTechnique("MAIN");
    static const D3DXHANDLE VertexColor("VertexColor");
    static const D3DXHANDLE DiffuseTexture("DiffuseTexture");
    static const D3DXHANDLE CameraPosition("CameraPosition");
    static const D3DXHANDLE WorldInverseTranspose("WorldInvTrans");
    static const D3DXHANDLE WordViewProjection("WorldViewProjection");
    static const D3DXHANDLE World("World");
    static const D3DXHANDLE AmbientColor("AmbientColor");
    static const D3DXHANDLE DiffuseColor("DiffuseColor");
    static const D3DXHANDLE AmbientIntensity("AmbientIntensity");
    static const D3DXHANDLE DiffuseIntensity("DiffuseIntensity");
    static const D3DXHANDLE SpecularIntensity("SpecularIntensity");
    static const D3DXHANDLE SpecularSize("SpecularSize");
    static const D3DXHANDLE LightPosition("LightPosition");
}