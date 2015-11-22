This page includes the connection messages and their values.

# Introduction #

Below,the table describes all the messages that is being used between client and server in order for them to communicate correctly. These messages are defined in the same way both client and server applications.Size of each message is 1 byte.

These messages are defined for UDP connection purposes,so some of them are not used in TCP connection.

Because TCP is a connection-oriented protocol,all the acknowledgments and disconnect messages are defined in protocol layer.So the messages that are defined for that purposes is not used in this version of program,which uses TCP.

# Connection Message Mapping #

| **Message** | **ID** | **Value** | **Notes** |
|:------------|:-------|:----------|:----------|
|To start a communication|HELLO   |0x01       |Deprecated in TCP used model|
|Acknowledgement message|OK      |0x02       |
|To deny a message|STOP    |0x03       |
|Request for left image|GET\_LEFT|0x04       |
|Request for right image|GET\_RIGHT|0x05       |
|Request for an image pair|GET\_PAIR|0x06       |
|Get server/client info|GET\_INFO|0x07       |
|Get server/client status|GET\_STATUS|0x08       |Deprecated in TCP used model|
|Suspend a connection|SUSPEND |0x09       |Deprecated in TCP used model|
|End of stream|EOS     |0x0A       |Deprecated in TCP used model|
|Close a connection|CLOSE   |0x00       |Deprecated in TCP used model|

# Example of Using Connection Messages #
  1. In a case that client wants to get the left image,after connection is established , client sends GET\_LEFT message.
  1. Server application gets the message,checks which message is it,then there are several cases that can happen;
    * If left camera is not ready , server application sends STOP message to client.In that case,after client gets STOP message,client program shows an error message.
    * If left camera is ready, server sends a 4-bytes long byte array that includes the size of the image in bytes.Client gets the message,and allocates memory with that size of image in memory.
  1. After receiving image size,without any messages between,server starts to send the byte array that contains the image info.If client receives it correctly, left image is shown in client application.


