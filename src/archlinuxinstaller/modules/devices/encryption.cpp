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

#include "archlinuxinstaller/modules/devices/encryption.hpp"

#include "archlinuxinstaller/modules/devices/partition.hpp"
#include "archlinuxinstaller/configuration/config.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace devices {

bool Encryption::create(const Partition& partition, const std::string& lvmPassphrasePath) const
{
	std::string cryptParams;
	if(hash) cryptParams += " -h " + *hash;
	if(keySize) cryptParams += " -s " + std::to_string(*keySize);
	if(iterTime) cryptParams += " -i " + std::to_string(*iterTime);

	bool cryptStatus[5];
	cryptStatus[0] = utils::SystemUtils::system("cryptsetup -q" + cryptParams + " luksFormat " + partition.getPath() + " - < " + lvmPassphrasePath);
	cryptStatus[1] = utils::SystemUtils::system("cryptsetup open --type luks --key-file " + lvmPassphrasePath + ' ' + partition.getPath() + " " + name);
	cryptStatus[2] = utils::SystemUtils::system("rm " + lvmPassphrasePath);

	cryptStatus[3] = utils::SystemUtils::system("pvcreate /dev/mapper/" + name);
	cryptStatus[4] = std::all_of(volumeGroups.begin(), volumeGroups.end(), [this](const VolumeGroup& vg) { return vg.create(*this); });

	return std::all_of(cryptStatus, cryptStatus + 5, [](bool s) { return s; });
}

void Encryption::fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const
{
	for(const VolumeGroup& vg : volumeGroups) vg.fillMountables(mountables);
}

bool Encryption::install(PackageInstaller& packageInstaller, const std::string& grubDevice, const std::string& grubDmname) const
{
	bool statusMkinitcpio = true;
	if(!sshDecrypt && packageInstaller.isInstalled("mkinitcpio"))
	{
		configuration::Config mkinitcpio("/etc/mkinitcpio.conf");
		mkinitcpio.setValue("HOOKS", "base udev autodetect modconf block keyboard keymap consolefont encrypt lvm2 filesystems fsck shutdown");
		mkinitcpio.save();

		statusMkinitcpio = utils::SystemUtils::system("mkinitcpio -p linux");
	}

	bool statusGrub = true;
	if(!sshDecrypt && packageInstaller.isInstalled("grub"))
	{
		configuration::Config grubConfig("/etc/default/grub");
		grubConfig.setValue("GRUB_CMDLINE_LINUX", "cryptdevice=" + grubDevice + ':' + grubDmname);
		grubConfig.save();

		statusGrub = utils::SystemUtils::system("grub-mkconfig -o /boot/grub/grub.cfg");
	}

	bool statusSshDecrypt = true;
	if(sshDecrypt) statusSshDecrypt = sshDecrypt->install(packageInstaller, grubDevice, grubDmname);

	return (statusMkinitcpio && statusGrub && statusSshDecrypt);
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::devices::Encryption>::decode(const Node& node, archlinuxinstaller::modules::devices::Encryption& encryption)
{
	if(node.IsMap())
	{
		encryption.name = node["name"].as<std::string>();

		if(node["hash"]) encryption.hash = node["hash"].as<std::string>();
		if(node["keySize"]) encryption.keySize = node["keySize"].as<unsigned int>();
		if(node["iterTime"]) encryption.iterTime = node["iterTime"].as<unsigned int>();

		encryption.volumeGroups = node["volumeGroups"].as<std::vector<archlinuxinstaller::modules::devices::VolumeGroup>>();

		return true;
	}

	return false;
}

}
