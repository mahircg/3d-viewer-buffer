# Introduction #

This document explains the steps for starting and using the static multi-view image stereo system.

In order to start connection between client and server and start transferring images,first server application **IS Capture v0.1** must be opened.After,client application "Stereo Viewer v0.4" can demand and display the image pair in stereo mode.

Objective on this part is to capture an image pair on server side from left and right cameras and transfer them over the network to the client.Client is responsible for all image transformation and rendering.So after a successful transfer,client is able to render the images in stereo mode and view can be seen with shutter glasses.


# Starting Server Application #

Before starting the server application,make sure that the server is connected to the network(Fontys network or an external router).

In order to start the server application,you can click to application shortcut on desktop,**IS Capture.exe**.After opening the application,both left and right camera devices must be opened.After making the devices ready,server is now ready the send the image pairs captured from the camera.Detailed information about how to open the devices can be found on document `IS_Capture_Server_Documentation`.

**Note:** You can open the application by debugging the project **IS Capture v0.1**.Important point about the debugging is that;there is another IS Capture project which is deprecated but uses the same project options with IS Capture v0.1.That deprecated version is named **IS Capture Server v0.2 DEPRECATED** and is not used except code sample.So,if you are going to use the IS Capture v0.1 by clicking the shortcut on desktop,make sure that the last built version of the software belongs to v0.1 .You can find all the projects under **Remote Stereo Project** folder on desktop.

# Starting Client Application #

Client application is used as a bridge between stereo renderer and server.So,it is used for communication,image transmission and transformation.Client application can be opened on client by simply clicking the icon on desktop,**Stereo Viewer**.This icon belongs to release version of the program.You can also debug and build the project file under **Remote Stereo Project** folder on client machine.You can find the details about the application on the document Stereo\_Viewer\_v04\_Documentation.