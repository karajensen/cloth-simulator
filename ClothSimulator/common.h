////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - common.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define _SCL_SECURE_NO_WARNINGS //for std::copy to directx vertex/index buffers

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "transform.h"

static const int NO_INDEX = -1;
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const int POINTS_IN_EDGE = 2;
static const int POINTS_IN_FACE = 3;
static const int POINTS_IN_TETRAHEDRON = 4;
static const float CAMERA_FAR = 1000.0f;
static const float CAMERA_NEAR = 1.0f;

/**
* Converts degrees to radians
*/
template<typename T> T DegToRad(T degrees)
{
    return (D3DX_PI / 180.0f) * (degrees);
}

/**
* Converts radians to degrees
*/
template<typename T> T RadToDeg(T radians)
{
    return (180.0f / D3DX_PI) * (radians);
}

/**
* Output the given value as a string
*/
template<typename T> std::string StringCast(const T& value)
{
    return static_cast<std::stringstream&>(
        std::stringstream() << value).str();
}

/**
* Shows a popup message
*/
inline void ShowMessageBox(const std::string& message)
{
    MessageBox(NULL, message.c_str(), TEXT("ERROR"), MB_OK);
}

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