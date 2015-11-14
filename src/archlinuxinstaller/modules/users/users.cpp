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

#include "archlinuxinstaller/modules/users/users.hpp"

#include <fstream>

#include "archlinuxinstaller/modules/userinput.hpp"
#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace users {

Users::Users() : Module("users", 0.23), std::vector<User>()
{
}

bool Users::decode(const YAML::Node& node)
{
	return YAML::convert<Users>::decode(node, *this);
}

void Users::addUserInputs(std::vector<UserInputBase*>& userInputs) const
{
	userInputs.push_back(new UserInput<std::string>("root", "UNIX password for user 'root'", UserInputType::Password));
	for(const User& user : *this)
	{
		userInputs.push_back(new UserInput<std::string>(user.name, "UNIX password for user '" + user.name + "'", UserInputType::Password));
	}
}

bool Users::runInside(const std::function<UIT>& ui)
{
	if(!empty()) return ui("Creating users", createUsers());
	return true;
}

bool Users::runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui)
{
	std::string usersPasswordsPath = "users-passwords-" + Module::PROGRAM_NAME + ".txt";
	std::ofstream usersPasswordsFile(usersPasswordsPath);

	usersPasswordsFile << "root:" << *userInputs.at("root") << std::endl;
	for(const User& user : *this)
	{
		usersPasswordsFile << user.name << ':' << *userInputs.at(user.name) << std::endl;
	}

	return ui("Setting users' passwords",
			  utils::SystemUtils::system("arch-chroot /mnt chpasswd < " + usersPasswordsPath) &&
			  utils::SystemUtils::system("rm " + usersPasswordsPath));
}

bool Users::createUsers() const
{
	return std::all_of(begin(), end(), [](const User& user)
	{
		return user.create();
	});
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::users::Users>::decode(const Node& node, archlinuxinstaller::modules::users::Users& users)
{
	if(node.IsSequence())
	{
		YAML::convert<std::vector<archlinuxinstaller::modules::users::User>>::decode(node, users);
		return true;
	}

	return true;
}

}
