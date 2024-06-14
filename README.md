# myPhaseSpace
Configuring and building codes for PhaseSpace X2E Motion Capture system

[2024-04-14 update] Note that PhaseSpace IP is 192.180.0.170

## Quick resources
1. There is more up to date and dynamic documentation embedded in the [PhaseSpace Configuration Manager Help section](http://customers.phasespace.com/anonymous/SDK/5.2/psdoc/index.html) with login credentials : anonymous, password: guest
2. The Windows installer for the [Master Client software application](http://customers.phasespace.com/anonymous/Software/5.3), look for: Master Client-5.2.315.0-x64.msi
3. The [PhaseSpace SDK packages](http://customers.phasespace.com/anonymous/SDK/5.2)


# Pre setup
Install [Visual Studio 2015 Community](http://download.microsoft.com/download/b/e/d/bedddfc4-55f4-4748-90a8-ffe38a40e89f/vs2015.3.com_enu.iso)

## Steps to run
1. STEP1_define_exp_v4.exe
2. Open record_PS_v1.sln in MS Visual Studio Community 2015
3. Retarget solution to 8.1 (likely you will need to approve some installataions VS2015 prompts)
4. Rescan solution
5. Rebuild solution
6. Rebuild record_PS_v1 under the x86 configuration
7. Run

## (Temporary instructions) To create a new solution
1. Copy and rename the vcxproj file
2. After opening it in visual studio, delete the old .cpp/.cc file
4. Add a new .cpp file (right click on project in solution explorer and Add new item) and build that instead.
5. When building retarget solution to 8.1 platform version
6. Rebuild solution and rebuild project
7. Run Windows Debugger in Release-x86 mode.

### Notes about IMU (Important)
1. For any experiment, re-address the microdrivers everytime.
2. Log in as root to the server using Powershell
3. ``ssh demo@192.180.0.170`` (password: demo)
4. ``su`` (password: phasespace)
5. Verify that you are root using whoami
6. ``fwload -r <rxaddr>`` to re-address your microdriver as rxaddr, press enter
7. Plug in microdriver into server USB port while holding down the red button.

The addressing scheme in the case of 4 drivers constitutes of a mask=0x03, select=[0x00|0x01|0x02|0x03]. Example:<br />
driver-1: rxaddr=0x8300<br />
driver-2: rxaddr=0x8301<br />
driver-3: rxaddr=0x8302<br />
driver-4: rxaddr=0x8303<br />

## Misc notes
1. Ensure all other Microdrivers and switched off during experiment.
2. Remember to set up session profiles and include specific microdrivers (TODO: consider how to isolate session-profile wise marker sampling in C++)
