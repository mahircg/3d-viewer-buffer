# Introduction #

This documentation explains;
  * How server application runs
  * Requirements for run-time and compiling
  * How to create the project
  * Explanation of crucial parts of source code
  * Mechanisms of image capturing and creating connection

## Project Properties ##

Program is written with C++ using Microsoft Foundation Class Library(MFC version:9.0.21022.8 (mfc90.dll) )

Visual C++ 2008 is used as development environment.

Hardware and software specifications of the computer that the project is developed in;
  * Windows XP Home Edition,Service Pack 3
  * Intel Pentium Dual CPU E2160 @ 1.80 Ghz
  * 2.0 GB RAM

## Opening the Project ##

In order to open the project ,following steps are followed;
  1. Open Visual Studio 2008, _Click File->New->Project_ on the strip menu.
  1. Click and expand _Visual C++_ button on the left menu,then select MFC on left menu again.After clicking MFC,on the right menu,under _Visual Studio installed templates_,select MFC application,give a name to project,then click OK.
  1. _MFC Application Wizard_ will be opened.On the first page click next.
  1. _Application Type_ page will be opened.On this page,under _Application Type_ label,select _Multiple documents_,and click Next.
  1. n the next page,under _Compound document support_ label,select _None_ and click Next.
  1. On _Document Template Strings_ menu,select appropriate names for the files,then click Next( Because the name _IS Capture_ is changed as project name in the beginning of the project,these name may differ if application name changes)
  1. Select _None_ on _Database support_ page and click Next
  1. Do not make changes in _User Interface Features_ page and click Next.
  1. Do not make changes in _Advanced Features_ page and click Next.
  1. On _Generated Classes_ page,you can specify the class and header file names for the project.For _IS Capture_ project,**CISCaptureApp** has been chosen as application class name.
  1. Click _Finish_ and the project is now ready.

## Setting Project Properties ##

In order to compile and link the application,following steps must be followed;
  1. Click _<Project Name> Properties_ button ,under _Project_ menu.
  1. Select _General_  on the left menu.Change _Output Directory_ property of the project to **C:\Program Files\The Imaging Source Europa GmbH\IC Imaging Control 3.0\classlib\debug**. After this change,all the output of the building(executable file,object file,etc.) will be saved to that output directory.Because all the library files that are required for executable to run are saved in that output directory,output directory is changed.You can pass over this step by copying all the necessary library files to the _System32_ directory,or by changing the _Path_ variable under _Environment Variables_ to that output directory.
  1. Click on the _C/C++_ tab and choose _Code Generation_.Make sure a _Multi-Threaded Debug DKK_ is selected in the _Runtime Library" text box.
  1. Select_General_in_C/C++_tab and set the directory in the text box to **C:\Program Files\The Imaging Source Europa GmbH\IC Imaging Control 3.0\classlib\include**
  1. Select_General_in the_Linker_tab and set_Additional Library Directories_to **C:\Program Files\The Imaging Source Europa GmbH\IC Imaging Control 3.0\classlib\debug**_

### Important Note ###

In IC Imaging Capture Documentation, a change is mentioned , such that _Additional Dependencies_ text box under _Linker->Input_ must be changed  to reference **TIS\_UDSHL06\_vc71d.lib** and **strmiids.lib**.However,if you make such a change and use these library files explicitly,program crashes when user wants to capture left or right image.So,do not make such a change,and program will use the libraries(**TIS\_UDSHL07\_vc71d.lib**) under the directories mentioned above.

Of course some changes may be necessary on different environments,such as Windows Vista or a later operating system.

## Adding Buttons To Strip Menu ##

Thought it seems like an easy thing to add some button to strip menu using C# designer or some other visual designer, the case is not the same if project is based on MFC. Following steps must be followed in order to add strip menu buttons to the MFC application;

  1. In _Solution Explorer_, open _<Project Name>.rc_ resource file,under _Resource Files_ tab.
  1. Find the line `#include "afxres.h"` , then right click on the text "afxres.h", then click _Open Document "afxres.h"_ .Header file _afxres.h_ will be opened after this operation. This header file must be manipulated in order to define some messages.
  1. Find the comment line _E800 -> E8FF reserved for other control bar commands_ and then add the following lines below it;
    * `#define ID_CAPTURE_LEFT 0xE816`
    * `#define ID_CAPTURE_RIGHT 0xE817`
    * `#define ID_CAPTURE_BOTH 0xE818`
  1. Now,three messages are defined in the header file.Application will use these messages to communicate with controls.Be aware that,in the header file on server computer,the hexadecimal valuer that we assigned to each message are not used by other messages.If these values are used manually by other messages,you have to assign a value between reserved range.
  1. Close the header file and open the resource file again.Under _IDR`_` < Project Name > TYPE MENU_,add the following line to assign some title to the buttons.
```
POPUP "&Capture"
	BEGIN
		MENUITEM "&Capture Left\tCtrl+L",		ID_CAPTURE_LEFT
		MENUITEM "&Capture Right\tCtrl+R",		ID_CAPTURE_RIGHT
		MENUITEM "&Capture Both",				ID_CAPTURE_BOTH
	END
	POPUP "&Window"
	BEGIN 
```
You can add some other features,like key combinations for buttons or helper texts that will appear in the status bar.

After resource and header files are updated correctly,these messages must be mapped with the functions in the application.To do this, following lines must be added to lines under function **BEGIN`_`MESSAGE`_`MAP(CISCaptureApp, CWinApp)** and **END`_`MESSAGE`_`MAP()** the application class implementation file;
```
ON_COMMAND(ID_CAPTURE_LEFT,&CISCaptureApp::OnCaptureLeft)
	ON_COMMAND(ID_CAPTURE_RIGHT,&CISCaptureApp::OnCaptureRight)
	ON_COMMAND(ID_CAPTURE_BOTH,&CISCaptureApp::OnCaptureBoth)
```

With the lines above,the functions are mapped to the messages,which means that every time one of the messages are sent by controls,that functions will be running.

## Opening Camera Devices ##

In order to enable both camera devices,two grabber objects must be created for each device.These grabber objects will undertake the task as a bridge between application and devices.Two grabber objects are defined in the header as below;
```
DShowLib::Grabber* pGrabberLeft;
DShowLib::Grabber* pGrabberRight; 
```

The grabbers are initialized in the implementation file every time a new child window for device is created.

### Initialization of Grabber Objects ###
IS Capture application uses configuration files in order to initialize devices with specific options.Initialization of the left grabber is shown below;
```
if( setupDeviceFromFile(*pGrabberLeft,"firstDeviceProperties.xml"))
		{
			
			pGrabberLeft->setHWND(childFrameList[0]->m_hWnd);
                }
                pGrabberLeft->startLive(false);							
	        isDeviceOpen=true;			
	        theApp.logMessage="Left camera is ready";
	        theApp.allowMessage=true;
```

After the function _setupDeviceFromFile_ is called,a dialog box appears.If the configuration file is not corrupted and exists,all the info on dialog box will be filled with the info in configuration file.Otherwise administrator has to specify the settings.Anyways,all these steps are printed in the backlog.

The window that first device will be displayed is determined after device is opened.Be aware that,the live stream will not be on the child window because live mode did not started yet on grabber object.

After these initialization, _startLive_ function is called with the argument `false`,because we do not want the window to display the stream,because of reasons mentioned in the documentation IS Capture Server Documentation.

Mechanism is the same for the right camera.

## Capturing the Images ##

In order to capture images from camera,some functions must be called after each device initialization.Steps below must be followed in order to capture image;

  1. Create a handler sink with the required color format and buffer count.
    * ` pSinkLeft = DShowLib::FrameHandlerSink::create( DShowLib::eRGB24, 1); `
  1. Enable capturing mode
    * ` pSinkLeft->setSnapMode(true);`
  1. Set the sink type for the grabber object.
    * `pGrabberLeft->setSinkType(pSinkLeft);`
  1. Check whether specified color format and buffer count is suitable for the grabber object
    * `if(!pGrabberLeft->prepareLive(false)){} `
  1. Get the output(image that is going to be sent) info.This info contains color format,buffer size,etc.
    * `pSinkLeft->getOutputFrameType( info );`

After these steps are done,image can be captured with the following steps;
  1. Create a new byte array to store the image with the size of the image
    * `leftBuffer[0]=new BYTE[info.buffersize];`
  1. Create a new memory buffer that the grabber will copy the byte stream firstly.
    * `leftCollection=DShowLib::!MemBufferCollection::create(info,1,leftBuffer);	`
  1. Bind the sink and memory buffer.All the bytes will be copied from memory stream to sink.
    * `pSinkLeft->setMemBufferCollection(leftCollection)`
  1. Lastly,snap the images.
    * `pSinkLeft->snapImages(1);`

In a possible need of saving the captured image,the commented line on the program can be uncommented and used.

## Connection Mechanism and Transferring Images ##

Information about properties of the communication between client and server can be found at ConnectionMechanism document.

In this part of documentation,communication implementations will be explained.

### Initializing Sockets ###

In order to initialize the host and client sockets,following steps must be followed;

  1. Declare a SocketData object which includes all the socket variables.
    * `SocketData socketElements;`
  1. Specify the socket properties that host is going to listen ( lines 402-406 in the code)
  1. Resolve the port and address number of host(line 409)
  1. Initialize server socket with the properties set.(line 416)
  1. Bind the socket tho the network system(line 427)

After these steps are done properly,application can listen for incoming calls.

### Establishing Communication ###

In order to establish a communication between client and server,following steps must be done;

  1. Listen the socket for the server(line 442)
  1. If client sent a connection request,declare a client socket and accept the calls over it(lines 449-452).With this client socket,application will send and receive streams with this socket.


### Send and Receive Packets ###

For client and server to communicate in a specific way,both applications use connection messages,that mentioned in the document ConnectionMessages. Steps for sending and receiving images are explained below;
  1. Start a loop that executes until client disconnects.(line 467)
  1. Wait for incoming messages(line 469)
  1. After that point ,there may be combination of messages between client and sever.Let us assume that client sends GET\_LEFT message and left camera is ready.
  1. Client sends GET\_LEFT image and server receives.
  1. Check if device is ready or not(device is ready in that case)(line 473)
  1. Capture the left image(line 476)
  1. Send the image size in bytes.In order to send this value,first the integer must be converted from integer to byte array.This operations are done in line 477.
  1. After image size is transferred,transfer image stream(line 478)

Images are sent using the mechanism above.It is the same for sending left,right image or pair of images.

## Threading Mechanism ##

In the application,three threads(two child and one main) are running.

One of the thread's runs the function InitializeSocket and responsible for remote operations.This function runs as long as the program is open.
`goto` and `while` statements in the function provides an infinite loop on the function.

Second thread is responsible for printing the actions that happened in both main thread and communication thread.This thread,also,runs as long as the main thread is open.This thread runs HandleLogWindow function.


Source code for the application class can be found at _Downloads_ section.


