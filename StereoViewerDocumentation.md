# Documentation Version: v0.2 #

This documentation explains the mechanism of how the program runs and requirements for program in order to run.

This documentation is valid for version 0.2 . For later versions,there will be different documentations.

## _Introduction_ ##

Version 0.2 simply displays two images taken from Bumblebee2 3D Camera.
One image is taken from left lens and the other one from the right lens.

Application is written in C++ with WINAPI.

Graphically DirectX 9.0 API is used in order to display the images.

Program is written using Visual Studio 2008 Express Edition.


## _Requirements_ ##

### _Software Requirements_ ###
The requirements below are for runtime purposes.
  * Microsoft Windows XP or higher operating system.
  * Visual C++ 2008 Redistributable or a higher Version
  * IZ3D Stereo Driver for AMD Graphics card
  * AMD Graphics Card Drivers
  * DirectX 9.0 or higher

The requirements below are for debugging purposes.
  * Microsoft Visual Studio 2008 Express Edition or higher
  * DirectX SDK ( June 2010 Release)
  * IZ3D Stereo Driver for AMD Graphics card

_It is highly recommended to install the last versions all of the drivers._


### _Hardware Requirements_ ###

  * AMD Graphics Card with HD3D Support.
  * LCD Shutter Glasses
  * Stereo Monitor with at least @120 Hz  refresh rate
  * CPU with at least 1.8 gHz clock rate



## _Mechanism of Operation_ ##

  1. **Inclusion of the header files**
    * windows.h
    * d3dx9.h
    * AtiDx9Stereo.h
  1. **Initialization of device**
    * Create a IDirect3DDevice9 pointer in order to setup the environment.Device initialization must be made with a temporary D3DPRESENT\_PARAMETERS struct which has windowed=TRUE.Created window must be in windowed mode even if the application works on fullscreen mode.
    * Create an offscreen surface with the format FOURCC\_AQBS in order to communicate with the driver.Resolution of this surface must be 10x10 and must be created after creating the device immediately.This surface enables the stereo mode and allocates the swap chain for application.
    * In this step,application must reset the device with actual window parameters.In order to do that,all D3DPOOL\_DEFAULT resources must be released including the FOURCC\_AQBS surface.
    * After releasing all default pool resources,application resets the window with new parameters.In this parameters,windowed mode must be equal to FALSE.In order for stereo to work proper,application must work on fullscreen mode. Another thing here is,window must have at least 2 levels of multisampling for stereo mode.This can be done by assigning **D3DMULTISAMPLE\_2\_SAMPLES** to the parameter's multisample variable.
    * After this step,device must be reset with the parameters that I mentioned above(full-screen).
    * In this step,images must be loaded as textures to the VRAM. In order to display the images in stereo mode,images must be imported as textures(NOT surface). **D3DCreateTextureFromFile** function can be used for that purpes.
    * After loading images,two render target must be created both for left and right images.Driver automatically allocates them for left and right eye views.So there is nothing that programmer should do explicitly.One thing is important here,while rendering to these buffers,programmer must specify the swap chain number for left and right buffers.In application `iSwapChin` 0 is for left and `iSwapChain` 1 is for right buffer.As I mentioned,this requires no attention while creating the render targets.Another important thing about these left and right render targets is ,targets has to be created with the sizes of images.And these target's multisample quality must be same with the parameters of full-screen window.
    * Last thing in initialization is creating the `ID3DXSprite` sprite for both left and right.They will be used for drawing the pictures to the buffers.
    * So as a result,there will be six variables for images.These are;
      * Render Targets
        1. Direct3DSurface9 rightImageBack;
        1. Direct3DSurface9 leftImageBack;
      * Textures
        1. Direct3DTexture9 leftImageTexture
        1. Direct3DTexture9 rightImageTexture
      * ID3DXSprite
        1. PD3DXSPRITE rightSprite
        1. PD3DXSPRITE leftSprite
    * The reason of using sprites and textures will be explained in later steps.
  1. **Rendering Sprites**
    * Begin the scene using `IDirect3DDevice9::Begin()`
    * Set the `rightImageBack` as render target.**IMPORTANT!!!** Because the left image is number zero in swap chain,this has to be specified here with the `IDirect3DDevice9::SetRenderTarget(0 , rightImageBack)`.That tells the driver that this buffer will be sent to the right part of the glass.
    * After setting `rightImageBack` as render target,draw the right sprite to the `rightImageBack`._(Begin(),Draw(...),End() )_
    * Set the `leftImageBack` as render target.Because `leftImageBack` has number 1 in swap chain,this number has to be specified in SetRenderTarget function as well.
    * **IMPORTANT NOTE** In application Stereo Viewer v0.2,names are just opposite of the documentation while rendering(drawing right to left and drawing left to right).This naming confusion will be fixed in later version.
    * End the current scene.
    * Present the buffers to the screen.
    * Render() function must be called in a loop until program terminates.

## _Important Notes_ ##
  * The strings in program are string pointers of type LPCWSTR used by WinAPI.This type cannot be presented like normal strings.There are two options for using these strings in program.
    * One solution is using L or `_`T before every constant string.Example: L"Example String" or `_`T"Example string"
    * Another solution is using changing the development environment settings and using constant strings as usual.In order to do that follow these steps;
      * Go to _Project -> <Project Name>_ Properties
      * Click to _Configuration Properties_ on the left menu.
      * Click _General_ in the sub-menu.
      * Change the _Character Set_ to _Use Multi-Byte Character Set_ , on the Project Defaults sub-menu on the right panel.
      * Apply the settings.
    * Now you can use LPCWSTR types as normal strings like "Example String"
  * While using Using DirectX 9 headers,application must notify the linker to search for the additional libraries in the lib directory of current DirectX SDK. This notification,called comment, will be written to the object file of the .cpp file and linker will look for that .lib files in link step.In order to do that,application must have these pre-processor directives in header file;
    * `#pragma comment(lib,"d3dx9.lib")`
    * `#pragma comment(lib,"d3d9.lib")`
  * Do not use IDirect3DSurface9 as image surfaces in order to get the stereo view.Because DirectX 9 do not have native stereo support,program cannot display the two surfaces with image relatively.You may thinking using D3DLoadSurfaceFromFile and do the same operations described above.However,AMD's drivers do not allow this kind of operation unlike nVidia.With nVidia drivers,such a think is possible and creating the stereo view is more easier than AMD and makes sense.Programmer know what's happening in behind and nVidia has pretty good documentation about image stereo and video stereo. Personally, I do not recommend the documentation of AMD's Quad Buffer SDK because none of the functions are working with shutter glasses.Even though I delete the `EnableStereo` lines in AMD's sample program ,program still works in stereo mode. Using sprites instead of surfaces in AMD stereo works fine.That is why I used ID3DXSprite in application.One of the biggest problems that I faced was using surfaces instead of textures to make the stereo effect.With AMD drivers,it is impossible to do such a thing.If you take a look at both nVidia's and AMD's documentations about stereo viewing , you will see the difference in mechanism.
  * Stereo Viewer v0.2 may not work properly in different environments because of no tests has been applied to the application.The environment that we used for testing is ;
    * MSI AE2420 All-in-one PC.
    * Windows 7 64-bit
    * Last version of IZ3D Stereo Driver(v1.13)

## _Bugs_ ##
  * There is a strain between left and right images.I am currently working on that one.
  * If you move the cursor over the images,a flicker may occur.This problem will be solved in next version.

## _Related Links_ ##
  * [AMD Supported Graphics Cards](http://www.amd.com/us/products/technologies/amd-hd3d/pages/supported-hardware.aspx)
  * [AMD Driver Download](http://support.amd.com/us/gpudownload/Pages/index.aspx)
  * [IZ3D Driver Download ](http://www.iz3d.com/driver)
  * [Quad Buffer SDK Download](http://developer.amd.com/sdks/QuadBufferSDK/Pages/default.aspx)
  * [DirectX SDK Download](http://www.microsoft.com/download/en/details.aspx?id=6812)












