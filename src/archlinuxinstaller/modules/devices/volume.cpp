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

#include "archlinuxinstaller/modules/devices/volume.hpp"

#include "archlinuxinstaller/modules/devices/volumegroup.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

bool Volume::hasRoot() const
{
	return (mountPoint && *mountPoint == "/");
}

bool Volume::create(const VolumeGroup& volumeGroup) const
{
	std::string volumeSize = utils::SystemUtils::getSizeByCommand(size, "lvcreate");
	std::string volumeSizeParam;
	if(volumeSize.find('%') == std::string::npos) volumeSizeParam = "-L " + volumeSize;
	else volumeSizeParam = "-l " + volumeSize;

	bool status = utils::SystemUtils::system("lvcreate " + volumeSizeParam + ' ' + volumeGroup.name + " -n " + name);
	if(filesystem) status &= utils::SystemUtils::createFilesystem(*filesystem, getPath());

	return status;
}

bool Volume::isMountable() const
{
	return (mountPoint || isSwap());
}

bool Volume::mount() const
{
	if(mountPoint)
	{
		std::string dir = "/mnt" + *mountPoint;
		bool status = utils::SystemUtils::system("mkdir -p " + dir);
		status &= utils::SystemUtils::system("mount " + getPath() + ' ' + dir);

		return status;
	}
	else if(isSwap())
	{
		return utils::SystemUtils::system("swapon " + getPath());
	}

	return false;
}

void Volume::fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const
{
	if(isMountable()) mountables.push_back(*this);
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::devices::Volume>::decode(Node node, archlinuxinstaller::modules::devices::Volume& volume)
{
	if(node["volume"]) node = node["volume"];

	if(node.IsMap())
	{
		volume.name = node["name"].as<std::string>();
		volume.size = node["size"].as<std::string>(); // TODO convert relative size to absolute

		if(node["filesystem"]) volume.filesystem = node["filesystem"].as<std::string>();
		if(node["mount"]) volume.mountPoint = node["mount"].as<std::string>();

		return true;
	}

	return false;
}

}
