#include "systemutils.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

using namespace utils;

int SystemUtils::csystem(string cmd)
{
	//cout << "csystem: " << cmd << endl;
	//return 1;

	return system(cmd.c_str());
}

string SystemUtils::ssystem(string cmd, int bufferSize)
{
	//cout << "ssystem: " << cmd << endl;
	//return "";

	FILE *file = popen(cmd.c_str(), "r");
	if(file == NULL)
	{
		return "";
	}

	string output;
	char *buffer = new char[bufferSize];

	while(fgets(buffer, bufferSize, file) != NULL)
	{
		output += string(buffer);
	}

	delete[] buffer;
	pclose(file);

	return output;
}

int SystemUtils::getRAMSize(char& unit)
{
	string cmd = "cat /proc/meminfo | grep MemTotal";
	istringstream iss(ssystem(cmd));

	string tmp;
	int memSize;
	string memUnit;

	iss >> tmp >> memSize >> memUnit;
	unit = memUnit[0];

	return memSize;
}

int SystemUtils::alignSize(int minSize, int aligment)
{
	return (((minSize - 1) / aligment) + 1) * aligment;
}
