/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* DirectX Shader clas
*****************************************************************/

#pragma once
#include "common.h"

struct ShaderConstants;

class Shader
{
public:
    
    Shader();
    ~Shader();

    /**
    * Loads the shader
    * @param the directX device
    * @param the filename of the shader
    * @return whether loading was successful
    */
    bool Load(LPDIRECT3DDEVICE9 d3ddev, const std::string& filename);

    /**
    * @return the DirectX Shader effect
    */
    LPD3DXEFFECT GetEffect(){ return m_effect; };

private:

    LPD3DXEFFECT m_effect; ///< DirectX shader
    
};

class Shader_Manager
{
public:

    /**
    * Avaliable shaders in the scene
    */
    enum SceneShader
    { 
        MAIN_SHADER, 
        CLOTH_SHADER, 
        BOUNDS_SHADER, 
        MAX_SHADERS 
    };

    /**
    * Initialise all shaders
    * @return whether initialisation succeeded
    */
    static bool Inititalise(LPDIRECT3DDEVICE9 d3ddev);

    /**
    * @param the shader to get
    * @return a shared pointer to the required shader
    */
    static std::shared_ptr<Shader> GetShader(SceneShader shader);

    /**
    * @return the shader effect for the world shader
    */
    static LPD3DXEFFECT GetWorldEffect();

    /**
    * @return whether to use the world shader
    */
    static bool UseWorldShader();

    /**
    * @param set whether to use the world shader
    */
    static void SetUseWorldShader(bool use);

    /**
    * @param set the currently used world shader
    */
    static void SetWorldShader(SceneShader shader);

private:

    typedef std::shared_ptr<Shader> ShaderPtr;
    static std::vector<ShaderPtr> m_shaders; ///< All shaders in scene

    static SceneShader m_worldShader;   ///< Index to the world shader
    static bool m_useWorldShader;      ///< Whether to use the world shader or not
};
