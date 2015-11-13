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

#include "archlinuxinstaller/modules/pacman/pacman.hpp"

#include "archlinuxinstaller/packageinstaller.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace pacman {

const double Pacman::ORDER = 0.22;

bool Pacman::runInside(const std::function<UIT>& ui)
{
	bool status = true;

	PackageInstaller installer;
	if(!packages.empty()) status &= ui("Installing packages", installer.installPackages(packages));
	if(!aurPackages.empty()) status &= ui("Installing AUR packages", installer.installAurPackages(aurPackages));

	return status;
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::pacman::Pacman>::decode(Node node, archlinuxinstaller::modules::pacman::Pacman& pacman)
{
	if(node["packages"]) pacman.packages = node["packages"].as<std::vector<std::string>>();
	if(node["aurPackages"]) pacman.aurPackages = node["aurPackages"].as<std::vector<std::string>>();

	return true;
}

}
