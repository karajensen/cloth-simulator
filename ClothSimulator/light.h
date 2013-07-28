/****************************************************************
* Kara Jensen (KaraPeaceJensen@gmail.com) 
* Basic lights for scene with multiple light support
*****************************************************************/
#pragma once

#include "common.h"

class Light
{
public:

    /**
    * Constructor
    */
    Light();

    /**
    * @param the unique index of the light
    */
    void SetIndex(int index);

    /**
    * Set the ambient colour of the light
    * @param Ambient color of light from 0-1
    * @param the intensity of the ambient colour
    */
    void SetAmbient(const D3DXVECTOR3& color, float intensity);

    /**
    * Set the diffuse colour of the light
    * @param diffuse color of light from 0-1
    * @param the intensity of the diffuse colour
    */
    void SetDiffuse(const D3DXVECTOR3& color, float intensity);

    /**
    * Set the specular colour of the light
    * @param specular color of light from 0-1
    * @param the intensity of the specular colour
    * @param the size of the specular highlight
    */
    void SetSpecular(const D3DXVECTOR3& color, float intensity, float size);

    /**
    * Set the attenuation of the light
    * @param the Constant attenuation value
    * @param the Linear attenuation value
    * @param the Quadratic attenuation value
    */
    void SetAttenuation(float a0, float a1, float a2);

    /**
    * Sets the light posiiton
    * @param the position to set in world coordinates
    */
    void SetPosition(const D3DXVECTOR3& position);

    /**
    * Send all parameters to the given shader
    * @param the shader to send to
    */
    void SendLightToShader(LPD3DXEFFECT shader);

    /**
    * Toggle light on/off
    */
    void LightSwitch();

private:

    D3DXVECTOR3 m_position;    ///< Position of the light
    D3DXVECTOR3 m_diffuse;     ///< Diffuse color of light from 0-1
    D3DXVECTOR3 m_specular;    ///< Specular color of light from 0-1
    D3DXVECTOR3 m_ambient;     ///< Ambient color of light from 0-1
    float m_attenuation0;      ///< Constant attenuation
    float m_attenuation1;      ///< Linear attenuation
    float m_attenuation2;      ///< Quadratic attenuation
    float m_ambientIntensity;  ///< Intensity of the ambient light
    float m_diffuseIntensity;  ///< Intensity of the diffuse light
    float m_specularIntensity; ///< Intensity of the specular light
    float m_specularSize;      ///< Size of the specular highlights
    bool m_active;             ///< Whether the light is on or not
    int m_index;               ///< The unique index of the light
};

class Light_Manager
{
public:

    /**
    * Avaliable lights in the scene
    */
    enum SceneLights
    { 
        MAIN_LIGHT, 
        MAX_LIGHTS 
    };

    /**
    * Initialise all lighting
    * @param whether initialisation succeeded or nots
    */
    static bool Inititalise();

    /**
    * Sends all lighting to the given shader
    */
    static void SendLightingToShader(LPD3DXEFFECT shader);

private:

    typedef std::shared_ptr<Light> LightPtr;
    static std::vector<LightPtr> m_lights; ///< All lights in scene
};