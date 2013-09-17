////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - matrix.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "transform.h"
#include "common.h"

Matrix::Matrix()
{ 
    D3DXMatrixIdentity(&m_matrix);
}

void Matrix::SetPosition(const D3DXVECTOR3& position)
{
    m_matrix._41 = position.x;
    m_matrix._42 = position.y;
    m_matrix._43 = position.z;
}

void Matrix::SetPosition(float x, float y, float z)
{
    m_matrix._41 = x;
    m_matrix._42 = y;
    m_matrix._43 = z;
}

void Matrix::MakeIdentity()
{
    D3DXMatrixIdentity(&m_matrix);
}

D3DXVECTOR3 Matrix::Right() const
{ 
    return D3DXVECTOR3(m_matrix._11, m_matrix._12, m_matrix._13);
}                                                 
                                                  
D3DXVECTOR3 Matrix::Up() const                 
{                                                 
    return D3DXVECTOR3(m_matrix._21, m_matrix._22, m_matrix._23); 
}                                                 
                                                  
D3DXVECTOR3 Matrix::Forward() const            
{                                                 
    return D3DXVECTOR3(m_matrix._31, m_matrix._32, m_matrix._33); 
}                                                 
                                                  
D3DXVECTOR3 Matrix::Position() const           
{                                                 
    return D3DXVECTOR3(m_matrix._41, m_matrix._42, m_matrix._43); 
}

void Matrix::Multiply(const D3DXMATRIX& matrix)
{
    m_matrix *= matrix;
}

void Matrix::Set(const D3DXMATRIX& matrix)
{
    m_matrix = matrix;
}

void Matrix::SetAxis(const D3DXVECTOR3& up,
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