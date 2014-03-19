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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>

using namespace std;

namespace utils
{
	class StringUtils
	{
		public:
			static bool startsWith(const string& str, const string& prefix);
			static vector<string> split(const string& str, char delim);
			static string join(char glue, const vector<string>& values);
			static string trim(const string& str);
	};
}

#endif // STRINGUTILS_H
