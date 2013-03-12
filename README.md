musicMaker
==========

An Aszgard project for homework for Virtual Reality

Particulars about this application, you can select using the '1' on either wiimote.
Selecting the original musical objects makes them play noise. 
Using the 'A' buttons you can move any object.

Using the '-' button on the right hand you can import objects, selection is done with
the left wiimote, up and down arrows and the right '-' button to select. Home button on
right wiimote ends selection.

The modified SQUAD technique uses the left wiimote, holding down the '1' button opens the 
selection menu. Once the menu is open, the arrow keys on the dpad are used for selection.
the SQUAD works by filtering through objects in certain quadrants. 
ie. if there are more than one item in the top quadrant and you press the up arrow then 
the items in that quadrant will redistribute themselves in all the quadrants.

Note: in order for the music to work the mp3 files need to be copied from the 
data/music folder into aszgard5/szg/rsc

<br />

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
