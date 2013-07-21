
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#include "vld/include/vld.h"
#endif

#include "simulation.h"
#include "GUIWrapper.h"
#include "GUICallbacks.h"
#include <windows.h>
#include <windowsx.h>

HWND hWnd = nullptr;                     ///< Handle to the window
LPDIRECT3DDEVICE9 d3ddev = nullptr;      ///< DirectX device
LPDIRECT3D9 d3d = nullptr;               ///< DirectX interface
LPDIRECT3DSURFACE9 backBuffer = nullptr; ///< Back buffer

bool runSimulation = true;               ///< Whether simulation can be run or not
std::unique_ptr<Simulation> simulation;  ///< Main simulation object
std::unique_ptr<GUI::GuiWrapper> gui;    ///< Interface for .NET GUI
GUI::GuiCallbacks callbacks;             ///< Callback list for the GUI

bool InitialiseGUI();
bool InitialiseDirectX();
bool InitialiseWindow(HINSTANCE* hInst);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow)
{
    gui.reset(new GUI::GuiWrapper());
    simulation.reset(new Simulation());

    runSimulation = 
        InitialiseWindow(&hInstance) && 
        InitialiseDirectX() &&  
        simulation->CreateSimulation(hInstance, hWnd, d3ddev) &&
        InitialiseGUI();

    while(runSimulation && gui->Update())
    {
        simulation->Update();
        simulation->Render();
    }

    d3ddev->Release();
    d3d->Release();
    backBuffer->Release();

    #ifdef _DEBUG
    OutputDebugString("Exiting Simulation\n");
    #endif
}

bool InitialiseGUI()
{
    callbacks.quitFn = [](){ runSimulation = false; };
    simulation->LoadGuiCallbacks(&callbacks);
    gui->SetCallbacks(&callbacks);
    gui->Show();
    return true;
}

bool InitialiseWindow(HINSTANCE* hInst)
{
    auto windowHandles = gui->GetWindowHandle();
    hWnd = windowHandles.handle;
    *hInst = windowHandles.instance;
    return true;
}

bool InitialiseDirectX()
{
    if(FAILED(d3d = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        Diagnostic::ShowMessage("Direct3D interface creation has failed");
        return false;
    }

    // Build present params
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    D3DMULTISAMPLE_TYPE antiAliasingLvl;
    bool antiAliasing = false;
    if(SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
        D3DDEVTYPE_HAL, TEXTURE_FORMAT, true, D3DMULTISAMPLE_2_SAMPLES, nullptr)))
    {
        d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
        antiAliasingLvl = D3DMULTISAMPLE_2_SAMPLES;
        antiAliasing = true;
    }
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.Windowed = true;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = TEXTURE_FORMAT;
    d3dpp.BackBufferWidth = WINDOW_WIDTH; 
    d3dpp.BackBufferHeight = WINDOW_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = BACKBUFFER_FORMAT; 

    if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddev)))
    {
        Diagnostic::ShowMessage("Direct3D interface creation has failed");
        return false;
    }

    // Create Z-buffer
    if(FAILED(d3ddev->CreateDepthStencilSurface(WINDOW_WIDTH, WINDOW_HEIGHT, BACKBUFFER_FORMAT,
        antiAliasing ? antiAliasingLvl : D3DMULTISAMPLE_NONE, NULL, TRUE, &backBuffer, NULL)))
    {
        Diagnostic::ShowMessage("Z-buffer creation has failed");
        return false;
    }
    d3ddev->SetRenderTarget(0,backBuffer);

    // Set render states
    d3ddev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, antiAliasing);

    // Check shader capabilities
    D3DCAPS9 caps;
    d3ddev->GetDeviceCaps(&caps);

    // Check for vertex shader version 2.0 support.
    if(caps.VertexShaderVersion < D3DVS_VERSION(2, 0)) 
    {
        Diagnostic::ShowMessage("Shader model 2.0 or higher is required");
        return false;
    }

    return true;
}