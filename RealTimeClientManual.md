# Introduction #

This document explains how to run the client application in order to retrieve stream pair from server.


# Details #

Before starting the application **Stereo Rendering Graph**,make sure that the server graph is in running state.Another issue is that make sure that client computer is connected to the same network with server.

After being certain of both conditions are met,you can click the icon on client software,**Stereo Rendering Graph** to open the application.There is one argument that user needs to enter before start streaming, channel addresses for left and right streams.In the starting of application,two property pages open.On this pages,user needs to enter left and right channel address.Format for left and right channel address' are explained in the document RealTimeStream\_Server\_Application,under Communication header.After entering channels for both left and right streams,application switches to fullscreen mode,and if all the conditions are met(connection,channel address',etc.),application starts to display stereo video in 5-6 seconds.

Terminating the application is done by ESC button.

In the beginning,if program does not starts streaming in 5-6 seconds,it is better to restart the client application.For testing purposes,you can try opening the stream with VLC Player by providing the channel number.