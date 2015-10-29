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

#ifndef ARCHLINUXINSTALLER_CONFIG_SSHDECRYPT_HPP
#define ARCHLINUXINSTALLER_CONFIG_SSHDECRYPT_HPP

#include <string>

#include <yaml-cpp/yaml.h>

#include "archlinuxinstaller/packageinstaller.hpp"

namespace archlinuxinstaller {
namespace config {

class SshDecrypt
{
public:
	std::string ip;
	std::string network;
	std::string sshServer;
	std::string sshKey;

	bool install(PackageInstaller& packageInstaller, const std::string& grubDevice = "", const std::string& grubDmname = "") const;
};

}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::config::SshDecrypt>
{
	static bool decode(const Node& node, archlinuxinstaller::config::SshDecrypt& sshDecrypt);
};

}

#endif // ARCHLINUXINSTALLER_CONFIG_SSHDECRYPT_HPP
