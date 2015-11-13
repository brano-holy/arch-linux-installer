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

#ifndef ARCHLINUXINSTALLER_UTILS_FORMATUTILS_HPP
#define ARCHLINUXINSTALLER_UTILS_FORMATUTILS_HPP

#include <ctime>
#include <string>

namespace archlinuxinstaller {
namespace utils {

class FormatUtils
{
public:
	static std::string formatDatetime(const std::tm *tmb, const std::string& fmt);
	static std::string formatNow(const std::string& fmt);
};

}}

#endif // ARCHLINUXINSTALLER_UTILS_FORMATUTILS_HPP
