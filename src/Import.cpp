#include "Import.h"


void Import::import(const string &filename,const string &path = "data/obj")
{
	int length = filename.length();
	if (filename[length-1] == 'j' && filename[length-2] == 'b' && filename[length-3] == 'o')
	{
		Object* newObj = new Object(2,1,1,1,filename,path);
		newObj->setMatrix(ar_translationMatrix(0, 4, -8)); // initial position
		objects.push_back(newObj);
	}
}

void Import::importCallback(vector<string> args)
{
	if(args.size() > 1)
	{
		import(args[0],args[1]);
	}
	else
	{
		import(args[0]);
	}
}

