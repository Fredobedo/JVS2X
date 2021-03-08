# JVS2X
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
 <BR>TODO: Implement HID for PS4, XBOX One, etc. :) 
 <BR> 
 <BR>This project is highly inspirated from other projects referenced at the bottom of this page.
 <BR>Do not hesitate to look at these projects to have more information.
 <BR> 
  
<h2>Building the hardware</h2>
  Here are the components you need to construct your adapter (total max. 15€):
   <img src="doc/components.jpg">
 
Please follow this diagram for soldering:
<img src="doc/schema.jpg">

It could look approximatelly like this:
<table>
<td><img src="doc/down.jpg"></td>
<td><img src="doc/up.jpg"></td>
</table>
There are also additional pictures from other realisations in the doc folder if needed.

<h2>Building the software</h2>
 For now on, I use VS Code with PlatformIO IDE extension:
 <BR>&nbsp&nbsp&nbsp-&nbspBoard: Teensy 2.0
 <BR>&nbsp&nbsp&nbsp-&nbspFramework: Arduino
 <BR>You don't have to compile it, there is already a compilted version in 'build' folder
  
 <h2>Upload software</h2>
 Please upload 'ps3_pc_mac_generic_gamepad.hex' in your Teensy with help of Teensy Loader (c/o pjrc.com).
 <BR>
 
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
<BR>Some PS3 buttons do not exist on a arcade control panel. These are mapped as follow:
<BR>&nbsp&nbsp&nbsp-&nbspSTART + Button 1 -> PS Button
<BR>&nbsp&nbsp&nbsp-&nbspSTART + Button 2 -> Select
<BR>

<h2>Fault tolerance</h2>
<BR>There is no special order to follow when starting your equipment, all orders are supported.
<BR> 
<BR>JVS2X will detect these follow errors and act uppon:
<BR>&nbsp&nbsp&nbsp-&nbspJVS cable removed &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp-> JVS2X will wait for JVS cable and reinitialize when detected
<BR>&nbsp&nbsp&nbsp-&nbspJVS IO Board powered off&nbsp&nbsp&nbsp -> JVS2X will wait for JVS IO Board powered on again and reinitialize when detected
<BR>&nbsp&nbsp&nbsp-&nbspPS3 Powered off and on again -> JVS2X is also powered off and on as it is powered by the PS3 USB.
<BR>
<BR>If for any reason, you have the impression that JVS2X is blocked or not respondiging accordingly, you can always try to restart it by pressing these 3 buttons at the same time: START + Button 1 + Button 2 -> Restart JVS2X

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
