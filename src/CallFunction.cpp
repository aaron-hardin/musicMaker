#include "CallFunction.h"

void CallFunction::add( const string& funName, void(*funPtr)(vector<string>) )
{
	data.insert(std::pair<std::string, void(*)(vector<string>)>(funName,funPtr) );
}

void CallFunction::call( const string& funName, vector<string> args )
{
	data[funName](args);
	/*
	for (int i = 0; function_lookup_table[ i ].fn; i++)
	{
		if (name == function_lookup_table[ i ].key)
		{
			(*(function_lookup_table[ i ].fn))();
			return true;
		}
	}
	*/
	//return false;
}

void male(vector<string> args)
  {
  int i = 5;
  cout << "I'm a man." << i << "\n";
  cout << args[0] << "\n";
  }

void female(vector<string> args)
  {
  cout << "I'm a woman.\n";
  }
  
void CallFunction::init()
{
	/*string args[2];
	args[0] = "Ruzicka Vit";
    args[1] = "Bily Saruman";
	add("male",&male);
	add("female",&female);
	call("male",args);
	call("female",args);*/
}

