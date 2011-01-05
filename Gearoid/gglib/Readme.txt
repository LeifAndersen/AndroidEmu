#############################################################################
                           ____       __  _______ 
                          / __ \_____/  |/  / __ \
                         / / / / ___/ /|_/ / / / /
                        / /_/ / /  / /  / / /_/ / 
                       /_____/_/  /_/  /_/_____/ 
 
                             DrMD By Reesy
			     
          DrMD is a Genesis / MegaDrive emulator for the GP32/GP2X/Gizmondo.	
	                GP32/GP2X versions sponsered By GP2X www.gp2x.co.uk
					Gizmondo version sponsered by Zektor
	              Support DrMD by donating via Paypal 
				Paypal account: drsms_reesy@yahoo.co.uk
				      
#############################################################################
 DISCLAIMER
     Delete functions have been added, I have tested them and I have not had
     any problems.  They have been added for convenience but if something goes
     wrong and all your data gets wiped I take no responsibility.  If your unsure
     just manually delete any files.  
	 
	 Please note:
	 In GP2X version press SELECT in rom browser to delete a rom.
	 In Gizmondo version press REWIND in rom browser to delete a rom.
	 
#############################################################################

What is it?
-----------
    DrMD is a Sega Megadrive / Gensis, Sega Master System and Sega Game Gear
	Emulator.  80% of the emulator is written in ARM assembler to gain as much 
	emulation speed as possible.
    
#############################################################################

DrMD - GP32 Version Instructions
-------------------------------
    Due to memory constaints on the GP32 it is not possible to include the MD,SMS and GG support
	in one program. So I am going to have to release to programs one for MD emulation and one
	for SMS/GG emulation.  I'm still working on the SMS/GG version but the MD version is working now
	so I thought I would release it now in case I never get around to releasing anything.
Installation
    Extract the contents of the zip file to the root of your SMC.  This should give you a 
	DRMD and ROMS directory in the GPMM directory. 	In DRMD directory you will find MD,SMS and GG
	directories, these will be used to store your savestates and menu options for each system. 
	
	In the ROMS directory you will again find a MD,SMS and GG directory,  place the roms that you want into
	the appropriate directory.  You can change the default location of roms by manually updating
	the ROMDIR.OPT file which will be in the GPMM\DRMD\MD,GPMM\DRMD\SMS and GPMM\DRMD\GG
	directories.  Simply open the file in a text editor and replace the existing directory with whatever
	you want.
	
DrMD - GP2X Version Instructions 
------------------------------------
Installation
	Copy DrMD to a folder on your SD card.  It can be placed in any directory but be warned it
	will create 3 sub directories in the folder than it is executed from.  These directories are called
	MD,SMS and GG and are used to hold savestates and configuration setting.  Because of this
	I recommend putting DrMD in its only directory /mnt/sd/DrMD for example.
	
	Create another 3 folders on your SD card, these folders will be used to hold your rom files.  The setup
	I use is as follows
	
	<SD CARD ROOT>roms\MD - Put your Megadrive games in here
	<SD CARD ROOT>roms\SMS - Put your Master System games in here
	<SD CARD ROOT>roms\GG - Put your Game Gear games in here
	
	Feel free to you your own setup, the directories can be called anything you like and can be located anywhere
	you like.  Just remember where you put them because you will need to browse to these directories the first
	time you try and load a game.
	
	Once you have your folders setup, copy some roms into each of the directories and you are ready to go.
	
Setting up default rom locations
	The first time you attempt to browse for roms, DrMD will start you in the DrMD installation directory.  This
	doesn't mean you have to put all of your roms in the installation directory, its simply the default place the
	rom browser will look for roms.  
	
	In the rom browser anything that starts with + is a directory, selecting a directory will move you into that
		directory, you will then see a list of the files in that directory.  
	".." is always the second option in the list of roms, selecting this will take you up a directory.
	
	Use the rom browser and navigate to the directory where you installed you MD roms.  Once you are in the
	directory return to the main menu and then enter the MD options screen.  Near the bottom of the screen
	is an option called "Save current rom directory", if you select this DrMD will save the curren directory and
	use it as the default directory when loading MD roms.  
	
	Repeat the above process for SMS and GG roms.
	
	Everything else is pretty simple, so I'll let you work it out............have fun.
	
###########################################################

DrMD - Gizmondo Version Instructions 
------------------------------------
Installation
	Create a folder in the root of your SD card called DrMD.
	Now copy autorun.exe, SDL.DLL and KGSDK.DLL into the DrMD folder.
	Only use the version of SDL.DLL provided in the zip, as its a hack version which supports more
		buttons in the joystick code than the existing version of SDL.
	Create another 3 folders on your SD card, these folders will be used to hold your rom files.  The setup
	I use is as follows
	
	<SD CARD ROOT>roms\MD - Put your Megadrive games in here
	<SD CARD ROOT>roms\SMS - Put your Master System games in here
	<SD CARD ROOT>roms\GG - Put your Game Gear games in here
	
	Feel free to you your own setup, the directories can be called anything you like and can be located anywhere
	you like.  Just remember where you put them because you will need to browse to these directories the first
	time you try and load a game.
	
	Once you have your folders setup, copy some roms into each of the directories and you are ready to go.
	
Setting up default rom locations
	The first time you attempt to browse for roms, DrMD will start you in the DrMD installation directory.  This
	doesn't mean you have to put all of your roms in the installation directory, its simply the default place the
	rom browser will look for roms.  
	
	In the rom browser anything that starts with + is a directory, selecting a directory will move you into that
		directory, you will then see a list of the files in that directory.  
	".." is always the second option in the list of roms, selecting this will take you up a directory.
	
	Use the rom browser and navigate to the directory where you installed you MD roms.  Once you are in the
	directory return to the main menu and then enter the MD options screen.  Near the bottom of the screen
	is an option called "Save current rom directory", if you select this DrMD will save the curren directory and
	use it as the default directory when loading MD roms.  
	
	Repeat the above process for SMS and GG roms.
	
	Everything else is pretty simple, so I'll let you work it out............have fun.

#############################################################################

What's New
Ver 5 beta 8
	- A bit of a tidy up for the GP32 version.
	- Added the latest cyclone core from v0963 of picodrive which seems to be better (Thanks Notaz).
	- 2 binaries released, 1 for BLU and 1 for BLU+
Ver 5 beta 7
    - 6 button support appears to have broken a few games, so I've added a menu option to allow you
	   to choose what type of pad is currently being used.  When defining your keys you still have to 
	   define keys for a 6 button pad no matter what the pad setting is, I don't plan on changing this.
	- Removed 44100hz sound mode in MD because its still broken.  I enabled it again while I was testing
	   but forgot to remove it again.
Ver 5 beta 6
    - Ported DrMD to the Gizmondo
	- Fixed save state problem
	- Added new cyclone core by Notaz
	- Fixed loads and loads and loads of bugs.
	- [GP2X] Improved MMU hacking routine.  This should get rid of the problem where the emulator would
	   lock up when starting games.
Ver 5 beta 5
    - With the help of Squidge, I have resolved the problem with the flickering lines on the left and right of the
	   screen.  Basically I have added a call to flush the cache just before flipping the frame buffer.
Ver 5 beta 4
    - With the help of Pepone I have applied Squidges MMU hack to DrMD.  This has increased performance.
	- You may notice the menu is a little to fast since the change,  I'll resolve this later.
Ver 5 beta 3
    - Rom browser updated, you can now change directory and setup your default rom directories for
	   MD, SMS and GG roms.
	- 6 button support added
	- GG support added
	- GG and SMS full screen modes added
	- Fixed bug in TV-out, now works perfectly
	- Fixed bug in MD rendering that was causing some games to crash that used to work on previous versions
	   of DrMD.
	- Re-wrote save state code which has broken backward compatiblity with beta 2.  I'm not going to fix this
	  because basically I have no idea what has changed.  The re-write means that save states should never be
	  broken again though.
Ver 5 beta 2
    - Options stored in current working directory
    - Fixed rom names wrapping
    - Fixed unzipped roms crashing file scan
    - 44100Hz audio removed for MD as appears to be broken
    - Redefine MD Controls screen fixed
Ver 5 beta 1
    - Added Sega Master System support.  This has meant a lot of changes to the menu code
	  hopefully I've not broken anything but with the amount of changes that I made I 
	  wouldn't be surprised.
	- Added cpu overclocking
	- Increased amount of roms to 3000
	- Fixed TV-out..maybe
Ver 4.3
    - Everybody's noticed problems when saving data.  It seems you need to call
	  the sync() function after writing to a file in order to force linux to finish
	  writing the file.  People were losing data because they switched off their
	  GP2X before Linux had written the data to disk.  I have now made sure that
	  any fwrite commands are now followed by sync() so we should be ok from now on.
	- Noticed a bug in my code that will crash the emulator if you try to scan more
	  than 512 roms.  Never been a problem before as the GP32 only had 128MB cards.
	  The rom limit is still 512 roms but now the code will stop and display a message
	  informing you that you have too many roms.  In the next version I'm planning on
	  a new file browser that will allow you to browse to any directory, this is why
	  I don't want to spend too much time updating the current browser system. 
	- Change the menu colour scheme.
	- Removed code that updated the gamma of the menu when you changed the gamma settings.
	  The gamma setting now only affects the games.
Ver 4.2
    - Yeah I know this is the first version released for the GP2X but I'm keeping
	  the GP32 version numbers as it stops me getting confused. Plus all you DrMD
	  newbies can see all of the history of DrMD and have some idea how much work
	  has gone into it ( again this is a blatent hint that you should donate! )
	- Ok this is the GP2X version, it does everything the GP32 version does but it
	  does it smoother and faster :)
	- This is just ya basic port, there are no optimisations yet.  The next few 
	  versions will see performance improvements by making use of the GP2X's
	  hardware a bit more.
	  
	- KNOWN BUGS
	    - Sound suddenly stops working.  Only way to get it back is to restart GP2X
		     Not sure whats causing this at the moment but I'll get to the bottom of
			 it sooner or later.
		- Menu flickers REALLY BADLY.  For some reason my vsync code only works on the 
		     latest firmware ( currently version 1.0.1 ).  I only have to use Vsync on
			 the menu's at the moment so the graphics while running a game should be fine.
			 I may have solution for this but it will have to wait until the next release.
			 
Ver 4.1
    - Added genesis rom banking, this fixes games like Earthworm Jim 1 + 2 which
	  require banking to be emulated when SRAM is being emulated as well.  This
	  may fix alot of other games as well.
	- DrMD now supports up to 512 roms again, 4.0 used an old version of the smc
	  library which only supports up to 128.
Ver 4.0
	- Fixed DMA emulation - Contra - Hard Corps now fixed
	- Added SRAM emulation - fixes PGA golf, yay!  This has meant a change to
	  the save state format and menu options.  Saves states from version 3.0
	  and Beta 11 will load on version 4.0 but version 4.0 save states will not
	  load on older versions.
	- Mega Bomberman now working again, I screwed up the Cyclone core in
	  the Beta 11 release.
	- Menu still at 40Mhz but scrolling background is back by popular demand ;)
	  Menu now switches to 133Mhz when uncompressing roms and other cpu intensive
	  tasks.
Beta 11
	- Rewrote input port emulation, this has fixed Decap Attack, Power Intinct
	  and Samurai Shodown.  
	- Made a first attempt at re-writing the DMA emulation.  Landstalker seems
	  to be working because of this re-write but I'm not 100% confident that the
	  DMA emulation is correct so I'm going to have another go.
	- Optimised the menu so it can run at lower clock speeds. You'll notice that
	  the scrolling background has gone, it was a bit pointless so it was first 
	  to go.  The menu used to run at 133Mhz, it now runs at 40Mhz so your 
	  batteries will last a bit longer when in the menu.
Ver 3.0
	- Could not get a Beta out to my beta testers so decided to just release
	  a new version, hopefully everything is okay but if its not let me know
	  on the GP32X forums www.gp32x.com.
	- DrMD now built using Cyclone core updated by Notaz, fixes alot of games
	- Fixed the split screen bug that occasionally happens when entering the
	  menu.  If it happens any more let me know :)
Beta 10 - Never made it thanks to carry Yahoo Email
	- DrMD now built using Cyclone core updated by Notaz, fixes alot of games
Beta 9
	- Some more Cyclone fixes, basically re-wrote all the changes I made
	  before to the divu,divs,asl,asr etc.  Hopefully they work now :)
	- Added Stop opcode to Cyclone - Fixes Thunderforce IV
	- Added support for the Genesis display mode that allows different
	  vertical scroll values for each column on the screen.  This fixes
	  the rocket level on Gunstar Heroes.
Beta 8
	- The first of Cyclone fixes, divu,divs,asl,asr,lsl,lsr.
	  Sensible soccer now works :)
Ver 2.0
	- Rewrote sound rendering engine in order to make DAC emulation
	  more accurate.  
	- Fixed FM timer emulation - they were running to slowly, this 
	  caused slow music in games such as Outrun.
	- Added support for v1.0 save states
	- Fixed pallet problems in Another World.  I was masking the 
	  pallet data with #F000 rather than #0xF000.
Beta 7
	- Removed a few of the sound options in order to speed up sound
	  emulation.
	- Added different sound rates 8Khz,11Khz,16Khz and 22Khz
	  note: 16Khz was the original speed
	- Major re-write of sprite routines - fixes Sonic start screen
	- Fixed FM timer emulation - fixes the sound in lots of games
	- Improved fixed frame skip options
	- Improved DAC emulation
       
Beta 6
	- Added rom delete option.
	- Added save state delete option.
	- Added save settings per game option.
	- Added delete settings per game option.
	- Added fixed frameskip options.
	- Gamma correction now effects menu as well.
	- Added finer control for gamma correction.
	- Changed fast menu scroll to be LEFT and RIGHT rather than L and R.
	- Added more cpu speed settings.
	- Added stereo reverse option.
       
Beta 5
	- Fixed slight bug in sound core, music in Sonic should now be correct again.
	- Added Quick Load/Save button combination :) Very swish
	  To save a state quickly to memory hold down L and press START
	  To load a state quickly from memory hold down R and press START
	  I've made sure that pressing START will not pause the game every time you 
	  load or save a state so it should be fine. Let me know what you think.
	- I removed the L and R to page the main menu options. 
	  It was proving to be more hassle than it was worth due to the fact 
	  that L and R are used to exit out of the menu.  So every time you exit 
	  from the menu you lose the last menu option you were on, 
	  which I find very useful.

Beta 4
	- Added correct gamma control to emulator.  This only effects the games
	  and not the menu system.
	- Re-compiled with faster rendering code, beta 3 was compiled with some
	  slightly slower test code I was working on.
	    
Beta 3
	- A few cyclone core fixes - Paperboy now works, Time in Mortal Combat 
	  and Streets of rage now works correctly.  Riders in Road Rash now 
	  lean in correct direction.  Still lots more Cyclone bugs to find 
	  though ;)
	- Corrected emulation of Hcount - fixes Road Rash Series.
	- Fixed palette problems in Sonic games as well as others.
	- Added Quick Save/Load function.  This saves only to memory so when you 
	  switch off the gp32 the save is lost, but it is usefull when you 
	  want to make quite a few saves when making your way through a 
	  difficult game.
	- Added reset game option - saves having to reload the whole rom.
	- Removed 16bit rendering option to free up memory as it was hardly ever used.
	- Fixed tile 0 rendering problem - fixes Battle Squadron.
	- Added R and L quick scroll to main menu.
       
ver 1.0
	- Split screen bug fixed...Hopefully
		...But if its not, at least you can save your game. :)
	- Added LCD type selection screen.  This is for those new gp32s with
	  different LCD's
	****************************************
	Important, if you have a new blu ( BLU+ as they're being called)
	When you first start DrMD the screen will be corrupted, simply go
	to the menu and then scroll down to the LCD menu option.  Then
	press A or B, this will switch the LCD settings.  The screen should
	now be fine.  You should then save the settings, by using the save
	options function near the bottom of the menu       
	****************************************
	- Now using Mr Mirko's SDK 
	- Zipped rom support 
		- using ZLIB 2.1 and
		- UNZIP.H by Gilles Vollant
	- Animated zipped save states
		- compression done using ZIP.H by Gilles Vollant
       
	- User definable Genesis pad configuration
	- Triple buffering fixed - hopefully :)
	- FPS monitor option
	- Menu has been tarted up
	- Menu options can now be saved
	- Rom selection screen now uses long filenames found in zip files or 
	  filename found using a CRC lookup on the GoodGEN database.
	- Auto region detection fixed
	- Irq callback support added to Cyclone core
       
ver 0.001
	- Initial Release
	- smd & bin rom support
	- Decent PSG & FM Sound Emulation  - By Rob "Cobbleware"
	- sound on/off option
	- 8Bit or 16Bit Rendering modes
	- Cpu speed selection
	- Triple Buffering option
	- Region Selection

#############################################################################

Thanks To
    Mr DJ Willis, Rob Brown, Squidge and Rlyeh.  You've all done an amazing job
	    hacking away at the GP2X hardware and getting the development scene for the
		GP2X off to a flying start.  
		You've all helped me a great deal, so thanks lads.

	Craigx for supplying dev kit and retail versions of the GP2X for me to play
	   with and for pretty much being the driving force behind the GP2X dev scene.
	   Keep it up mate!
	Note to reader - when your showing your mates DrMD and they go wow!, 
	                 I've got to get a gp2x.  Send them to www.gp2x.co.uk
					 
    Dave "FinalBurn" for releasing the source code for Gigadrive.  
       I've basically learn't how to code in C from using your source code,
       DrMD simply would not exist without all of the work you put into
       GigaDrive and Cyclone.  Thanks your da man as well!
	Notaz for continuing fDave's work and do a damm fine job, cheers.

	Zektor for donating his old Gizmondo in order to encourage me to port DrMD to it.
	   Cheers mate!
	   
    Gilles Vollant for his ZIP/UNZIP functions which made adding zip support
        DrMD a breeze. 
    And everybody else I've forgotten...

	Wow you read everything....well done. 
	
###############################################################################
                              www.gp2x.co.uk	
			                 Sponsers of DrMD
################################################################################
