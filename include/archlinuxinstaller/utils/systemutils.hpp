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

#ifndef ARCHLINUXINSTALLER_UTILS_SYSTEMUTILS_HPP
#define ARCHLINUXINSTALLER_UTILS_SYSTEMUTILS_HPP

#include <string>

namespace archlinuxinstaller {
namespace utils {

	class SystemUtils
	{
	public:
		static bool DEBUG;

		static int csystem(const std::string& cmd);
		static std::string ssystem(const std::string& cmd, int bufferSize = 256);

		static int getRAMSize(char& unit);
		static int alignSize(int minSize, int aligment = 2048);

		static std::string getSizeByCommand(std::string size, const std::string& command);

		static void createFilesystem(const std::string& filesystem, const std::string& partition);

		static std::string readPassword(const std::string& passwordName);
	};

}}

#endif // ARCHLINUXINSTALLER_UTILS_SYSTEMUTILS_HPP
