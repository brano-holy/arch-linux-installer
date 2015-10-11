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

#include <iostream>

#include "archlinuxinstaller/archlinuxinstaller.hpp"

void printHelp(const std::string& programName, const std::string& programVersion);

int main(int argc, char **argv)
{
	std::string programName = "archlinux-installer";
	std::string programVersion = "1.0";

	if(argc < 2)
	{
		printHelp(programName, programVersion);
		return 0;
	}

	std::string configPath(argv[1]);

	archlinuxinstaller::ArchLinuxInstaller installer(configPath, programName);
	return installer.install(argc, argv);
}

void printHelp(const std::string& programName, const std::string& programVersion)
{
	std::cout << "Arch Linux Installer " << programVersion << std::endl;
	std::cout << "Usage: " << programName << " <config-path>" << std::endl;
}
