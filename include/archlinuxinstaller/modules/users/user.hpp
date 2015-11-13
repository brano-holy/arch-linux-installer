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

#ifndef ARCHLINUXINSTALLER_MODULES_USERS_USER_HPP
#define ARCHLINUXINSTALLER_MODULES_USERS_USER_HPP

#include <string>
#include <experimental/optional>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace archlinuxinstaller {
namespace modules {
namespace users {

class User
{
public:
	std::string name;
	std::experimental::optional<std::string> comment;
	bool createHome;
	std::vector<std::string> groups;
	std::vector<std::string> authorizedKeys;

	bool create() const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::users::User>
{
	static bool decode(Node node, archlinuxinstaller::modules::users::User& user);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_USERS_USERS_HPP
