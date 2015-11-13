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

const double Users::ORDER = 0.23;

void Users::addUserInputs(std::vector<UserInputBase*>& userInputs) const
{
	userInputs.push_back(new UserInput<std::string>("users-root-password", "UNIX password for user 'root'", UserInputType::Password));
	for(const User& user : *this)
	{
		userInputs.push_back(new UserInput<std::string>("users-" + user.name + "-password", "UNIX password for user '" + user.name + "'", UserInputType::Password));
	}
}

bool Users::runInside(const std::function<UIT>& ui)
{
	return ui("Creating users", createUsers());
}

bool Users::runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui)
{
	std::string usersPasswordsPath = "devices-root-password-" + Module::PROGRAM_NAME + ".txt";
	std::ofstream usersPasswordsFile(usersPasswordsPath);

	usersPasswordsFile << "root:" << *userInputs.at("users-root-password") << std::endl;
	for(const User& user : *this)
	{
		usersPasswordsFile << user.name << ':' << *userInputs.at("users-" + user.name + "-password") << std::endl;
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

bool convert<archlinuxinstaller::modules::users::Users>::decode(Node node, archlinuxinstaller::modules::users::Users& users)
{
	if(node.IsSequence())
	{
		YAML::convert<std::vector<archlinuxinstaller::modules::users::User>>::decode(node, users);
		return true;
	}

	return true;
}

}
