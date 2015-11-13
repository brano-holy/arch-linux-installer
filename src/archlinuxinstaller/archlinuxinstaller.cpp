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

#include <boost/algorithm/string/trim.hpp>

#include "archlinuxinstaller/configuration/config.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {

const std::string ArchLinuxInstaller::AUR_URL = "https://aur.archlinux.org/cgit/aur.git/snapshot/%s.tar.gz";
const std::string ArchLinuxInstaller::BASE_PACKAGES = "wget";

ArchLinuxInstaller::ArchLinuxInstaller(const std::string& configPath, const std::string& programName) :
	configPath(configPath), programName(programName), argChroot(false), argLog(false)
{
	loadConfig(configPath);
	loadEfi();

	packageInstaller.addAfterInstall(std::bind(&ArchLinuxInstaller::afterInstall, this, std::placeholders::_1));
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
	efi = utils::SystemUtils::system("efivar -l > /dev/null 2>&1");
}

void ArchLinuxInstaller::loadConfig(const std::string& configPath)
{
	YAML::Node config = YAML::LoadFile(configPath);

	debug = config["debug"].as<bool>(false);
	log = config["log"].as<bool>(false);
	keepProgram = config["keepProgram"].as<bool>(false);
	keepConfig = config["keepConfig"].as<bool>(false);

	_devices = config["devices"].as<archlinuxinstaller::modules::devices::Devices>();
	_settings = config["settings"].as<archlinuxinstaller::modules::settings::Settings>();
	_users = config["users"].as<std::vector<archlinuxinstaller::modules::users::User>>();

	if(config["packages"]) packages = config["packages"].as<std::vector<std::string>>();
	if(config["aurPackages"]) aurPackages = config["aurPackages"].as<std::vector<std::string>>();

	utils::SystemUtils::DEBUG = debug;
}

int ArchLinuxInstaller::installWithLog(int, char **)
{
	std::string cmd = ""; // utils::StringUtils::join(argv, argv + argc, ' ');
	return utils::SystemUtils::csystem(cmd + " --log 2>&1 | tee log-" + programName + ".txt");
}

int ArchLinuxInstaller::install(int argc, char **argv)
{
	loadArgs(argc, argv);

	if(!argLog && log) return installWithLog(argc, argv);
	if(argChroot) return installChroot();

	printTitle("Checking");
	if(!printInfo("Looking for the root partition", _devices.hasRoot())) return 1;

	printTitle("Basic settings");
	printInfo("Setting keyboard", _settings.setKeyboard());
	printInfo("Setting font", _settings.setFont());

	printTitle("Entering passwords");
	printInfo("Reading passwords", readPasswords());

	printTitle("Preparing drives");
	printInfo("Creating partitions", _devices.createPartitions(lvmPassphrasePath));
	printInfo("Mounting partitions", _devices.mountPartitions());

	printTitle("Installation");
	printInfo("Installing Arch Linux base", installBase());

	runChroot();

	printTitle("Finishing");
	printInfo("Setting users' passwords", setUsersPasswords());
	printInfo("Final clean up", cleanUp());

	std::cout << "> Installation has finished. Press [Enter] to reboot..." << std::endl;
	std::string tmpReboot;
	std::getline(std::cin, tmpReboot);

	utils::SystemUtils::csystem("reboot");

	return 0;
}

int ArchLinuxInstaller::installChroot()
{
	printTitle("Basic settings");
	printInfo("Setting keyboard", _settings.setKeyboard(true));
	printInfo("Setting font", _settings.setFont(true));
	printInfo("Setting locales", _settings.setLocales());
	printInfo("Setting lang", _settings.setLang());
	printInfo("Setting timezone", _settings.setTimezone());
	printInfo("Setting hostname", _settings.setHostname());

	printInfo("Setting clock", setClock());
	printInfo("Configuring network", setNetwork());

	printInfo("Creating users", createUsers());
	printInfo("Installing GRUB", installGrub());

	printTitle("Additional packages");
	if(!packages.empty()) printInfo("Installing packages", packageInstaller.installPackages(packages));
	if(!aurPackages.empty())
	{
		packageInstaller.installAurRequirements();
		printInfo("Installing AUR packages", packageInstaller.installAurPackages(aurPackages));
	}

	printTitle("Additional features");
	if(_devices.hasEncryption())
	{
		std::string grubDevice, grubDmname;
		_devices.getGrubParams(grubDevice, grubDmname);

		std::string message = "Installing encryption";
		if(_devices.getEncryption()->sshDecrypt) message += " & SSH decrypt";

		printInfo(message, _devices.getEncryption()->install(packageInstaller, grubDevice, grubDmname));
	}

	printInfo("Creating users", createUsers());

	return 0;
}

bool ArchLinuxInstaller::setClock() const
{
	return (utils::SystemUtils::system("hwclock --systohc --utc") &&
			utils::SystemUtils::system("timedatectl set-ntp true"));
}

bool ArchLinuxInstaller::setNetwork() const
{
	return utils::SystemUtils::system("systemctl enable dhcpcd.service");
}

bool ArchLinuxInstaller::readPasswords()
{
	std::cout << "> Keyboard layout: " << _settings.keyboard.value_or("default (probably 'us')") << std::endl;

	try
	{
		if(_devices.hasEncryption())
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

		for(const modules::users::User& user : _users)
		{
			std::string password = utils::SystemUtils::readPassword("UNIX password for user '" + user.name + '\'');

			usersPasswordsFile << user.name << ':' << password << std::endl;
			std::cout << std::endl;
		}

		return true;
	}
	catch(const std::exception&)
	{
		return false;
	}
}

bool ArchLinuxInstaller::installBase() const
{
	std::string basePackages = "base " + BASE_PACKAGES;
	if(efi) basePackages += " efivar";

	bool status = utils::SystemUtils::system("pacstrap /mnt " + basePackages);
	status &= utils::SystemUtils::system("genfstab -U -p /mnt >> /mnt/etc/fstab");

	return status;
}

void ArchLinuxInstaller::runChroot() const
{
	utils::SystemUtils::csystem("mkdir /mnt/root/" + programName);
	utils::SystemUtils::csystem("cp " + argProgramPath + " /mnt/root/" + programName + '/' + programName);
	utils::SystemUtils::csystem("cp " + configPath + " /mnt/root/" + programName + "/config.yaml");

	std::string chrootParams = "--chroot";
	if(log) chrootParams += " --log";
	utils::SystemUtils::csystem("arch-chroot /mnt /root/" + programName + '/' + programName + " /root/" + programName + "/config.yaml " + chrootParams);
}

bool ArchLinuxInstaller::setUsersPasswords() const
{
	return (utils::SystemUtils::system("arch-chroot /mnt chpasswd < " + usersPasswordsPath) &&
			utils::SystemUtils::system("rm " + usersPasswordsPath));
}

bool ArchLinuxInstaller::cleanUp() const
{
	bool status = true;

	if(!keepProgram) status &= utils::SystemUtils::system("rm /mnt/root/" + programName + '/' + programName);
	if(!keepConfig) status &= utils::SystemUtils::system("rm /mnt/root/" + programName + "/config.yaml");
	if(log) status &= utils::SystemUtils::system("cp log-" + programName + ".txt /mnt/root/" + programName + "/log.txt");
	status &= utils::SystemUtils::system("rmdir --ignore-fail-on-non-empty /mnt/root/" + programName);

	status &= utils::SystemUtils::system("umount -R /mnt");

	return status;
}

void ArchLinuxInstaller::afterInstall(const std::string& packageName)
{
	if(packageName == "sudo")
	{
		utils::SystemUtils::csystem("echo \"%wheel ALL=(ALL) ALL\" > /etc/sudoers.d/99-wheel");
	}
	else if(packageName == "grub")
	{
		if(efi)
		{
			std::string grubTarget = (boost::algorithm::trim_copy(utils::SystemUtils::ssystem("uname -m")) == "x86_64" ? "x86_64-efi" : "i386-efi");
			utils::SystemUtils::csystem("grub-install --target=" + grubTarget + " --efi-directory=" + _devices.getEfiDirectory() + " --bootloader-id=grub --recheck");
		}
		else
		{
			utils::SystemUtils::csystem("grub-install --recheck " + _devices.front().path);
		}

		utils::SystemUtils::csystem("grub-mkconfig -o /boot/grub/grub.cfg");
	}
}

bool ArchLinuxInstaller::installGrub() const
{
	if(efi) return packageInstaller.installPackages({"grub", "dosfstools", "efibootmgr"});
	else return packageInstaller.installPackage("grub");
}

bool ArchLinuxInstaller::createUsers() const
{
	return std::all_of(_users.begin(), _users.end(), [](const modules::users::User& user)
	{
		return user.create();
	});
}

void ArchLinuxInstaller::printTitle(const std::string& title)
{
	std::string border(title.length(), '=');

	std::cout << border << std::endl;
	std::cout << title << std::endl;
	std::cout << border << std::endl;
}

bool ArchLinuxInstaller::printStatus(bool status)
{
	std::cout << (status ? "OK" : "Failed") << std::endl;
	return status;
}

bool ArchLinuxInstaller::printInfo(const std::string& message, bool status)
{
	std::cout << "> " << message << "... ";
	return printStatus(status);
}

}
