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

#ifndef ARCHLINUXINSTALLER_MODULES_GENERAL_GENERAL_HPP
#define ARCHLINUXINSTALLER_MODULES_GENERAL_GENERAL_HPP

#include "archlinuxinstaller/modules/module.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace general {

class General : public Module
{
public:
	bool debug;
	bool keepConfig;
	bool keepProgram;

	std::string programFilename;
	std::string configFilename;

	General();

	virtual bool decode(const YAML::Node& node);

	virtual bool runOutsideBefore(const std::function<UIT>& ui);
	virtual bool runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::general::General>
{
	static bool decode(const Node& node, archlinuxinstaller::modules::general::General& general);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_GENERAL_GENERAL_HPP
