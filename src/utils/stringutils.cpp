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

#include "stringutils.hpp"

#include <sstream>

using namespace std;
using namespace utils;

bool StringUtils::startsWith(const string& str, const string& prefix)
{
	return (str.substr(0, prefix.length()) == prefix);
}

vector<string> StringUtils::split(const string& str, char delim)
{
	vector<string> parts;
	istringstream iss(str);

	string part;
	while(getline(iss, part, delim))
	{
		parts.push_back(part);
	}

	return parts;
}

string StringUtils::join(char glue, const vector<string>& values)
{
	string str = values[0];
	for(vector<string>::size_type i = 1; i < values.size(); i++)
	{
		str += glue;
		str += values[i];
	}

	return str;
}

string StringUtils::trim(const string& str)
{
	string whiteChars = " \n\r\t";

	string::size_type firstPos = str.find_first_not_of(whiteChars);
	if(firstPos == string::npos)
	{
		return "";
	}

	string::size_type lastPos = str.find_last_not_of(whiteChars);
	return str.substr(firstPos, lastPos - firstPos + 1);
}
