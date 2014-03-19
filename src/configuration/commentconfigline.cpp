/*
 * Arch Linux Installer
 * Copyright (C) 2014  Branislav Holý <brano.holy@gmail.com>
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

#include "commentconfigline.hpp"

using namespace configuration;

using namespace std;

CommentConfigLine::CommentConfigLine()
{
	hasComment = false;
}

void CommentConfigLine::parseLine(const string& line)
{
	string::size_type hashPos = line.find('#');
	if(hashPos != string::npos)
	{
		hasComment = true;
		comment = line.substr(hashPos + 1);
	}
}

string CommentConfigLine::writeToLine()
{
	if(hasComment)
	{
		return '#' + comment;
	}

	return "";
}

string CommentConfigLine::getComment() const
{
	return comment;
}

void CommentConfigLine::setComment(const string& comment)
{
	this->comment = comment;
	hasComment = true;
}

void CommentConfigLine::removeComment()
{
	comment = "";
	hasComment = false;
}
