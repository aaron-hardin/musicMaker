#include "Object.h"



// Interactable object's draw function.
void Object::draw() 
{
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
				glPushMatrix();
				
				float scale = 2.f;
				
				glScalef(scale/_length, scale/_height, scale/_width);
			
				int timetrans = (int)(currentTimeGlobal/100);
				int modded = timetrans % 100;
				float ytranslat = (float)((float)modded / 100.0f)/scale;
				
				float xtranslat = (float)((float)(2-((int(currentTimeGlobal/1000)) % 5))/4.0f)/scale;
				
				glTranslatef(xtranslat,ytranslat,1.0f);
				//loadedOBJ.draw();
				musicNotey.draw();
				//glTranslatef(-xtranslat,-ytranslat,-1.0f);
				
				//glScalef(_length, _height, _width);
				glPopMatrix();
			}
		}
			
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
}
