////////////////////////////////////////////////////////////////////////////////////////
// Kara Jensen - mail@karajensen.com - directx.cpp
////////////////////////////////////////////////////////////////////////////////////////

#include "directx.h"
#include "common.h"

DirectX::DirectX() :
    m_swapchain(nullptr),
    m_device(nullptr),
    m_context(nullptr),
    m_backbuffer(nullptr)
{
}

DirectX::~DirectX()
{
    m_backbuffer->Release();
    m_swapchain->Release();
    m_context->Release();
    m_device->Release();
}

bool DirectX::Initialise(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    scd.BufferCount = 1;                                 
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;

    if(FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, 
        NULL, D3D11_SDK_VERSION, &scd, &m_swapchain, &m_device, NULL, &m_context)))
    {
        Diagnostic::ShowMessage("Direct3D interface creation has failed");
        return false;
    }

    // Create the back buffer
    ID3D11Texture2D* bbuffer;
    m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bbuffer);
    m_device->CreateRenderTargetView(bbuffer, NULL, &m_backbuffer);
    bbuffer->Release();
    m_context->OMSetRenderTargets(1, &m_backbuffer, NULL);

    // Create the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = WINDOW_WIDTH;
    viewport.Height = WINDOW_HEIGHT;
    m_context->RSSetViewports(1, &viewport);


    return true;
}

void DirectX::BeginRendering()
{
    m_context->ClearRenderTargetView(m_backbuffer, 
        D3DXCOLOR(0.745f, 0.745f, 0.765f, 1.0f));
}

void DirectX::EndRendering()
{
    m_swapchain->Present(0, 0);
}