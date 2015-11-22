# Introduction #
This documentation describes the client side mechanism of the Client/Server application for Remote Stereo Project.

# Requirements #

  * ### Software Requirements ###
Requirements for the software is the same with version 0.3 .You can find these requirements on the documentation for v0.3

  * ### Connection Requirements ###
All the necessary software and hardware for establishing a connection with server and fire-wire cameras will be explained in a different document called Connection Requirements.

  * ### Hardware Requirements ###
Hardware requirements for the application is same as the version 0.3 .

# Mechanism of Operations #

In this demonstration of mechanism, operations that are involved in controls,enabling stereo mode and process relations do not take place,because these issues are commented in the previous documentation with version 0.3 .

For version 0.4 , mechanism is as follows ;


  1. For Client/Server communication , 1 byte long messages are defined.Applications send these messages each other to send the right file, suspend the connection, stop an operation , and close connection.These constant messages are represented in the document ConnectionMessages
  1. A _System.Net.Sockets.Socket_ object is initialized and instantiated in a way that it will be used for streaming and will use IPv4.
  1. After Connect button is clicked, application calls _Socket.Connect_ function with servers host address and server application's port number as arguments, in order to connect to the server.This starts a three way handshake between server and client ,if server application is ready.Otherwise an error message is shown.After connection is established, client and server are ready for receiving and sending TCP packets to each other.
  1. Now that connection is established,client can get image byte streams from remote cameras via server application.
  1. If user clicks _Get Left Image_ button ,following operations take place on application ;
    * Client applications sends a GET\_LEFT message to the server.
    * After receiving this message,server application first checks whether left camera is open or not.If camera is open, server application captures an image from left camera and saves it to the memory of the server. Then, server sends an integer that stores the size of the image as bytes. If camera is not open , server sends a STOP message to client.
    * Client gets the value,converts its byte order from Network Byte Order(big-endian) to Host Byte Order(little-endian).Reason of such a conversion is the bytes which server(uses little endian) sends is converted into Network Byte Order.So without a conversion,incoming value of image size is not correct,such that first byte and fourth byte swaps place ,etc. After conversion,application creates a new byte array which size is equal to image size,if the incoming byte stream is not a STOP message.Otherwise, application shows an error message.
    * Then application waits for receiving the image's byte stream from server.After getting all the bytes, application creates a new _System.Drawing.Bitmap_ object which width is 1024 and height is 768.
    * Then application creates a new _System.Drawing.Imaging.BitmapData_ object.This object is used for manipulating the byte values of recently created Bitmap object's byte values.In order to do that, application locks the Bitmap object in system memory,then copies all the contents of the received image stream to Bitmap object,starting from index and length of image size in bytes. After copying is done,Bitmap object is unlocked.
    * Here,an issue occurs with above mentioned byte order conversion.Because the orders of the bytes in image stream is changed as well,a conversion from big-endian to little-endian is needed.Otherwise,image is shown as upside-down. To solve this problem,a rotation on the Bitmap object with 180 degrees is done,around its origin.With that way,image is looking as it is captured in the server side. Then,image is shown in the PictureBox object on the form.
  1. If user clicks  _Get Right Image_ button ,same mechanism works as _Get Left Image_,except that GET\_RIGHT message is sent,instead of GET\_LEFT.
  1. If user clicks _Get Image Pair_ button,following operations are executed;
    * Client sends a GET\_PAIR message to the server.
    * If both cameras are ready, server captures image from both left and right cameras.This time,server sends an integer stores two times of the image size.
    * If incoming message is not STOP,application receives this integer(makes the endian conversion of course) and dynamically allocates two arrays(one for left image and one for right image),that is each of their size is the half of the received pair size.Application also creates a new array which size is the number of bytes in the pair.
    * After allocating resources , application waits for receiving the image pair byte stream.
    * If the stream is received successively,application goes into a loop that executes from 0 to one image size in bytes.In that loop,application copies the first _image size in bytes_ bytes to the left array,and copies the second _image size in bytes_ (which index is not zero,but _image size in bytes_)bytes to the right array. With that way,left and right arrays are filled with the left and right image's bytes.
    * All the conversions that is mentioned in step 5 are still has to be applied to left and right images.After these conversions,application shows left and right images in picture boxes in form.
  1. After each image receive,regardless from if it is left,right or pair,these images are saved into a directory that executable file exits.Reason of this operation is to run StrView.exe with the directories of these images ,however in later versions,this operation will change place with one of the interprocess communication methods.
  1. If the steps above are completed successively,application can enable stereo mode with the same mechanism as previous version.

# Backlog Window #

Backlog form is a new feature with version 0.4 . In every remote operation, backlog window prints the operation and it's time.You can open or close the backlog form anytime you want.However previous data will not be saved.
In future implementations,there will be features like saving the backlog or saving the previous data during run-time.

# Typical Scenarios #

  * If user wants to disconnect from the server,_Disconnect_ button under _Remote Options_ can be used.However,during the same session(without restarting the application),application does not allow to reconnect with same socket synchronously  again.This is an issue with the client side's socket object.In future implementations,there will be a workaround about this issue.
  * If user closes down the application and opens it again,while server is open during that time,system allows client to be connected to the server again.So in a possible run-time error or crash,if server is still ready,client can be restarted.

  * If user opens the files that has been received and saved to the disk,without any remote option(without receiving from server),and in the meantime user tries to get the images remotely from server,a run-time occurs because application tries to write over a memory block that is used by the operating system. Fortunately,application can be run after error,if user click _Continue_ on the dialog box appears after error.

# Important Notes #
  * If application tries to connect to the server which is not switched on,program freezes when user clicks connect button.There will be an option which enables user to enter the IP address for host and port number for application,in future implementations.Unless these features are added,it is not recommended to use _Connect_ option outside the Fontys Mechatronics Lab(connection is being done in Fontys LAN).This situation still occurs even if client is in Fontys building ,but server is switched off.In next version,this problem will be solved.
  * If you receive "Could not receive image data size correctly.Try to get the image again" warning message,it is recommended to restart the server application.Reason of that bug is,if TCP cache is fully loaded on client side,and application cannot receive all the bytes correctly from buffer at a specific interval,server keeps sending the bytes as long as all the bytes are transferred completely,and this causes an overflow on client,because server sends more data bytes than it supposed to be. This issue will be solved in next implementations.
# Installation #

With the installation file on documents page,Stereo Viewer application can be easily installed and uninstalled.






