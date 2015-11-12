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

#include "archlinuxinstaller/modules/module.hpp"

#include "archlinuxinstaller/modules/userinput.hpp"

namespace archlinuxinstaller {
namespace modules {

std::string Module::PROGRAM_NAME = "";

Module::~Module()
{
	for(UserInputBase *userInput : userInputs) delete userInput;
	userInputs.clear();
}

void Module::addUserInputs(std::vector<UserInputBase*>&) const
{
}

std::vector<UserInputBase*> Module::getUserInputs()
{
	if(userInputs.empty()) addUserInputs(userInputs);
	return userInputs;
}

bool Module::runOutsideBefore(const std::function<UIT>&)
{
	return true;
}

bool Module::runOutside(const std::map<std::string, UserInputBase*>&, const std::function<UIT>&)
{
	return true;
}

bool Module::runInsideBefore(const std::function<UIT>&)
{
	return true;
}

bool Module::runInside(const std::function<UIT>&)
{
	return true;
}

bool Module::runInsideAfter(const std::function<UIT>&)
{
	return true;
}

bool Module::runOutsideAfter(const std::map<std::string, UserInputBase*>&, const std::function<UIT>&)
{
	return true;
}

bool Module::operator<(const Module& other) const
{
	return getOrder() < other.getOrder();
}

}}
