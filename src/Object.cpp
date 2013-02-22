#include "main.h"

// Object class
// Purpose:
//		- Creates an interactable object based on provided dimensions.
class Object:public arInteractableThing {
	
	//extern double currentTimeGlobal;
	
	public:
	
		bool _selected;
	
		// Default constructor. 
		// Parameters are:
		//		type - 0 for cube, 1 for sphere, 2 for loaded OBJ file
		//		length - length (X-axis) of object
		//		height - height (Y-axis) of object
		//		width - width (Z-axis) of object
		Object(int type = 0, float length = 1.0, float height = 1.0, float width = 1.0, const string& filename = ""):arInteractableThing() 
		{
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
		arOBJRenderer getOBJ() { return loadedOBJ; }
		
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
