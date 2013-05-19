
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#include "vld/include/vld.h"
#endif

#include "simulation.h"
#include <windows.h>
#include <windowsx.h>

HWND hWnd = nullptr;                     ///< Handle to the window
LPDIRECT3DDEVICE9 d3ddev = nullptr;      ///< DirectX device
LPDIRECT3D9 d3d = nullptr;               ///< DirectX interface
LPDIRECT3DSURFACE9 backBuffer = nullptr; ///< Back buffer
std::unique_ptr<Simulation> simulation;  ///< Main simulation object

void InitialiseWindow(HINSTANCE hInstance, int cmdShow);
bool HandleMessages();
bool InitDirect3D();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow)
{
    simulation.reset(new Simulation());
    InitialiseWindow(hInstance, cmdShow);

    bool successful = InitDirect3D() && 
        simulation->CreateSimulation(hInstance, hWnd, d3ddev);

    while(successful && HandleMessages())
    {
        simulation->Update();
        simulation->Render();
    }

    simulation->Release();
    d3ddev->Release();
    d3d->Release();
    backBuffer->Release();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_MOUSEMOVE:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            simulation->SetMouseCoord(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
} 

void InitialiseWindow(HINSTANCE hInstance, int cmdShow)
{
    std::string windowName = "ClothSimulation";

    //Register the window
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc; 
    wc.hInstance = hInstance; 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
    wc.lpszClassName = WINDOW_NAME;

    RegisterClassEx(&wc); 

    //Display the window
    int win_x_position = (GetSystemMetrics(SM_CXSCREEN)/2)-(WINDOW_WIDTH/2); 
    int win_y_position = (GetSystemMetrics(SM_CYSCREEN)/2)-(WINDOW_HEIGHT/2);

    hWnd = CreateWindowEx(NULL,WINDOW_NAME,WINDOW_NAME,
                          WS_EX_TOPMOST | WS_POPUP, //window style (WS_EX_TOPMOST for no borders)
                          win_x_position, win_y_position, WINDOW_WIDTH, WINDOW_HEIGHT,
                          NULL, NULL, hInstance, NULL); 

    ShowWindow(hWnd, cmdShow);
}

bool HandleMessages()
{
    static MSG msg;
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return FALSE;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // check the escape key and quit
    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
    {
        PostMessage(hWnd, WM_DESTROY, 0, 0);
        return false;
    }
    return true;
}

bool InitDirect3D()
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
    if(SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, TEXTURE_FORMAT, 
                                                 true, D3DMULTISAMPLE_2_SAMPLES, nullptr)))
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
                                D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                &d3dpp, &d3ddev)))
    {
        Diagnostic::ShowMessage("Direct3D interface creation has failed");
        return false;
    }

    // Create Z-buffer
    if(FAILED(d3ddev->CreateDepthStencilSurface(WINDOW_WIDTH, WINDOW_HEIGHT, BACKBUFFER_FORMAT,
                                                antiAliasing ? antiAliasingLvl : D3DMULTISAMPLE_NONE,
                                                NULL, TRUE, &backBuffer, NULL)))
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