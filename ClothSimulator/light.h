////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - light.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "common.h"

/**
* Basic lights for scene with multiple light shader support
*/
class Light
{
public:

    /**
    * Constructor
    */
    Light();

    /**
    * @param index The unique index of the light
    */
    void SetIndex(int index);

    /**
    * Set the ambient colour of the light
    * @param color Ambient color of light from 0-1
    * @param intensity The intensity of the ambient colour
    */
    void SetAmbient(const D3DXVECTOR3& color, float intensity);

    /**
    * Set the diffuse colour of the light
    * @param color Diffuse color of light from 0-1
    * @param intensity The intensity of the diffuse colour
    */
    void SetDiffuse(const D3DXVECTOR3& color, float intensity);

    /**
    * Set the specular colour of the light
    * @param intensity The intensity of the specular colour
    * @param size The size of the specular highlight
    */
    void SetSpecular(float intensity, float size);

    /**
    * Set the Attenuation of the light
    * @param a0 The Constant attenuation value
    * @param a1 The Linear attenuation value
    * @param a2 The Quadratic attenuation value
    */
    void SetAttenuation(float a0, float a1, float a2);

    /**
    * Sets the light posiiton
    * @param position The position to set in world coordinates
    */
    void SetPosition(const D3DXVECTOR3& position);

    /**
    * @return the position of the light
    */
    const D3DXVECTOR3& GetPosition() const;

    /**
    * Send all parameters to the given shader
    * @param shader The shader to send to
    */
    void SendLightToShader(LPD3DXEFFECT shader);

    /**
    * Toggle light on/off
    */
    void LightSwitch();

private:

    D3DXVECTOR3 m_position;    ///< Position of the light
    D3DXVECTOR3 m_diffuse;     ///< Diffuse color of light from 0-1
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

/**
* Manager for simulation lights
*/
class LightManager
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
    * Constructor
    */
    LightManager();

    /**
    * Updates the lights
    */
    void UpdateLights();

    /**
    * Initialise all lighting
    * @return whether initialisation succeeded or not
    */
    bool Inititalise();

    /**
    * Sends all lighting to the given shader
    * @param shader The shader to send lighting data to
    */
    void SendLightingToShader(LPD3DXEFFECT shader);

private:

    typedef std::shared_ptr<Light> LightPtr;

    /**
    * Prevent copying
    */
    LightManager(const LightManager&);
    LightManager& operator=(const LightManager&);

    std::vector<LightPtr> m_lights; ///< All lights in scene
};