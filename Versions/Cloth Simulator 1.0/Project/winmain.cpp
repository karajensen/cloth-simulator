///////////////////////////////////////////////////////////////////////////////////
//WINDOWS API
///////////////////////////////////////////////////////////////////////////////////

#include "includes.h"
#include "global.h"
#define WIN32_LEAN_AND_MEAN

//String conversion
const char * ConvertSTR(string & str) { return (str.c_str()); };

//Globals
GAMEWINDOW gwd(true,800,600);
int nCmdShow;
LPCSTR WindowName = TEXT("WindowClass");

///////////////////////////////////////////////////////////////////////////////////
//SETTING UP WINDOWS
///////////////////////////////////////////////////////////////////////////////////

//ENTRY POINT
int WINAPI WinMain(HINSTANCE Instance, //handle to instance of program; auto generated int
                   HINSTANCE hPrevInstance, //handle to previous instance; now NULL for all instances
                   LPSTR lpCmdLine, //Long pointer to string storing command line instructions for calling program
                   int CmdShow) //program state (minimised etc)
{
	HINSTANCE hInstance = Instance;
	nCmdShow = CmdShow;

	RegisterWindow(hInstance);
    DisplayWindow(hInstance);
    InitDirect3D();
	InitDirectInput(hInstance, &gwd);

	MainLoop();

    CloseDirect3D();
	CloseDirectInput();

	
    return 0;
}


void RegisterWindow(HINSTANCE hInstance)
{
	//REGISTERING THE WINDOW CLASS
	WNDCLASSEX wc; //struct containing info for the window class
    ZeroMemory(&wc, sizeof(WNDCLASSEX)); //initialise all to NULL

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW; //style of the window
    wc.lpfnWndProc = (WNDPROC)WindowProc; //tells window class what function to use when getting a message from Windows
    wc.hInstance = hInstance; //value of handle for application
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //default mouse image
    wc.lpszClassName = WindowName; //name of the window class

    RegisterClassEx(&wc); //register the window 
}

void DisplayWindow(HINSTANCE hInstance)
{
	//CREATING A WINDOW FROM THE CLASS
    gwd.hWnd = CreateWindowEx(NULL,WindowName,TEXT("Cloth Simulation"), //title of window
							  WS_EX_TOPMOST | WS_POPUP, //window style (WS_EX_TOPMOST for no borders)
                              gwd.win_x_position, //x position of window 
							  gwd.win_y_position, //y position of window
                              gwd.WINDOW_WIDTH, //width of window 
							  gwd.WINDOW_HEIGHT, //height of window
                              NULL, //tells which parent window created this window
                              NULL, //menu or not
                              hInstance, //application handle
                              NULL); //used when multiple windows

    ShowWindow(gwd.hWnd, nCmdShow);
}


//MESSAGE HANDLING: THE MAIN LOOP
bool HandleMessages()
{
    static MSG msg;

	//check to see if any messages are waiting in the queue (PeekMessage returns true if message, PM_REMOVE Removes it)
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT) //if the message is WM_QUIT, exit the inifinite while loop
            return FALSE;

        TranslateMessage(&msg);
        DispatchMessage(&msg); //calls WindowProc()
    }

	// check the escape key and quit
    if(KEY_DOWN(VK_ESCAPE))
	{
        PostMessage(gwd.hWnd, WM_DESTROY, 0, 0);
		return FALSE;
	}

    return TRUE;
}


//MESSAGE HANDLING: THE WINDOWPROC() FUNCTION
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			{
				InputData.mX = GET_X_LPARAM(lParam);
				InputData.mY = GET_Y_LPARAM(lParam);
				return 0;
				break;
			}
		case WM_KEYDOWN:
			{
				HandleKeyPress(wParam); 
				break;
				return 0;
			}
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
				break;
            }
    }

	//handle any message the switch didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
} 

//Handle key presses
void HandleKeyPress(UINT wParam)
{
	/*if (wParam == 0x08)//backspace
	{
		if (keyInput.length() > 0)//make sure not at start of string before erasing letter
		{
			string temp(&keyInput[0], &keyInput[keyInput.length()-1]); //copy of values minus last character
			keyInput = temp; //allocate temp to input string
		}
	}
	else if ((wParam == VK_SHIFT)||(wParam == VK_TAB)||(wParam == VK_CAPITAL)) //do nothing
	{
	}
    else if (keyInput.length() < 12)//check for name limit and tabs
	{
		if(GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_CAPITAL))
			keyInput += wParam;
		else
			keyInput += tolower(wParam);

		playerText.SetText(keyInput);
    }
	*/
}
