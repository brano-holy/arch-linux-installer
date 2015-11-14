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

#include "archlinuxinstaller/packageinstaller.hpp"

#include <algorithm>
#include <fstream>
#include <random>
#include <set>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/format.hpp>

namespace archlinuxinstaller {

const std::string PackageInstaller::DEFAULT_AUR_URL = "https://aur.archlinux.org/cgit/aur.git/snapshot/%s.tar.gz";
std::vector<std::function<void(const std::string&)>> PackageInstaller::globalAfterInstall;

PackageInstaller::PackageInstaller() :
	aurUrl(DEFAULT_AUR_URL), aurUserCreated(false), aurRequirementsInstalled(false)
{
	static std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(1000, 1000000);

	aurUser = "aur-installer-" + std::to_string(uni(rng));
}

std::vector<std::string> PackageInstaller::packagesToInstall(const std::string& packageName) const
{
	std::string command;
	if(isGroup(packageName)) command = "pacman -Sg base | sed -r 's/^[^ ]+ ([^ ]+)$/\\1/g' | xargs -I {} pactree -su {} | sort | uniq | xargs pacman -T";
	else command = "pactree -su " + packageName + " | xargs pacman -T";

	std::vector<std::string> packages;
	std::string packagesString = utils::SystemUtils::ssystem(command);

	return boost::algorithm::split(packages, packagesString, boost::is_any_of("\n"));
}

void PackageInstaller::runAfterInstall(const std::string& packageName) const
{
	for(const auto& func : globalAfterInstall) func(packageName);
	for(const auto& func : afterInstall) func(packageName);
}

bool PackageInstaller::installAurRequirements()
{
	if(!aurRequirementsInstalled)
	{
		if(installPackage("base-devel")) aurRequirementsInstalled = true;
		else return false;
	}

	return true;
}

bool PackageInstaller::installPackage(const std::string& packageName) const
{
	return installPackage(packageName, false);
}

bool PackageInstaller::installPackage(const std::string& packageName, bool asDeps) const
{
	if(boost::algorithm::trim_copy(packageName).empty()) return false;

	std::vector<std::string> toInstall = packagesToInstall(packageName);

	std::string asDepsParam = (asDeps ? "--asdeps " : "");
	bool status = utils::SystemUtils::system("pacman -S --noconfirm --needed " + asDepsParam + packageName);
	if(status) runAfterInstall(toInstall.begin(), toInstall.end());

	return status;
}

bool PackageInstaller::installPackages(const std::vector<std::string>& packageNames) const
{
	return installPackages(packageNames, false);
}

bool PackageInstaller::installPackages(const std::vector<std::string>& packageNames, bool asDeps) const
{
	if(packageNames.empty()) return false;

	std::set<std::string> toInstall;
	for(const std::string& packageName : packageNames)
	{
		std::vector<std::string> tmpToInstall = packagesToInstall(packageName);
		toInstall.insert(tmpToInstall.begin(), tmpToInstall.end());
	}

	std::string asDepsParam = (asDeps ? "--asdeps " : "");
	bool status = utils::SystemUtils::system("pacman -S --noconfirm --needed " + asDepsParam + boost::algorithm::join(packageNames, " "));

	if(status) runAfterInstall(toInstall.begin(), toInstall.end());

	return status;
}

bool PackageInstaller::createAurUser()
{
	if(!aurUserCreated)
	{
		aurUserCreated = utils::SystemUtils::system("useradd -m " + aurUser);
		return aurUserCreated;
	}

	return false;
}

bool PackageInstaller::removeAurUser()
{
	if(aurUserCreated)
	{
		aurUserCreated = !utils::SystemUtils::system("userdel -r " + aurUser);
		return aurUserCreated;
	}

	return false;
}

bool PackageInstaller::downloadAurPackage(const std::string& packageName) const
{
	if(boost::algorithm::trim_copy(packageName).empty()) return false;

	std::string downloadCommand = "cd;";
	downloadCommand += "wget " + boost::str(boost::format(aurUrl) % packageName) + ';';
	downloadCommand += "tar -xf " + packageName + ".tar.gz";

	return utils::SystemUtils::system("su " + aurUser + " -c \"" + downloadCommand + '"');
}

std::string PackageInstaller::getAurDepends(const std::string& pkgbuildPath) const
{
	std::ifstream pkgbuild(pkgbuildPath);

	std::string line, depends;
	while(std::getline(pkgbuild, line))
	{
		if(boost::algorithm::starts_with(line, "depends=(") ||
		   boost::algorithm::starts_with(line, "makedepends=("))
		{
			std::size_t startPos = line.find('(');
			std::size_t endPos = line.rfind(')');

			if(startPos != std::string::npos && endPos != std::string::npos)
			{
				startPos++;
				depends += boost::algorithm::trim_copy(line.substr(startPos, endPos - startPos));
				depends += ' ';
			}
		}
	}

	return boost::algorithm::trim_copy(depends);
}

void PackageInstaller::getMissingDepends(const std::string& pkgbuildPath, std::vector<std::string>& depends, std::vector<std::string>& aurDepends) const
{
	std::vector<std::string> missingDepends;
	std::string packagesString = utils::SystemUtils::ssystem("pacman -T " + getAurDepends(pkgbuildPath));
	boost::algorithm::split(missingDepends, packagesString, boost::is_any_of("\n"));

	for(const std::string& pkg : missingDepends)
	{
		std::string pkgName = pkg.substr(0, pkg.find_first_of("><="));
		if(utils::SystemUtils::system("pacman -Ss '^" + pkgName + "$'"))
		{
			depends.push_back('\'' + pkg + '\'');
		}
		else
		{
			aurDepends.push_back(pkgName);
		}
	}
}

bool PackageInstaller::installAurPackage(const std::string& packageName)
{
	return installAurPackage(packageName, false);
}

bool PackageInstaller::installAurPackage(const std::string& packageName, bool asDeps)
{
	if(boost::algorithm::trim_copy(packageName).empty()) return false;
	bool deleteAurUser = createAurUser();

	installAurRequirements();

	std::string packagePath = "/home/" + aurUser + '/' + packageName;

	downloadAurPackage(packageName);

	std::vector<std::string> pacmanDepends, aurDepends;
	getMissingDepends(packagePath + "/PKGBUILD", pacmanDepends, aurDepends);

	installPackages(pacmanDepends, true);
	installAurPackages(aurDepends, true);

	std::string createCommand = "cd " + packagePath + ";makepkg";
	utils::SystemUtils::csystem("su " + aurUser + " -c \"" + createCommand + '"');

	std::string params = "--noconfirm";
	if(asDeps) params += " --asdeps";
	bool status = utils::SystemUtils::system("pacman " + params + " -U " + packagePath + '/' + packageName + "*.pkg.tar.xz");
	utils::SystemUtils::csystem("rm -r " + packagePath);

	if(deleteAurUser) removeAurUser();

	if(status) runAfterInstall(packageName);

	return status;
}

bool PackageInstaller::installAurPackages(const std::vector<std::string>& packageNames)
{
	return installAurPackages(packageNames, false);
}

bool PackageInstaller::installAurPackages(const std::vector<std::string>& packageNames, bool asDeps)
{
	if(packageNames.empty()) return false;

	bool deleteAurUser = createAurUser();

	bool status = std::all_of(packageNames.begin(), packageNames.end(), [this, asDeps](const std::string& packageName)
	{
		return this->installAurPackage(packageName, asDeps);
	});

	if(deleteAurUser) removeAurUser();

	return status;
}

}
