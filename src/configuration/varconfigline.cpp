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

#include "varconfigline.hpp"

#include "../utils/stringutils.hpp"

using namespace std;
using namespace utils;
using namespace configuration;

VarConfigLine::VarConfigLine()
{
}

VarConfigLine::VarConfigLine(const string& name)
{
	this->name = name;
}

void VarConfigLine::parseLine(const string& line)
{
	CommentConfigLine::parseLine(line);

	string varLine;
	string::size_type hashPos = line.find('#');
	if(hashPos != string::npos) varLine = line.substr(0, hashPos);
	else varLine = line;

	varLine = StringUtils::trim(varLine);
	string::size_type startValPos = varLine.find("=\"") + 2;
	// string::size_type endValPos = varLine.find("\"", startValPos) - 1;

	name = StringUtils::trim(varLine.substr(0, startValPos - 2));
	value = varLine.substr(startValPos, varLine.length() - startValPos - 1);
}

string VarConfigLine::writeToLine()
{
	string comment = CommentConfigLine::writeToLine();
	if(comment.length() > 0) comment = ' ' + comment;

	return name + "=\"" + value + '"' + comment;
}

string VarConfigLine::getName()
{
	return name;
}

string VarConfigLine::getValue()
{
	return value;
}

void VarConfigLine::setValue(string value)
{
	this->value = value;
}
