# Introduction #

This documentation describes the mechanism for server-side application of the real-time system.

This part of documentation describes some of the interfaces that are used in application.

## DirectShow and Filters ##

Both in server and client applications,DirectShow filters are used in order to provide a flow between elements that are required to setup an end-to-end connection.DirectShow is a Microsoft software architecture for multimedia applications.More information about DirectShow filters can be found on the References section at the end of the page.

## OLE/COM Framework ##
Because of DirectShow interface is based on Component Object Model(COM) framework,both client and server applications user COM libraries as well.Registration,initialization,etc. parts are different from .NET framework in most ways,so it takes a careful consideration before delving into designing and implementation steps.Again,more info about COM framework can be found on References section.

## GraphEdit ##
GraphEdit is an application that enables developers to combine DirectShow filters and render them.Though it is a very useful application if you need to just connect filters and render them, it lacks if you need to manipulate the flow between filters or if you use a custom filter.In the scenarios that requires manipulation or customization,developer should connect the filter in a custom application that uses DirectShow and COM libraries.In the server side of the system,I've written a software that links the filters and renders them.Even though the server application does not need any customization between filters,exception handling and logging mechanisms are required ,where achieving these features is not possible with GraphEdit application.

# Mechanism and Work Flow #

Drawing below demonstrates the data flow and process' involved in connection between client and server.All the elements of the architectural design will be explained in this section.

<img src='http://3d-viewer-buffer.googlecode.com/files/Real%20Time%20Streaming.png' height='600' width='1000' />

## DFK31BF03 Color Firewire Camera ##

As you can see in the previous phase of the system,two !firewire cameras are used in the application.Most important thing about using them concurrently is the bandwidth issues on the system bus.Solutions and workarounds about the bandwidth issues are still the same with the previous phase of the project,that involves static images.Document about the bandwidth issue can be found at Camera\_Specifications document.

## Encoding ##

Without doubt,video stream for each camera has to be compressed before sent over the network.There is not enough network bandwidth and speed that can carry two separate video streams on the same network,so it is an indisputable fact that compression has to be made on both streams.

It is not possible to display or send the raw video file over the network.In any case,in the application level,compression is made by the application that uses the camera,and generally the default encoding filter for compression is Microsoft's DV Video Encoder.Though DV is considered as a compression option in some of the applications,the main thing that DV filter does is to encode the raw(uncompressed) video stream into digital video.It uses AVI file format as container.As you can see from the method,DV Video Encoder does not compress the video,but acts as a transducer,that converts analogue signal into digital signal.

After deciding that compression has to be made,one big and attention required question is,which compression technique to use?

As I will mention in the Connection section, we user Real Time Streaming Protocol(RTSP) to control and Real-Time Transport Protocol(RTP) to packetize and send the stream.However,using these protocols brings a limitation about the container format that can be sent over the channel.By using RTP , not every file format can be sent over the network.This situation decreases some of the encoder options that we can select(format limitations about the RTP protocol will be mentioned in the Connection section).

There are a lot of encoding techniques that you can found on the internet,however,H.264/MPEG-4 video compression standard is the most commonly used in distribution of high resolution video.

We used ffdshow Codec,that uses H.264/MPEG-4 video compression technique ,for both encoding(server side) and decoding(client side) the stream.

We encode/decode the video in MPEG-4 format,which is supported by RTP, and send it over the channel.

One thing that requires attention is that because H.264/MPEG-4 technique consumes  a huge CPU time,encoding two streams using ffdshow concurrently is not possible normally.It requires a lot of CPU power and hardware/software level optimization on CPU.

### Debayering ###

A workaround on this issue is to decrease the stream quality for both of the cameras,that is using BY8(8-bit raw data in Bayer format) color format.Normally , that 8-bit image is in gray scale and has no color information on each pixel.Rather,each pixel records how many photons struck it  with the odds of recording a photon being higher at 550
nm than at 400 nm .Imaging Source Color cameras has red,blue and green matrix in front of the sensor,that enables Debayering option.With Debayering,basically it is possible to have a color image from a gray scale image.Detailed technical information about Debayering can be found at References section.

Imaging Source cameras has an option to auto-enable Debayering when BY8 output format is selected.In our graph,we use Imaging Source Debayering Filters that is connected to the output pins of the Firewire Cameras.

After having a 8-bit frame to compress in low-level, compressing both streams at the same time is not a problem anymore. It is helpful to mention,not every camera has filter sensors that enables Debayering.So this technique can be used with DFK31BF03 cameras,but there is no guarantee that every other models or manufacturers support this feature.

In the properties of the ffdshow encoder,there are detailed options that enables programmer to make a trade-off between network bandwidth/speed or CPU time and parallelism. One important change we have done in the properties page is to increase the thread number from 1 to 2.Even though we do not know the threading mechanism for ffdshow software,it is not a good practice to increase the thread number to huge amounts,as it can cause deadlocks or some other synchronization problems.

## Communication ##

In communication part,we use RTSP\_Stream filter to stream the video to the client.This filter is written by a group of RTSP developers.Its source code is not available,however,release version is distributed free.

RTSP Stream Filter has options for both multicast and unicast streaming.In our case,server application starts streaming on demand,which requires client to request a session start first.

As a nature of RTSP,we use seperate RTSP/RTP channels to transfer the stream from server to client.For that purpose,we have one RTSP\_Server filter with two input pins.Note that RTSP\_Server supports up to 16 pins to stream over 16 different channels.

The address template for the RTSP Server is like:

> rtsp://"xxxx"-"xxxx"-"xxxx"-"xxxx"/ch"y" , where x's represents the IPv4 address and y represents the channel number for the stream.

In our application,channel number zero represents the left stream,and channel number 1 represent the right stream.So,for instance,in order to stream the left channel,following IP address can be used with appropriate client application: rtsp://145.85.50.37/ch0.

RTSP Server filter has options like HTTP Tunneling,TCP,UDP. In our application,packets are sent over UDP packets encapsulated by RTP.

One thing I mentioned about the format limitations on RTP is the payload type.By using a filter,programmer does not need to select the appropriate payload type for the channel,but filter does it automatically,taking the information from its input pin.If the format is not supported,streaming does not start.

Payload type is an 7-bits indicator in the RTP header file.Payload types are identified by RFC 3551 standards.In our case,because we use MPEG-4 compression, payload type name is: MP4V-ES.Each payload type has attributes like name,clock speed,number of channels,type,etc.In References section,you can find a link to page that describes all the payload types.

# Conclusion #

After connecting all these filters relatively,running the graph starts a session in the server side.Implementation phase and client side documentation will be in Documents page.

# References #
  * [DirectShow Reference](http://msdn.microsoft.com/enus/library/windows/desktop/dd375454(v=vs.85).aspx)
  * [COM Reference](http://msdn.microsoft.com/en-us/library/windows/desktop/ms680573(v=vs.85).aspx)
  * [Debayering Reference](http://www.stark-labs.com/craig/articles/assets/Debayering_API.pdf)
  * [RTSP Filters](http://rtpstream.com)
  * [RTP Payload Types](http://en.wikipedia.org/wiki/RTP_audio_video_profile)