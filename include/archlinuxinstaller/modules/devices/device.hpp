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

#ifndef ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICE_HPP
#define ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICE_HPP

#include "partition.hpp"

#include <experimental/optional>

namespace archlinuxinstaller {
namespace modules {
namespace devices {

class Device
{
public:
	std::experimental::optional<std::string> name;
	std::string path;
	std::experimental::optional<std::string> erase;
	std::vector<Partition> partitions;

	bool hasEncryption() const;
	const Encryption* getEncryption() const;

	bool hasRoot() const;

	std::string getEfiDirectory() const;

	std::string getPrintName() const;

	bool eraseDevice() const;
	bool createPartitions(const std::string& lvmPassphrasePath = "") const;
	void fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::devices::Device>
{
	static bool decode(Node node, archlinuxinstaller::modules::devices::Device& device);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICE_HPP
