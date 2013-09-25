////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - directx.h
////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

/**
* DirectX engine
*/
class DirectX
{
public:

    DirectX();
    ~DirectX();
    bool Initialise(HWND hWnd);
    void BeginRendering();
    void EndRendering();

    ID3D11Device* GetDevice() { return m_device; }
    ID3D11DeviceContext* GetContext() { return m_context; }

private:

    IDXGISwapChain* m_swapchain;           ///< swap chain interface
    ID3D11Device* m_device;                ///< Direct3D device interface
    ID3D11DeviceContext* m_context;        ///< Direct3D device context
    ID3D11RenderTargetView* m_backbuffer;  ///< Frame back buffer

};