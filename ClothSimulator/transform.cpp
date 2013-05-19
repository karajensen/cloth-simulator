
#include "transform.h"
#include "common.h"

Transform::Transform():
    m_fullUpdateFn(nullptr),
    m_positionalUpdateFn(nullptr),
    m_scaleFactor(1.0f,1.0f,1.0f)
{ 
    D3DXMatrixIdentity(&Matrix);
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
    Matrix._11 = x;
    Matrix._22 = y;
    Matrix._33 = z;
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

    D3DXVECTOR3 position(Matrix._41, Matrix._42, Matrix._43);
    SetPosition(0,0,0);

    D3DXMatrixScaling(&Matrix, x, y, z);

    SetPosition(position);
    CallObserver(true);
}

void Transform::RotateX(float radians, bool local)
{
    D3DXVECTOR3 position(Matrix._41, Matrix._42, Matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    float c = cos(radians);
    float s = sin(radians);
    rotation._22 = c;  
    rotation._32 = s;  
    rotation._23 = -s; 
    rotation._33 = c; 
    Matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::RotateY(float radians, bool local)
{
    D3DXVECTOR3 position(Matrix._41, Matrix._42, Matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    float c = cos(radians);
    float s = sin(radians);
    rotation._11 = c;  
    rotation._13 = s;  
    rotation._31 = -s; 
    rotation._33 = c; 
    Matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::RotateZ(float radians, bool local)
{
    D3DXVECTOR3 position(Matrix._41, Matrix._42, Matrix._43);
    if(local)
    {
        SetPosition(0,0,0);
    }

    D3DXMATRIX rotation;
    float c = cos(radians);
    float s = sin(radians);
    rotation._11 = c;  
    rotation._21 = s;  
    rotation._12 = -s; 
    rotation._22 = c; 
    Matrix *= rotation;

    if(local)
    {
        SetPosition(position);
    }
    CallObserver(true);
}

void Transform::SetPosition(const FLOAT3& pos)
{
    Matrix._41 = pos.x;
    Matrix._42 = pos.y;
    Matrix._43 = pos.z;
    CallObserver(false);
}

void Transform::SetPosition(float x, float y, float z)
{
    Matrix._41 = x;
    Matrix._42 = y;
    Matrix._43 = z;
    CallObserver(false);
}

void Transform::SetPosition(const D3DXVECTOR3& pos)
{
    Matrix._41 = pos.x;
    Matrix._42 = pos.y;
    Matrix._43 = pos.z;
    CallObserver(false);
}

void Transform::Translate(const D3DXVECTOR3& pos)
{
    Matrix._41 += pos.x;
    Matrix._42 += pos.y;
    Matrix._43 += pos.z;
    CallObserver(false);
}

void Transform::Translate(float x, float y, float z)
{ 
    Matrix._41 += x;
    Matrix._42 += y;
    Matrix._43 += z;
    CallObserver(false);
}

void Transform::MakeIdentity()
{
    m_scaleFactor.x = 1.0f;
    m_scaleFactor.y = 1.0f;
    m_scaleFactor.z = 1.0f;
    D3DXMatrixIdentity(&Matrix);
    CallObserver(true);
}

D3DXVECTOR3 Transform::Right() const
{ 
    return D3DXVECTOR3(Matrix._11,Matrix._12,Matrix._13);
}

D3DXVECTOR3 Transform::Up() const
{ 
    return D3DXVECTOR3(Matrix._21,Matrix._22,Matrix._23); 
}

D3DXVECTOR3 Transform::Forward() const
{ 
    return D3DXVECTOR3(Matrix._31,Matrix._32,Matrix._33); 
}

D3DXVECTOR3 Transform::Position() const
{ 
    return D3DXVECTOR3(Matrix._41,Matrix._42,Matrix._43); 
}
