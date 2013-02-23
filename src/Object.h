#ifndef OBJECT_H
#define OBJECT_H

#include "main.h"

// Object class
// Purpose:
//		- Creates an interactable object based on provided dimensions.
class Object:public arInteractableThing {
	
	//extern double currentTimeGlobal;
	
	public:
	
		bool _selected;
		
		// Object's loaded OBJ file
		arOBJRenderer loadedOBJ;
	
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
		arOBJRenderer* getOBJ() { return &loadedOBJ; }
		
	private:
		
		// Object's type (0 for cube, 1 for sphere, 2 for loaded object)
		int _type;
		// Object's length, height, and width.
		float _length;
		float _height;
		float _width;
		
		
		//bool _selected;
};

#endif
