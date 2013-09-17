////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - transform.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "transform.h"
#include "common.h"

Transform::Transform():
    m_yaw(0.0f),
    m_pitch(0.0f),
    m_roll(0.0f),
    m_position(0.0f, 0.0f, 0.0f),
    m_fullUpdateFn(nullptr),
    m_positionalUpdateFn(nullptr)
{ 
    D3DXMatrixIdentity(&m_scale);
    D3DXMatrixIdentity(&m_rotation);
}

void Transform::SetObserver(UpdateFn fullUpdate, UpdateFn positionalUpdate)
{
    m_fullUpdateFn = fullUpdate;
    m_positionalUpdateFn = positionalUpdate;
}

void Transform::Update(bool fullupdate)
{
    m_matrix = m_scale * m_rotation;
    m_matrix._41 = m_position.x;
    m_matrix._42 = m_position.y;
    m_matrix._43 = m_position.z;

    if(fullupdate)
    {
        if(m_fullUpdateFn != nullptr)
        {
            m_fullUpdateFn();
        }
    }
    else
    {
        if(m_positionalUpdateFn != nullptr)
        {
            m_positionalUpdateFn();
        }
    }
}

void Transform::SetScale(float scale)
{
    SetScale(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
    m_scale._11 = x;
    m_scale._22 = y;
    m_scale._33 = z;
    Update(true);
}

void Transform::Scale(float x, float y, float z)
{
    const float minimum = 0.1f;
    m_scale._11 = max(minimum, m_scale._11 + x);
    m_scale._22 = max(minimum, m_scale._22 + y);
    m_scale._33 = max(minimum, m_scale._33 + z);
    Update(true);
}

void Transform::Rotate(float x, float y, float z)
{
    D3DXMATRIX rotation;
    D3DXMatrixIdentity(&rotation);

    if(x != 0.0f)
    {
        float c = cos(x);
        float s = sin(x);
        rotation._22 = c;
        rotation._32 = s;
        rotation._23 = -s;
        rotation._33 = c;
    }
    else if(y != 0.0f)
    {
        float c = cos(y);
        float s = sin(y);
        rotation._11 = c;
        rotation._13 = s;
        rotation._31 = -s;
        rotation._33 = c;
    }
    else if(z != 0.0f)
    {
        float c = cos(z);
        float s = sin(z);
        rotation._11 = c;
        rotation._21 = s;
        rotation._12 = -s;
        rotation._22 = c;
    }

    m_rotation *= rotation;
    Update(true);
}

void Transform::SetPosition(const D3DXVECTOR3& position)
{
    m_position.x = position.x;
    m_position.y = position.y;
    m_position.z = position.z;
    Update(false);
}

void Transform::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    Update(false);
}

void Transform::Translate(const D3DXVECTOR3& position)
{
    m_position.x += position.x;
    m_position.y += position.y;
    m_position.z += position.z;
    Update(false);
}

void Transform::Translate(float x, float y, float z)
{ 
    m_position.x += x;
    m_position.y += y;
    m_position.z += z;
    Update(false);
}

void Transform::MakeIdentity()
{
    m_yaw = 0.0f;
    m_roll = 0.0f;
    m_pitch = 0.0f;
    m_position.x = 0.0f;
    m_position.y = 0.0f;
    m_position.z = 0.0f;
    D3DXMatrixIdentity(&m_scale);
    D3DXMatrixIdentity(&m_rotation);
    Update(true);
}

D3DXVECTOR3 Transform::GetScale() const
{
    return D3DXVECTOR3(m_scale._11, m_scale._22, m_scale._33); 
}
