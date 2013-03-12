/*
Summary: this function is used to browse directories and implements a callback to handle the file result
TODO: needs to be cleaned and documented
		some functions need to be moved from main to help modularity

Author: Aaron Hardin

*/

#include "VirtualDirectory.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void VirtualDirectory::startBrowse(const string& cb,void(*funPtr)(vector<string>))
{
	currentIndex = 0;
	currentDir.clear();
	findingFile = true;
	foundFile = false;
	filename = "";
	callback = cb;
	cf.add(cb, funPtr);
	dirName = "c:\\";
	
	openDir("c:\\");
	
	/*cout << "current directory" << flush;
	vector<string>::iterator it;
	for (it=currentDir.begin(); it!=currentDir.end(); ++it)
		cout << ' ' << *it << '\n';
	*/
}

void VirtualDirectory::upPressed()
{
	currentIndex = (currentIndex-1+currentDir.size()) % currentDir.size();
}

void VirtualDirectory::downPressed()
{
	currentIndex = (currentIndex+1) % currentDir.size();
}

void VirtualDirectory::selectFile()
{
	//DIR *dir;
	//struct dirent *ent;
	string chosenFile = currentDir[currentIndex];
	
	/*printf("file is %s, %d\n",chosenFile.c_str(),currentIndex);
	
	if ((dir = opendir (dirName.c_str())) != NULL) 
	{
		
		while ((ent = readdir (dir)) != NULL) 
		{
			if(ent->d_name == chosenFile)
			{
				printf("found the file\n");
				closedir (dir);
				break;
			}
		}
		closedir (dir);
	} 
	else 
	{
		
		perror ("");
		//return EXIT_FAILURE;
	}*/

	//printf("found the file %s\n", ent->d_name);
	
	chosenFile = dirName + "\\" + chosenFile;
	
	struct stat st;
	if(stat(chosenFile.c_str(), &st) == -1)//ent->d_name, &st) == -1)
	{
		perror("");
	}
	else
	{
		if(S_ISDIR(st.st_mode))
		{
			printf("\t DIRECTORY\n");
			dirName = chosenFile +"\\";
			currentIndex = 0;
			openDir(chosenFile);
		}
		else
		{
			printf("\t FILE\n");
			currentIndex = 0;
			vector<string> filenamev;
			filenamev.push_back(chosenFile);
			filenamev.push_back(dirName);
			cf.call(callback,filenamev);
			findingFile = false;
		}
	}
}

void VirtualDirectory::openDir(string directory)
{
//cout << "opendir\n" << flush;
	currentDir.clear();
//cout << "cleared" << flush;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (directory.c_str())) != NULL) 
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
		//cout << "start add file" << flush;
			//printf ("%s\n", ent->d_name);
			currentDir.push_back(ent->d_name);
			//cout << "end add file" << flush;
		}
		//cout << "closedir" << flush;
		closedir (dir);
		//cout << "endclosedir" << flush;
	} 
	else 
	{
		/* could not open directory */
		perror ("");
		//return EXIT_FAILURE;
	}
}

void VirtualDirectory::draw() const 
{

	// Always start with glPushMatrix to avoid matrix multiplications done here from
	// affecting other portions of the scene.
	glPushMatrix();
		
		//string files [] = currentDir.getFiles()
		int upperbound = currentIndex+15;//std::min(currentIndex+15, (int)(currentDir.size()-currentIndex-1));
		//int i = 0;
		//cout << "starting vd.draw2\n" << flush;
		renderPrimitive(-2.5f, false);
		//cout << "starting vd.draw3\n" << flush;
		for (int i=currentIndex;i<upperbound;++i)
		{
			//display filename
			if(i==currentIndex)
			{
				drawText(-2.4f, 800-(i-currentIndex)*120, currentDir[i%(currentDir.size()+1)],true);
			}
			else if (i == (int)currentDir.size()) //leave a blank
			{
				drawText(-2.4f, 800-(i-currentIndex)*120, "------------------------------", false);
			}
			else
			{
				drawText(-2.4f, 800-(i-currentIndex)*120, currentDir[i%(currentDir.size()+1)], false);
			}
		}
		//cout << "end vd.draw3\n" << flush;
	// Always finish with glPopMatrix to match glPushMatrix above.
	glPopMatrix();
	//cout << "end vd.draw2\n" << flush;
}

void VirtualDirectory::drawText(float distance, float ypos, string text, bool selected = false) const
{
	glPushMatrix();
		glPushAttrib(GL_COLOR_BUFFER_BIT);
		glColor3f(1, 1, 1);
		glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations  
		  
		glTranslatef(0.0f, 0.0, distance); // Push eveything 5 units back into the scene, otherwise we won't see the primitive  
		
		glScalef(0.001, 0.001, 0.001);
		glTranslatef(0.0, ypos, 0.0);
		//glRotatef(-20, 1.0, 0.0, 0.0);
		

//cout << "starting vd.draw4\n" << flush;

			int lenghOfQuote = min((int)text.length(),300);
			glPushMatrix();
			//glTranslatef(-(lenghOfQuote*37), 0.0, 0.0);
			glTranslatef(-770, 0.0, 0.0);
			if(selected)
			{
				glPushAttrib(GL_COLOR_BUFFER_BIT);
				glColor3f(1, 0, 0);
			}
			//cout << "starting vd.draw5\n" << flush;
			if(lenghOfQuote < 300)
			{
			for (int i = 0; i < lenghOfQuote; i++)
			{
			//cout << "starting vd.draw6   " << lenghOfQuote << flush;
			//cout << "text:" << text[i] << "\n" << flush;
				//glColor3f((UpwardsScrollVelocity/10)+300+(l*10),(UpwardsScrollVelocity/10)+300+(l*10),0.0);
				glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
			//	cout << "end vd.draw6\n" << flush;
			}
			}
			if(selected)
			{
				glPopAttrib();
			}
			//cout << "end vd.draw5\n" << flush;
			glPopMatrix();
			//cout << "end vd.draw4\n" << flush;
		glPopAttrib();
			
	glPopMatrix();
}
