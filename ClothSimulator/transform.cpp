////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - transform.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "transform.h"
#include "common.h"

Transform::Transform():
    m_scaleFactor(1.0f,1.0f,1.0f),
    m_fullUpdateFn(nullptr),
    m_positionalUpdateFn(nullptr)
{ 
    D3DXMatrixIdentity(&m_matrix);
}

void Transform::SetObserver(UpdateFn fullUpdate, UpdateFn positionalUpdate)
{
    m_fullUpdateFn = fullUpdate;
    m_positionalUpdateFn = positionalUpdate;
}

void Transform::CallObserver(bool fullupdate)
{
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
    m_matrix._11 = x;
    m_matrix._22 = y;
    m_matrix._33 = z;
    m_scaleFactor.x = x;
    m_scaleFactor.y = y;
    m_scaleFactor.z = z;
    CallObserver(true);
}

void Transform::ScaleLocal(float x, float y, float z)
{
    m_scaleFactor.x *= x;
    m_scaleFactor.y *= y;
    m_scaleFactor.z *= z;

    D3DXVECTOR3 position(m_matrix._41, m_matrix._42, m_matrix._43);
    SetPosition(0,0,0);

    D3DXMatrixScaling(&m_matrix, x, y, z);

    SetPosition(position);
    CallObserver(true);
}

void Transform::RotateX(float radians, bool local)
{
    D3DXVECTOR3 position(m_matrix._41, m_matrix._42, m_matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    D3DXMatrixIdentity(&rotation);
    float c = cos(radians);
    float s = sin(radians);
    rotation._22 = c;  
    rotation._32 = s;  
    rotation._23 = -s; 
    rotation._33 = c; 
    m_matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::RotateY(float radians, bool local)
{
    D3DXVECTOR3 position(m_matrix._41, m_matrix._42, m_matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    D3DXMatrixIdentity(&rotation);
    float c = cos(radians);
    float s = sin(radians);
    rotation._11 = c;  
    rotation._13 = s;  
    rotation._31 = -s; 
    rotation._33 = c; 
    m_matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::RotateZ(float radians, bool local)
{
    D3DXVECTOR3 position(m_matrix._41, m_matrix._42, m_matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    D3DXMatrixIdentity(&rotation);
    float c = cos(radians);
    float s = sin(radians);
    rotation._11 = c;  
    rotation._21 = s;  
    rotation._12 = -s; 
    rotation._22 = c; 
    m_matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::SetPosition(const D3DXVECTOR3& position)
{
    m_matrix._41 = position.x;
    m_matrix._42 = position.y;
    m_matrix._43 = position.z;
    CallObserver(false);
}

void Transform::SetPosition(float x, float y, float z)
{
    m_matrix._41 = x;
    m_matrix._42 = y;
    m_matrix._43 = z;
    CallObserver(false);
}

void Transform::Translate(const D3DXVECTOR3& position)
{
    m_matrix._41 += position.x;
    m_matrix._42 += position.y;
    m_matrix._43 += position.z;
    CallObserver(false);
}

void Transform::Translate(float x, float y, float z)
{ 
    m_matrix._41 += x;
    m_matrix._42 += y;
    m_matrix._43 += z;
    CallObserver(false);
}

void Transform::MakeIdentity()
{
    m_scaleFactor.x = 1.0f;
    m_scaleFactor.y = 1.0f;
    m_scaleFactor.z = 1.0f;
    D3DXMatrixIdentity(&m_matrix);
    CallObserver(true);
}

void Transform::SetAxis(const D3DXVECTOR3& up,
    const D3DXVECTOR3& forward, const D3DXVECTOR3& right)
{
    m_matrix._11 = right.x;
    m_matrix._12 = right.y;
    m_matrix._13 = right.z;
    m_matrix._21 = up.x;
    m_matrix._22 = up.y; 
    m_matrix._23 = up.z;
    m_matrix._31 = forward.x;
    m_matrix._32 = forward.y; 
    m_matrix._33 = forward.z;
}

D3DXVECTOR3 Transform::Right() const
{ 
    return D3DXVECTOR3(m_matrix._11,m_matrix._12,m_matrix._13);
}

D3DXVECTOR3 Transform::Up() const
{ 
    return D3DXVECTOR3(m_matrix._21,m_matrix._22,m_matrix._23); 
}

D3DXVECTOR3 Transform::Forward() const
{ 
    return D3DXVECTOR3(m_matrix._31,m_matrix._32,m_matrix._33); 
}

D3DXVECTOR3 Transform::Position() const
{ 
    return D3DXVECTOR3(m_matrix._41,m_matrix._42,m_matrix._43); 
}

void Transform::Multiply(const D3DXMATRIX& matrix)
{
    //given matrix assumed to have no scaling
    m_matrix *= matrix;
}

void Transform::Equals(const D3DXMATRIX& matrix, const D3DXVECTOR3& scalefactor)
{
    m_matrix = matrix;
    m_scaleFactor = scalefactor;
}
