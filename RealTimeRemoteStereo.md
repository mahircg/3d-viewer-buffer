# Introduction #

This documentation explains the whole system architecture.


# Details #

Since the beginning of the project,our aim was to develop and end-to-end real-time stereo rendering system.For this purpose,we first developed a system that works with the image pairs taken by the cameras and sent to the client over the network.With that way,we have seen that the theoretical proofs that we have built the project on were working on practical as well.

Next phase for the project was to develop the system in a way that server will stream both left and right videos to the client,and client will render these videos by enabling stereo mode.

So far,we succeeded on both parts,and we have a running real-time system.We assume that the system we have is a prototype,so in future,there will be improvements on it.We can list the improvements such that;
  1. Mounting a motor system to the camera mechanism so that we will be able to rotate the camera towards each other in order to minimize the focusing problem.
  1. Using Joint Multi-View Coding(JMVC) compression technique instead of MPEG-4 encoding/decoding.JMVC is designed for compressing stereo video,using the mutual area for left and right frames,at basic level.So far,there are a lot of improvements on this technique,however still there is no software that can compress the stream using JMVC in real-time.As a future improvement,using this technique in real-time will increase the transfer speed by reducing the packet size.This can be done with a strong CPU and GPU support of course.

You can find all the details about the project both on Downloads and Wiki pages.There are no documentation for deprecated project files,however,code snippets on that projects can be used as example.