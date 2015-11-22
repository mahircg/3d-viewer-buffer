# Introduction #

This documentation demonstrates the mechanism for client side of the system.You can find the details about design of the system on this document.For implementation detail,there will be another document on Wiki page.




# Details #

For the client side of the application,same interfaces are used with the server side application.For transmission purposes,RTSP Client filter is used,which is developed by the same team that programmed RTSP Server filter.On decoding part,we used ffdshow Video Decoder.For the rendering part,we used our own renderer,which is a customized Video Mixing Renderer (developed by Microsoft).All these filters and elements will be explained in this section.

## Communication ##

As mentioned early,we use RTSP\_source filter developed by a group of RTP software developers.With this filter,after starting session on the server side,it is possible to demand the stream from the server with correct parameters,such as channel address,tunneling mode,etc.

RTSP\_Source filter is familiar with a file source filter,only difference is it gets stream from the network,not from the disk.Like RTSP\_Server filter,RTSP\_Source filter is based on RFC 3550 transmission standard.

We use two separate RTSP\_source filter in order to get each channel separately and decode them concurrently.Output pins for both of the source filters are connected to separate instances of ffdshow decoders.



## Decoding ##

As decoder,we use ffdshow decoders as mentioned early.Two instances of ffdshow decoders are connected to left and right stream sources relatively. Client system has an Intel Core i5 processor,which has native encoding/decoding optimization.So,even if each stream is encoded in RGB format and sent over the network,decoding both streams will not cause some bottlenecks in run-time.Another reason of course is clearly decoding does not consume CPU time as much as encoding.

One important detail about decoding is that,each instance of ffdshow decoder runs as separate process.So there is not one instance running and performing decoding on both streams,which prevents possible delays internally on client side.Output pins for each decoder is connected to the custom renderer Video Mixing Renderer 9(VMR9) filter.

### Video Mixing Renderer 9 ###

VMR9 is an advanced video renderer with DirectX 9 support.Most important part about VMR9 is that it supports DirectX 9.

Importance of this feature for our system comes from the first version that we have implemented on static images.At simplest phase,in order to enable stereo mode on the screen, a full screen DirectX 9 application is required.Reasons for not using OpenGL,but DirectX is that client system is using an AMD graphics card,which uses a third party stereo driver,IZ3D.IZ3D driver does not allow OpenGL stereo on AMD video cards,so implementing the rendering mechanism in OpenGL is not an option.

One of the effective reasons that shifted us to use VMR9 is that,beyond DirectX 9 support,VMR9 can have multiple input pins and render them in a mode called VMR Mixing Mode. Mixing Mode is enabled automatically when there is more than one input pins are connected to VMR filter.Detailed information can be found on References section.

Video stream consists of images that are rendered to the screen in a specific interval called refresh rate.So,we can use our static image renderer software that we implemented in the first phase of the project.

We have implemented custom VMR presenter/allocator filter that enables the usage of DirectX 9 features.In the software,a class is designed that inherits from base VMR classes and necessary changes are made in the methods.Details will be in client implementation document.

## Rendering ##

Output filters for both ffdshow decoders are connected to the custom VMR9 filter.

After a successful connection of all filters,application restarts the graphics device and switches to full screen mode.When images start to come from the downstream filter(connection establishes and packets starts to arrive to client),custom renderer draws the both frames in a sequence(left frames first,then right frames,then left frames,...),that its mechanism is the same with the static image rendering application.


With VMR9,images coming from the downstream filter is presented in an order called Z-order. Z-order of a surface is determined by its pin,such Z-order of pin zero is zero,Z-order of pin one is one,etc.In screen,the surface that has the biggest Z-order value is rendered in default VMR.In our application,renderer application first presents both textures to the screen by splitting the screen in such a way that both left and right images are visible on the screen.Details about the rendering will be explained in client implementation document.

Briefly,renderer works as follows;

  * Get the frames for both left and right streams from downstream filter(decoders).
  * Split the rendering scene such that both streams are visible in screen in top-down way(left stream is rendered above,right stream is rendered below).
  * Assign every relevant frame to a DirectX 9  texture for each pair in the stream,such that assign left image to left texture and assign right frame to right texture.
  * Render left image , blacken right part of shutter glass.
  * Render right image,blacken left part of the shutter glass.

With that way,as long as frames keep coming from RTSP\_Source and decoded,renderer displays the images in stereo mode.

One important issue we face is that in the beginning of the application,because of connection establishment and two way packet transmission,RTSP source filter does not start to feed the upstream filter(decoder) immediately.Because of that reason,application does not start to render the images at the same time the process starts.There is a delay between application start time and streaming start time.This time depends on the connection speed,how much CPU that server uses,etc. For instance,if server is in waiting mode,delay increases.Most effective factor on this delay of course is the transmission establishment and streaming protocol architecture.Payload on the network directly effects the delay between application start time and streaming start time.

# Conclusion #

With the mechanism explained above,we established connection with server and enabled stereo streaming in client side.

# References #

[VMR9 Reference](http://msdn.microsoft.com/en-us/library/windows/desktop/dd407344(v=vs.85).aspx)

[RTP Source Filter](http://rtpstream.com/cgi-bin/mwf/topic_show.pl?tid=9)

[ffdshow Reference](http://ffdshow-tryout.sourceforge.net/)

