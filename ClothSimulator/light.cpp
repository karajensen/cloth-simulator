////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - light.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "light.h"
#include "shader.h"
#include "diagnostic.h"
#include <algorithm>

LightManager::LightManager()
{
}

Light::Light() :
    m_position(0.0f, 0.0f, 0.0f),
    m_diffuse(1.0f, 1.0f, 1.0f),
    m_ambient(1.0f, 1.0f, 1.0f),
    m_attenuation0(0.0f),
    m_attenuation1(1.0f),
    m_attenuation2(0.0f),
    m_ambientIntensity(0.2f),
    m_diffuseIntensity(0.7f),
    m_specularIntensity(0.3f),
    m_specularSize(10.0f),
    m_active(true),
    m_index(NO_INDEX)
{
}

const D3DXVECTOR3& Light::GetPosition() const
{
    return m_position;
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

void Light::SetSpecular(float intensity, float size)
{
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
        shader->SetFloatArray(DxConstant::AmbientColor, &m_ambient.x, 3);
        shader->SetFloatArray(DxConstant::DiffuseColor, &m_diffuse.x, 3);
        shader->SetFloat(DxConstant::AmbientIntensity, m_ambientIntensity);
        shader->SetFloat(DxConstant::DiffuseIntensity, m_diffuseIntensity);
        shader->SetFloat(DxConstant::SpecularIntensity, m_specularIntensity);
        shader->SetFloat(DxConstant::SpecularSize, m_specularSize);
        shader->SetFloatArray(DxConstant::LightPosition, &m_position.x, 3);
    }
}

bool LightManager::Inititalise()
{
    m_lights.resize(MAX_LIGHTS);
    std::generate(m_lights.begin(), m_lights.end(),
        [&](){ return LightManager::LightPtr(new Light()); });

    m_lights[MAIN_LIGHT]->SetIndex(MAIN_LIGHT);
    m_lights[MAIN_LIGHT]->SetPosition(D3DXVECTOR3(-10.0f,10.0f,-18.0f));
    m_lights[MAIN_LIGHT]->SetAmbient(D3DXVECTOR3(0.94f,0.94f,1.0f),0.05f);
    m_lights[MAIN_LIGHT]->SetDiffuse(D3DXVECTOR3(0.94f,0.94f,1.0f),0.7f);
    return true;
}

void LightManager::SendLightingToShader(LPD3DXEFFECT shader)
{
    // For now only one light is needed. When multiple 
    // lights are needed, change to allow this
    std::for_each(LightManager::m_lights.begin(), LightManager::m_lights.end(),
        [&](const LightManager::LightPtr& light)
    {
        light->SendLightToShader(shader);
    });
}

void LightManager::UpdateLights()
{
    if(Diagnostic::AllowDiagnostics(Diagnostic::GENERAL))
    {
        const float radius = 0.5f;
        std::for_each(LightManager::m_lights.begin(), LightManager::m_lights.end(),
            [&](const LightManager::LightPtr& light)
        {
            Diagnostic::UpdateSphere(Diagnostic::GENERAL, "Light"+StringCast(&light), 
                Diagnostic::RED, light->GetPosition(), radius);
        });
    }
}