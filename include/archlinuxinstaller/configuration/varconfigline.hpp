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

#ifndef ARCHLINUXINSTALLER_CONFIGURATION_VARCONFIGLINE_HPP
#define ARCHLINUXINSTALLER_CONFIGURATION_VARCONFIGLINE_HPP

#include "commentconfigline.hpp"

namespace archlinuxinstaller {
namespace configuration {

	class VarConfigLine : public CommentConfigLine
	{
	protected:
		std::string name;
		std::string value;

	public:
		VarConfigLine();
		VarConfigLine(const std::string& name);

		virtual void parseLine(const std::string& line);
		virtual void writeLine(std::ostream& out) const;

		inline std::string getName() const { return name; }

		inline std::string getValue() const { return value; }
		inline void setValue(const std::string& value) { this->value = value; }
	};

}}

#endif // ARCHLINUXINSTALLER_CONFIGURATION_VARCONFIGLINE_HPP
