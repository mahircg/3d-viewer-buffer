# Introduction #

This document demonstrates how to use the IS Capture Server Application.

# Running the Application #

In order to run the application,you can use both the shortcut on the desktop,or you can run the executable file named **IS Capture.exe** in the directory; **_C:\Program Files\The Imaging Source Europa GmbH\IC Imaging Control 3.0\classlib\debug_**.

## Requirements ##

To run the application properly, following SDK run time libraries must be installed;

**_Microsoft Windows SDK 6.0_**

**_IC Imaging Control 3.0 ( Requires a valid serial key)_**

In the server application,while implementing the program,output directory for the project has been chosen as the directory mentioned above.This directory also includes the .dll files for communicating the cameras.So it is not required to move the .dll files to the System32 directory.

The library files for establishing the connection is installed with Windows SDK to the Sytem32 file.In a possible missing library file error,library files can be copied to these directories.

Internet connection or LAN connection in Fontys building is required for communication purposes.

## Usage of Application ##

After running the program,there are some steps that has to be done in order to make the system work properly.

### Connection ###

In server side,there is nothing has to be done explicitly for establishing communication.If client sends a request,IS Server application responds to the client automatically,if server has a connection,without any extra effort.All these operations that are being done in background are echoed in the command line window.So administrator can see when the client connected,disconnected,made request,etc. As long as application is open,IS Capture listens for coming request all the time.

### Starting Connection and Capturing ###

Administrator has to start to camera with the application manually.Starting the cameras is the first step of the mechanism,otherwise server will not be able to send the images to client. So,after opening the application,the first step must be opening the cameras.This can be done with the following ways;
  1. Press CTRL+N or click File->New to open the left camera.
  1. There will be a dialog box that shows which options will be used with the device.Application uses configuration files called _firstDeviceSettings.xml_ and _secondDeviceSettings.xml_ to provide an option set that is compatible with the firewire bandwidth.So in that dialog box,do not change anything and click OK.
  1. Then do the same steps again for the right camera.Now there should be two child windows' that are empty.
  1. After doing that steps,cameras are now ready for snapping.There is no live stream on cameras.Reason for that is,if the stream is shown on the screen,because of intensive usage of bandwidth , application will crash after a while because of an error in library files.Note that Imaging Source SDK files do not have native support for two cameras on same bandwidth,so everything done in the application is written out of documentation.Details about these issues will be explained in the document that explains the mechanism of IS Capture software.Anyways,there may be an option switches between live stream  and suspended stream on server.
  1. After opening the camera,if the client is connected to the server,which administrator can follow from command line,client can receive images from cameras.All these actions are printed in the command line.As long as application is open,client can connect and receive images from server.

## Important Notes ##
  * As mentioned previously,server application listens for connections as long as it is open.This enables client to restart the client application and reconnect to the server.However,if server is restarted during a connection,client application must be restarted as well.
  * On the command line,if there is a stream of operations that executes concurrently,server application must be restarted,because the packets are entered in a loop,so server is sending all the bytes again and again.This issue is mentioned also in client documentation and will be fixed in later implementations.
  * While opening the camera devices,if program shows an error indicating that bandwidth is full,it means that configuration files have been corrupted or deleted.In that case,solution is to copy the configuration files in the desktop to the ...\classlib\debug directory.
  * It is not recommended to use some different pixel format while opening the cameras,because it may cause overflows in the client side.Furthermore,best combination of pixel format,resolution and refresh rate is the one mentioned in the camera specifications.
  * _Capture Left_ , _Capture Right_ and _Capture Both_ buttons do not saves the images in that release,because,it causes some performance issues while sending the images to the client,if application saves the image to disk every time. These options will be enabled by application with different function calls in the next release.