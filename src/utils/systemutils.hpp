/*
 * Arch Linux Installer
 * Copyright (C) 2014  Branislav Holý <brano.holy@gmail.com>
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

#ifndef UTILS_SYSTEMUTILS_HPP
#define UTILS_SYSTEMUTILS_HPP

#include <string>

namespace utils {
	class SystemUtils
	{
	public:
		static int csystem(std::string cmd);
		static std::string ssystem(std::string cmd, int bufferSize = 256);

		static int getRAMSize(char &unit);
		static int alignSize(int minSize, int aligment = 2048);
	};
}

#endif // UTILS_SYSTEMUTILS_HPP
