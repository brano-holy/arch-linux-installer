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

#include "archlinuxinstaller/modules/devices/partition.hpp"

#include "archlinuxinstaller/modules/devices/device.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

bool Partition::hasRoot() const
{
	if(encryption)
	{
		return std::any_of(encryption->volumeGroups.begin(), encryption->volumeGroups.end(), [](const VolumeGroup& vg) { return vg.hasRoot(); });
	}

	return Volume::hasRoot();
}

bool Partition::hasSshDecrypt() const
{
	return (encryption && encryption->sshDecrypt);
}

std::string Partition::getPrintName() const
{
	std::string printName;

	if(!name.empty()) printName += '\'' + name + "' (";
	printName += getPath();
	if(!name.empty()) printName += ')';

	return printName;
}

bool Partition::create(const Device& device, const std::string& lvmPassphrasePath) const
{
	std::string partitionSize = utils::SystemUtils::getSizeByCommand(size, "sgdisk");
	std::string ids = std::to_string(id);
	bool status = utils::SystemUtils::system("sgdisk -n " + ids + ":0:" + partitionSize + " -t " + ids + ':' + type + ' ' + device.path);

	if(filesystem) status &= utils::SystemUtils::createFilesystem(*filesystem, getPath());
	if(encryption) status &= encryption->create(*this, lvmPassphrasePath);

	return status;
}

void Partition::fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const
{
	if(encryption) encryption->fillMountables(mountables);
	else Volume::fillMountables(mountables);
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::devices::Partition>::decode(Node node, archlinuxinstaller::modules::devices::Partition& partition)
{
	if(node["partition"]) node = node["partition"];

	if(node.IsMap())
	{
		YAML::convert<archlinuxinstaller::modules::devices::Volume>::decode(node, partition);
		partition.type = node["type"].as<std::string>();
		if(node["encryption"]) partition.encryption = node["encryption"].as<archlinuxinstaller::modules::devices::Encryption>();

		return true;
	}

	return false;
}

}
