//
//Sipes, Patricia
//Beginning Game Programming
//Chapter 4, exercise 1
//Draw bitmap and constrain to portion of it.
//

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <iostream>


using namespace std;

//#pragma comment( lib, "d3d9.lib" )
//#pragma comment( lib, "d3dx9.lib" )

//program constants
const string APPTITLE = "Load Partial Bitmap";
const int SCREENW = 800;
const int SCREENH = 600;

//macro for keys
#define KEY_DOWN( vk_code ) (( GetAsyncKeyState( vk_code )&0x8000 )?1:0)


//D3D objects
LPDIRECT3D9          d3d        = NULL;
LPDIRECT3DDEVICE9    d3ddev     = NULL;
LPDIRECT3DSURFACE9   backbuffer = NULL;
LPDIRECT3DSURFACE9   surface    = NULL;

bool gameover = false;

//Game INIT
bool Game_Init(HWND window)
{
   //initialize D3D
   d3d = Direct3DCreate9( D3D_SDK_VERSION );
   if( !d3d )
   {
      MessageBox( window, "Error intializing D3D", "Error", MB_OK );
      return false;
   }

   //set D3D presentation params
   D3DPRESENT_PARAMETERS      d3dpp;
   ZeroMemory( &d3dpp, sizeof(d3dpp) );
   d3dpp.Windowed          =     TRUE;
   d3dpp.SwapEffect        =     D3DSWAPEFFECT_DISCARD;
   d3dpp.BackBufferFormat  =     D3DFMT_X8R8G8B8;
   d3dpp.BackBufferCount   =     1; //change for multi buffer system
   d3dpp.BackBufferWidth   =     SCREENW;
   d3dpp.BackBufferHeight  =     SCREENH;
   d3dpp.hDeviceWindow     =     window;

   //create d3d device
   d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
                      D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                      &d3dpp, &d3ddev );

   if( !d3ddev )
   {
      MessageBox( window, "Error creating D3D device", "Error", MB_OK );
      return false;
   }

 

   //clear the backbuffer to COLOR
   //why xrgb and not introduce argb sooner?
   d3ddev->Clear( 0, NULL,D3DCLEAR_TARGET, 
                  D3DCOLOR_XRGB(0,0,0), 
                  1.0f, 0 );
   
   
   //create surface
   HRESULT result = d3ddev->CreateOffscreenPlainSurface(
                              SCREENW,            //width of surface
                              SCREENH,            //height of surface
                              D3DFMT_X8R8G8B8,//SURFACE FORMAT
                              D3DPOOL_DEFAULT,//memory pool to use
                              &surface,       //pointer to surface
                              NULL );         //RESERVED, always NULL
   
   if( !SUCCEEDED(result) ) return false; //if fail, start abort
   
   D3DXIMAGE_INFO info;

    result = D3DXGetImageInfoFromFile( "spriteSheet.png", &info );
    if (result != D3D_OK )   return NULL;

    
    RECT srcRect;

    srcRect.top = 0;
    srcRect.left = 24;
    srcRect.bottom = 24;
    srcRect.right = 48;
   
   //load surface from file into newly created surface
   result = D3DXLoadSurfaceFromFile(
                  surface,  //dest
                  NULL,     //dest palette
                  NULL,     //dest rect
                  "spriteSheet.png",     //source
                  NULL //srcRect,     //source rec
                  D3DX_DEFAULT,
                  0, //transparency
                  NULL);   //source image info, usually null
   //file loaded?
   if(!SUCCEEDED(result))return false;
   return true;

}

//Update function

void Game_Run( HWND hwnd )
{
   //make sure d3ddev valid
   if( !d3ddev ) return; //fall out

   //pointer to blackness
   d3ddev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuffer);

   //start render
   if( d3ddev->BeginScene() )
   {

      d3ddev->StretchRect(surface, NULL, backbuffer, NULL, D3DTEXF_NONE);
      



      

      //stop render
      d3ddev->EndScene();

      //display back buffer to screen
      d3ddev->Present(NULL, NULL, NULL, NULL);
   }

   //check esc key
   if(KEY_DOWN(VK_ESCAPE))
      PostMessage(hwnd, WM_DESTROY, 0, 0);
}

//Game shutdown (destruction manager)
void Game_End(HWND hwnd)
{
   if( surface )surface->Release();
   if( d3ddev )d3ddev->Release();
   if( d3d )d3d->Release();
}


//Win event callback

LRESULT WINAPI WinProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_DESTROY:
         gameover = true;
         PostQuitMessage(0);
         return 0;
   }
   return DefWindowProc( hwnd, msg, wParam, lParam);
}


//entry point function

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow )
{
   //creat window class struct
   WNDCLASSEX wc;
   wc.cbSize        =     sizeof(WNDCLASSEX);
   wc.style         =     CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   =     (WNDPROC)WinProc;
   wc.cbClsExtra    =     0;
   wc.cbWndExtra    =     0;
   wc.hIcon         =     NULL;
   wc.hCursor       =     LoadCursor(NULL, IDC_ARROW);

   wc.hbrBackground =     (HBRUSH)GetStockObject(WHITE_BRUSH);
   wc.lpszMenuName  =     NULL;
   wc.lpszClassName =     APPTITLE.c_str();
   wc.hIconSm       =     NULL;
   RegisterClassEx(&wc);

   //create new window
   HWND window = CreateWindow(APPTITLE.c_str(), APPTITLE.c_str(),
                              WS_OVERLAPPEDWINDOW, 
                              CW_USEDEFAULT,CW_USEDEFAULT, 
                              SCREENW, SCREENH, NULL, NULL,
                              hInstance, NULL);
   //window error?
   if (window ==0)return 0;

   //display window
   ShowWindow(window, nCmdShow);
   UpdateWindow(window);

   //init
   if(!Game_Init(window))return 0;

   //main message loop
   MSG message;
   
   while( !gameover )
   {
      if( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
      {
         TranslateMessage(&message);
         DispatchMessage(&message);
      }
      Game_Run(window);
   }

   return message.wParam;

}


