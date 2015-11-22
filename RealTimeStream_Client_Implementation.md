# Introduction #

This document includes implementation details for the client side application of the system.

All the mechanism is demonstrated step by step.

It is recommended to take a look at the RealTimeStream\_Client\_Application document before reading the details.

_Stereo Rendering Graph.rar_ in Downloads page includes the project file that is described in this document.

## Environment ##

Visual Studio 2010 Premium Edition is used as IDE.

For library files and headers,last versions of Windows SDK, DirectX SDK(June 2011) is installed on the system.

# Project Properties #

## Filters ##

In order to build the graph and run it,we need to register the filters that we will use.Sequence of the graph that we will use is;

RTSP\_Source -> ffdshow Decoder -> VMR Custom Allocator

RTSP\_Source -> ffdshow Decoder -> VMR Custom Allocator

VMR Custom Allocator,in this case is not a registered filter,but we implement a custom VMR9 allocator/renderer and use it in the application.Other filters,ffdshow Decoder and RTSP\_Source filters need to be downloaded and registered.

### ffdshow Codec ###

For ffdshow codec,we use K-Lite Codec Pack that includes most of the necessary filters for multimedia applications,as well as ffdshow encoder and decoder.Version of the packet is not crucial,as long as 32-bit version of the packet is installed.In both client and sever applications,we use 32-bit version of ffdshow codec.You can use [this](http://www.codecguide.com/) link to install K-Lite Codec Pack.To verify that you have successively installed ffdshow codec,you need to open Graphedit ,press Ctrl + F ,and then click **Video Compressors**.In the sub-menu ,you can see **ffdshow Video Codec** and **ffdshow Video Encoder**,if installation was successful.

### RTSP Source Filter ###

We use RTSP\_Source filter in client side,in order to provide receive RTSP/RTP packets in client side. Filter is developed by a group of RTSP programmers called **RTPstream expert group**.Last version of the filter is published as demo,but fulfills all the transmission requirements.In order to register the filter,we need to download the filter first.You can use [this](http://rtpstream.com/files/RTSP_source.090204.zip) link to directly download the filter.You will see the file RTSP\_Source.ax in the compressed file.After extracting the file to a proper location(that you should not delete it from that locations later on),you need to run the windows library registration tool,**regsvr32**.After extraction of the file,you can run regsvr32 in that syntax; regsvr32 (Filter Directory)\RTSP\_Source.ax.If you do the operation in right way,you will see a confirmation screen.After that,you can see the filter in Graphedit under DirectShow Filters menu.

## Dependencies ##

In order to build the project,following libraries are included in the project.

### DirectShow BaseClass Libraries ###

You can find DirectShow base classes under the directory; "%(WindowsSDKDir)\v7.0\Samples\multimedia\directshow\baseclasses".On this folder,you need to build the solution file in order to user library files.For the future usage,you may need to use the Release version of the libraries,so it is better the build the project both in Debug and Release modes. For our project,we use Debug version of the libraries.After building the project,under Debug folder on current directory,you will see that library file **strmbasd.lib** is generated.

**Important Note:**If you use an 64-bit operating system,Samples folder for Windows SDK will be in _C:\Program Files\Windows SDK\..._ directory,not in _C:\Program Files(x86)\Windows SDK\..._ !

Depends on which architecture your system is built on,you need to add DirectShow library directory to the project as **Additional Library Directories**.For our project , one of the additional directories is;

**C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses\Debug**

### DirectX Libraries ###

DirectX libraries can be found at DirectX SDK directory.Important part here is that even if operating system uses 64-bit architecture, 32-bit library files have to be imported to the project.
So,for the DirectX libraries,following directory must be added to the project as Additional Library Directory;

**$(DXSDK\_DIR)\Lib\x86**

After specifying all the library directories,following library files must be added to the project as **Additional Dependency**.To see how to add Additional Dependencies to a project,you can take a look at the previous implementation documents that describes the procedure.

_strmbasd.lib;winmm.lib;d3d9.lib;d3dx9.lib;dxguid.lib;%(AdditionalDependencies)_

%(AdditionalDependencies) macro includes core libraries,such that; kernel32.lib,user32.lib,etc.

## Header Files ##

In order to include the necessary header files for the project,following directories must bu added to the project as **Additional Include Directory".**

  * **C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\baseclasses** for DirectShow base classes
  * **C:\Program Files\Microsoft SDKs\Windows\v7.0\Samples\multimedia\directshow\common** for common DirectShow headers
  * **C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include** for DirectX header files.

Another important thing about the header files is that project uses precompiled header file mechanism.In order to enable it,we include "stdafx.h" header file in the project.Precompiled headers reduces compile time and enables program not to compile some core header files in every build.

## Other Options ##

  * Character Set option for the project must be set to **Use Multi-Byte Character Set**

  * In order to reduce the compile time for the project and in the meantime,to exclude unnecessary API's(Sockets,Cryptography,etc), we use WIN32\_LEAN\_AND\_MEAN preprocessor definition(can be found at Preprocessor part under C/C++ section on Project Properties).

  * Code optimization is disabled for the project.Reason for that is default code optimization may cause unexpected results in real-time application.

  * Run-time library for linking is set to Multi-threaded Debug DLL,under Code Generation section in C/C++ menu.

  * Application is built as a Windows application.

After checking all the conditions are met that are described above,project now can be compiled.

On the External Dependencies folder at Solution Explorer,you can see all the headers that are included in the project.

# Implementation Details #

Briefly,in the application,we implement a custom VMR9 allocator/presenter,and then build the proper graph for connecting, retrieving and decoding both left and right streams from server.

## Imported Files ##

Following source files are used in the project.

### CustomAllocator.h ###
This file includes the class prototype for the custom VMR9 allocator/presenter.

### DShowHelper.h ###
This file includes the helper function definitions for the main application.Helper functions are responsible for connecting pins and filters in the graph.File also includes the CLSID definitions for non-default libraries such as RTSP\_Server and ffdshow decoder.

### utils.h ###
This file includes the helper functions for custom allocator/presenter.

### CustomAllocator.cpp ###
This file includes the class definition for custom allocator/presenter.

### RenderMain.cpp ###

This file includes the main function and some other function that initializes the graph(create,add,connect filters).

## Implementation ##

This part of the document will explain the implementation phase step by step.

### Implementing Custom Allocator/Presenter ###

In order to render the image pair to the screen,a default renderer does not work.So we have to implement a custom renderer for this purpose.

First a class must be implemented that uses  IVMRSurfaceAllocator9 and  IVMRImagePresenter9 interfaces.

  1. Create a class that inherits the public members of IVMRSurfaceAllocator9 and IVMRImagePresenter9  interfaces.In our application,**CAllocator** class is implemented for that purpose(CustomAllocator.h file).
  1. Implement all the abstract methods for both interfaces.We need to implement the functions that we will customize as well,for instance;rendering function,initializing function,etc.All the implemented functions can be found at the project file.Only the important function implementations will be demonstrated here.Note that in all functions,_!CAutoLock_ object is used in order to lock the scope,so that only one instance of the class can use the method.
    * Implement the function `InitializeDevice()` that allocates the buffer for frame transferred by downstream filter.It is important that allocating all the texture objects in this function.Because all the information about the stream on input pin is received in the scope of this function,we can allocate all the textures using the optimal parameters,for instance allocating textures regarding to the input stream's coordinates.This function also makes notifications for the current renderer class.
```
 D3DCAPS9 d3dcaps;
    DWORD dwWidth = 1;
    DWORD dwHeight = 1;
   
	m_D3DDev->GetDeviceCaps( &d3dcaps );
    if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( dwWidth < lpAllocInfo->dwWidth )
            dwWidth = dwWidth << 1;
        while( dwHeight < lpAllocInfo->dwHeight )
            dwHeight = dwHeight << 1;


        lpAllocInfo->dwWidth = dwWidth;
        lpAllocInfo->dwHeight = dwHeight;
    }

	lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;

     DeleteSurfaces();
	 m_surfaces.resize(*lpNumBuffers);

	 hr=m_lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(0) );

	 FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
                                    1, 
                                    D3DUSAGE_RENDERTARGET, 
                                    lpAllocInfo->Format, 
                                    D3DPOOL_DEFAULT /* default pool - usually video memory */, 
                                    & m_leftFrontTexture, NULL ) );

	 	 FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
                                    1, 
                                    D3DUSAGE_RENDERTARGET, 
                                    lpAllocInfo->Format, 
                                    D3DPOOL_DEFAULT /* default pool - usually video memory */, 
                                    & m_rightFrontTexture, NULL ) );

	  leftSourceRect.left=0;leftSourceRect.right=lpAllocInfo->dwWidth;leftSourceRect.top=0;leftSourceRect.bottom=(lpAllocInfo->dwHeight)/2;

	  rightSourceRect.left=0;rightSourceRect.right=lpAllocInfo->dwWidth;
	  rightSourceRect.top=(lpAllocInfo->dwHeight)/2;rightSourceRect.bottom=lpAllocInfo->dwHeight;
	
	 

	 if(FAILED(hr) && !(lpAllocInfo->dwFlags & VMR9AllocFlag_3DRenderTarget))
    {
        exit(1);
    }

	return hr;
	
   
}
```
> > It is important that `VMR9AllocFlag_TextureSurface` flag is used because we are going to render the frames to the textures first.It is not possible to draw primitives to the surfaces.
> > Then we flush the surface list and reallocate it such that it contains the allocated buffer that the downstream is providing.So frames coming from decoder will be held by this surface object.
> > We also make sure that we allocate a surface that its coordinates are big enough to hold the frames coming from decoder.
> > After creating both left and right textures as render targets with proper sizes,we create the boundary rectangles regarding to texture's coordinates.Reason for doing this is that after receiving frame from the decoder,we split the surface such a way that top half will contain the left frame and bottom-half will contain the right frame.So we arrange the rectangles in that order.
    * Implement the `CAllocator::CreateDevice()`function.This function creates the Direct3D device ,switches to full screen and creates the backbuffers for rendering.All these steps are the same with StereoViewerDocumentation,except that the _textures are not created in this functions scope_.Textures are created in a different way in this application.After this function is called,application switches to the fullscreen mode with device restart.Most important parts here are;
```
 D3DDISPLAYMODE currentMode;
	m_D3DDev->GetDisplayMode(0,&currentMode);

	resetParameters.BackBufferCount=1;
	resetParameters.MultiSampleType=D3DMULTISAMPLE_2_SAMPLES;	//Multisample type has to be 2 in order to stereo work.
	resetParameters.Flags=0;
	resetParameters.FullScreen_RefreshRateInHz = currentMode.RefreshRate;
	resetParameters.BackBufferHeight=currentMode.Height;
	resetParameters.BackBufferWidth=currentMode.Width;
	resetParameters.BackBufferFormat= (D3DFORMAT)currentMode.Format;
	resetParameters.Windowed=false;								//Fullscreen mode must be enabled for stereo mode.
	resetParameters.EnableAutoDepthStencil=false;
	
	resetParameters.SwapEffect=D3DSWAPEFFECT_DISCARD;
	resetParameters.PresentationInterval=2147483648;

	FAIL_RET(m_D3DDev->Reset(&resetParameters));

	
	FAIL_RET(m_D3DDev->CreateRenderTarget(resetParameters.BackBufferWidth,resetParameters.BackBufferHeight,resetParameters.BackBufferFormat,resetParameters.MultiSampleType
		,resetParameters.MultiSampleQuality,false,&m_leftImageBack,NULL));
	FAIL_RET(m_D3DDev->CreateRenderTarget(resetParameters.BackBufferWidth,resetParameters.BackBufferHeight,resetParameters.BackBufferFormat,resetParameters.MultiSampleType
		,resetParameters.MultiSampleQuality,false,&m_rightImageBack,NULL));

	m_D3DDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_leftImageBack);
	m_D3DDev->GetBackBuffer(1,0,D3DBACKBUFFER_TYPE_MONO,&m_rightImageBack);
```
> > Multi Sample Type property for reset parameters must be assigned to `D3DMULTISAMPLE_TYPE_2` in order to enable stereo mode.Refresh rate must be initialized to desktop refresh rate,which is 120 Hz.
> > After reset device parameters,another important thing is the order of the back buffers.We need to draw to both buffers in one presentation session,so we must present both stream in proper swap chain.A swap chain is a collection of buffers that are used for displaying frames to the user.We use first buffer for presenting left stream first,and second buffer to present right stream second.Detailed information about swap chain can be found on References part.
    * Implement the function `CAllocator::PresentImage()` that it is called whenever there is a frame calling from the downstream filter(decoder).Though actual drawing is done by another function,PresentImage function checks whether something went wrong during presenting the images such as device lost.
```
if( hr == D3DERR_DEVICELOST)
    {
        if (m_D3DDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) 
        {
            DeleteSurfaces();
            FAIL_RET( CreateDevice() );

            HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

            FAIL_RET( m_lpIVMRSurfAllocNotify->ChangeD3DDevice( m_D3DDev, hMonitor ) );

        }

        hr = S_OK;
    }
```
    * Implement the function **PresentHelper()** that is called inside **PresentImage()** to make the actual drawing to the screen.Mechanism for the drawing is explained in the document RealTimeStream\_Client\_Application.Note that this function takes a _IDirect3DSurface9_ object as an argument.With the arrangement we made with VMR9 Mixer Mode in the main function,input surface object in this function contains both frames such that left frame is in the top half of the surface,and right frame is in the bottom half of it.
      1. Copy input surface's top-half to the left texture as a texture.
```
lpPresInfo->lpSurf->GetContainer( __uuidof(IDirect3DTexture9),(LPVOID*)&m_sourceTexture);	//Get seperated frames as a texture.

	m_sourceTexture->GetSurfaceLevel(0,&m_sourceSurfaceTemp);						
	m_leftFrontTexture->GetSurfaceLevel(0,&m_targetSurfaceTemp);
	m_D3DDev->StretchRect(m_sourceSurfaceTemp,&leftSourceRect,m_targetSurfaceTemp,NULL,D3DTEXF_NONE);
```
      1. Copy input surface's bottom-hald to the right texture as a texture.
```
m_rightFrontTexture->GetSurfaceLevel(0,&m_targetSurfaceTemp);
	m_D3DDev->StretchRect(m_sourceSurfaceTemp,&rightSourceRect,m_targetSurfaceTemp,NULL,D3DTEXF_NONE);	//Copy the bottom side of the frame to the right stream.
```
> > > > Here,`leftSourceRect` is a RECT object and its boundaries enclose the top-half of the whole input surface.`rightSourceRect` is a RECT object as well,but its boundaries enclose the bottom-half of the whole surface.
      1. After splitting the surface to textures,now we can start rendering the left and right textures by changing the render target for each texture.
```
 HRESULT hr;
	m_D3DDev->BeginScene();		
	

	hr=m_D3DDev->SetRenderTarget(0,m_leftImageBack);						//Switch to left buffer.

	
	hr=m_leftSprite->Begin(D3DXSPRITE_ALPHABLEND);
	hr=m_leftSprite->Draw(m_leftFrontTexture,NULL,NULL,NULL,0xffffffff);	//Render left image to the left back buffer.
	hr=m_leftSprite->End();
	
	
	m_D3DDev->SetRenderTarget(0,m_rightImageBack);							//Switch to right buffer.						
	
	m_rightSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_rightSprite->Draw(m_rightFrontTexture,NULL,NULL,NULL,0xffffffff);		//Render right image to the right back buffer	
	m_rightSprite->End();
	
	
	m_D3DDev->EndScene();
    FAIL_RET( m_D3DDev->Present( NULL, NULL, NULL, NULL ) );			

    return hr;
```

> > > As stated in the previous documents,after starting to render the scene,we first draw the left texture to left buffer and present it,in the meantime,right part of shutter glasses is blackened.Then we draw the right texture to right buffer,and in the meantime,left part of the glass is blackened.Sending signals to the shutter glass is a part of the IR Emitter mounted on stereo screen. IZ3D drivers control all the signal processing between IR Emitter and shutter glasses.IZ3D libraries are loaded in the application when DirectX application switches to full screen rendering mode in 120 Hz.
  1. Define external GUID's for RTSP\_Source filter and ffdshow decoder,in the helper class for main function.You can find CLSID for the filters in GraphEdit or in registry.Another way to get detailed class info for the filter is to use freeware software,_DirectShow Filter Manager_.
  1. In RenderMain.cpp file,define StartGraph() function that initializes the graph and adds the necessary filters to it.For connecting filters,we use helper functions defined in DShowHelper.h .file.
    * Initialize the graph builder and filters.
```

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
    .
    .
    .
```
    * In order to render the stream with our custom renderer,not with default VMR,we need to set the rendering mode as **VMR9 Renderless Mode**
```
 if (SUCCEEDED(hr))
    {
        hr = filterConfig->SetRenderingMode( VMR9Mode_Renderless );																				
	
    }
```
    * For rendering multiple streams,VMR9 Mixer Mode must be enabled.Unlike VMR7,VMR9 enables Mixer Mode in an easier way.Setting the number of streams to a number greater than one enables Mixer Mode automatically.
```
 if (SUCCEEDED(hr))
    {
        hr = filterConfig->SetNumberOfStreams(2);
				
    }
```
    * After initializing Mixer Mode Control and Media Control interfaces,we can add the filter to the graph.
```

g_graph->AddFilter(g_left_RTSP_source,NULL);		
g_graph->AddFilter(g_right_RTSP_source,NULL);
g_graph->AddFilter(g_decoder_left,NULL);
g_graph->AddFilter(g_decoder_right,NULL);
g_graph->AddFilter(g_renderer_filter,NULL);

```
    * Before connecting the filters,we need to provide transmission information to the RTSP\_Stream source.It is important that the server graph is in running state in this phase,because RTSP\_Source filter has to provide media information to upstream filter in order to perform connection.After making sure that the server graph is running,in the property page for each source filter,we provide the channel address for each stream.
```
DisplayPropertyPage(g_left_RTSP_source);
DisplayPropertyPage(g_right_RTSP_source);
```
> > > You can see the definition of DisplayPropertyPage() function in source code.
    * After adding filters to the graph and providing channel information,application needs to initialize DirectX device and create surfaces,textures for drawing.It is important to call initialize function for custom allocator before connecting any filter to any input pin of custom renderer.
```
 if (SUCCEEDED(hr))
    {
        hr = SetAllocatorPresenter( g_renderer_filter, window );			//Create DirectX Resources and switch to fullscreen.
    }

	if(FAILED(hr))
		DestroyWindow(window);
```
    * If all the operations so far has been completed successively,we can connect the filters now.Note that as long as filters support **Smart Connect** method that enables automatic pin seek among the filters in graph,helper connector function binds the filter without specifying any explicit pin info.
```
hr=ConnectFilters(g_graph,g_left_RTSP_source,g_decoder_left);			//Connect RTSP sources to the decoder filters.
hr=ConnectFilters(g_graph,g_right_RTSP_source,g_decoder_right);

hr=ConnectFilters(g_graph,g_decoder_left,g_renderer_filter);			//Connect decoder filters to the renderer.
hr=ConnectFilters(g_graph,g_decoder_right,g_renderer_filter);
```
    * If every connection is completed successively,we need to make sure that both streams are rendered to the screen.In default VMR9 renderer,if there are multiple video streams connected to renderer filter,VMR9 puts the frames in to a order called Z-order that the stream has the biggest Z-order is rendered to the screen,and the other streams are not stretched in to a visible area.Furthermore,we arrange the visible area for the custom renderer such that upper half of the stretch area accommodates left frames and lower half of the area accommodates right frames.
```
        //In VMR Mixing mode,multiple pins are presented in a Z-order,that is the stream that has the biggest Z-order is visible on the screen.
	//Z-order is determined by the pin number.
	//Left stream is connected to pin 0,so its Z-order is 0.Right stream is connected to pin 1,so its Z-order is 1.In that case,right stream is visible on the screen.
	//In order make both streams visible,we divide the screen to half,and show the left stream at the top of the half,and right stream at the bottom of the half.

	VMR9NormalizedRect rLeft={0.0f,0.0f,1.0f,0.5f};							
	VMR9NormalizedRect rRight={0.0f,0.5f,1.0f,1.0f};
```
> > > In the _PresentHelper()_ function that explained above,we combine two streams and generate stereo vision on the screen regarding to this arrangement we made.
    * Lastly ,we run the graph.
  1. In main function ,we call the StartGraph() function and go into the loop that renders the stream frame by frame.Note that in every COM application,**CoInitialize()** function must be called before using any COM component.

All the important implementation details are explained in this documentation.Rest of the functions and details can be found on project file.Note that any slight chance on compiler options or dependencies may cause a hundred of error messages.So it is crucial to make the configuration in correct way.