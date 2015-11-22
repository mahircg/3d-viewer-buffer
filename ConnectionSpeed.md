# Introduction #

This documentation describes some issues and reports about the connection between client and server.

# Bandwidth Information #

Each of the devices are connected to the internet with a network cable.So both of their bandwidth seems like 1.0 Gbps in network properties.However,this value is the network's speed,so nor client neither server cannot send or receive on that speed.

In order to measure both average download and average upload speeds , a test has been made using http://www.speedtest.net . Results are illustrated below;

| | **Download** | **Upload**|
|:|:-------------|:----------|
| **Client**|92.35 Mbps    |66.16 Mbps |
| **Server** |39.16 Mbps    |40.47 Mbps |

It is worthwhile to say that in client, Windows 7 64 bit OS is installed,while in server , it is Windows XP  bit.

# Packet Transfer Time #

In order to measure the total elapsed time between first packet and last packet, Wireshark Software has been used to sniff the packets.The statistics for just one image are illustrated below;

  * **Operation:**  Client receives left image and server sends.
  * **Total bytes that has to be sent:** 2359296 bytes(one image)
  * **Arrival time of first packet:** 9.590179 seconds
  * **Arrival time of last packet:** 9.6160007 seconds
  * **Total time elapsed:** 0.02 seconds
  * **Total packets transferred:** 1616 Data Packets,109 ACK packets

So,it takes 0.02 seconds for all the TCP packets, including image data and TCP header,acknowledgement,etc. , to be transferred from server to client.Client gets and buffers the byte stream.Then client application loads this data from buffer,rotates it ,saves it and displays it in the picture box.That is why client cannot display the image in 0.02 seconds after clicking _Get Left Image_ button.

  * **Operation:**  Client receives image pair and server sends.
  * **Total bytes that has to be sent:** 47118592 bytes(two images)
  * **Arrival time of first packet:** 6.079796 seconds
  * **Arrival time of last packet:** 6.126254 seconds
  * **Total time elapsed:** 0,046 seconds
  * **Total packets transferred:** 3232 Data Packets,188 ACK packets

It is obvious that while sending an image pair,it takes nearly twice of one image's transfer time to complete the transfer for a pair.Above mentioned issues are valid for a pair as well,furthermore,because application does twice processing ,twice buffer reading and twice allocating, it takes more than twice time of displaying one image in application,for displaying a pair in the application.

Note that every TCP packets that transfer data includes 1460 bytes,which is a part of the image byte array.Only the last packet's size varies from other parts,because it does not have to be filled with the number of bytes that is the Maximum Transfer Unit for the network.




