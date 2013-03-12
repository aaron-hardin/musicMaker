#ifndef RIGHTVIRTUALHAND_H
#define RIGHTVIRTUALHAND_H

#include "main.h"	
#include "Object.h"

// RightVirtualHand class
// Purpose:
//		- Creates a 0.5 ft. cube to represent the right hand.
//		- Provides for asynchronous translations and rotations.
//		- Uses basic collision detection for better object grabbing.
// Notes:
//		Extends the arEffector class (szg/src/interaction/arEffector.h).
class RightVirtualHand:public arEffector {

	public:
	
		// Default constructor. 
		// Parameters for arEffector are:
		//		matrixIndex - index of matrix to use as position (1 is the right hand)
		//		numButtons - number of buttons to use from device array (11 buttons for Wiimote)
		//		loButton - index in device array of desired first button, (0 for right Wiimote)
		//				   which gets mapped to virtual button index 0 by default 
		//		buttonOffset - offset for mapping virtual buttons (0 for no offset)
		//		numAxes - number of axes to use from device (2 axes for Wiimote)
		//		loAxis - index in device array of desired first axis, (0 for right Wiimote)
		//				 which gets mapped to virtual axis index 0 by default
		//		axisOffset - offset for mapping virtual axes (0 for no offset)
		RightVirtualHand(const string& filename = ""):arEffector(1, 11, 0, 0, 2, 0, 0) {
		if(filename != "") {
			if(!loadedOBJ.readOBJ(filename, "data/obj")) { 
				cout << "Cound not load OBJ file: " << filename << '\n';
			}
		}
		
		
			// Set "tip" or point of interaction. This is primarily for rays.
			setTipOffset(arVector3(0, 0, 0));
			
			// Set to interact with closest object within 0.5 ft. of tip using a distance selector.
			// See szg/src/interaction/arInteractionSelector.h for alternative selectors.
			_size = 0.5;
			_selector.setMaxDistance(_size);
			setInteractionSelector(_selector);
			
			// Create grab condition when "A" button is pressed more than 0.5 to 
			// translate the selected object without rotating it.
			// Wiimote index/button pairs:
			//		0	"2"
			//		1	"1"
			//		2	"B"
			//		3	"A"
			//		4	"-"
			//		5	"+"
			//		6	"L"
			//		7	"D"
			//		8	"R"
			//		9	"U"
			//		10	"H" or Home
			setDrag(arGrabCondition(AR_EVENT_BUTTON, 3, 0.5), arWandTranslationDrag());
			
			// Create grab condition when "B" button is pressed more than 0.5 to 
			// rotate and translate the selected object.
			setDrag(arGrabCondition(AR_EVENT_BUTTON, 2, 0.5), arWandRelativeDrag());
		}
		
		// Resizes distance selector for basic collision detection.
		void detectCollisions(arEffector& self, vector<arInteractable*>& objects);
		
		// Draw a representation for the right hand.
		void draw() ;//const;
	
	private:
	
		float _size;								// effector size (length, height, and width)
		arDistanceInteractionSelector _selector;	// distance selector
	
		arOBJRenderer loadedOBJ;
};

#endif
