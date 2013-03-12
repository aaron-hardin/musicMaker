#ifndef IMPORT_H
#define IMPORT_H

#include "main.h"
#include "Object.h"

//extern list<arInteractable*> objects;

class Import
{
	public:
		
		void copyTo(); //used to copy file to proper folder in current project directory
		static void import(const string &filename, const string &path); //used to import into our environment
		static void importCallback(vector<string> args);
		/*
		Object theCello(3, 0.5, 0.5, 0.5, "cello.obj");
		theCello.setMatrix(ar_translationMatrix(0, 4, -8)); // initial position
		objects.push_back(&theCello);
		*/
};


#endif
