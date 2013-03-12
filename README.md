musicMaker
==========

An Aszgard project for homework for Virtual Reality

<b>Selection</b>
<br />
The '1' button on either wiimote selects objects.
The left wiimote is a ray and selects the object it is pointed at.
The right wiimote is a virtual hand and selects the object it is touching.
Select the original (see import below) musical objects to make them play music. 
Press the 'A' button on either wiimote to move any object.

<b>Import</b>
<br />
The '-' button on the right hand opens the directory menu.
The left wiimote up and down arrows are used to navigate the directory menu.
The right '-' button selects the highlighted file or directory (folder). 
If a directory is selected the menu will navigate to that directory.
If a file is selected it will be imported (if it is an OBJ).
The Home button on the right wiimote exits selection (without importing a file).

<b>SQUAD</b>
<br />
The modified SQUAD technique uses the left wiimote.
Hold down the left '1' button to open the selection menu. 
Once the menu is open, the arrow keys on the left d-pad are used for selection (see above).
The SQUAD works by filtering through objects in certain quadrants (areas, seperated by lines). 
ie. If the top quadrant contains more than one item, and you press the up arrow, then 
	the items in the top quadrant will redistribute themselves in all the quadrants.

<b>Running notes</b>
<br />
<i>Note</i>: in order for the music to work the mp3 files need to be copied from the 
data/music folder into aszgard5/szg/rsc
<br />
Development path = /c/aszgard5/szg/projects/musicMaker

<br /><br />
There is a README.txt because it was required by the instructor. 
There is a README.md because that is the defualt github readme. 
If there is any ambiguity, the README.md is taken to be the accurate version.
<br /><br /><br /><br />

original readme from demo

This directory is a build template for user applications. Copy it wherever
you want, replace the source files and modify build/makefiles/Makefile.my_app
appropriately.

To build, type 'make' in the top directory, i.e. in 'skeleton'.
'make clean' to clean up.

Build targets are specified in build/makefiles/Makefile.my_app.

For the build to work, you must set $SZGHOME (pointing to the top-level
directory of your syzygy install). Of course, you must have built the
Syzygy libraries first.

You will also want to set $SZGBIN (which indicates the directory in which
executables will be copied). If you don't set it, the exeutable for this
project will end up in szg/build/<platform> e.g. szg/build/win32 on
Windows.
Finally, if you are using external libraries
(such as sound), you will need to set $SZGEXTERNAL appropriately. Please
see the syzygy documentation.

skeleton.cpp is a simple, old-style master/slave application. It uses
some fancy user-interaction classes, but those can easily be ignored/removed.
In particular, if you just want to do simple things with user input you can
query the framework directly (using e.g. getButton( <index #> )) instead
of using an effector.

oopskel.cpp is an object-oriented version of the same, in which the
arMasterSlaveFramework is subclassed and callback methods (those
starting with on...()) are overridden instead of installing
callback functions.

arTeapotGraphicsPlugin.cpp is a scene-graph (szgrender) plugin, to show
how to use the skeleton to build a shared library.
