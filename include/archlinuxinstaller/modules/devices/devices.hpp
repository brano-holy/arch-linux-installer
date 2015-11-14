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

#ifndef ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICES_HPP
#define ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICES_HPP

#include "archlinuxinstaller/modules/module.hpp"

#include <vector>

#include "device.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

class Devices : public Module, public std::vector<Device>
{
protected:
	virtual void addUserInputs(std::vector<UserInputBase*>& userInputs);

public:
	Devices();

	virtual bool decode(const YAML::Node& node);

	virtual bool runOutsideBefore(const std::function<UIT>& ui);
	virtual bool runOutside(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);
	virtual bool runInside(const std::function<UIT>& ui);

	bool hasEncryption() const;
	const Encryption* getEncryption() const;

	bool hasRoot() const;

	std::string getEfiDirectory() const;
	void getGrubParams(std::string& grubDevice, std::string& grubDmname) const;

	bool createPartitions(const std::string& lvmPassphrasePath = "") const;
	bool mountPartitions() const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::devices::Devices>
{
	static bool decode(const Node& node, archlinuxinstaller::modules::devices::Devices& devices);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_DEVICES_DEVICES_HPP
