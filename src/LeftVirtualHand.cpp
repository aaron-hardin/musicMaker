#include "LeftVirtualHand.h"

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
void LeftVirtualHand::extend(arEffector& self, vector<arInteractable*>& objects, float maxLength) {

	// Return if grabbing an object.
	if(getGrabbedObject() != 0) return;

	// Reset tip to 0.0 ft. length.
	_currentLength = 0.0;
	setTipOffset(arVector3(0, 0, -_currentLength));

	list<arInteractable*> objectlist;
	std::copy(objects.begin (), objects.end (), std::back_inserter(objectlist));
	
	// Check if the maximum length has been reached or an object has been touched.
	while(_currentLength < maxLength && !ar_pollingInteraction(self, objectlist)) {
		// If not, increase tip length by interaction distance.
		_currentLength += _interactionDistance;
		setTipOffset(arVector3(0, 0, -_currentLength));
	}
	if(selectionMode == 3 && ar_pollingInteraction(self, objectlist)) //if it interacted
	{
		//cout << "hit an object" << '\n';
		//find out which object it interacted with
		vector<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			if(ar_pollingInteraction(self, *i))
			{
				Object *oby = ((Object*)(*i));
				oby->_selected = !oby->_selected;
				selectionMode = 0;
			}
		}
		selectionMode = 0;
	}
	else if(selectionMode==3)
	{
		selectionMode = 0;
	}
	else if(selectionMode == 1)
	{
				
		leftSelectedObjects.clear();
		upSelectedObjects.clear();
		rightSelectedObjects.clear();
		downSelectedObjects.clear();
		int numObjects = 0;
	
		vector<arInteractable*>::iterator i;
		for(i=objects.begin(); i != objects.end(); ++i) 
		{
			// get object location
			Object* oby = ((Object*)(*i));
			arMatrix4 objLoc = oby->getMatrix();
			// if object is in cone, then add to selected list
			float x[] = {objLoc[12], objLoc[13], objLoc[14]};
			float height = getLength();
			float radius = height/2.f;
			arMatrix4 tp = getBaseMatrix();
			arMatrix4 bm = getMatrix();
			float t[] = {tp[12],tp[13],tp[14]};
			float b[] = {bm[12],bm[13],bm[14]};
			
			if(isLyingInCone(x, t, b, radius, height))
			{
				if((numObjects) % 4 == 0)
				{
					leftSelectedObjects.push_back(oby);
				}
				else if(numObjects % 4 == 1)
				{
					upSelectedObjects.push_back(oby);
				}
				else if(numObjects % 4 == 2)
				{
					rightSelectedObjects.push_back(oby);
				}
				else if(numObjects % 4 == 3)
				{
					downSelectedObjects.push_back(oby);
				}
				++numObjects;
			}
		}
		if(numObjects == 0)
		{
			selectionMode = 0;
		}
		else if(numObjects == 1)
		{
			//if only one object, select it
			selectionMode = 0;
			Object* oby = leftSelectedObjects.front();
			//set the one object to selected
			oby->_selected = !oby->_selected;
		}
		else
		{
			//else selectionMode = 2, selected lists have all the items
			selectionMode = 2;
		}		
	}
}


// Left hand effector's draw function.
void LeftVirtualHand::draw() //const 
{

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
		if(!handy)
		{
			glutSolidCube(1.0);
		}
		else
		{
			loadedOBJ.draw();
		}
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
		glColor3f(0.0, 0.0, 0.0); //TODO fix the bug that the color is left as red here and draws a red square
		if (!handy)
		{
			glColor3f(0.0, 0.0, 0.0);
			glutWireCube(1.01);
		}
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}

float LeftVirtualHand::getLength()
{
	return _currentLength;
}
