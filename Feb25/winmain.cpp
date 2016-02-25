//Sipes, Patricoa
//Chap 3.
//D3D Full Screen Program
//Pg 83, Ex 2: Change the resolution
//Resolution set to 800 x 600 (ln 21/22, SCREENW, SCREENH)


#include <windows.h>
#include <d3d9.h>

#include <time.h>
#include <iostream>

using namespace std;

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

//prgoram settings
const string APPTITLE = "Full Screened"; //for full-screen is this really needed?
const int SCREENW = 800;
const int SCREENH = 600;

//Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;

bool gameover = false;

//macro to detect key presses
#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code)&0x8000)?1:0)

//Game Init function

bool Game_Init(HWND window)
{
              //where, main message, header, icon
   MessageBox( window, "~*HAMMERTIME*~", "   STOP   ", 0);
   
   //Initialize Direct3D
   d3d = Direct3DCreate9(D3D_SDK_VERSION);
   if (d3d == NULL)
   {
      MessageBox(window, "Error intitializing Direct3D", "Error", MB_OK);
      return 0;
   }

   //Direct3d presentation params
   D3DPRESENT_PARAMETERS d3dpp;
   ZeroMemory( &d3dpp, sizeof(d3dpp) );
   d3dpp.Windowed = FALSE;
   d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
   d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
   d3dpp.BackBufferCount = 1;

   d3dpp.BackBufferWidth = SCREENW;
   d3dpp.BackBufferHeight = SCREENH;
   d3dpp.hDeviceWindow = window;

   //create d3d device
   d3d->CreateDevice(
         D3DADAPTER_DEFAULT,
         D3DDEVTYPE_HAL,
         window,
         D3DCREATE_HARDWARE_VERTEXPROCESSING,
         &d3dpp,
         &d3ddev);

   if (d3ddev == NULL)
   {
      MessageBox(window, "Error creating D3D device", "Error", MB_OK);
      return 0;
   }

   return true;
}


//game update

void Game_Run(HWND hwnd)
{
   //make sure d3ddev is valid
   if(!d3ddev) return;

   //clear backbuffer to a Fuschiaish...
   d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(200,23,127), 1.0F, 0);

   //start rendering
   if (d3ddev->BeginScene())
   {
      //do something?
      
      //stop rendering
      d3ddev->EndScene();

      //copy back buffer on screen
      d3ddev->Present(NULL, NULL, NULL, NULL);
   }

   //check for esc key
   if (KEY_DOWN(VK_ESCAPE))
   {
      PostMessage(hwnd, WM_DESTROY, 0, 0);
   }

}

//Game shutown function

void Game_End(HWND hwnd)
{
   //display close message
   MessageBox(hwnd, "Program about to end", "Game_End", MB_OK);

   //free mem
   if(d3ddev)
   {
      d3ddev->Release();
   }
   if(d3d)
   {
      d3d->Release();
   }
}

//Windows event handling
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch( msg )
   {
      case WM_DESTROY:
         gameover = true;
         PostQuitMessage(0);
         return 0;
   }
   return DefWindowProc( hWnd, msg, wParam, lParam );
}

//Main Win entry func

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow )
{
   //set new win prop
   //prev found in MyRegisterClass function
   WNDCLASSEX wc;
   wc.cbSize = sizeof(WNDCLASSEX);
   wc.style  = CS_HREDRAW | CS_VREDRAW;

   wc.lpfnWndProc    = (WNDPROC)WinProc;
   wc.cbClsExtra     = 0;
   wc.cbWndExtra     = 0;
   wc.hInstance      = hInstance;
   wc.hIcon          = NULL;
   wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName   = NULL;
   wc.lpszClassName  = APPTITLE.c_str();
   wc.hIconSm        = NULL;
   RegisterClassEx(&wc);

   //create new window
   //previously found in InitInstance function
   HWND window = CreateWindow( APPTITLE.c_str(), APPTITLE.c_str(),
                               //WS_OVERLAPPEDWINDOW,
                               WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               SCREENW, SCREENH,
                               NULL, NULL, hInstance, NULL );

   //error creating window?
   if(window == 0)
   {
      return 0;
   }

   //display window
   ShowWindow( window, nCmdShow );
   UpdateWindow ( window );

   //Init Game
   if (!Game_Init(window))
   {
      return 0;
   }

   //main msg loop
   MSG message;
   while (!gameover)
   {
      if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
      {
         TranslateMessage(&message);
         DispatchMessage(&message);
      }
      Game_Run(window);
   }

   Game_End(window);

   return message.wParam;

}