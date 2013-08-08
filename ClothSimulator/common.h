/****************************************************************
* Kara Jensen (mail@karajensen.com) 
* Common functions/variables for the simulator
*****************************************************************/

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

namespace DxConstant
{
    static const D3DXHANDLE DefaultTechnique("MAIN");
    static const D3DXHANDLE VertexColor("Color");
    static const D3DXHANDLE DiffuseTexture("DiffuseTexture");
    static const D3DXHANDLE CameraPosition("CameraPos");
    static const D3DXHANDLE WorldInverseTranspose("WorldInvTrans");
    static const D3DXHANDLE WordViewProjection("WorldViewProjection");
    static const D3DXHANDLE World("World");
    static const D3DXHANDLE AmbientColor("AmbientColor");
    static const D3DXHANDLE MeshColor("MeshColor");
    static const D3DXHANDLE DiffuseColor("DiffuseColor");
    static const D3DXHANDLE SpecularColor("SpecularIntensity");
    static const D3DXHANDLE AmbientIntensity("AmbientIntensity");
    static const D3DXHANDLE DiffuseIntensity("DiffuseIntensity");
    static const D3DXHANDLE SpecularIntensity("SpecularIntensity");
    static const D3DXHANDLE SpecularSize("SpecularSize");
    static const D3DXHANDLE LightPos("LightPos");
    static const D3DXHANDLE LightOn("LightOn");
}

template<typename T> T DegToRad(T degrees)
{
    return (D3DX_PI / 180.0f) * (degrees);
}

template<typename T> T RadToDeg(T radians)
{
    return (180.0f / D3DX_PI) * (radians);
}

typedef std::stringstream sstream;
template<typename T> std::string StringCast(const T& value)
{
    return static_cast<sstream&>(sstream() << value).str();
}

inline D3DXVECTOR3 MultiplyVector(const D3DXVECTOR3& vec1, const D3DXVECTOR3& vec2)
{
    return D3DXVECTOR3(vec1.x*vec2.x, vec1.y*vec2.y, vec1.z*vec2.z);
}