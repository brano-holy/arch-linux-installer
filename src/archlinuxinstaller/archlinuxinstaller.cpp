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

/*
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

#include <boost/algorithm/string/trim.hpp>

#include "archlinuxinstaller/configuration/config.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"
*/

#include "archlinuxinstaller/modules/userinput.hpp" // TODO: needed?

#include "archlinuxinstaller/modules/general/general.hpp"
#include "archlinuxinstaller/modules/logging/logging.hpp"
#include "archlinuxinstaller/modules/devices/devices.hpp"
#include "archlinuxinstaller/modules/settings/settings.hpp"
#include "archlinuxinstaller/modules/pacman/pacman.hpp"
#include "archlinuxinstaller/modules/users/users.hpp"

namespace archlinuxinstaller {

const std::string ArchLinuxInstaller::BASE_PACKAGES = "wget";

ArchLinuxInstaller::ArchLinuxInstaller(const std::string& configPath, const std::string& programName) :
	configPath(configPath), programName(programName), argChroot(false), ui(std::cout.rdbuf()),
	uif(std::bind(&ArchLinuxInstaller::printInfo, std::placeholders::_1, std::placeholders::_2))
{
	loadConfig(configPath);
	loadEfi();

	PackageInstaller::addGlobalAfterInstall(std::bind(&ArchLinuxInstaller::afterInstall, this, std::placeholders::_1));
}

void ArchLinuxInstaller::loadArgs(int argc, char **argv)
{
	argProgramPath = std::string(argv[0]);
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "--chroot") == 0) argChroot = true;
	}
}

void ArchLinuxInstaller::loadEfi()
{
	efi = utils::SystemUtils::system("efivar -l > /dev/null 2>&1");
}

void ArchLinuxInstaller::loadConfig(const std::string& configPath)
{
	namespace m = archlinuxinstaller::modules;

	YAML::Node config = YAML::LoadFile(configPath);

	modules.push_back(new m::general::General());
	modules.push_back(new m::devices::Devices());
	modules.push_back(new m::users::Users());

	if(config["logging"]) modules.push_back(new m::logging::Logging());
	if(config["settings"]) modules.push_back(new m::settings::Settings());
	if(config["pacman"]) modules.push_back(new m::pacman::Pacman());

	for(m::Module *module : modules)
	{
		module->loadConfig(config); // TODO: rethrow error, cannot parse required module
	}
}

int ArchLinuxInstaller::install(int argc, char **argv)
{
	namespace m = archlinuxinstaller::modules;

	loadArgs(argc, argv);

	if(argChroot) return installChroot();

	std::sort(modules.begin(), modules.end(), [](m::Module *a, m::Module *b) { return a->getOrder() < b->getOrder(); });
	for(m::Module *module : modules)
	{
		if(!module->runOutsideBefore(uif)) return 1;
	}

	std::map<std::string, std::map<std::string, m::UserInputBase*>> modulesUserInputs = readUserInputs();

	for(m::Module *module : modules)
	{
		if(!module->runOutside(modulesUserInputs[module->getTagName()], uif)) return 1;
	}


	printTitle("Installation");
	printInfo("Installing Arch Linux base", installBase());

	runChroot();

	std::sort(modules.begin(), modules.end(), [](m::Module *a, m::Module *b) { return a->getOrder() > b->getOrder(); });
	for(m::Module *module : modules)
	{
		if(!module->runOutsideAfter(modulesUserInputs[module->getTagName()], uif)) return 1;
	}

	printTitle("Finishing");
	printInfo("Final clean up", cleanUp());

	std::cout << "> Installation has finished. Press [Enter] to reboot..." << std::endl;
	std::string tmpReboot;
	std::getline(std::cin, tmpReboot);

	utils::SystemUtils::csystem("reboot");

	return 0;
}

int ArchLinuxInstaller::installChroot()
{
	namespace m = archlinuxinstaller::modules;

	std::sort(modules.begin(), modules.end(), [](m::Module *a, m::Module *b) { return a->getOrder() < b->getOrder(); });
	for(m::Module *module : modules)
	{
		if(!module->runInsideBefore(uif)) return 1;
	}

	for(m::Module *module : modules)
	{
		if(!module->runInside(uif)) return 1;
	}

	for(m::Module *module : modules)
	{
		if(!module->runInsideAfter(uif)) return 1;
	}
	printInfo("Setting clock", setClock());
	printInfo("Configuring network", setNetwork());
	printInfo("Installing GRUB", installGrub());

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

std::map<std::string, std::map<std::string, modules::UserInputBase*>> ArchLinuxInstaller::readUserInputs()
{
	namespace m = archlinuxinstaller::modules;

	std::map<std::string, std::map<std::string, m::UserInputBase*>> modulesUserInputs;
	for(m::Module *module : modules)
	{
		std::map<std::string, m::UserInputBase*> moduleUserInputs;

		std::vector<m::UserInputBase*> userInputs = module->getUserInputs();
		for(m::UserInputBase *userInput : userInputs)
		{
			moduleUserInputs[userInput->getKey()] = userInput;

			if(userInput->getType() == m::UserInputType::Text)
			{
				ui << userInput->getName() << ": ";
				std::cin >> *userInput;
			}
			else if(userInput->getType() == m::UserInputType::Password)
			{
				std::istringstream iss(utils::SystemUtils::readPassword(userInput->getName()));
				iss >> *userInput;
			}

			ui << std::endl;
		}

		modulesUserInputs[module->getTagName()] = moduleUserInputs;
	}

	return modulesUserInputs;
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
	utils::SystemUtils::csystem("arch-chroot /mnt /root/" + programName + '/' + programName + " /root/" + programName + "/config.yaml " + chrootParams);
}

bool ArchLinuxInstaller::cleanUp() const
{
	return utils::SystemUtils::system("umount -R /mnt");
}

void ArchLinuxInstaller::afterInstall(const std::string& packageName)
{
	if(packageName == "grub")
	{
		/*
		if(efi)
		{
			std::string grubTarget = (boost::algorithm::trim_copy(utils::SystemUtils::ssystem("uname -m")) == "x86_64" ? "x86_64-efi" : "i386-efi");
			utils::SystemUtils::csystem("grub-install --target=" + grubTarget + " --efi-directory=" + _devices.getEfiDirectory() + " --bootloader-id=grub --recheck");
		}
		else
		{
			utils::SystemUtils::csystem("grub-install --recheck " + _devices.front().path);
		}
		*/

		utils::SystemUtils::csystem("grub-mkconfig -o /boot/grub/grub.cfg");
	}
}

bool ArchLinuxInstaller::installGrub() const
{
	PackageInstaller packageInstaller;
	if(efi) return packageInstaller.installPackages({"grub", "dosfstools", "efibootmgr"});
	else return packageInstaller.installPackage("grub");
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
