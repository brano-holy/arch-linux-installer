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

#ifndef ARCHLINUXINSTALLER_PACKAGEINSTALLER_HPP
#define ARCHLINUXINSTALLER_PACKAGEINSTALLER_HPP

#include <functional>
#include <string>
#include <vector>


#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {

class PackageInstaller
{
private:
	static const std::string DEFAULT_AUR_URL;
	static const std::string DEFAULT_AUR_USER;

	std::string aurUrl;
	std::string aurUser;
	bool aurUserCreated;

	std::vector<std::function<void(const std::string&)>> afterInstall;

	void runAfterInstall(const std::string& packageName) const;

	template <typename ForwardIterator>
	void runAfterInstall(ForwardIterator begin, ForwardIterator end) const;

	bool installPackage(const std::string& packageName, bool asDeps) const;
	bool installPackages(const std::vector<std::string>& packageNames, bool asDeps) const;

	bool createAurUser();
	bool removeAurUser();

	bool downloadAurPackage(const std::string& packageName) const;
	std::string getAurDepends(const std::string& pkgbuildPath) const;
	void getMissingDepends(const std::string& pkgbuildPath, std::vector<std::string>& depends, std::vector<std::string>& aurDepends) const;

	bool installAurPackage(const std::string& packageName, bool asDeps);
	bool installAurPackages(const std::vector<std::string>& packageNames, bool asDeps);

public:
	PackageInstaller();

	inline const std::string& getAurUrl() const { return aurUrl; }
	inline void setAurUrl(const std::string& aurUrl) { this->aurUrl = aurUrl; }

	inline const std::string& getAurUser() const { return aurUser; }
	inline void setAurUser(const std::string& aurUser) { this->aurUser = aurUser; }

	inline void addAfterInstall(const std::function<void(const std::string&)>& func) { afterInstall.push_back(func); }

	inline bool isInstalled(const std::string& packageName) const { return utils::SystemUtils::system("pacman -Q " + packageName); }
	inline bool isGroup(const std::string& name) const { return utils::SystemUtils::system("pacman -Sg " + name); }

	std::vector<std::string> packagesToInstall(const std::string& packageName) const;

	bool installAurRequirements() const;

	bool installPackage(const std::string& packageName) const;
	bool installPackages(const std::vector<std::string>& packageNames) const;

	bool installAurPackage(const std::string& packageName);
	bool installAurPackages(const std::vector<std::string>& packageNames);
};

template <typename ForwardIterator>
void PackageInstaller::runAfterInstall(ForwardIterator begin, ForwardIterator end) const
{
	for(auto it = begin; it != end; it++) runAfterInstall(*it);
}

}

#endif // ARCHLINUXINSTALLER_PACKAGEINSTALLER_HPP
