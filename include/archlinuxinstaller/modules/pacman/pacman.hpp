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

#ifndef ARCHLINUXINSTALLER_MODULES_PACMAN_PACMAN_HPP
#define ARCHLINUXINSTALLER_MODULES_PACMAN_PACMAN_HPP

#include "archlinuxinstaller/modules/module.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace pacman {

class Pacman : public Module
{
public:
	static const double ORDER;

	std::vector<std::string> packages;
	std::vector<std::string> aurPackages;

	virtual inline double getOrder() const { return ORDER; }
	virtual bool runInside(const std::function<UIT>& ui);
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::pacman::Pacman>
{
	static bool decode(Node node, archlinuxinstaller::modules::pacman::Pacman& pacman);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_PACMAN_PACMAN_HPP
