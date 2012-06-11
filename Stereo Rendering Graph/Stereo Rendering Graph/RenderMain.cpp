#include "stdafx.h"				//For pre-compiled headers
#include "util.h"				
#include "CustomAllocator.h"
#include"DShowHelper.h"

#define MAX_LOADSTRING 100

//Forward declarations
BOOL                MyRegisterClass(HINSTANCE hInstance);				//Registers created window
BOOL                InitInstance(HINSTANCE, int, HWND& );				//Creates the window
void                PaintWindow(HWND hWnd);								//Brushes the window when there is no playback
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);				//Callback function for I/O and internal messages
HRESULT             StartGraph(HWND window);							//For running the graph
HRESULT             CloseGraph(HWND window);							//Deallocates all the resources
HRESULT             SetAllocatorPresenter( IBaseFilter *filter, HWND window );	//Initializes DirectX resources and starts rendering
BOOL                VerifyVMR9(void);									//Checks if VMR9 is supported by the system
BOOL				DisplayPropertyPage(IBaseFilter *filter);			//Displays property page for given filter,if filter supports.



SmartPtr<IGraphBuilder>          g_graph;							
SmartPtr<IMediaControl>          g_mediaControl;					
SmartPtr<IVMRSurfaceAllocator9>  g_allocator;

SmartPtr<IBaseFilter>            g_renderer_filter;
SmartPtr<IBaseFilter>            g_left_RTSP_source;
SmartPtr<IBaseFilter>            g_right_RTSP_source;
SmartPtr<IBaseFilter>            g_decoder_left;
SmartPtr<IBaseFilter>            g_decoder_right;


DWORD_PTR                       g_userId = 0xACDCACDC;
HWND                            g_hWnd;


HINSTANCE hInst;                       
TCHAR szTitle[MAX_LOADSTRING];						//Title for main window       
TCHAR szWindowClass[MAX_LOADSTRING];				//Registered class name for main window



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	int ret = -1;

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if(!VerifyVMR9())
    {
        CoUninitialize();
        return FALSE;
    }
	__try 
    {
        MSG msg;
        HACCEL hAccelTable;

       
        MyRegisterClass(hInstance);

        // Perform application initialization:
        if (!InitInstance (hInstance, nCmdShow, g_hWnd)) 
        {
            // Exit the try block gracefully.  Returning FALSE here would
            // lead to a performance penalty unwinding the stack.
            __leave;
        }

        hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)"Allocator9");
		StartGraph(g_hWnd);
        // Main message loop:
        while (GetMessage(&msg, NULL, 0, 0)) 
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        ret = (int) msg.wParam;
    }
    __finally
    {
        // make sure to release everything at the end
        // regardless of what's happening
        CloseGraph(g_hWnd);
        CoUninitialize();
    }

    return ret;

	
}

HRESULT CloseGraph(HWND window)
{
    if( g_mediaControl != NULL ) 
    {
        OAFilterState state;
        do {
            g_mediaControl->Stop();
            g_mediaControl->GetState(0, & state );
        } while( state != State_Stopped ) ;
    }

    g_allocator    = NULL;        
    g_mediaControl = NULL;        
    g_renderer_filter       = NULL;        
    g_graph        = NULL;
    ::InvalidateRect( window, NULL, true );
    return S_OK;
}

HRESULT StartGraph(HWND window)
{
    // Clear DirectShow interfaces (COM smart pointers)
    CloseGraph(window);

    SmartPtr<IVMRFilterConfig9> filterConfig;					//IVMRFilterConfig9 interface configures operation mode and rendering mechanism for VMR9
	SmartPtr<IVMRMixerControl9> mixerControl;					//IVMRMixerControl9 interface allows to manipulate multiple filters
	


    HRESULT hr;
    
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&g_graph);		//Create graph instance.

    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_renderer_filter);		//Create VMR9 instance
    }

    if (SUCCEEDED(hr))
    {
        hr = g_renderer_filter->QueryInterface(IID_IVMRFilterConfig9, reinterpret_cast<void**>(&filterConfig));				//Get configuration for VMR9
    }

	if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_RTSPClient, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_left_RTSP_source);	//Create RTSP_Source instance for left stream
    }

	if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_RTSPClient, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_right_RTSP_source); //Create RTSP_Source instance for right stream
    }

	if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_FFDSHOW, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_decoder_left);		//Create ffdshow encoder instance for left stream
    }

	if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_FFDSHOW, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&g_decoder_right);	//Create ffdshow encoder instance for right stream
    }

	//Set rendering mode to VMR9 Renderless mode because rendering will be performed by custom allocator-presenter,not by VMR
    if (SUCCEEDED(hr))
    {
        hr = filterConfig->SetRenderingMode( VMR9Mode_Renderless );																				
	
    }

	//In order to enabe VMR Mixing Mode,number of streams has to be set to the number of input pins.
    if (SUCCEEDED(hr))
    {
        hr = filterConfig->SetNumberOfStreams(2);
				
    }

		if (SUCCEEDED(hr))
    {
        hr = g_renderer_filter->QueryInterface(IID_IVMRMixerControl9, reinterpret_cast<void**>(&mixerControl));		//Create VMR9 Mixer instance
    }


   

    if (SUCCEEDED(hr))
    {
        hr = g_graph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&g_mediaControl));					//Create media control that handles incoming messages
    }

	
	
	//AddFilter function inserts the instances to the graph.Without adding filters to the graph,trying to connect the filters will fail.
	g_graph->AddFilter(g_left_RTSP_source,NULL);		
	g_graph->AddFilter(g_right_RTSP_source,NULL);
	g_graph->AddFilter(g_decoder_left,NULL);
	g_graph->AddFilter(g_decoder_right,NULL);
	g_graph->AddFilter(g_renderer_filter,NULL);


	//Displaying property pages for left and right RTSP stream sources.Before program starts,user has to enter the channel address for both left and right streams

	DisplayPropertyPage(g_left_RTSP_source);
	DisplayPropertyPage(g_right_RTSP_source);

	

	 if (SUCCEEDED(hr))
    {
        hr = SetAllocatorPresenter( g_renderer_filter, window );			//Create DirectX Resources and switch to fullscreen.
    }

	if(FAILED(hr))
		DestroyWindow(window);

	hr=ConnectFilters(g_graph,g_left_RTSP_source,g_decoder_left);			//Connect RTSP sources to the decoder filters.
	hr=ConnectFilters(g_graph,g_right_RTSP_source,g_decoder_right);

	hr=ConnectFilters(g_graph,g_decoder_left,g_renderer_filter);			//Connect decoder filters to the renderer.
	hr=ConnectFilters(g_graph,g_decoder_right,g_renderer_filter);

	//In VMR Mixing mode,multiple pins are presented in a Z-order,that is the stream that has the biggest Z-order is visible on the screen.
	//Z-order is determined by the pin number.
	//Left stream is connected to pin 0,so its Z-order is 0.Right stream is connected to pin 1,so its Z-order is 1.In that case,right stream is visible on the screen.
	//In order make both streams visible,we divide the screen to half,and show the left stream at the top of the half,and right stream at the bottom of the half.

	VMR9NormalizedRect rLeft={0.0f,0.0f,1.0f,0.5f};							
	VMR9NormalizedRect rRight={0.0f,0.5f,1.0f,1.0f};

	mixerControl->SetOutputRect(0,&rLeft);
	mixerControl->SetOutputRect(1,&rRight);

	//After splitting,we run the graph.
	
	
    if (SUCCEEDED(hr))
    {
        hr = g_mediaControl->Run();
    }

   

    return hr;
}

BOOL DisplayPropertyPage(IBaseFilter *filter)
{
	HRESULT hr;
	ISpecifyPropertyPages *pProp;

	hr = filter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
		// Get the filter's name and IUnknown pointer.
		FILTER_INFO FilterInfo;
		hr = filter->QueryFilterInfo(&FilterInfo); 
		IUnknown *pFilterUnk;
		filter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		pProp->Release();
		OleCreatePropertyFrame(
			NULL,                   // Parent window
			0, 0,                   // Reserved
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);

		// Clean up.
		pFilterUnk->Release();
		FilterInfo.pGraph->Release(); 
		CoTaskMemFree(caGUID.pElems);
	}
	return hr;

}
HRESULT SetAllocatorPresenter( IBaseFilter *filter, HWND window )
{
    if( filter == NULL )
    {
        return E_FAIL;
    }

    HRESULT hr;

    SmartPtr<IVMRSurfaceAllocatorNotify9> lpIVMRSurfAllocNotify;
    FAIL_RET( filter->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, reinterpret_cast<void**>(&lpIVMRSurfAllocNotify)) );

    // create our surface allocator
    g_allocator.Attach(new CAllocator( hr, window ));
    if( FAILED( hr ) )
    {
        g_allocator = NULL;
        return hr;
    }

    // let the allocator and the notify know about each other
    FAIL_RET( lpIVMRSurfAllocNotify->AdviseSurfaceAllocator( g_userId, g_allocator ) );
    FAIL_RET( g_allocator->AdviseNotify(lpIVMRSurfAllocNotify) );
    
    return hr;
}
BOOL VerifyVMR9(void)
{
    HRESULT hr;

    // Verify that the VMR exists on this system
    IBaseFilter* pBF = NULL;
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL,
                          CLSCTX_INPROC,
                          IID_IBaseFilter,
                          (LPVOID *)&pBF);
    if(SUCCEEDED(hr))
    {
        pBF->Release();
        return TRUE;
    }
    else
    {
        MessageBox(NULL,
            TEXT("This application requires the VMR-9.\r\n\r\n")

            TEXT("The VMR-9 is not enabled when viewing through a Remote\r\n")
            TEXT(" Desktop session. You can run VMR-enabled applications only\r\n") 
            TEXT("on your local computer.\r\n\r\n")

            TEXT("\r\nThis sample will now exit."),

            TEXT("Video Mixing Renderer (VMR9) capabilities are required"), MB_OK);

        return FALSE;
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
    switch (message) 
    {
        
        case WM_PAINT:
            PaintWindow(hWnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				g_allocator.Release();
				DestroyWindow(g_hWnd);
				exit(1);
				break;
			}

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
void PaintWindow(HWND hWnd)
{
    bool bNeedPaint = false;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    if( g_mediaControl == NULL ) // we know that there is nothing loaded
    {
        bNeedPaint = true;
    }
    else
    {
        // If we have a movie loaded, we only need to repaint
        // when the graph is stopped
        OAFilterState state;
        if( SUCCEEDED( g_mediaControl->GetState(0, & state ) ) )
        {
            bNeedPaint = ( state == State_Stopped );
        }
    }

    if ( bNeedPaint ) 
    {
        RECT rc2;
        GetClientRect(hWnd, &rc2);
        FillRect(hdc, &rc2, (HBRUSH)(COLOR_WINDOW+1));
    }

    EndPaint( hWnd, &ps );
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow("STEREORENDER", szTitle, WS_OVERLAPPEDWINDOW,
                       100, 100, 600, 500, 
                       NULL, NULL, hInstance, NULL);
   

   if (!hWnd)
      return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
BOOL MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));

 
    wc.hInstance     = hInstance;
    wc.lpfnWndProc   = WndProc;
    wc.lpszClassName = "STEREORENDER";
    wc.lpszMenuName  = (LPCTSTR)"Stereo Render";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hbrBackground = NULL;        // No background brush
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon         = NULL;
    if(!RegisterClass(&wc))
    {
        return FALSE;
    }

    return TRUE;
}