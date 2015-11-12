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

#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace general {

const double General::ORDER = 0;

bool General::runOutsideBefore(const std::function<UIT>&)
{
	utils::SystemUtils::DEBUG = debug;
	return true;
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::general::General>::decode(Node node, archlinuxinstaller::modules::general::General& general)
{
	general.debug = node["debug"].as<bool>(false);
	general.keepConfig = node["keepConfig"].as<bool>(false);
	general.keepProgram = node["keepProgram"].as<bool>(false);

	return true;
}

}
