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

#include "archlinuxinstaller/utils/systemutils.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>

#include <termios.h>
#include <unistd.h>

#include "archlinuxinstaller/utils/stringutils.hpp"

namespace archlinuxinstaller {
namespace utils {

bool SystemUtils::DEBUG = false;

int SystemUtils::csystem(const std::string& cmd)
{
	if(DEBUG)
	{
		std::cout << ">     csystem: " << cmd << std::endl;
		return 1;
	}

	std::cout << ">     csystem: " << cmd << std::endl;
	return ::system(cmd.c_str());
}

bool SystemUtils::system(const std::string& cmd)
{
	return (csystem(cmd) == 0);
}

std::string SystemUtils::ssystem(const std::string& cmd, int bufferSize)
{
	if(DEBUG)
	{
		std::cout << ">     ssystem: " << cmd << std::endl;
		return "";
	}

	std::cout << ">     ssystem: " << cmd << std::endl;

	FILE *file = popen(cmd.c_str(), "r");
	if(file == NULL) return "";

	std::string output;
	char *buffer = new char[bufferSize];

	while(fgets(buffer, bufferSize, file) != NULL)
	{
		output += std::string(buffer);
	}

	delete[] buffer;
	pclose(file);

	return output;
}

int SystemUtils::getRAMSize(char& unit)
{
	std::istringstream iss(ssystem("cat /proc/meminfo | grep MemTotal"));

	std::string tmp, memUnit;
	int memSize;

	iss >> tmp >> memSize >> memUnit;
	unit = memUnit[0];

	return memSize;
}

int SystemUtils::alignSize(int minSize, int aligment)
{
	return (((minSize - 1) / aligment) + 1) * aligment;
}

std::string SystemUtils::getSizeByCommand(std::string size, const std::string& command)
{
	std::size_t percentPos = size.find("%RAM");
	if(percentPos != std::string::npos)
	{
		double percentSize = std::stod(size.substr(0, percentPos));

		char ramSizeUnit;
		int ramSize = getRAMSize(ramSizeUnit);
		size = std::to_string(alignSize(ramSize * percentSize)) + ramSizeUnit;
	}

	if(command == "sgdisk")
	{
		if(size == "rest") size = "0";
		else size = '+' + size;
	}
	else if(command == "lvcreate")
	{
		if(size == "rest") size = "100%FREE";
	}

	return size;
}

bool SystemUtils::createFilesystem(const std::string& filesystem, const std::string& partition)
{
	std::string command;
	if(filesystem == "swap") command = "mkswap";
	else command = "mkfs." + filesystem;

	return SystemUtils::system(command + ' ' + partition);
}

std::string SystemUtils::readPassword(const std::string& passwordName)
{
	struct termios terminal;
	tcgetattr(STDIN_FILENO, &terminal);

	struct termios hiddenTerminal = terminal;
	hiddenTerminal.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &hiddenTerminal);

	std::string password, passwordCheck;
	while(true)
	{
		std::cout << "Enter " << passwordName << ": ";
		std::getline(std::cin, password);
		std::cout << std::endl;

		std::cout << "Verify " << passwordName << ": ";
		std::getline(std::cin, passwordCheck);
		std::cout << std::endl;

		if(password == passwordCheck) break;
		else std::cout << "Error: Passwords do not match!" << std::endl << std::endl;
	}

	tcsetattr(STDIN_FILENO, TCSANOW, &terminal);

	return password;
}

bool SystemUtils::exportKey(const std::string& fromPath, const std::string& toPath, bool del)
{
	bool status = true;

	std::string localPath = fromPath;
	if(utils::StringUtils::startsWith(fromPath, "http://") || utils::StringUtils::startsWith(fromPath, "https://"))
	{
		localPath = "ssh_key.pub";
		status &= utils::SystemUtils::system("wget " + fromPath + " -O " + localPath);
	}

	std::string toPathDir = toPath.substr(0, toPath.rfind('/'));
	status &= utils::SystemUtils::system("mkdir -p " + toPathDir);
	status &= utils::SystemUtils::system("cat " + localPath + " >> " + toPath);

	if(del) status &= utils::SystemUtils::system("rm " + localPath);

	return status;
}

}}
