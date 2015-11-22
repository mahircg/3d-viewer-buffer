This documentation explains the mechanism for generating a frame sequential stereo stream from a stream pair.

# Frame Sequential Stereo #

Frame sequential stereo term is one of the methods that is used for displaying the stereo video on a screen.There are different methods as well,such as,interleaved,side by side, over and under, etc. In our project,we use frame sequential stereo display method,for displaying the stereo stream that is received from server.

Basically method works as this;

  * Left stream's refresh rate is 15 Hz.So it has 15 frames in each 1 second interval.
  * Right stream's refresh rate is 15 Hz as well.
  * Generated frame sequential stereo stream's refresh rate is 30 Hz,so it has both frames for left and right stream.

Stereo stream has left stream's frames in its even frame numbers,and right frames in its odd frame numbers or vice-verse .Figure below explains the mechanism;

<img src='http://3d-viewer-buffer.googlecode.com/files/FrameSequentialDraw.png' height='400' width='1000' />

Left stream line represents the frames for left video at one time unit(second) and right stream line represents the frames for right video at one time unit.

In stereo stream,there are 30 frames in every second,which the odd numbered frames are left frames and even numbered frames are right frames.A basic algorithm can be generalized as follows;

  1. start proc
    1. integer i = 1
    1. integer j = 1
    1. while (i <= 30)
      1. read the frame number i from left stream
      1. write the read frame to frame number j of stereo stream
      1. read the frame number i from right stream
      1. write the read frame to frame number j+1 of stereo stream
      1. increment i by 1
      1. increment j by 2
    1. end while
  1. end proc


# Rendering the Frame Sequential Stream #

The generated stereo stream will be displayed in a stereo supported 120 Hz monitor with the shutter glasses. Monitor has an infrared emitter that sends signals and synchronizes with the shutter glasses,so that left and right view of glass will be blackened in the correct order.
While the left view of the glass is blackened,right view is displayed(right stream) and while the right view is blackened,left image is displayed in screen.Because the refresh rate is much more higher than a standard monitor,flicker will not be noticed. So, each eye will get actually 60 frames per second sequentially. This mechanism will be explained in detail in final documentation.