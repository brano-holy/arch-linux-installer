/*
 * Arch Linux Installer
 * Copyright (C) 2014  Branislav Holý <branoholy@gmail.com>
 *
 * This file is part of Arch Linux Installer.
 *
 * Arch Linux Installer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arch Linux Installer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arch Linux Installer.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "systemutils.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;
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
