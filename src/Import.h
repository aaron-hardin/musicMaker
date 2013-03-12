// see cpp for documentation


#ifndef IMPORT_H
#define IMPORT_H

#include "main.h"
#include "Object.h"

class Import
{
	public:
		
		void copyTo(); //used to copy file to proper folder in current project directory
		static void import(const string &filename, const string &path); //used to import into our environment
		static void importCallback(vector<string> args);
};


#endif
