////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - utils.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "directx.h"
#include "transform.h"

#include <string>
#include <vector>
#include <sstream>

static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;
static constexpr int POINTS_IN_EDGE = 2;
static constexpr int POINTS_IN_FACE = 3;
static constexpr int POINTS_IN_TETRAHEDRON = 4;
static constexpr float CAMERA_FAR = 1000.0f;
static constexpr float CAMERA_NEAR = 1.0f;

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
