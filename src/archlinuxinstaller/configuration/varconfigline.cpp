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

#include "archlinuxinstaller/configuration/varconfigline.hpp"

#include <boost/algorithm/string/trim.hpp>

namespace archlinuxinstaller {
namespace configuration {

VarConfigLine::VarConfigLine() : CommentConfigLine()
{
}

VarConfigLine::VarConfigLine(const std::string& name) : CommentConfigLine(),
	name(name)
{
}

void VarConfigLine::parseLine(const std::string& line)
{
	CommentConfigLine::parseLine(line);

	std::string varLine;
	std::size_t hashPos = line.find('#');
	if(hashPos != std::string::npos) varLine = line.substr(0, hashPos);
	else varLine = line;

	boost::algorithm::trim(varLine);
	std::size_t startValPos = varLine.find("=\"") + 2;
	// string::size_type endValPos = varLine.find("\"", startValPos) - 1;

	name = boost::algorithm::trim_copy(varLine.substr(0, startValPos - 2));
	value = varLine.substr(startValPos, varLine.length() - startValPos - 1);
}

void VarConfigLine::writeLine(std::ostream& out) const
{
	out << name << "=\"" << value << '"';
	if(hasComment()) out << ' ';
	CommentConfigLine::writeLine(out);
}

}}
