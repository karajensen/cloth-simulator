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
#include "diagnostic.h"

static const int NO_INDEX = -1;
static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const int FULL_ALPHA = 255;
static const int HALF_ALPHA = 128;
static const LPCSTR WINDOW_NAME("Cloth Simulator");
static const D3DFORMAT BACKBUFFER_FORMAT(D3DFMT_D16);
static const D3DFORMAT TEXTURE_FORMAT(D3DFMT_A8R8G8B8);

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
typedef std::stringstream sstream;
template<typename T> std::string StringCast(const T& value)
{
    return static_cast<sstream&>(sstream() << value).str();
}

/**
* Mesh vertex structure
*/
struct Vertex
{
    D3DXVECTOR3 position;   ///< Vertex position
    D3DXVECTOR3 normal;     ///< Vertex normal
    D3DXVECTOR2 uvs;        ///< Vertex UV information

    /**
    * Constructor
    */
    Vertex() :
        position(0.0f, 0.0f, 0.0f),
        normal(0.0f, 0.0f, 0.0f),
        uvs(0.0f, 0.0f)
    {
    }
};
