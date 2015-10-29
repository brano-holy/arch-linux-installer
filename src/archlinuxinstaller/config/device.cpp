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

#include "archlinuxinstaller/config/device.hpp"

#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace config {

bool Device::hasEncryption() const
{
	return std::any_of(partitions.begin(), partitions.end(), [](const Partition& p) { return p.encryption; });
}

const Encryption* Device::getEncryption() const
{
	for(const Partition& p : partitions)
	{
		if(p.encryption) return &p.encryption.value();
	}

	return nullptr;
}

bool Device::hasRoot() const
{
	return std::any_of(partitions.begin(), partitions.end(), [](const Partition& p) { return p.hasRoot(); });
}

std::string Device::getEfiDirectory() const
{
	for(const Partition& p : partitions)
	{
		if(p.mountPoint && p.type == "ef00") return *p.mountPoint;
	}

	return "";
}

std::string Device::getPrintName() const
{
	std::string printName;
	if(name) printName += '\'' + *name + "' (";
	printName += path;
	if(name) printName += ')';

	return printName;
}

bool Device::eraseDevice() const
{
	std::string eraseType = erase.value_or("");

	if(eraseType == "zero" || eraseType == "random")
	{
		return utils::SystemUtils::system("dd if=/dev/" + eraseType + " of=" + path + " bs=1M");
	}
	else
	{
		return utils::SystemUtils::system("sgdisk -Z " + path);
	}
}

bool Device::createPartitions(const std::string& lvmPassphrasePath) const
{
	return std::all_of(partitions.begin(), partitions.end(), [this, &lvmPassphrasePath](const Partition& p) { return p.create(*this, lvmPassphrasePath); });
}

void Device::fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const
{
	for(const Partition& p : partitions) p.fillMountables(mountables);
}

}}

namespace YAML {

bool convert<archlinuxinstaller::config::Device>::decode(Node node, archlinuxinstaller::config::Device& device)
{
	if(node["device"]) node = node["device"];

	if(node.IsMap())
	{
		if(node["name"]) device.name = node["name"].as<std::string>();
		device.path = node["path"].as<std::string>();
		if(node["erase"]) device.erase = node["erase"].as<std::string>();
		device.partitions = node["partitions"].as<std::vector<archlinuxinstaller::config::Partition>>();

		size_t i = 1;
		for(archlinuxinstaller::config::Partition& p : device.partitions)
		{
			p.id = i;
			p.path = device.path + std::to_string(i);

			i++;
		}

		return true;
	}

	return false;
}

}
