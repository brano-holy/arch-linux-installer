/*
 * Arch Linux Installer
 * Copyright (C) 2015  Branislav Holý <branoholy@gmail.com>
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

#include "archlinuxinstaller/configuration/commentconfigline.hpp"

namespace archlinuxinstaller {
namespace configuration {

CommentConfigLine::CommentConfigLine() :
	commentPresent(false)
{
}

void CommentConfigLine::parseLine(const std::string& line)
{
	std::size_t hashPos = line.find('#');
	if(hashPos != std::string::npos)
	{
		commentPresent = true;
		comment = line.substr(hashPos + 1);
	}
}

void CommentConfigLine::writeLine(std::ostream& out) const
{
	if(commentPresent) out << '#' << comment;
}

void CommentConfigLine::setComment(const std::string& comment)
{
	this->comment = comment;
	commentPresent = true;
}

void CommentConfigLine::removeComment()
{
	comment = "";
	commentPresent = false;
}

}}
