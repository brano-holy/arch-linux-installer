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

#ifndef ARCHLINUXINSTALLER_MODULES_USERS_USERS_HPP
#define ARCHLINUXINSTALLER_MODULES_USERS_USERS_HPP

#include "archlinuxinstaller/modules/module.hpp"

#include "user.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace users {

class Users : public Module, public std::vector<User>
{
protected:
	virtual void addUserInputs(std::vector<UserInputBase*>& userInputs) const;

public:
	static const double ORDER;

	virtual inline double getOrder() const { return ORDER; }

	virtual bool runInside(const std::function<UIT>& ui);
	virtual bool runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);

	bool createUsers() const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::users::Users>
{
	static bool decode(Node node, archlinuxinstaller::modules::users::Users& users);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_USERS_USERS_HPP
