This documentation file contains instructions about Stereo Viewer version 0.3

# General Info #

Stereo Viewer v0.3 contains the same basics with the previous DirectX 9 version,but also includes a Graphical User Interface to make usage of software easier.Some features like,browsing images,checking 3D compability,etc. are added in this version.

Application is developed using C# with .NET v3.5

IMPORTANT:Currently v0.3 supports only AMD cards with HD3D support.Further implementations will support both nVidia and AMD cards.


# Details #

## Run Time Tips ##

### Browsing Images ###
In order to browse an image pair(left and right),you can use the 'Open Left Image' and 'Open Right Image' menu items under File menu.
Alternatively,you can copy an image and paste it as left or right image to the application using 'Paste as Left Image' or 'Paste as Right Image'  menu items under Edit menu.

Remember that,without opening an image pair,it is not possible to run DirectX 9 application.

### Checking Compatibility ###
After loading the images,application needs to check if graphics adapter supports stereo view or not(in this version,only AMD cards).If the card is incompatible with AMD HD3D,application does not allow to open DirectX 9 software that displays the image pair in stereo mode. You can check if your device supports HD3D feature from [this](http://www.amd.com/us/products/technologies/amd-hd3d/pages/supported-hardware.aspx) link.

### Drivers and Third Party Software ###

For the best image quality and consistency,use the Catalyst Driver with the version 10.5 . In next versions of Stereo Viewer,some bugs will be handled with the further versions of Catalyst Driver.

And also for application to work properly,use the IZ3D driver with the version 1.13 .

### Mechanism ###
C# application opens a process and sends the image pairs directories to the DirectX application(StrView.exe) as command line arguments by starting a process with the entry point of _StrView.exe_ . **So,_StrView.exe_ can not be executed as a standalone application.**


