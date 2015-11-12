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

#ifndef ARCHLINUXINSTALLER_MODULES_DEVICES_PARTITION_HPP
#define ARCHLINUXINSTALLER_MODULES_DEVICES_PARTITION_HPP

#include <experimental/optional>

#include <yaml-cpp/yaml.h>

#include "encryption.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

class Partition : public Volume
{
public:
	std::size_t id;

	std::string type;
	std::experimental::optional<Encryption> encryption;

	virtual bool hasRoot() const;
	bool hasSshDecrypt() const;
	std::string getPrintName() const;

	bool create(const Device& device, const std::string& lvmPassphrasePath = "") const;
	void fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::devices::Partition>
{
	static bool decode(Node node, archlinuxinstaller::modules::devices::Partition& partition);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_DEVICES_PARTITION_HPP
