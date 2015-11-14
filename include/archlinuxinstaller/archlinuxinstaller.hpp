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

#ifndef ARCHLINUXINSTALLER_ARCHLINUXINSTALLER_HPP
#define ARCHLINUXINSTALLER_ARCHLINUXINSTALLER_HPP

#include "archlinuxinstaller/modules/module.hpp"

#include "packageinstaller.hpp"

namespace archlinuxinstaller {

class ArchLinuxInstaller
{
private:
	static const std::string BASE_PACKAGES;

	std::string configPath;
	std::string programName;

	std::string argProgramPath;
	bool argChroot;

	std::ostream ui;
	std::function<modules::Module::UIT> uif;

	std::vector<modules::Module*> modules;
	bool efi;

	void loadArgs(int argc, char **argv);
	void loadEfi();
	void loadConfig(const std::string& configPath);

	int installChroot();

	bool setClock() const;
	bool setNetwork() const;
	std::map<std::string, std::map<std::string, modules::UserInputBase*>> readUserInputs();

	bool installBase() const;
	void runChroot() const;
	bool cleanUp() const;

	void afterInstall(const std::string& packageName);

	bool installGrub() const;

	static void printTitle(const std::string& title);
	static bool printStatus(bool status);
	static bool printInfo(const std::string& message, bool status);

public:
	ArchLinuxInstaller(const std::string& configPath, const std::string& programName = "archlinux-installer");

	int install(int argc, char **argv);
};

}

#endif // ARCHLINUXINSTALLER_ARCHLINUXINSTALLER_HPP
