# Introduction #

This documentation explains the properties of cameras,camera mechanism,image format and some Firewire bandwidth issues.

# Camera Properties #

Camera mechanism that has been set with two Imaging Source Firewire cameras can be seen the picture below ;
<img src='http://3d-viewer-buffer.googlecode.com/files/DSC00081.gif' alt='IS Camera System' width='800' height='600'>

Both cameras has the same model: <b>DFK 31BF03</b>

Both cameras has external lenses on them.Model of the lenses is;<b>FUJINON HF9HA-1B</b>

There are focus options that can be arranged between 1.4 and 16.We choose 4 as focus option,which focuses neither close nor far.<br>
<br>
Cameras are connected to the computer via Firewire.<br>
<br>
For now,cameras are fixed in the mechanism.In later phases,it it planned as there will be a motor that controls the cameras,and that motor can be controlled via remote client application.With that way,cameras will be able to rotate and focus on close objects.Drawings for that design will be published here after it is finished.<br>
<br>
<h1>Bandwidth Issues</h1>

Because both cameras are connected to the same firewire bus,some limitation issues occur.<br>
<br>
There are different color format settings on the camera and because each of their pixel format(and its size of course) is different,each pixel format's bandwidth consumption is different.So there has to be a choice between pixel formats,and that format's consumption for both cameras should not exceed firewire bus bandwidth.The link below illustrates different bus requirements  for different pixel formats.<br>
<br>
<a href='http://www.theimagingsource.com/downloads/fwcamsbandwwp.en_US.pdf'>http://www.theimagingsource.com/downloads/fwcamsbandwwp.en_US.pdf</a>

According to that mapping,using Y800 format,on 1024x768 resolution at 15 Hz refresh rate for images does not exceed bandwidth limit.Besides the images' quality is fine,and refresh rate is at a level that does not make the user uncomfortable.So,images sent from server to client has these specifications that mentioned above.<br>
<br>
In that constant specifications,the image size is fixed without a compression,and it is <i>2359296</i> bytes.<br>
<br>
If some other color format or refresh rate is used that exceeds bandwidth,either one of the cameras will not work or one of them will work improperly.