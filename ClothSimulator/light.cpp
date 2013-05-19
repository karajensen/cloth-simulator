#include "light.h"
#include <algorithm>

std::vector<Light_Manager::LightPtr> Light_Manager::m_lights;

Light::Light() :
    m_position(0.0f, 0.0f, 0.0f),
    m_diffuse(1.0f, 1.0f, 1.0f),
    m_specular(1.0f, 1.0f, 1.0f),
    m_ambient(1.0f, 1.0f, 1.0f),
    m_attenuation0(0.0f),
    m_attenuation1(1.0f),
    m_attenuation2(0.0f),
    m_ambientIntensity(0.1f),
    m_diffuseIntensity(0.7f),
    m_specularIntensity(0.3f),
    m_specularSize(10.0f),
    m_active(true),
    m_index(NO_INDEX)
{
}

void Light::SetIndex(int index)
{
    m_index = index;
}

void Light::SetPosition(const D3DXVECTOR3& position)
{
    m_position = position;
}

void Light::LightSwitch()
{
    m_active = !m_active;
}

void Light::SetAmbient(const D3DXVECTOR3& color, float intensity)
{
    m_ambient = color;
    m_ambientIntensity = intensity;
}

void Light::SetDiffuse(const D3DXVECTOR3& color, float intensity)
{
    m_diffuse = color;
    m_diffuseIntensity = intensity;
}

void Light::SetSpecular(const D3DXVECTOR3& color, float intensity, float size)
{
    m_specular = color;
    m_specularIntensity = intensity;
    m_specularSize = size;
}

void Light::SetAttenuation(float a0, float a1, float a2)
{
    m_attenuation0 = a0;
    m_attenuation1 = a1;
    m_attenuation2 = a2;
}

void Light::SendLightToShader(LPD3DXEFFECT shader)
{
    if(m_active)
    {
        shader->SetFloat(DxConstant::LightOn, 1.0f);
        shader->SetFloatArray(DxConstant::AmbientColor, &m_ambient.x, 3);
        shader->SetFloatArray(DxConstant::DiffuseColor, &m_diffuse.x, 3);
        shader->SetFloatArray(DxConstant::SpecularColor, &m_specular.x, 3);
        shader->SetFloat(DxConstant::AmbientIntensity, m_ambientIntensity);
        shader->SetFloat(DxConstant::DiffuseIntensity, m_diffuseIntensity);
        shader->SetFloat(DxConstant::SpecularIntensity, m_specularIntensity);
        shader->SetFloat(DxConstant::SpecularSize, m_specularSize);
        shader->SetFloatArray(DxConstant::LightPos, &m_position.x, 3); 
    }
    else
    {
        shader->SetFloat(DxConstant::LightOn, 0.0f);
    }
}

void Light_Manager::Inititalise()
{
    m_lights.resize(MAX_LIGHTS);
    std::generate(m_lights.begin(), m_lights.end(), [&](){ return Light_Manager::LightPtr(new Light()); });

    m_lights[MAIN_LIGHT]->SetIndex(MAIN_LIGHT);
    m_lights[MAIN_LIGHT]->SetPosition(D3DXVECTOR3(-10.0f,5.0f,-18.0f));
    m_lights[MAIN_LIGHT]->SetAmbient(D3DXVECTOR3(0.94f,0.94f,1.0f),0.05f);
    m_lights[MAIN_LIGHT]->SetDiffuse(D3DXVECTOR3(0.94f,0.94f,1.0f),0.7f);
}

void Light_Manager::SendLightingToShader(LPD3DXEFFECT shader)
{
    //For now only one light is needed. When multiple lights are needed, change to allow this
    auto sendLight = [&](const Light_Manager::LightPtr& light){ light->SendLightToShader(shader); };
    std::for_each(Light_Manager::m_lights.begin(), Light_Manager::m_lights.end(), sendLight);
}