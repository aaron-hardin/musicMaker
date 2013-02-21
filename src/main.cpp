//************************************************************************************************
// Name:		basicMasterSlave
// Version:		1.0
// Description:	This arMasterSlaveFramework application demonstrates interactable objects,
//				effectors, openGL graphics, loading OBJ files, and incorporating sounds.
//************************************************************************************************

// The precompiled header include MUST appear as the first non-commented line. See 
// arCallingConventions.h for details.
#include "arPrecompiled.h"

// Includes for a MasterSlave framework application with interactable things.
#define SZG_DO_NOT_EXPORT
#include <stdio.h>
#include <stdlib.h>
#include "arMasterSlaveFramework.h"
#include "arInteractableThing.h"
#include "arInteractionUtilities.h"
#include "arNavigationUtilities.h"
#include "arGlut.h"
#include "arOBJ.h"

// Feet to local units conversion.
// For example, if you use centimeters to create your models, then set this to 12*2.54 or 30.48 to
// account for 12 inches per foot and 2.54 centimeters per inch.
// By default, Syzygy uses feet so this is set to 1.0.
const float FEET_TO_LOCAL_UNITS = 1.0;

// Near and far clipping plane distances
const float nearClipDistance = 0.1 * FEET_TO_LOCAL_UNITS;
const float farClipDistance = 100.0 * FEET_TO_LOCAL_UNITS;

bool coneselection = false;

arOBJRenderer musicNotey;
double currentTimeGlobal; //in millis

int selectionMode = 0; //0 means null, 1 means just entered, 2 means in selection

static int button1 = 0;
static double pressed1 = 0.0;
const double threshold = 2000.0;    // 2 seconds


// Object class
// Purpose:
//		- Creates an interactable cube or ellipsoid based on provided dimensions.
class Object:public arInteractableThing {

	public:
	
		bool _selected;
	
		// Default constructor. 
		// Parameters are:
		//		type - 0 for cube, 1 for sphere, 2 for loaded OBJ file
		//		length - length (X-axis) of object
		//		height - height (Y-axis) of object
		//		width - width (Z-axis) of object
		Object(int type = 0, float length = 1.0, float height = 1.0, float width = 1.0, const string& filename = ""):arInteractableThing() {
			// Track values.
			_type = type;
			_length = length;
			_height = height;
			_width = width;
			_selected = false;
			// Read OBJ file if provided.
			if(filename != "") {
				cout << "loading " << filename << '\n';
				cout.flush();
				if(!loadedOBJ.readOBJ(filename,"data")) { 
					cout << "Cound not load OBJ file: " << filename << '\n';
					_type = -1;
				}
				else if(_type == 0) {
					// Set type to loaded file
					_type = 2;
				}
			}
			// invalid type and file combination
			else if(_type >= 2) { _type = -1; }
		}
		
		// Draw the object's representation.
		void draw();
		
		// Get object's type.
		int getType() { return _type; }
		
		// Get object's length, height, or width.
		float getLength() { return _length; }
		float getHeight() { return _height; }
		float getWidth() { return _width; }
		
	private:
		
		// Object's type (0 for cube, 1 for sphere, 2 for loaded object)
		int _type;
		// Object's length, height, and width.
		float _length;
		float _height;
		float _width;
		// Object's loaded OBJ file
		arOBJRenderer loadedOBJ;
		
		//bool _selected;
};


// Interactable object's draw function.
void Object::draw() {
  	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		// Multiply by object's matrix.
		glMultMatrixf( getMatrix().v );
		// Scale to meet length, height, and width requirements.
		glScalef(_length, _height, _width);
		// Set color based on whether object has been selected.
		glColor3f(0.0, 1.0, 0.0);
		// Draw solid based on type.
		if(_type == 0) {
			// Create 1 ft. cube with black or highlighted wireframe.
			glutSolidCube(1.0);
			if(getHighlight()) { glColor3f(1.0, 1.0, 0.0); }
			else { glColor3f(0.0, 0.0, 0.0); }
			glutWireCube(1.01);
		}
		else if(_type == 1) {
			// Create 1 ft. diameter sphere with black or highlighted wireframe.
			glutSolidSphere(0.5, 16, 16);
			if(getHighlight()) { glColor3f(1.0, 1.0, 0.0); }
			else { glColor3f(0.0, 0.0, 0.0); }
			glutWireSphere(0.505, 16, 16);
		}
		else if(_type == 2) { 
			// Draw loaded OBJ file.
			loadedOBJ.draw();			
		}
		else if(_type > 2) { 
			// Draw loaded OBJ file.
			loadedOBJ.draw();
			if (_selected)
			{
				//draw little music notes
			/*	arOBJRenderer musicNote;
				musicNote.readOBJ("MusicNote.obj","data");
				musicNote.draw();
			*/
				
				//cout << "drawing note" << '\n';
			
				glScalef(1/_length, 1/_height, 1/_width);
			
				int timetrans = (int)(currentTimeGlobal/100);
				int modded = timetrans % 100;
				float ytranslat = (float)((float)modded / 100.0f);
				
				float xtranslat = (float)((float)(2-((int(currentTimeGlobal/1000)) % 5))/4.0f);
				
				glTranslatef(xtranslat,ytranslat,1.0f);
				//loadedOBJ.draw();
				musicNotey.draw();
				glTranslatef(-xtranslat,-ytranslat,-1.0f);
				
				glScalef(_length, _height, _width);
			}
		}
			
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}

	
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
			if(!loadedOBJ.readOBJ(filename)) { 
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
		void detectCollisions(arEffector& self, list<arInteractable*>& objects);
		
		// Draw a representation for the right hand.
		void draw() ;//const;
	
	private:
	
		float _size;								// effector size (length, height, and width)
		arDistanceInteractionSelector _selector;	// distance selector
	
		arOBJRenderer loadedOBJ;
};


// Right hand effector's collision detection function for multiple objects.
// Purpose:
//		- Determine closest object (based on center) that effector is touching.
// Notes:
//		The list must be a list of Object instances, defined above.
//		This function does not account for rotation of the object or the effector.
void RightVirtualHand::detectCollisions(arEffector& self, list<arInteractable*>& objects) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;
	
	// Set maximum distance for testing collision dection to 1000 ft.
	float maxDistance = 1000.0;
	
	// Track closest object and its distance. No object is initially closest.
	arInteractable* closestObject = 0;
	float closestDistance = maxDistance;
	
	// Step through list of objects and test each one for collisions.
	for(list<arInteractable*>::iterator it=objects.begin(); it != objects.end(); ++it) {
	
		// Get information about object's position and dimensions.
		const arMatrix4 objectMatrix = (*it)->getMatrix();
		arMatrix4 objectTransMatrix = ar_extractTranslationMatrix(objectMatrix);
		const float objectX = objectTransMatrix[12];
		const float objectY = objectTransMatrix[13];
		const float objectZ = objectTransMatrix[14];
		const float objectLength = ((Object*)(*it))->getLength();
		const float objectHeight = ((Object*)(*it))->getHeight();
		const float objectWidth = ((Object*)(*it))->getWidth();
		
		// Get information about effector's position and dimensions. 
		arMatrix4 effectorTransMatrix = ar_extractTranslationMatrix(getMatrix());
		const float effectorX = effectorTransMatrix[12];
		const float effectorY = effectorTransMatrix[13];
		const float effectorZ = effectorTransMatrix[14];
		const float effectorLength = _size;
		const float effectorHeight = _size;
		const float effectorWidth = _size;
		
		// Determine if effector is within object along X-axis.
		if((objectX - objectLength/2.0) <= (effectorX + effectorLength/2.0) && (effectorX - effectorLength/2.0) <= (objectX + objectLength/2.0)) {
		// Determine if effector is within object along Y-axis.
		if((objectY - objectHeight/2.0) <= (effectorY + effectorHeight/2.0) && (effectorY - effectorHeight/2.0) <= (objectY + objectHeight/2.0)) {
		// Determine if effector is within object along Z-axis.
		if((objectZ - objectWidth/2.0) <= (effectorZ + effectorWidth/2.0) && (effectorZ - effectorWidth/2.0) <= (objectZ + objectWidth/2.0)) {
		
			// Collision detected. Now use selector to determine distance to center of the object.
			_selector.setMaxDistance(maxDistance);
			float objectDistance = _selector.calcDistance(self, objectMatrix);
			
			// Determine if object is closest so far.
			if(objectDistance < closestDistance) {
				// If so, remember object and distance to object.
				closestObject = *it;
				closestDistance = objectDistance;
			}
		}}}
	}
	
	// Check if an object was touched.
	if(closestObject != 0) {
		// If so, set selector's distance to match object's distance.
		_selector.setMaxDistance(closestDistance);
		setInteractionSelector(_selector);
		
		//cout << "object was selected?" << '\n';
		
		if(getOnButton(1) == 1)
		{
			//cout << "object was selected" << '\n';
			Object* oby = (Object*)closestObject;
			oby->_selected = !oby->_selected;
		}
	}
	else {
		// If not, set selector's distance to size of effector.
		_selector.setMaxDistance(_size);
		setInteractionSelector(_selector);
	}
}


// Right hand effector's draw function.
void RightVirtualHand::draw() //const {
{

	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		// Set to center of effector.
		glMultMatrixf(getCenterMatrix().v);
		// Scale to make 1 ft. cube into a 0.5 ft. cube to match interaction distance.
	//	glScalef(0.5, 0.5, 0.5);
		// Draw the right hand red.
	//	glColor3f(1.0, 0.0, 0.0);
		// Create the 1 ft. cube.
	//	glutSolidCube(1.0);
		// Superimpose slightly larger black wireframe cube to make it easier to see shape.
	//	glColor3f(0.0, 0.0, 0.0);
	//	glutWireCube(1.01);
		glRotatef(180,0,1,0);
		glRotatef(90,0,0,1);
		glScalef(-1,1,1);
		loadedOBJ.draw();
	
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}


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
		LeftVirtualHand():arEffector(2, 11, 11, 0, 2, 2, 0) {
		
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
		void extend(arEffector& self, list<arInteractable*>& objects, float maxLength = 15.0);
		
		// Draw a representation for the right hand.
		void draw() const;
		
		float getLength();
	
	private:
		float _currentLength;
		float _interactionDistance;
	
};


// Left hand effector's extend function for a single object.
// Purpose:
//		- Extends effector until it touches the object or reaches the maximum length.
void LeftVirtualHand::extend(arEffector& self, arInteractable* object, float maxLength) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;

	// Reset tip to 0.0 ft. length.
	_currentLength = 0.0;
	setTipOffset(arVector3(0, 0, -_currentLength));

	// Check if the maximum length has been reached or an object has been touched.
	while(_currentLength < maxLength && !ar_pollingInteraction(self, object)) {
		// If not, increase tip length by interaction distance.
		_currentLength += _interactionDistance;
		setTipOffset(arVector3(0, 0, -_currentLength));
	}
}


// Left hand effector's extend function for multiple objects.
// Purpose:
//		- Extends effector until it touches an object or reaches the maximum length.
void LeftVirtualHand::extend(arEffector& self, list<arInteractable*>& objects, float maxLength) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;

	// Reset tip to 0.0 ft. length.
	_currentLength = 0.0;
	setTipOffset(arVector3(0, 0, -_currentLength));

	// Check if the maximum length has been reached or an object has been touched.
	while(_currentLength < maxLength && !ar_pollingInteraction(self, objects)) {
		// If not, increase tip length by interaction distance.
		_currentLength += _interactionDistance;
		setTipOffset(arVector3(0, 0, -_currentLength));
	}
	if(selectionMode == 3 && ar_pollingInteraction(self, objects)) //if it interacted
	{
		//cout << "hit an object" << '\n';
		//find out which object it interacted with
		list<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			if(ar_pollingInteraction(self, *i))
			{
				Object *oby = ((Object*)(*i));
				oby->_selected = !oby->_selected;
				selectionMode = 0;
			}
		}

	}
}


// Left hand effector's draw function.
void LeftVirtualHand::draw() const {

	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		// Set to center of effector.
		glMultMatrixf(getCenterMatrix().v);
		// Scale to make 1 ft. cube into a 2" x 2" rod with current length in ft.
		glScalef(2.0/12.0, 2.0/12.0, _currentLength);
		// Draw the left hand red.
		glColor3f(1.0, 0.0, 0.0);
		// Create the 1 ft. cube.
		glutSolidCube(1.0);
		// Superimpose slightly larger black wireframe cube to make it easier to see shape.
		
		if(coneselection == true)
		{
			glScalef(12.0/2.0, 12.0/2.0, 1/_currentLength);
			glColor3f(1.0, 1.0, 0.0);
			//GLdouble base=min(1*_currentLength,10.0f);
			GLdouble base=_currentLength/2.0;
			GLdouble height=1*_currentLength;
			GLint slices =50;
			GLint stacks =50;
			glTranslatef(0.0,0.0,-height/2.0f);
			glutSolidCone(base,height,slices,stacks);
			glTranslatef(0.0,0.0,height/2.0f);
			glScalef(2.0/12.0, 2.0/12.0, _currentLength);
		}
		
		glColor3f(0.0, 0.0, 0.0);
		glutWireCube(1.01);
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}

float LeftVirtualHand::getLength()
{
	return _currentLength;
}

/* dotProd, dif and magn copied from http://stackoverflow.com/questions/10768142/verify-if-point-is-inside-a-cone-in-3d-space
 credit to: furikuretsu
*/
static float dotProd(float a[], float b[])
{
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}
/*
static float* dif(float a[], float b[])
{
	float retval [] = {a[0]-b[0],a[1]-b[1],a[2]-b[2]};
    return retval;
}
*/
static float magn(float a[])
{
    return (float) (sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]));
}

/**
 * @param x coordinates of point to be tested 
 * @param t coordinates of apex point of cone
 * @param b coordinates of center of basement circle
 * @param aperture in radians
 
 Code copied from http://stackoverflow.com/questions/10768142/verify-if-point-is-inside-a-cone-in-3d-space
 credit to: furikuretsu
 
 altered to suit this purpose
 
 */
static bool isLyingInCone(float x[], float t[], float b[], float radius, float height)
{
	float aperture = 2.f * atan(radius/height);

    // This is for our convenience
    float halfAperture = aperture/2.f;

    // Vector pointing to X point from apex
    float apexToXVect[] = {t[0]-x[0],t[1]-x[1],t[2]-x[2]};

    // Vector pointing from apex to circle-center point.
    float axisVect[] = {t[0]-b[0],t[1]-b[1],t[2]-b[2]};

    // X is lying in cone only if it's lying in 
    // infinite version of its cone -- that is, 
    // not limited by "round basement".
    // We'll use dotProd() to 
    // determine angle between apexToXVect and axis.
    
	bool isInInfiniteCone = dotProd(apexToXVect,axisVect)/magn(apexToXVect)/magn(axisVect) > cos(halfAperture);
	
	// We can safely compare cos() of angles 
    // between vectors instead of bare angles.


    return isInInfiniteCone;
}


// Global objects.
Object theCello(3, 0.5, 0.5, 0.5, "cello.obj");
Object theViolin(3, 0.5, 0.5, 0.5, "violin.obj");
Object thePiano(3, 0.5, 0.5, 0.5, "piano.obj");



// List of objects.
list<arInteractable*> objects;

// Global effectors.
RightVirtualHand rightHand("handy.obj");//("Hand.obj");
LeftVirtualHand leftHand;

// Global sound variables.
int soundTransformID;
int clickSound;
int celloSound;
int violinSound;
int pianoSound;
int cpSound;
int cvSound;
int pvSound;
int cpvSound;

// MasterSlave transfer variables for shared memory. Only used in cluster-based systems. Hence,
// not needed in our system, but here's some examples.
// For this program, we just need to transfer each object's placement matrix and whether it is
// highlighted or not.
int cubeHighlighted = 0;
arMatrix4 cubeMatrix;
int boxHighlighted = 0;
arMatrix4 boxMatrix;
int sphereHighlighted = 0;
arMatrix4 sphereMatrix;
int ellipseHighlighted = 0;
arMatrix4 ellipseMatrix;
arMatrix4 celloMatrix;
arMatrix4 violinMatrix;
arMatrix4 pianoMatrix;





// start callback
// Purposes:
// 		- Register shared memory by adding transfer fields. This is to syncronize cluster-based 
//		  systems, hence we don't need to do this. But there's a few examples anyway.
// 		- Set up navigation through the framework by specifying translation conditions, rotation
//		  conditions, translation speed, and rotation speed. We provide a couple examples here.
//		- Initialize any global variables specific to your application.
// Notes: 
//		DO NOT initialize OpenGL here. The start callback is called before window creation. 
//		Instead initialize OpenGL in the windowStartGL callback.
bool start(arMasterSlaveFramework& framework, arSZGClient& client ) {

	// Register shared memory. Not needed for non-cluster-based systems.
	// framework.addTransferField(char* name, void* address, arDataType type, int numElements);
	framework.addTransferField("cubeHighlighted", &cubeHighlighted, AR_INT, 1);
	framework.addTransferField("cubeMatrix", &cubeMatrix, AR_FLOAT, 16);
	framework.addTransferField("boxHighlighted", &boxHighlighted, AR_INT, 1);
	framework.addTransferField("boxMatrix", &boxMatrix, AR_FLOAT, 16);
	framework.addTransferField("sphereHighlighted", &sphereHighlighted, AR_INT, 1);
	framework.addTransferField("sphereMatrix", &sphereMatrix, AR_FLOAT, 16);
	framework.addTransferField("ellipseHighlighted", &ellipseHighlighted, AR_INT, 1);
	framework.addTransferField("ellipseMatrix", &ellipseMatrix, AR_FLOAT, 16);
	framework.addTransferField("celloMatrix", &celloMatrix, AR_FLOAT, 16);
	framework.addTransferField("violinMatrix", &violinMatrix, AR_FLOAT, 16);
	framework.addTransferField("pianoMatrix", &pianoMatrix, AR_FLOAT, 16);
	
	
	// Set up navigation. 
	
	// A traditional pointing technique using the joystick with left and right rotating the scene.
	// Translate along Z-axis (forwards/backwards) if joystick is pressed more than 20% along axis 1.
	framework.setNavTransCondition('z', AR_EVENT_AXIS, 1, 0.2);
	// Rotate around Y-axis (vertical) if joystick is pressed more than 20% along axis 0.
	//framework.setNavRotCondition('y', AR_EVENT_AXIS, 0, 0.2);  //FOR ROTATE
	framework.setNavTransCondition('x', AR_EVENT_AXIS, 0, 0.2);  //FOR STRAFING
	// Set translation speed to 5 feet/second.
	framework.setNavTransSpeed(5.0);
	// Set rotation speed to 30 degrees/second.
	framework.setNavRotSpeed(30.0);
	
	
	// Initialize application variables here.
	
	// Move object's to initial positions.
	theCello.setMatrix(ar_translationMatrix(0, 5, -10));
	theViolin.setMatrix(ar_translationMatrix(2, 5, -5));
	thePiano.setMatrix(ar_translationMatrix(-2, 5, -5));
	// Keep list of objects to interact with.
	objects.push_back(&theCello);
	objects.push_back(&theViolin);
	objects.push_back(&thePiano);
	
	
	// Create sound transform.
	soundTransformID = dsTransform("world", framework.getNavNodeName(), ar_scaleMatrix(1.0));
	// Parameters are:
	//		name - string name for sound
	//		transformName - string name for dsTransform
	//		loopType - 1 for continuous, -1 for one-time, 0 to stop
	//		loudness - float from 0.0 (quiet) to 1.0 (max)
	//		positionVector - vector position of sound origin
	// Create loop for click sound.
	clickSound = dsLoop("click", "world", "click.mp3", 0, 1.0, arVector3(0, 0, 0));
	
	celloSound = dsLoop("cello", "world", "cello.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	violinSound = dsLoop("violin", "world", "violin.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	pianoSound = dsLoop("piano", "world", "piano.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	
	cpSound = dsLoop("cp", "world", "cp.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	cvSound = dsLoop("cv", "world", "cv.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	pvSound = dsLoop("pv", "world", "pv.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	
	cpvSound = dsLoop("cpv", "world", "cpv.mp3", 1, 1.0, arVector3(0, 5, -6)); 
	
	musicNotey.readOBJ("MusicNote.obj","data");
	
	// Return true if everything is initialized correctly.
	return true;
}


// windowStartGL callback
// Purposes:
// 		- Initialize each window (a Syzygy application can have more than one window).
//		- Initialize OpenGL.
void windowStartGL(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo) {
	
	// Initialize OpenGL. Set clear color to black.
	glClearColor(1, 1, 1, 0);
}


// windowEvent callback
// Purposes:
//		- Process window events, such as resizing
// Notes:
//		The values are defined in src/graphics/arGUIDefines.h.
//		arGUIWindowInfo is in arGUIInfo.h.
//		The window manager is in arGUIWindowManager.h.
void windowEvent(arMasterSlaveFramework& framework, arGUIWindowInfo* windowInfo) {

	// Process window events. Here, we handle window resizing.
	if(windowInfo->getState() == AR_WINDOW_RESIZE) {
		const int windowID = windowInfo->getWindowID();
#ifdef UNUSED
		const int x = windowInfo->getPosX();
		const int y = windowInfo->getPosY();
#endif
		const int width = windowInfo->getSizeX();
		const int height = windowInfo->getSizeY();
		framework.getWindowManager()->setWindowViewport(windowID, 0, 0, width, height);
	}
}


// preExchange callback
// Purposes:
//		- Handle navigation updates.
//		- Process user input.
//		- Set random variables.
//		- Update shared memory.
// Notes:
//		This is only called on the master node of the cluster and before shared memory is 
//		transferred to the slave nodes.
void preExchange(arMasterSlaveFramework& framework) {

	// Handle navigation update. The resulting navigation matrix is automatically transferred
	// to the slave nodes.
	framework.navUpdate();
	
	currentTimeGlobal = framework.getTime();
	
	// Process user input.
	
	
	
	/*if(leftHand.getOnButton(1) == 1)
	{
		coneselection = !coneselection;
	}*/
	
	

	//const unsigned int numButtons = framework.getNumberButtons();

	// in milliseconds
	double currentTime = framework.getTime();

	if(leftHand.getButton(0) || leftHand.getButton(2) || leftHand.getButton(3) || leftHand.getButton(4) || leftHand.getButton(5) || leftHand.getButton(10))
	{
		selectionMode = 0;
		coneselection = false;
	}
	
	button1 = leftHand.getButton(1);

	if(button1) {
		if(pressed1 == 0.0) {
			// get time since first I/O in milliseconds
			pressed1 = currentTime;
			//cout << "1 down\n";
		}
	}
	else {
		if(pressed1 > 0.0) 
		{
			//cout << "1 up\n";
			if(coneselection)
			{
				selectionMode = 1; // just entered selection mode
				coneselection = false;
				
				
				// for now lets just find out if our piano is in the cone
				float x[] = {pianoMatrix[12], pianoMatrix[13], pianoMatrix[14]};
				float height = leftHand.getLength();
				float radius = height/2.f;
				arMatrix4 tp = leftHand.getBaseMatrix();
				arMatrix4 bm = leftHand.getMatrix();
				float t[] = {tp[12],tp[13],tp[14]};
				float b[] = {bm[12],bm[13],bm[14]};
				cout << "is piano in cone?: " << isLyingInCone(x, t, b, radius, height) << '\n';
				cout << "x " << x[0] << " " << x[1] << " " << x[2] << '\n';
				cout << "t " << t[0] << " " << t[1] << " " << t[2] << '\n';
				cout << "b " << b[0] << " " << b[1] << " " << b[2] << '\n';
				cout << "radius " << radius << '\n';
			}
			else
			{
				//i just pointed and clicked but didn't hold down
				//TODO select what I pointed at
				selectionMode = 3;
			}
		}
		pressed1 = 0.0;
	}

	if(pressed1 > 0.0) {
		if((currentTime - pressed1) > threshold) {
			coneselection = true;
		}
	}
	
	
	
	// Detect right hand collisions.
	rightHand.detectCollisions(rightHand, objects);
	// Extend left ray to collision point.
	leftHand.extend(leftHand, objects);
	// Update input state (placement matrix & button states) of our effectors.
	rightHand.updateState(framework.getInputState());
	leftHand.updateState(framework.getInputState());
	// Handle any interaction with the objects (see interaction/arInteractionUtilities.h).
	ar_pollingInteraction(rightHand, objects);
	ar_pollingInteraction(leftHand, objects);
	
	
	// Play click sound if right hand has grabbed an object.
	if(rightHand.getGrabbedObject() != 0) {
		dsLoop(clickSound, "click.mp3", -1, 1.0, arVector3(0, 0, 0));
	}
	// Or reset the trigger
	else {
		dsLoop(clickSound, "click.mp3", 0, 1.0, arVector3(0, 0, 0));
	}
	
	
	
	// Update shared memory.
	
	// Transfer data about objects to slave nodes.
	celloMatrix = theCello.getMatrix();
	violinMatrix = theViolin.getMatrix();
	pianoMatrix = thePiano.getMatrix();
	
	
	arMatrix4 navMatrix = ar_getNavMatrix();
	
	dsLoop(celloSound, "cello.mp3", 0, 0, arVector3(celloMatrix[12], celloMatrix[13], celloMatrix[14]));
	dsLoop(violinSound, "violin.mp3", 0, 0, arVector3(violinMatrix[12], violinMatrix[13], violinMatrix[14]));
	dsLoop(pianoSound, "piano.mp3", 0, 0, arVector3(pianoMatrix[12], pianoMatrix[13], pianoMatrix[14]));
	dsLoop(cvSound, "cv.mp3", 0, 0, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	
	//TODO, find out if location matters here...if so fix it
	dsLoop(cpSound, "cp.mp3", 0, 0, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	dsLoop(pvSound, "pv.mp3", 0, 0, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	dsLoop(cpvSound, "cpv.mp3", 0, 0, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	
	if(theCello._selected == true && theViolin._selected == true && thePiano._selected == true)
	{
		//TODO, fix distance
		float cpvSoundDistance = sqrt((navMatrix[12] - violinMatrix[12])*(navMatrix[12] - violinMatrix[12]) +
								   (navMatrix[13] - violinMatrix[13])*(navMatrix[13] - violinMatrix[13]) +
								   (navMatrix[14] - violinMatrix[14])*(navMatrix[14] - violinMatrix[14]));
		float cpvLoudness = 1.0 - (cpvSoundDistance / 100.0);
		
		dsLoop(cpvSound, "cpv.mp3", 1, cpvLoudness, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	}
	else if(theCello._selected == true && theViolin._selected == false && thePiano._selected == false)
	{
		float celloSoundDistance = sqrt((navMatrix[12] - celloMatrix[12])*(navMatrix[12] - celloMatrix[12]) +
								   (navMatrix[13] - celloMatrix[13])*(navMatrix[13] - celloMatrix[13]) +
								   (navMatrix[14] - celloMatrix[14])*(navMatrix[14] - celloMatrix[14]));
		float celloLoudness = 1.0 - (celloSoundDistance / 100.0);
		
		dsLoop(celloSound, "cello.mp3", 1, celloLoudness, arVector3(celloMatrix[12], celloMatrix[13], celloMatrix[14]));
	}
	else if(theCello._selected == false && theViolin._selected == true && thePiano._selected == false)
	{
		float violinSoundDistance = sqrt((navMatrix[12] - violinMatrix[12])*(navMatrix[12] - violinMatrix[12]) +
								   (navMatrix[13] - violinMatrix[13])*(navMatrix[13] - violinMatrix[13]) +
								   (navMatrix[14] - violinMatrix[14])*(navMatrix[14] - violinMatrix[14]));
		float violinLoudness = 1.0 - (violinSoundDistance / 100.0);
		
		dsLoop(violinSound, "violin.mp3", 1, violinLoudness, arVector3(violinMatrix[12], violinMatrix[13], violinMatrix[14]));
	}
	else if(theCello._selected == false && theViolin._selected == false && thePiano._selected == true)
	{
		float pianoSoundDistance = sqrt((navMatrix[12] - pianoMatrix[12])*(navMatrix[12] - pianoMatrix[12]) +
								   (navMatrix[13] - pianoMatrix[13])*(navMatrix[13] - pianoMatrix[13]) +
								   (navMatrix[14] - pianoMatrix[14])*(navMatrix[14] - pianoMatrix[14]));
		float pianoLoudness = 1.0 - (pianoSoundDistance / 100.0);
		
		dsLoop(pianoSound, "piano.mp3", 1, pianoLoudness, arVector3(pianoMatrix[12], pianoMatrix[13], pianoMatrix[14]));
	}
	else if(theCello._selected == true && theViolin._selected == true && thePiano._selected == false)
	{
	
		//TODO, fix distance
		float cvSoundDistance = sqrt((navMatrix[12] - violinMatrix[12])*(navMatrix[12] - violinMatrix[12]) +
								   (navMatrix[13] - violinMatrix[13])*(navMatrix[13] - violinMatrix[13]) +
								   (navMatrix[14] - violinMatrix[14])*(navMatrix[14] - violinMatrix[14]));
		float cvLoudness = 1.0 - (cvSoundDistance / 100.0);
		
		dsLoop(cvSound, "cv.mp3", 1, cvLoudness, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	}
	else if(theCello._selected == true && theViolin._selected == false && thePiano._selected == true)
	{
		//TODO, fix distance
		float cpSoundDistance = sqrt((navMatrix[12] - violinMatrix[12])*(navMatrix[12] - violinMatrix[12]) +
								   (navMatrix[13] - violinMatrix[13])*(navMatrix[13] - violinMatrix[13]) +
								   (navMatrix[14] - violinMatrix[14])*(navMatrix[14] - violinMatrix[14]));
		float cpLoudness = 1.0 - (cpSoundDistance / 100.0);
		
		dsLoop(cpSound, "cp.mp3", 1, cpLoudness, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	}
	else if(theCello._selected == false && theViolin._selected == true && thePiano._selected == true)
	{
		//TODO, fix distance
		float pvSoundDistance = sqrt((navMatrix[12] - violinMatrix[12])*(navMatrix[12] - violinMatrix[12]) +
								   (navMatrix[13] - violinMatrix[13])*(navMatrix[13] - violinMatrix[13]) +
								   (navMatrix[14] - violinMatrix[14])*(navMatrix[14] - violinMatrix[14]));
		float pvLoudness = 1.0 - (pvSoundDistance / 100.0);
		
		dsLoop(pvSound, "pv.mp3", 1, pvLoudness, arVector3((violinMatrix[12]+celloMatrix[12])/2, (violinMatrix[13]+celloMatrix[13])/2, (violinMatrix[14]+celloMatrix[14])/2));
	}
	else
	{
		//also TODO
	}
}


// postExchange callback
// Purposes:
//		- Update effectors based on input state transferred from master node.
//		- Synchronize slave nodes with master node based on transferred data. This is not necessary
//		  for our system, but here's an example for cluster-based systems.
void postExchange(arMasterSlaveFramework& framework) {

	// Presumably the master node already is up-to-date, so we ignore it.
	if(!framework.getMaster()) {
	
		// Update effectors.
		rightHand.updateState(framework.getInputState());
		leftHand.updateState(framework.getInputState());
		
		// Synchronize shared memory.
		theCello.setMatrix(celloMatrix.v);
		theViolin.setMatrix(violinMatrix.v);
		thePiano.setMatrix(pianoMatrix.v);
	}
}


// draw callback
// Purposes:
//		- Load the navigation matrix to reflect frame's navigation
//		- Use OpenGL to generate computer graphics.
void draw(arMasterSlaveFramework& framework) {

	// Load the navigation matrix.
	framework.loadNavMatrix();
	
	// Generate graphics.
	
	// Draw the objects.
	theCello.draw();
	theViolin.draw();
	thePiano.draw();
	
	// Draw the effectors.
	rightHand.draw();
	leftHand.draw();
}


// main entry to MasterSlave application
int main(int argc, char** argv) {

	// Declare a MasterSlave framework.
	arMasterSlaveFramework framework;
	// Inform the framework what units you're using.
	framework.setUnitConversion(FEET_TO_LOCAL_UNITS);
	// Set the clipping plane distances.
	framework.setClipPlanes(nearClipDistance, farClipDistance);
	// Set start callback to register shared memory, set up navigation, and initialize variables.
	framework.setStartCallback(start);
	// Set windowStartGL callback to initialize OpenGL.
	framework.setWindowStartGLCallback(windowStartGL);
	// Set windowEvent callback to handle window events, like resizing.
	framework.setWindowEventCallback(windowEvent);
	// Set preExchange callback for master to process before shared memory is exchanged.
	framework.setPreExchangeCallback(preExchange);
	// Set postExchange callback for slaves to update themselves based on shared memory.
	framework.setPostExchangeCallback(postExchange);
	// Set draw callback for computer graphics.
	framework.setDrawCallback(draw);
	
	// Initialize MasterSlave framework.
	if(!framework.init(argc, argv)) {
		return 1;
	}
	
	// Start framework. Never returns unless something goes wrong.
	return framework.start() ? 0 : 1;
}
