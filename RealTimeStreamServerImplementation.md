# Introduction #

This document explains the implementation details for server application that is used to open RTP channels and send the stream.

For the application , we use Graphedit for connecting the filters.Although we have a COM application written in C for that,because of a problem caused by dynamic pin generation,we can stream only left or right with that application.Reason for such a program is to gain more control over the capturing,transforming and streaming.In the future,if Graphedit remains insufficient for the requirements of system,that application named **RTSP Stereo Server** can be modified and used.You can find both the graph file and project file for server application in Downloads section.

Basically,we connect the filters in this order;

**DFK 31FB03 -> Debayer Transform -> ffdshow Encoder -> RTSP Server**

**DFK 31FB03 1 -> Debayer Transform -> ffdshow Encoder -> RTSP Server**

# Details #

## Registering Filters ##

### IS Device Filters ###
For camera devices,the filters are registered automatically after installing the drivers for them,as well as **Debayer Transformation Filter**. Problem about the filters is both filters are registered with the same name in registry.So in order to make the separation between left and right devices,I changed one of the devices name to **DFK 31BF03 1** ,while default registry contains **DFK 31BF03** for both devices.

### ffdshow Encoder Filter ###

As described in stereo application design documentation,after installing K-Lite Codec pack,ffdshow encoders and decoders registered to the registry.

### RTSP Server Filter ###

In order to use RTSP Server Filter,the filter must be downloaded first from [this](http://rtpstream.com/files/RTSP_server.090204.zip) link.After saving the .ax file to a proper location(that is a permanent location for file),you can register it by using the command **regsvr32**.


As described in the design documentation for server application,we connect proper filters and run the graph in order to start streaming.Though connecting  filter in Graphedit is not a challenging issue,there are some configurations that has to be made on filter properties.

## Setting Up Graph ##

As mentioned before,connecting the filters can be done easily with Graphedit after a little research.Here,I will mention about a couple of configurations on the filters.

### Device Filters ###

Main issue about initializing and connecting the filters cameras is the bandwidth overload.In the initialization step,both devices are created with default properties(UVYV,15 FPS),which causes insufficient bandwidth for both devices.Even though you can create devices in Graphedit with this properties,it is not possible to make connection with the filters.In order to do that,output for each filter must be changed to BY8 and frame rate must be set to 15 FPS by right clicking to the output pin for each device filter,**Pin Properties**.

### RTSP Server Filter ###

RTSP Server filter supports dynamic pin creation,which means you can connect the output pin of multiple filters to the input pins of the server filter,  that are generated in run-time.Every generated input pin is assigned a new channel name,starting with 0,1,2,... In our application,we connect the left device to input pin 0 of server filter,and right device to input pin 0 of it.

## Running the Graph ##

After making all the configurations that are mentioned above,graph can be run by clicking the Run button on the top menu.

## Issues ##

  1. Because of dynamic pin creation,even though you save the graph file on every exit,you will need to connect the second device to the RTSP Server filter every time you open the graph.

  1. In the first run of the graph,sometimes you can see an error message says **insufficient thread locking around ...**.You can simply stop the graph if you see that message and restart it without closing Graphedit.

  1. If there is no network connection while starting the Graphedit,you may see all the filters are disconnected.In that case,checking the connection is recommended.
