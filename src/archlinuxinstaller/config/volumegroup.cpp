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

#include "archlinuxinstaller/config/volumegroup.hpp"

#include "archlinuxinstaller/config/encryption.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace config {

bool VolumeGroup::hasRoot() const
{
	return std::any_of(volumes.begin(), volumes.end(), [](const Volume& v) { return v.hasRoot(); });
}

bool VolumeGroup::create(const Encryption& encryption) const
{
	bool status = utils::SystemUtils::system("vgcreate " + name + " /dev/mapper/" + encryption.name);
	status &= std::all_of(volumes.begin(), volumes.end(), [this](const Volume& v) { return v.create(*this); });

	return status;
}

void VolumeGroup::fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const
{
	for(const Volume& v : volumes) v.fillMountables(mountables);
}

}}

namespace YAML {

bool convert<archlinuxinstaller::config::VolumeGroup>::decode(Node node, archlinuxinstaller::config::VolumeGroup& volumeGroup)
{
	if(node["volumeGroup"]) node = node["volumeGroup"];

	if(node.IsMap())
	{
		volumeGroup.name = node["name"].as<std::string>();
		volumeGroup.volumes = node["volumes"].as<std::vector<archlinuxinstaller::config::Volume>>();

		for(archlinuxinstaller::config::Volume& v : volumeGroup.volumes)
		{
			v.path = "/dev/" + volumeGroup.name + '/' + v.name;
		}

		return true;
	}

	return false;
}

}
