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

#include <yaml-cpp/yaml.h>

namespace archlinuxinstaller {

class MountData
{
public:
	std::string device, dir;
	bool swap;
	MountData(const std::string& device, const std::string& dir, bool swap = false);
};

class ArchLinuxInstaller
{
private:
	static const std::string AUR_URL;

	std::string configPath;
	std::string programName;

	std::string argProgramPath;
	bool argChroot;
	bool argLog;

	bool efi;

	bool debug;
	bool log;
	bool keepProgram;
	bool keepConfig;

	std::string keyboard;
	std::string font;
	std::vector<std::string> locales;
	std::string lang;
	std::string timezone;
	std::string hostname;

	bool encryption;
	std::string lvmPassphrasePath;
	std::string usersPasswordsPath;

	std::string efiDirectory;
	std::string grubDevice;
	std::string grubDmname;

	YAML::Node devices;
	bool rootPartition;
	std::vector<MountData> partitionsToMount;

	YAML::Node users;
	std::vector<std::string> packages;
	std::vector<std::string> aurPackages;

	void loadArgs(int argc, char **argv);
	void loadEfi();
	void loadConfig(const std::string& configPath);

	int installWithLog(int argc, char **argv);
	int installChroot();

	void setKeyboard(bool permanent = false) const;
	void setFont(bool permanent = false) const;
	void setLocales() const;
	void setLang() const;
	void setTimezone() const;
	void setClock() const;
	void setHostname() const;

	void readPasswords();
	void createPartitions() const;
	void mountPartitions() const;
	void installBase() const;
	void runChroot() const;
	void setUsersPasswords() const;
	void finish() const;

	void updateMkinitcpio() const;
	void installGrub() const;
	void installPackages() const;
	void installAurPackages() const;
	void createUsers() const;

	static std::string getDeviceName(const YAML::Node& node, const std::string& path = "");
	static int installAurPackage(const std::string& packageName, const std::string& user, bool asdeps = false);

public:
	ArchLinuxInstaller(const std::string& configPath, const std::string& programName = "archlinux-installer");

	int install(int argc, char **argv);
};

}

#endif // ARCHLINUXINSTALLER_ARCHLINUXINSTALLER_HPP
