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

#ifndef ARCHLINUXINSTALLER_MODULES_DEVICES_ENCRYPTION_HPP
#define ARCHLINUXINSTALLER_MODULES_DEVICES_ENCRYPTION_HPP

#include <experimental/optional>

#include <yaml-cpp/yaml.h>

#include "sshdecrypt.hpp"
#include "volumegroup.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

class Partition;

class Encryption
{
public:
	std::string name;

	std::experimental::optional<std::string> hash;
	std::experimental::optional<unsigned int> keySize;
	std::experimental::optional<unsigned int> iterTime;

	std::vector<VolumeGroup> volumeGroups;
	std::experimental::optional<SshDecrypt> sshDecrypt;

	bool create(const Partition& partition, const std::string& lvmPassphrasePath) const;
	void fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const;

	bool install(PackageInstaller& packageInstaller, const std::string& grubDevice = "", const std::string& grubDmname = "") const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::devices::Encryption>
{
	static bool decode(const Node& node, archlinuxinstaller::modules::devices::Encryption& encryption);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_DEVICES_ENCRYPTION_HPP
