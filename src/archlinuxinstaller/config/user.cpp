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

#include "archlinuxinstaller/config/user.hpp"

#include "archlinuxinstaller/utils/stringutils.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace config {

bool User::create() const
{
	std::string params;
	if(createHome) params += " -m";
	if(comment) params += " -c \"" + *comment + '"';
	if(!groups.empty()) params += " -G " + utils::StringUtils::join(groups.begin(), groups.end(), ',');

	bool status = utils::SystemUtils::system("useradd" + params + ' ' + name);

	if(createHome)
	{
		for(const std::string& key : sshKeys)
		{
			status &= utils::SystemUtils::exportKey(key, "/home/" + name + "/.ssh/authorized_keys");
		}
	}

	return status;
}

}}

namespace YAML {

bool convert<archlinuxinstaller::config::User>::decode(Node node, archlinuxinstaller::config::User& user)
{
	if(node["user"]) node = node["user"];

	if(node.IsMap())
	{
		user.name = node["name"].as<std::string>();
		if(node["comment"]) user.comment = node["comment"].as<std::string>();
		user.createHome = node["createHome"].as<bool>(true);
		if(node["groups"]) user.groups = node["groups"].as<std::vector<std::string>>();
		if(node["sshKeys"]) user.sshKeys = node["sshKeys"].as<std::vector<std::string>>();

		return true;
	}

	return false;
}

}
