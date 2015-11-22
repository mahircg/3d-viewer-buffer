# Introduction #

This documentation explains the mechanism of connection between server and client.


# Transport Layer #

For transferring packets between server and client, Transmission Control Protocol is used.

Before starting the implementation of server application, planned transport layer was User Datagram Protocol. Implementation started with that protocol,however,it is obvious that packet losses on  UDP caused a lot of trouble in a transport that there should not be any packet drop during transmission. Because of that losses,image received by client seems blurry,corrupted , long story short , not as it is supposed to be.Moreover,our goal in that project is not to reinvent a TCP like protocol,so working on terms like congestion control,acknowledgement on UDP is just a waste of time,rather than using TCP.

Because on this version of client/server model,our goal is sending image pairs to the client, overhead of TCP does not create a huge difference between UDP.So,TCP is used as transport layer of system.

Anyways,on the later implementations,which the goal is sending stream from server to client, Real Time Streaming Protocol is planned to be used as application layer.There will be some other considerations of course, for now, RTSP seems like most suitable one.

On the document ConnectionSpeed, speed of incoming/outgoing packets is explained with detail.

## IP Address ##

Server is connected to internet through Fontys DNS server and uses an ethernet cable to connect.An unused IP address is assigned to the server every time it restarts. So,in the client application,host address to connect is changed in every new session of server.There are several solutions to this problem.

One of the solutions is enabling the client to determine to host address manually every time the user wants to establish a connection.This time ,however,there is a possibility that client does not know the host address of server.

So,another and better solution is to assign a static IP address to the server,so it does not change in every new session.

This issue will be solved in the later implementation of the system.

# Application Layer #

In the application layer,there is no well known protocol to handle the requests/responses from both sides. Instead of that, server application IS Capture uses port number _27015_ and listens all the requests from that port.This port number is defined in client as well,so client sends all the packets using that port.

# Mechanism of Connection #
Mechanism of connection between client and server is illustrated in the figure below;

<img src='http://3d-viewer-buffer.googlecode.com/files/Transferring%20Image%20Pair%20Mechanism.gif' alt='Architectural Design for Connection' height='600' width='800' />

As you can see with the drawing,the packets are not routed through another routers.Because client and server are both on the same local network,packets reach to the destination without routed to another destination.

Template for a connection between client and server can be described in 4 steps;
  1. Client clicks connect button,and three way handshake starts.Client sends request,server sends ACK,client sends ACK,then connection is established.
  1. After connection is established,both sides have each others socket info(IP address address and port number).
  1. Client sends get request,server sends it if application is running and cameras are ready,client gets and displays the image.
  1. Client or server disconnects.

Details of main steps can be found on Stereo Viewer v0.4 documentation and IS Capture documentation.