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
 
 <h2>Use cases</h2>
Here are some games I tested on Sony Playstation 3:
<BR>
<BR>
<BR>

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
