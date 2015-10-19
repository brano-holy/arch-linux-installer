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

#include "archlinuxinstaller/archlinuxinstaller.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

#include "archlinuxinstaller/configuration/config.hpp"

#include "archlinuxinstaller/utils/stringutils.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {

const std::string ArchLinuxInstaller::AUR_URL = "https://aur.archlinux.org/cgit/aur.git/snapshot/%s.tar.gz";

ArchLinuxInstaller::ArchLinuxInstaller(const std::string& configPath, const std::string& programName) :
	configPath(configPath), programName(programName), argChroot(false), argLog(false)
{
	loadConfig(configPath);
	loadEfi();
}

void ArchLinuxInstaller::loadArgs(int argc, char **argv)
{
	argProgramPath = std::string(argv[0]);
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "--chroot") == 0) argChroot = true;
		else if(strcmp(argv[i], "--log") == 0) argLog = true;
	}
}

void ArchLinuxInstaller::loadEfi()
{
	efi = (utils::SystemUtils::csystem("efivar -l > /dev/null 2>&1") == 0);
}

void ArchLinuxInstaller::loadConfig(const std::string& configPath)
{
	YAML::Node config = YAML::LoadFile(configPath);

	debug = config["debug"].as<bool>(false);
	log = config["log"].as<bool>(false);
	keepProgram = config["keepProgram"].as<bool>(false);
	keepConfig = config["keepConfig"].as<bool>(false);

	keyboard = config["settings"]["keyboard"].as<std::string>("");
	font = config["settings"]["font"].as<std::string>("");
	if(config["settings"]["locales"]) locales = config["settings"]["locales"].as<std::vector<std::string>>();
	if(config["settings"]["lang"]) lang = config["settings"]["lang"].as<std::string>();
	if(config["settings"]["timezone"]) timezone = config["settings"]["timezone"].as<std::string>();
	if(config["settings"]["hostname"]) hostname = config["settings"]["hostname"].as<std::string>();

	encryption = false;
	rootPartition = false;

	devices = config["devices"];
	for(const YAML::Node& deviceItem : devices)
	{
		const YAML::Node& device = deviceItem["device"];
		std::string devicePath = device["path"].as<std::string>();

		std::size_t i = 1;
		for(const YAML::Node& partitionItem : device["partitions"])
		{
			const YAML::Node& partition = partitionItem["partition"];
			std::string partitionPath = devicePath + std::to_string(i++);

			if(partition["mount"])
			{
				std::string mountPoint = partition["mount"].as<std::string>();

				if(mountPoint == "/") rootPartition = true;
				if(partition["type"].as<std::string>() == "ef00") efiDirectory = mountPoint;

				partitionsToMount.emplace_back(partitionPath, mountPoint);
			}

			if(partition["filesystem"].as<std::string>("") == "swap") partitionsToMount.emplace_back(partitionPath, "", true);

			if(partition["encryption"])
			{
				encryption = true;

				for(const YAML::Node& volumeGroupItem : partition["encryption"]["volumeGroups"])
				{
					const YAML::Node& volumeGroup = volumeGroupItem["volumeGroup"];
					std::string groupName = volumeGroup["name"].as<std::string>();

					for(const YAML::Node& volumeItem : volumeGroup["volumes"])
					{
						const YAML::Node& volume = volumeItem["volume"];
						std::string volumePath = "/dev/" + groupName + '/' + volume["name"].as<std::string>();

						if(volume["mount"])
						{
							std::string mountPoint = volume["mount"].as<std::string>();
							if(mountPoint == "/")
							{
								rootPartition = true;
								grubDevice = partitionPath;
								grubDmname = groupName;
							}

							partitionsToMount.emplace_back(volumePath, mountPoint);
						}

						if(volume["filesystem"].as<std::string>("") == "swap") partitionsToMount.emplace_back(volumePath, "", true);
					}
				}
			}
		}
	}

	std::sort(partitionsToMount.begin(), partitionsToMount.end(), [](const MountData& firstData, const MountData& secondData)
	{
		if(firstData.swap) return false;
		if(secondData.swap) return true;

		std::string first = firstData.dir;
		std::string second = secondData.dir;

		if(first.back() != '/') first += '/';
		if(second.back() != '/') second += '/';

		return (std::count(first.begin(), first.end(), '/') < std::count(second.begin(), second.end(), '/'));
	});

	if(config["users"]) users = config["users"];
	if(config["packages"]) packages = config["packages"].as<std::vector<std::string>>();
	if(config["aurPackages"]) aurPackages = config["aurPackages"].as<std::vector<std::string>>();

	utils::SystemUtils::DEBUG = debug;
}

int ArchLinuxInstaller::installWithLog(int argc, char **argv)
{
	std::string cmd = utils::StringUtils::join(argv, argv + argc, ' ');
	return utils::SystemUtils::csystem(cmd + " --log | tee log-" + programName + ".txt");
}

int ArchLinuxInstaller::installChroot()
{
	setKeyboard(true);
	setFont(true);
	setLocales();
	setLang();
	setTimezone();
	setClock();

	std::cout << "> Enabling wired network..." << std::endl;
	utils::SystemUtils::csystem("systemctl enable dhcpcd.service");

	setHostname();

	updateMkinitcpio();
	installGrub();
	installPackages();
	installAurPackages();
	createUsers();

	return 0;
}

int ArchLinuxInstaller::install(int argc, char **argv)
{
	loadArgs(argc, argv);

	if(!argLog && log) return installWithLog(argc, argv);
	if(argChroot) return installChroot();

	std::cout << "> Looking for the root partition... ";
	if(rootPartition) std::cout << "OK" << std::endl;
	else
	{
		std::cout << "Failed" << std::endl;
		return 1;
	}

	setKeyboard();
	setFont();
	readPasswords();
	createPartitions();
	mountPartitions();
	installBase();
	runChroot();
	setUsersPasswords();
	finish();

	std::cout << "> Installation has finished. Press [Enter] to reboot..." << std::endl;
	std::string tmpReboot;
	std::getline(std::cin, tmpReboot);

	utils::SystemUtils::csystem("reboot");

	return 0;
}

void ArchLinuxInstaller::setKeyboard(bool permanent) const
{
	if(keyboard != "")
	{
		std::cout << "> Setting keyboard to '" << keyboard << "'..." << std::endl;
		utils::SystemUtils::csystem("loadkeys " + keyboard);

		if(permanent) utils::SystemUtils::csystem("echo \"KEYMAP=" + keyboard + "\" >> /etc/vconsole.conf");
	}
}

void ArchLinuxInstaller::setFont(bool permanent) const
{
	if(font != "")
	{
		std::cout << "> Setting font to '" << font << "'..." << std::endl;
		utils::SystemUtils::csystem("setfont " + font + " -m 8859-2");

		if(permanent) utils::SystemUtils::csystem("echo \"FONT=" + font + "\" >> /etc/vconsole.conf");
	}
}

void ArchLinuxInstaller::setLocales() const
{
	std::cout << "> Setting locales..." << std::endl;
	for(const std::string& locale : locales)
	{
		utils::SystemUtils::csystem("echo \"" + locale + "\" >> /etc/locale.gen");
	}
	utils::SystemUtils::csystem("locale-gen");
}

void ArchLinuxInstaller::setLang() const
{
	if(!lang.empty())
	{
		std::cout << "> Setting lang '" << lang << "'..." << std::endl;
		utils::SystemUtils::csystem("echo LANG=" + lang + " > /etc/locale.conf");
		utils::SystemUtils::csystem("export LANG=" + lang);
	}
}

void ArchLinuxInstaller::setTimezone() const
{
	if(!timezone.empty())
	{
		std::cout << "> Setting timezone to '" << timezone << "'..." << std::endl;
		utils::SystemUtils::csystem("ln -s /usr/share/zoneinfo/" + timezone + " /etc/localtime");
	}
}

void ArchLinuxInstaller::setClock() const
{
	std::cout << "> Setting HW clock to UTC..." << std::endl;
	utils::SystemUtils::csystem("hwclock --systohc --utc");

	std::cout << "> Starting systemd-timesyncd service..." << std::endl;
	utils::SystemUtils::csystem("timedatectl set-ntp true");
}

void ArchLinuxInstaller::setHostname() const
{
	if(!hostname.empty())
	{
		std::cout << "> Setting hostname to '" << hostname << "'..." << std::endl;
		utils::SystemUtils::csystem("echo " + hostname + " > /etc/hostname");
	}
}

void ArchLinuxInstaller::readPasswords()
{
	std::cout << "> Entering passwords..." << std::endl;
	std::cout << "> Keyboard layout: " << (keyboard == "" ? "deafult (probably 'us')" : keyboard) << std::endl;

	if(encryption)
	{
		std::string lvmPassphrase = utils::SystemUtils::readPassword("LVM passphrase");
		std::cout << std::endl;

		lvmPassphrasePath = "lvm-passphrase-" + programName + ".txt";
		std::ofstream lvmPassphraseFile(lvmPassphrasePath);
		lvmPassphraseFile << lvmPassphrase;
	}

	usersPasswordsPath = "users-passwords-" + programName + ".txt";
	std::ofstream usersPasswordsFile(usersPasswordsPath);

	usersPasswordsFile << "root:" << utils::SystemUtils::readPassword("UNIX password for user 'root'") << std::endl;
	std::cout << std::endl;

	for(const YAML::Node& userItem : users)
	{
		const YAML::Node& user = userItem["user"];
		std::string username = user["name"].as<std::string>();
		std::string password = utils::SystemUtils::readPassword("UNIX password for user '" + username + '\'');

		usersPasswordsFile << username << ':' << password << std::endl;
		std::cout << std::endl;
	}
}

void ArchLinuxInstaller::createPartitions() const
{
	for(const YAML::Node& deviceItem : devices)
	{
		const YAML::Node& device = deviceItem["device"];
		std::string devicePath = device["path"].as<std::string>();

		if(device["erase"])
		{
			std::string erase = device["erase"].as<std::string>();

			if(erase == "zero" || erase == "random")
			{
				std::cout << "> Erasing device " << getDeviceName(device) << " with mode '" << erase <<  "'..." << std::endl;
				utils::SystemUtils::csystem("dd if=/dev/" + erase + " of=" + devicePath + " bs=1M");
			}
			else
			{
				if(erase != "none") std::cout << "> Unknown erase mode '" << erase << "' for device " << getDeviceName(device) << "! Using default mode 'none'." << std::endl;

				std::cout << "> Destroying the old partition table of device " << getDeviceName(device) << "..." << std::endl;
				utils::SystemUtils::csystem("sgdisk -Z " + devicePath);
			}
		}

		std::size_t i = 1;
		for(const YAML::Node& partitionItem : device["partitions"])
		{
			const YAML::Node& partition = partitionItem["partition"];

			std::string partitionId = std::to_string(i++);
			std::string partitionPath = devicePath + partitionId;
			std::string partitionSize = utils::SystemUtils::getSizeByCommand(partition["size"].as<std::string>(), "sgdisk");
			std::string partitionType = partition["type"].as<std::string>();
			std::string partitionFilesystem = partition["filesystem"].as<std::string>("");

			std::cout << "> Creating a partition for " << getDeviceName(partition, partitionPath);
			if(partitionFilesystem != "") std::cout << " with filesystem '" << partitionFilesystem << '\'';
			std::cout << "..." << std::endl;

			utils::SystemUtils::csystem("sgdisk -n " + partitionId + ":0:" + partitionSize + " -t " + partitionId + ':' + partitionType + ' ' + devicePath);
			if(partitionFilesystem != "") utils::SystemUtils::createFilesystem(partitionFilesystem, partitionPath);

			if(partition["encryption"])
			{
				std::string physicalVolume = partition["encryption"]["name"].as<std::string>();

				std::cout << "> Setting up encryption for " << getDeviceName(partition, partitionPath) << "..." << std::endl;

				std::string cryptParams;
				if(partition["encryption"]["hash"]) cryptParams += " -h " + partition["encryption"]["hash"].as<std::string>();
				if(partition["encryption"]["keySize"]) cryptParams += " -s " + std::to_string(partition["encryption"]["keySize"].as<int>());
				if(partition["encryption"]["iterTime"]) cryptParams += " -i " + std::to_string(partition["encryption"]["iterTime"].as<int>());

				utils::SystemUtils::csystem("cryptsetup -q" + cryptParams + " luksFormat " + partitionPath + " - < " + lvmPassphrasePath);
				utils::SystemUtils::csystem("cryptsetup open --type luks --key-file " + lvmPassphrasePath + ' ' + partitionPath + " " + physicalVolume);
				utils::SystemUtils::csystem("rm " + lvmPassphrasePath);

				std::cout << "> Creating a physical volume (" << physicalVolume << ")..." << std::endl;
				utils::SystemUtils::csystem("pvcreate /dev/mapper/" + physicalVolume);

				for(const YAML::Node& volumeGroupItem : partition["encryption"]["volumeGroups"])
				{
					const YAML::Node& volumeGroup = volumeGroupItem["volumeGroup"];

					std::string groupName = volumeGroup["name"].as<std::string>();
					std::cout << "> Creating a volume group (" << groupName << ")..." << std::endl;
					utils::SystemUtils::csystem("vgcreate " + groupName + " /dev/mapper/" + physicalVolume);

					for(const YAML::Node& volumeItem : volumeGroup["volumes"])
					{
						const YAML::Node& volume = volumeItem["volume"];

						std::string volumeName = volume["name"].as<std::string>();
						std::string volumePath = "/dev/" + groupName + '/' + volumeName;

						std::string volumeSize = utils::SystemUtils::getSizeByCommand(volume["size"].as<std::string>(), "lvcreate");
						std::string volumeSizeParam;
						if(volumeSize.find('%') == std::string::npos) volumeSizeParam = "-L " + volumeSize;
						else volumeSizeParam = "-l " + volumeSize;

						std::string volumeFilesystem = volume["filesystem"].as<std::string>("");

						std::cout << "> Creating a logical volume (" << volumePath << ')';
						if(volumeFilesystem != "") std::cout << " with filesystem '" << volumeFilesystem << '\'';
						std::cout << "..." << std::endl;

						utils::SystemUtils::csystem("lvcreate " + volumeSizeParam + ' ' + groupName + " -n " + volumeName);
						if(volumeFilesystem != "") utils::SystemUtils::createFilesystem(volumeFilesystem, volumePath);
					}
				}
			}
		}
	}
}

void ArchLinuxInstaller::mountPartitions() const
{
	for(const MountData& partition : partitionsToMount)
	{
		if(partition.swap)
		{
			std::cout << "> Turning swap on " << partition.device << std::endl;
			utils::SystemUtils::csystem("swapon " + partition.device);
		}
		else
		{
			std::cout << "> Mounting " << partition.device << " -> " << partition.dir << std::endl;

			std::string dir = "/mnt" + partition.dir;
			utils::SystemUtils::csystem("mkdir -p " + dir);
			utils::SystemUtils::csystem("mount " + partition.device + ' ' + dir);
		}
	}
}

void ArchLinuxInstaller::installBase() const
{
	std::cout << "> Installing Arch Linux base..." << std::endl;

	std::string basePackages = "base";
	if(efi) basePackages += " efivar";

	utils::SystemUtils::csystem("pacstrap /mnt " + basePackages);

	std::cout << "> Generating fstab..." << std::endl;
	utils::SystemUtils::csystem("genfstab -U -p /mnt >> /mnt/etc/fstab");
}

void ArchLinuxInstaller::runChroot() const
{
	std::cout << "> Changing root into the new system..." << std::endl;
	utils::SystemUtils::csystem("mkdir /mnt/root/" + programName);
	utils::SystemUtils::csystem("cp " + argProgramPath + " /mnt/root/" + programName + '/' + programName);
	utils::SystemUtils::csystem("cp " + configPath + " /mnt/root/" + programName + "/config.yaml");

	std::string chrootParams = "--chroot";
	if(log) chrootParams += " --log";
	utils::SystemUtils::csystem("arch-chroot /mnt /root/" + programName + '/' + programName + " /root/" + programName + "/config.yaml " + chrootParams);
}

void ArchLinuxInstaller::setUsersPasswords() const
{
	std::cout << "> Setting users' passwords..." << std::endl;
	utils::SystemUtils::csystem("arch-chroot /mnt chpasswd < " + usersPasswordsPath);
	utils::SystemUtils::csystem("rm " + usersPasswordsPath);
}

void ArchLinuxInstaller::finish() const
{
	std::cout << "> Finishing installation..." << std::endl;
	if(!keepProgram) utils::SystemUtils::csystem("rm /mnt/root/" + programName + '/' + programName);
	if(!keepConfig) utils::SystemUtils::csystem("rm /mnt/root/" + programName + "/config.yaml");
	if(log) utils::SystemUtils::csystem("cp log-" + programName + ".txt /mnt/root/" + programName + "/log.txt");
	utils::SystemUtils::csystem("rmdir --ignore-fail-on-non-empty /mnt/root/" + programName);

	utils::SystemUtils::csystem("umount -R /mnt");
}

void ArchLinuxInstaller::updateMkinitcpio() const
{
	if(encryption)
	{
		std::cout << "> Creating an initial ramdisk environment..." << std::endl;

		configuration::Config mkinitcpio("/etc/mkinitcpio.conf");
		// HOOKS="base udev autodetect modconf block filesystems keyboard fsck"
		mkinitcpio.setValue("HOOKS", "base udev autodetect modconf block keyboard keymap consolefont encrypt lvm2 filesystems fsck shutdown");
		mkinitcpio.save();

		utils::SystemUtils::csystem("mkinitcpio -p linux");
	}
}

void ArchLinuxInstaller::installGrub() const
{
	std::cout << "> Installing and configuring GRUB..." << std::endl;
	if(efi)
	{
		std::string grubTarget = (utils::StringUtils::trim(utils::SystemUtils::ssystem("uname -m")) == "x86_64" ? "x86_64-efi" : "i386-efi");

		utils::SystemUtils::csystem("pacman -S --noconfirm grub dosfstools efibootmgr");
		utils::SystemUtils::csystem("grub-install --target=" + grubTarget + " --efi-directory=" + efiDirectory + " --bootloader-id=grub --recheck");
	}
	else
	{
		utils::SystemUtils::csystem("pacman -S --noconfirm grub");

		std::string device = devices[0]["device"]["path"].as<std::string>();
		utils::SystemUtils::csystem("grub-install --recheck " + device);
	}

	if(encryption)
	{
		configuration::Config grubConfig("/etc/default/grub");
		grubConfig.setValue("GRUB_CMDLINE_LINUX", "cryptdevice=" + grubDevice + ':' + grubDmname);
		// grubConfig.setValue("GRUB_DISABLE_SUBMENU", "y");
		grubConfig.save();
	}

	utils::SystemUtils::csystem("grub-mkconfig -o /boot/grub/grub.cfg");
}

void ArchLinuxInstaller::installPackages() const
{
	if(!packages.empty())
	{
		std::cout << std::endl << "> Installing additional packages..." << std::endl;
		utils::SystemUtils::csystem("pacman -S --noconfirm " + utils::StringUtils::join(packages.begin(), packages.end(), ' '));

		for(const std::string& package : packages)
		{
			if(package == "sudo")
			{
				utils::SystemUtils::csystem("echo \"%wheel ALL=(ALL) ALL\" > /etc/sudoers.d/99-wheel");
			}
		}
	}
}

void ArchLinuxInstaller::installAurPackages() const
{
	if(!aurPackages.empty())
	{
		std::cout << std::endl << "> Installing additional AUR packages..." << std::endl;
		utils::SystemUtils::csystem("pacman -S --noconfirm base-devel");

		std::string tempUser = "temp-archlinux-installer";
		utils::SystemUtils::csystem("useradd -m " + tempUser);

		for(const std::string& package : aurPackages)
		{
			installAurPackage(package, tempUser);
		}

		utils::SystemUtils::csystem("userdel -r " + tempUser);
	}
}

void ArchLinuxInstaller::createUsers() const
{
	std::cout << "> Creating users..." << std::endl;
	for(const YAML::Node& userItem : users)
	{
		const YAML::Node& user = userItem["user"];
		std::string username = user["name"].as<std::string>();

		std::string params;
		if(user["createHome"].as<bool>(true)) params += " -m";
		if(user["comment"]) params += " -c \"" + user["comment"].as<std::string>() + '"';
		if(user["groups"])
		{
			std::vector<std::string> groups = user["groups"].as<std::vector<std::string>>();
			params += " -G " + utils::StringUtils::join(groups.begin(), groups.end(), ',');
		}

		utils::SystemUtils::csystem("useradd" + params + ' ' + username);
	}
}

std::string ArchLinuxInstaller::getDeviceName(const YAML::Node& node, const std::string& path)
{
	std::string name = "";
	if(node["name"]) name += '\'' + node["name"].as<std::string>() + "' (";

	if(path == "") name += node["path"].as<std::string>();
	else name += path;

	if(node["name"]) name += ')';

	return name;
}

int ArchLinuxInstaller::installAurPackage(const std::string& packageName, const std::string& user, bool asdeps)
{
	std::string packagePath = "/home/" + user + '/' + packageName;

	std::cout << "> Downloading AUR package '" << packageName << "'..." << std::endl;

	std::string downloadCommand = "cd;";
	downloadCommand += "wget " + utils::StringUtils::sprintf(AUR_URL, packageName.c_str()) + ';';
	downloadCommand += "tar -xf " + packageName + ".tar.gz";
	utils::SystemUtils::csystem("su " + user + " -c \"" + downloadCommand + '"');

	std::cout << "> Searching for missing dependencies for '" << packageName << "'..." << std::endl;

	std::string pkgbuildDepends = utils::SystemUtils::ssystem("grep '^depends' " + packagePath + "/PKGBUILD | sed -r 's/^depends=\\((.*)\\)$/\\1/g'");

	std::vector<std::string> pacmanDepends, aurDepends;
	std::vector<std::string> missingDepends = utils::StringUtils::split(utils::SystemUtils::ssystem("pacman -T " + pkgbuildDepends), '\n');
	for(const std::string& pkg : missingDepends)
	{
		std::string pkgName = pkg.substr(0, pkg.find_first_of("><="));
		if(utils::SystemUtils::csystem("pacman -Ss '^" + pkgName + "$'") == 0)
		{
			pacmanDepends.push_back('\'' + pkg + '\'');
		}
		else
		{
			aurDepends.push_back(pkgName);
		}
	}

	std::cout << "> Installing the missing dependencies for '" << packageName << "'..." << std::endl;
	utils::SystemUtils::csystem("pacman -Sy --noconfirm --needed --asdeps " +
								utils::StringUtils::join(pacmanDepends.begin(), pacmanDepends.end(), ' '));

	std::cout << "> Installing the missing AUR dependencies for '" << packageName << "'..." << std::endl;
	for(const std::string& pkg : aurDepends)
	{
		installAurPackage(pkg, user, true);
	}

	std::cout << "> Creating AUR package '" << packageName << "'..." << std::endl;
	std::string createCommand = "cd " + packagePath + ";makepkg";
	utils::SystemUtils::csystem("su " + user + " -c \"" + createCommand + '"');

	std::cout << "> Installing AUR package '" << packageName << "'..." << std::endl;
	std::string params = "--noconfirm";
	if(asdeps) params += " --asdeps";
	int installStatus = utils::SystemUtils::csystem("pacman " + params + " -U " + packagePath + '/' + packageName + "*.pkg.tar.xz");
	utils::SystemUtils::csystem("rm -r " + packagePath);

	return installStatus;
}

MountData::MountData(const std::string& device, const std::string& dir, bool swap) :
	device(device), dir(dir), swap(swap)
{
}

}
