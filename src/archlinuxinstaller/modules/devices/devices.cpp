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

#include "archlinuxinstaller/modules/devices/devices.hpp"

#include <functional>
#include <fstream>

#include "archlinuxinstaller/modules/userinput.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

Devices::Devices() : Module("devices", 0.1, true)
{
}

bool Devices::decode(const YAML::Node& node)
{
	return YAML::convert<Devices>::decode(node, *this);
}

void Devices::addUserInputs(std::vector<UserInputBase*>& userInputs)
{
	if(hasEncryption())
	{
		userInputs.push_back(new UserInput<std::string>("lvm-passphrase", "LVM Passphrase", UserInputType::Password));
	}
}

bool Devices::runOutsideBefore(const std::function<UIT>& ui)
{
	return ui("Looking for the root partition", hasRoot());
}

bool Devices::runOutside(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui)
{
	std::string lvmPassphrasePath;
	if(hasEncryption())
	{
		lvmPassphrasePath = "devices-lvm-passphrase-" + Module::PROGRAM_NAME + ".txt";
		std::ofstream lvmPassphraseFile(lvmPassphrasePath);
		lvmPassphraseFile << *userInputs.at("lvm-passphrase");
	}

	bool status = ui("Creating partitions", createPartitions(lvmPassphrasePath));
	status &= ui("Mounting partitions", mountPartitions());

	return status;
}

bool Devices::runInside(const std::function<UIT>& ui)
{
	if(hasEncryption())
	{
		const Encryption* encryption = getEncryption();
		std::string grubDevice, grubDmname;
		getGrubParams(grubDevice, grubDmname);

		return ui("Installing encryption", encryption->install(grubDevice, grubDmname));
	}

	return true;
}

bool Devices::hasEncryption() const
{
	return std::any_of(begin(), end(), [](const Device& d) { return d.hasEncryption(); });
}

const Encryption* Devices::getEncryption() const
{
	for(const Device& d : *this)
	{
		if(d.hasEncryption()) return d.getEncryption();
	}

	return nullptr;
}

bool Devices::hasRoot() const
{
	return std::any_of(begin(), end(), [](const Device& d) { return d.hasRoot(); });
}

std::string Devices::getEfiDirectory() const
{
	for(const Device& d : *this)
	{
		std::string efiDirectory = d.getEfiDirectory();
		if(efiDirectory.length() > 0) return efiDirectory;
	}

	return "";
}

void Devices::getGrubParams(std::string& grubDevice, std::string& grubDmname) const
{
	for(const Device& d : *this)
	{
		for(const Partition& p : d.partitions)
		{
			if(p.encryption)
			{
				for(const VolumeGroup& vg : p.encryption->volumeGroups)
				{
					if(vg.hasRoot())
					{
						grubDevice = p.getPath();
						grubDmname = vg.name;

						return;
					}
				}
			}
		}
	}
}

bool Devices::createPartitions(const std::string& lvmPassphrasePath) const
{
	return std::all_of(this->begin(), this->end(), [&lvmPassphrasePath](const Device& d)
	{
		return (d.eraseDevice() && d.createPartitions(lvmPassphrasePath));
	});
}

bool Devices::mountPartitions() const
{
	std::vector<std::reference_wrapper<const Volume>> mountables;
	for(const Device& d : *this) d.fillMountables(mountables);

	std::sort(mountables.begin(), mountables.end(), [](const Volume& volumeA, const Volume& volumeB)
	{
		if(volumeA.isSwap()) return false;
		if(volumeB.isSwap()) return true;

		std::string mountPointA = *volumeA.mountPoint;
		std::string mountPointB = *volumeB.mountPoint;

		if(mountPointA.back() != '/') mountPointA += '/';
		if(mountPointB.back() != '/') mountPointB += '/';

		return (std::count(mountPointA.begin(), mountPointA.end(), '/') < std::count(mountPointB.begin(), mountPointB.end(), '/'));
	});

	return std::all_of(mountables.begin(), mountables.end(), [](const Volume& v) { return v.mount(); });
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::devices::Devices>::decode(const Node& node, archlinuxinstaller::modules::devices::Devices& devices)
{
	if(node.IsSequence())
	{
		YAML::convert<std::vector<archlinuxinstaller::modules::devices::Device>>::decode(node, devices);
		return true;
	}

	return true;
}

}
