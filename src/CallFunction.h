//see cpp for documentation

#ifndef CALLFUNCTION_H
#define CALLFUNCTION_H

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

class CallFunction 
{
	std::map<string, void(*)(vector<string>)> data;
	
	public:
		void add(const string& funName, void(*funPtr)(vector<string>) );
		void call(const string& funName, vector<string> args);
		void init();
};

#endif
