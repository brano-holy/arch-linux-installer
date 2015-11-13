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

#include "archlinuxinstaller/utils/formatutils.hpp"

#include <iomanip>
#include <sstream>

namespace archlinuxinstaller {
namespace utils {

std::string FormatUtils::formatDatetime(const tm *tmb, const std::string& fmt)
{
	std::stringstream ss;
	ss << std::put_time(tmb, fmt.c_str());

	return ss.str();
}

std::string FormatUtils::formatNow(const std::string& fmt)
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	return formatDatetime(&tm, fmt);
}

}}
