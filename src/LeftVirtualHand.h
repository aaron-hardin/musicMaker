#ifndef LEFTVIRTUALHAND_H
#define LEFTVIRTUALHAND_H

#include "main.h"
#include "Object.h"
#include "selectedObjects.h"



// LeftVirtualHand class
// Purpose:
//		- Creates a dynamic pointer to represent the left hand and provide for asynchronous
//		  translations and rotations.
class LeftVirtualHand:public arEffector {

	public:
	
		// Default constructor. 
		// Parameters for arEffector are:
		//		matrixIndex - index of matrix to use as position (2 is the left hand)
		//		numButtons - number of buttons to use from device array (11 buttons for Wiimote)
		//		loButton - index in device array of desired first button, (11 for left Wiimote)
		//				   which gets mapped to virtual button index 0 by default 
		//		buttonOffset - offset for mapping virtual buttons (0 for no offset)
		//		numAxes - number of axes to use from device (2 axes for Wiimote)
		//		loAxis - index in device array of desired first axis, (2 for left Wiimote)
		//				 which gets mapped to virtual axis index 0 by default
		//		axisOffset - offset for mapping virtual axes (0 for no offset)
		LeftVirtualHand(const string& filename = ""):arEffector(2, 11, 11, 0, 2, 2, 0) {
		handy = false;
		if(filename != "") {
			handy = true;
			if(!loadedOBJ.readOBJ(filename,"data/obj")) { 
				cout << "Cound not load OBJ file: " << filename << '\n';
				handy = false;
			}
		}
		
			// Set "tip" or point of interaction. Make ray initially 5 ft. long because we're going
			// to dynamically extend it to whatever object it touches first.
			_currentLength = 5.0;
			setTipOffset(arVector3(0, 0, -_currentLength));
			
			// Set to interact with closest object within 0.5 ft. of tip.
			// See szg/src/interaction/arInteractionSelector.h for alternative ways to select objects.
			_interactionDistance = 0.5;
			setInteractionSelector(arDistanceInteractionSelector(_interactionDistance));
			
			// Create grab condition when "A" button is pressed more than 0.5 to 
			// translate and rotate the selected object.
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
			setDrag(arGrabCondition(AR_EVENT_BUTTON, 3, 0.5), arWandRelativeDrag());
		}
		
		// Manipulator to make the effector extend until it touches the object or reaches
		// a maximum length (15 ft. by default).
		void extend(arEffector& self, arInteractable* object, float maxLength = 15.0);
		
		// Manipulator to make the effector extend until it touches an object or reaches
		// a maximum length (15 ft. by default).
		void extend(arEffector& self, vector<arInteractable*>& objects, float maxLength = 15.0);
		
		// Draw a representation for the right hand.
		void draw(); //const;
		
		float getLength();
	
	private:
		float _currentLength;
		float _interactionDistance;
		
		bool handy;
		arOBJRenderer loadedOBJ;
	
};

#endif

