////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - directx.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

/**
* Determines if the given vector is the zero vector
*/
inline bool IsZeroVector(const D3DXVECTOR3& vector)
{
    return vector.x == 0.0f && vector.y == 0.0f && vector.z == 0.0f;
}

/**
* Sets the given vector to the zero vector
*/
inline void MakeZeroVector(D3DXVECTOR3& vector)
{
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.0f;
}