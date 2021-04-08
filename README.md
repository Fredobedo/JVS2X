<img src="images/banner.jpg">
 <h2>Introduction </h2>
 I was not really in favour of integrating Gaming consoles in arcade CABs but I gave a try with a Sony Playstation 3 and could confirm that:
 <BR>&nbsp&nbsp&nbsp-&nbspIt can natively output 640x480@60Hz with 4:3 aspect ratio (not all games)
 <BR>&nbsp&nbsp&nbsp-&nbspIt natively supports Logitech G25 racing Wheel with full FFB and has some excellent arcade ports (i.e. OutRun 2 SP, Sega Rally, GTI Club)
 <BR>&nbsp&nbsp&nbsp-&nbspIt can run arcade Shoot'em ups (i.e. RayStorm HD) and Beat'em ups (i.e. Street Fighter IV Arcade Edition)
 <BR>&nbsp&nbsp&nbsp-&nbspIt could be an excellent solution for rail shooting games with also very intersting arcade ports
 <BR>
 <BR>For the racing games, it was not a problem as I could directly connect my racing wheel but I needed an interface for being able to play other games with my CAB's controller. 
 <BR> -> I decided to use a Teensy 2.0 for this purpose as already done by other users but with small changes to fit my needs. 
 <BR> 
 <BR>So, this Teensy acts as an interface between the Game console/PC and the Arcade JVS IO Board, converting JVS protocol to USB (software HID Device). This USB composite Device exposes 3 HID interfaces: 2 gamepad controllers + 1 debug. HID
 <BR>
 <BR>Here is a non exhaustive list of systems compatible today (TBC):
 <BR>&nbsp&nbsp&nbsp-&nbspSony Playstation 3 (Home button supported: 'Start + Button 1')
 <BR>&nbsp&nbsp&nbsp-&nbspMicrosoft Windows (i.e. Mame)
 <BR>&nbsp&nbsp&nbsp-&nbspLinux based systems (i.e. RetroPie)
 <BR>&nbsp&nbsp&nbsp-&nbspNintendo Switch 
 <BR>&nbsp&nbsp&nbsp-&nbspEtc.
 <BR>
 <BR>This project is not something new; it is highly inspirated from other projects referenced at the bottom of this page.
 <BR>Do not hesitate to look at these projects to have more information.
 <BR>
 <BR>What could be considered as new here, is the way it is implemented (technical aspects):
 <BR>&nbsp&nbsp&nbsp-&nbspBulk JVS Commands dynamically built based on supported JVS client features
 <BR>&nbsp&nbsp&nbsp-&nbspAnalog and LightGun support
 <BR>&nbsp&nbsp&nbsp-&nbspDaisy chain support
 <BR>&nbsp&nbsp&nbsp-&nbspConfigurable multi CABs
 <BR>&nbsp&nbsp&nbsp-&nbspOptimized USB report updates
 <BR>&nbsp&nbsp&nbsp-&nbspBasic Analog Fuzz filters
 <BR>&nbsp&nbsp&nbsp-&nbspController remapping facilities
 <BR>&nbsp&nbsp&nbsp-&nbspEtc. 
 <BR>
 <BR>TODO: Implement HID for PS4, XBOX One, etc. when I have time or with your help :) 
 
<h2>Supported arcade layout</h2>
JVS2X is developed to support device chaining, thus it supports multiple JVS IO boards in the same CAB or in different CABs (Clone mode, VS mode, etc.)
<BR>All is implemented but I can't test as I do only have 1 CAB :( 
<BR>If someone is willing to give it a try, I will be more than happy to help in case of trouble
<BR>
<BR>You can configure your CAB's layout by updating file jvs_host_config.h:
<BR><img src="images/arcadeLayout.JPG"> 

<h2>Building the hardware</h2>
As said, this project is based on other projects which use the same hardware and soldering. 
<BR>Here are the components you need to construct your adapter (total max. 15€):
   <img src="images/components.jpg">
 
Please follow this diagram for soldering:
<img src="images/schema.jpg">

This is the way I soldered mine (c/o compatible enclosure below if needed):
<table>
<td><img src="images/down.jpg"></td>
<td><img src="images/up.jpg"></td>
</table>
There are also additional pictures from other realizations for other persons, in the doc folder if needed.

<h2>Printing an enclosure</h2>
I also designed a box for the JVS2X:
<table>
<tr><td colspan="2"><img src="images/3Dmodel.JPG"></td></tr>
<tr><td><img src="images/box2.jpg"></td><td><img src="images/box3.jpg"></td></tr>
</table>
In case you are interested, You can find the 3 STL files in the 'Enclosure' folder.

<h2>Compiling the software</h2>
 For now on, I use VS Code with PlatformIO IDE extension:
 <BR>&nbsp&nbsp&nbsp-&nbspBoard: Teensy 2.0
 <BR>&nbsp&nbsp&nbsp-&nbspFramework: Arduino
 <BR>If you don't want to compile it, there is already a compiled version in 'build' folder
  
 <h2>Upload software</h2>
 Please upload 'jvs2x.hex' in your Teensy with help of Teensy Loader (c/o pjrc.com).
 <BR>
 
 <h2>Debugging</h2>
You can compile JVS2X with DEBUG flags by for instance, update the platformio.ini like this:
<BR>"build_flags = -D USB_DISABLED -D JVSDEBUG=1 -D TARGET=JVS2PS3"
<BR>
<BR>On the debug terminal (c/o hid_listen.exe used for Teensy), you should see these traces:
<BR><<
<BR><img src="images/traces.jpg">
<BR>>>
<BR>If you do not want to compile it, I also have placed debug versions in 'build' folder too.

 <h2>LED States</h2>
JVS2X uses the Teensy internal LED to indicate several states:
<BR>&nbsp&nbsp&nbsp-&nbspContinuously blinking: waiting for a JVS cable to be connected
<BR>&nbsp&nbsp&nbsp-&nbspNo blink: init phase started
<BR>&nbsp&nbsp&nbsp-&nbspStaidy ON: JVS init is finished succesfully and the device is ready

<h2>Compatibilty list</h2>
Here are some games confirmed to work on Sony Playstation 3:
<BR>&nbsp&nbspWorking:
<BR>&nbsp&nbsp&nbsp-&nbspUnder defeat HD
<BR>&nbsp&nbsp&nbsp-&nbspRayStorm HD
<BR>&nbsp&nbsp&nbsp-&nbspSuper Street Fighter IV Arcade Edition
<BR>&nbsp&nbsp&nbsp-&nbspVirtua Fighter 5 
<BR>&nbsp&nbsp&nbsp-&nbspSöldner-X 2
<BR>&nbsp&nbsp&nbsp-&nbspSuper Street Fighter ii Turbo HD Remix
<BR>&nbsp&nbsp&nbsp-&nbspR-Type Dimensions
<BR>&nbsp&nbsp&nbsp-&nbspMetal Slug 2 
<BR>&nbsp&nbsp&nbsp-&nbspCapcom Arcade Cabinet (Ghodtd'n goblins, Commando, Gun Smoke, section7, etc.)
<BR>&nbsp&nbsp&nbsp-&nbspSky force Anniversary
<BR>&nbsp&nbsp&nbsp-&nbspAfter burner 
<BR>&nbsp&nbsp&nbsp-&nbspSonic generations 
<BR>&nbsp&nbsp&nbsp-&nbspWipeOut HD Fury
<BR>
<h2>Controller mapping</h2>
Some PS3 buttons do not exist on a arcade control panel. These are mapped as follow:
<BR>&nbsp&nbsp&nbsp-&nbspSTART + Button 1 -> PS Button
<BR>&nbsp&nbsp&nbsp-&nbspSTART + Button 2 -> Select
<BR>
<BR> You can reconfigure the default control mapping to your likings by updating the file jvs_controller_ps3_config.h:
<BR><img src="images/PS3ControllerMapping.JPG">

<h2>Fault tolerance</h2>
There is no special order to follow when starting your JVS2X, all orders are supported.
<BR> 
<BR>JVS2X will detect these follow errors and act upon:
 <table>
  <tr><td>&nbsp&nbsp&nbsp-&nbspJVS cable removed</td><td>-> JVS2X will wait for JVS cable and reinitialize when detected.</td></tr>
  <tr><td>&nbsp&nbsp&nbsp-&nbspJVS IO Board powered off</td><td>-> JVS2X will wait for JVS IO Board powered on again and reinitialize when detected.</td></tr>
  <tr><td>&nbsp&nbsp&nbsp-&nbspPS3 Powered off and on again</td><td>-> JVS2X is also powered off and on as it is powered by the PS3 USB.</td></tr>
  </table>
<BR>If for any reason, you have the impression that JVS2X is blocked or not responding accordingly, you can always try to restart it by pressing these 3 buttons at the same time: START + Button 1 + Button 2 -> Restart JVS2X
<h2>Sony Playstation 3 video output</h2>
 In my journey to integrate the Sony PS3 in my CAB, I spent several weeks finding the best way to connect the video output.
 <BR>Let me share my experience and my final implementation.
 <h3>640x480@60Hz</h3>
 <BR>The PS3 can output this resolution ONLY if the region is set to NTSC:
 <BR><img src="images/ntsc.jpg">
 (extract from https://manuals.playstation.net/document/en/ps3/3_15/settings/videooutput.html)
 <BR>
 <BR> -> So you have 2 choices, either you have a NTSC PS3 or you you have a PAL PS3 that you can change region with help of CFW and some registry updates.
 <BR>
 <BR>Next, we have to look at the video connectivity and I first tried one of those PS3 to VGA cable.
 <BR>This was a mistake, the image was very dark, washed and colorimetry was wrong. It turned out that this was not a standard VGA output (RGB HV) but D-Terminal (Sync on Green, D2). This video format can also be found on Sony PS2 and Nintendo GameCube.  More info at https://en.m.wikipedia.org/wiki/D-Terminal.
 <BR>  
   
 <BR>Then I tried the component Video out cable that I connected to my GBS Control (custom CBS 8200) configured to direct output to same resolution in VGA and surprise, it worked well!
 <BR>Looking at it a bit more after some days, it was kind of working but the image was a little too blury and it was a bit overkill, no? So I decided to look at something else.
 <BR>
 
 <BR>HDMI 2 VGA DAC converter:
 <BR>I decided to give a try to this little DAC converter, after all that is was we can also find on Video cards with VGA output:
  
 <BR>The result is just perfect! Image is sharp, without any lags or screen tearing, soo simple to use and so cheap !?! 
 <BR> -> That is my solution.
 <BR> 
 <BR>Please note that this little adapter does not downscale, etc. It is just a DAC converter. So the HDMI is configured to output 480p.
 
 
 
<h2>External references</h2>
 This project is based on:
 <BR>&nbsp&nbsp&nbsp-&nbspJVSy from k4roshi (https://github.com/k4roshi/JVSy)., 
 <BR>&nbsp&nbsp&nbsp-&nbspAdapted version of JVSy (https://github.com/gtranche/JVSy).
 <BR>&nbsp&nbsp&nbsp-&nbspOpenJVS (https://github.com/OpenJVS/OpenJVS).
 <BR>
 <BR> Many thanks for sharing all these precious sources of information and also a special thanks to Bobby for all his help, ideas and support!
  <BR>
  <BR>Have fun & best regards.
  <BR>Fred
