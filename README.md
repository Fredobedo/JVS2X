# JVS2X
 <h2>Introduction </h2>
 This project aim to use Home consoles or PC's in an arcade CAB.
 <BR>It acts as an interface between the host and the Arcade IO Board, converting JVS protocol to USB (software HID Device).
 <BR>The host (home console/PC) will detect it as a compatible game controller/Keyboard. 
 <BR>
 <BR>This project is highly inspirated from other projects referenced at the bottom of this page.
 <BR>Do not hesitate to look at these projects to have more information.
 <BR> 
  
<h2>Building the hardware</h2>
  Here are the components you need to construct your adapter:
   <img src="doc/components.jpg">
 
Please follow this diagram for soldering:
<img src="doc/schema.jpg">

It should look approximatelly like this:
<table>
<td><img src="doc/down.jpg"></td>
<td><img src="doc/up.jpg"></td>
</table>
<h2>Building the software</h2>
 For now on, I use VS Code with PlatformIO IDE extension:
 <BR>   - Board: Teensy 2.0
 <BR>   - Framework: Arduino
 <BR>You don't have to compile it, there is already a compilted version in 'build' folder
  
 <h2>Upload software</h2>
 Please upload 'ps3_pc_mac_generic_gamepad.hex' in your Teensy with help of Teensy Loader (c/o pjrc.com).
 <BR>
 
 <h2>External references</h2>
 <BR>This project is based on https://github.com/k4roshi/JVSy, many thanks for sharing the information.
 <BR>It is also highly inspirated from the excellent OpenJVS project from Bobbydilley, thank you again for all your help. (https://github.com/OpenJVS/OpenJVS)
 <BR>
