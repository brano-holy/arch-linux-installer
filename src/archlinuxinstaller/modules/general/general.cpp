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

#include "archlinuxinstaller/modules/general/general.hpp"

#include <boost/filesystem.hpp>

#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace general {

General::General() : Module("general", 0)
{
}

bool General::decode(const YAML::Node& node)
{
	return YAML::convert<General>::decode(node, *this);
}

bool General::runOutsideBefore(const std::function<UIT>&)
{
	utils::SystemUtils::DEBUG = debug;
	return true;
}

bool General::runOutsideAfter(const std::map<std::string, UserInputBase*>&, const std::function<UIT>&)
{
	bool status = true;
	std::string dir = "/mnt/root/" + Module::PROGRAM_NAME;

	if(!keepConfig) status &= boost::filesystem::remove(dir + '/' + programFilename);
	if(!keepProgram) status &= boost::filesystem::remove(dir + '/' + configFilename);
	if(boost::filesystem::is_empty(dir)) status &= boost::filesystem::remove(dir);

	return status;
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::general::General>::decode(const Node& node, archlinuxinstaller::modules::general::General& general)
{
	general.debug = node["debug"].as<bool>(false);
	general.keepConfig = node["keepConfig"].as<bool>(false);
	general.keepProgram = node["keepProgram"].as<bool>(false);

	return true;
}

}
