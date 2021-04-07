<img src="images/banner.jpg">
 <h2>Introduction </h2>
 This project aims to use Home consoles or PC's in an arcade CAB.
 <BR>It acts as an interface between the host and the Arcade IO Board, converting JVS protocol to USB (software HID Device).
 <BR>The host (home console/PC) will detect it as a compatible game controller/Keyboard. 
 <BR>
 <BR>The Device Controller has 3 HID interfaces; 2 gamepads + 1 debug. In other words, it 'emulates' 2 gamepads.
 <BR>Here are the systems compatible as of today:
 <BR>&nbsp&nbsp&nbsp-&nbspSony Plastation 3 (Home button supported: 'Start + Button 1')
 <BR>&nbsp&nbsp&nbsp-&nbspMicrosoft Windows
 <BR>&nbsp&nbsp&nbsp-&nbspLinux
 <BR>&nbsp&nbsp&nbsp-&nbspApple
 <BR>
 <BR>TODO: Implement HID for PS4, XBOX One, etc. when I have time or with your help :) 
 <BR> 
 <BR>This project is highly inspirated from other projects referenced at the bottom of this page.
 <BR>Do not hesitate to look at these projects to have more information.
 <BR> 
  
<h2>Building the hardware</h2>
  Here are the components you need to construct your adapter (total max. 15€):
   <img src="images/components.jpg">
 
Please follow this diagram for soldering:
<img src="images/schema.jpg">

It could look approximatelly like this:
<table>
<td><img src="images/down.jpg"></td>
<td><img src="images/up.jpg"></td>
</table>
There are also additional pictures from other realisations in the doc folder if needed.

<h2>Printing an enclosure</h2>
I also designed a bow for the JVS2X:
<table>
<tr><td><img src="images/3Dmodel.JPG"></td></tr>
<tr><td><img src="images/box2.jpg"></td><td><img src="images/box3.jpg"></td></tr>
</table>
In case you are interested, You can find the 3 STL files in the 'Enclosure' folder.

<h2>Building the software</h2>
 For now on, I use VS Code with PlatformIO IDE extension:
 <BR>&nbsp&nbsp&nbsp-&nbspBoard: Teensy 2.0
 <BR>&nbsp&nbsp&nbsp-&nbspFramework: Arduino
 <BR>If you don't want to compile it, there is already a compilted version in 'build' folder
  
 <h2>Upload software</h2>
 Please upload 'jvs2x.hex' in your Teensy with help of Teensy Loader (c/o pjrc.com).
 <BR>
 
 <h2>Debugging</h2>
You can compile JVS2X with DEBUG flags by for instance, update the platformio.ini like this:
<BR>"build_flags = -D USB_DISABLED -D JVSDEBUG=1 -D TARGET=JVS2PS3"
<BR>
<BR>On the debug terminal (c/o hid_listen.exe used for Teensy), you shoudl see these traces:
<BR><<
<BR><img src="images/traces.jpg">
<BR>>>
<BR>If you don't want to compile it, I also have placed debug versions in 'build' folder too.

 <h2>LED States</h2>

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

<h2>Fault tolerance</h2>
There is no special order to follow when starting your equipment, all orders are supported.
<BR> 
<BR>JVS2X will detect these follow errors and act uppon:
 <table>
  <tr><td>&nbsp&nbsp&nbsp-&nbspJVS cable removed</td><td>-> JVS2X will wait for JVS cable and reinitialize when detected.</td></tr>
  <tr><td>&nbsp&nbsp&nbsp-&nbspJVS IO Board powered off</td><td>-> JVS2X will wait for JVS IO Board powered on again and reinitialize when detected.</td></tr>
  <tr><td>&nbsp&nbsp&nbsp-&nbspPS3 Powered off and on again</td><td>-> JVS2X is also powered off and on as it is powered by the PS3 USB.</td></tr>
  </table>
<BR>If for any reason, you have the impression that JVS2X is blocked or not respondiging accordingly, you can always try to restart it by pressing these 3 buttons at the same time: START + Button 1 + Button 2 -> Restart JVS2X

<h2>CAB Supported configurations</h2>

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
