///////////////////////////////////////////////////////////////////////////////////
//GLOBAL DIRECTX
///////////////////////////////////////////////////////////////////////////////////


#include "includes.h"
#include "global.h"


//DIRECTX
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3D9 d3d = NULL;
D3DPRESENT_PARAMETERS d3dpp;
LPDIRECT3DSURFACE9 z_buffer = NULL; 
D3DMULTISAMPLE_TYPE AntiAliasingLvl;
bool AntiAliasing = false;

//ASSETS
GAME Game;
CAMERA * Camera;

//INPUT
LPDIRECTINPUT8 din; 
LPDIRECTINPUTDEVICE8 dinkeyboard;
LPDIRECTINPUTDEVICE8 dinmouse;
INPUTDATA InputData; 


///////////////////////////////////////////////////////////////////////////////////
//@INITIALISING
///////////////////////////////////////////////////////////////////////////////////

// Create Direct3D interface and the Direct3D Device
void InitDirect3D()
{

	//CREATE DIRECT3D INTERFACE
	if(FAILED(d3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(NULL, TEXT("Direct3D interface creation has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//FILL OUT PARAMETERS FOR DEVICE
	BuildPresentParams();
		
	//CREATE THE DIRECT3D DEVICE
    if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, gwd.hWnd,
								D3DCREATE_HARDWARE_VERTEXPROCESSING,
								&d3dpp, &d3ddev)))
	{
		MessageBox(NULL, TEXT("Direct3D device creation has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}

	//CREATE ZBUFFER
	CreateZBuffer();

	//SET MULTISAMPING
	SetMultisampling();

	//CREATE ASSETS
	Game.InitialiseGame();
	
	//CREATE Camera
	Camera = new CAMERA(0.0f,0.0f,-30.0f); 
	Camera->CreateProjMatrix();
}

//build d3dpp struct
void BuildPresentParams()
{
	ZeroMemory(&d3dpp, sizeof(d3dpp)); //clear out the struct for use


	//CHECK IF MULTISAMPLING SUPPORTED FOR FULLSCREEN/HALFSCREEN
	if(SUCCEEDED(d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, TEXTURE_FORMAT, 
											     gwd.Windowed ? TRUE : FALSE, //windowed = true, fullscreen = false
											     D3DMULTISAMPLE_2_SAMPLES, NULL)))
	{
		//MessageBox(NULL, TEXT("Multisampling lvl 2 enabled"), TEXT("PC TEST"), MB_OK);
		d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		AntiAliasingLvl = D3DMULTISAMPLE_2_SAMPLES;
		AntiAliasing = true;
	}
	else
		AntiAliasing = false;


	//FILL IN D3DPP STRUCT
	d3dpp.hDeviceWindow = gwd.hWnd;
	d3dpp.Windowed = gwd.Windowed; //program windowed (true) or fullscreen (false)
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //discard old frames
	d3dpp.BackBufferFormat = TEXTURE_FORMAT;
	d3dpp.BackBufferWidth = gwd.WINDOW_WIDTH; // set the width of the buffer
    d3dpp.BackBufferHeight = gwd.WINDOW_HEIGHT; // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = TRUE; //z-buffer autorun
    d3dpp.AutoDepthStencilFormat = BACKBUFFER_FORMAT; 
}

void SetMultisampling()
{
	if(AntiAliasing)
		d3ddev->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
}

void CreateZBuffer()
{
    if(FAILED(d3ddev->CreateDepthStencilSurface(gwd.WINDOW_WIDTH, gwd.WINDOW_HEIGHT, BACKBUFFER_FORMAT,
												AntiAliasing ? AntiAliasingLvl : D3DMULTISAMPLE_NONE,
												NULL, TRUE, &z_buffer, NULL)))
	{
		MessageBox(NULL, TEXT("Z-buffer creation has failed"), TEXT("ERROR"), MB_OK);
		exit(EXIT_FAILURE);
	}
	d3ddev->SetRenderTarget(0,z_buffer);
}

///////////////////////////////////////////////////////////////////////////////////
//@CLOSE DIRECTX
///////////////////////////////////////////////////////////////////////////////////

// Close the Device and Direct3D
void CloseDirect3D()
{
	if(Camera != NULL) { delete Camera; }
	Game.ReleaseGame();
    d3ddev->Release();
    d3d->Release();
	z_buffer->Release();

	//for memory log
	d3dpp.~D3DPRESENT_PARAMETERS();
	d3ddev->~IDirect3DDevice9();
	z_buffer->~IDirect3DSurface9();
}

///////////////////////////////////////////////////////////////////////////////////
//@RENDERING
///////////////////////////////////////////////////////////////////////////////////

//Start rendering
void StartRender()
{
    d3ddev->BeginScene();
}

//Clear the buffer
void ClearRenderBuffer()
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(175, 172, 175), 1.0f, 0); //clear window to black
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0); //clear z-buffer
}

//Stop Rendering
void EndRender()
{
    d3ddev->EndScene();
    d3ddev->Present(NULL, NULL, NULL, NULL);
}


///////////////////////////////////////////////////////////////////////////////////
//@DIRECTINPUT
///////////////////////////////////////////////////////////////////////////////////

// Create DirectInput and the Keyboard and Mouse Devices
void InitDirectInput(HINSTANCE hInstance, GAMEWINDOW* gwd)
{
    DirectInput8Create(hInstance,
                      DIRECTINPUT_VERSION,
                      IID_IDirectInput8,
                      (void**)&din,
                      NULL);

    din->CreateDevice(GUID_SysKeyboard, &dinkeyboard, NULL);
    din->CreateDevice(GUID_SysMouse, &dinmouse, NULL);

    dinkeyboard->SetDataFormat(&c_dfDIKeyboard);
    dinmouse->SetDataFormat(&c_dfDIMouse);

    dinkeyboard->SetCooperativeLevel(gwd->hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    dinmouse->SetCooperativeLevel(gwd->hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

    dinmouse->Acquire();
}

// Get the Current Keyboard Status
void GetKeys(BYTE* KeyState)
{
    dinkeyboard->Acquire();
    dinkeyboard->GetDeviceState(256, (LPVOID)KeyState);

    for(int Index = 0; Index < 256; Index++)
        *(KeyState + Index) &= 0x80;
}

// Get the Latest Mouse Movements and Mouse Buttons
void GetMouse(DIMOUSESTATE* MouseState)
{
    dinmouse->Acquire();
    dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)MouseState);
    MouseState->rgbButtons[0] &= 0x80;
    MouseState->rgbButtons[1] &= 0x80;
    MouseState->rgbButtons[2] &= 0x80;
    MouseState->rgbButtons[3] &= 0x80;
}

// Close the Devices and DirectInput
void CloseDirectInput(void)
{
    dinkeyboard->Unacquire();
    dinmouse->Unacquire();
    din->Release();
}


