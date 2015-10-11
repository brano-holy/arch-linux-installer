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

#ifndef ARCHLINUXINSTALLER_UTILS_STRINGUTILS_HPP
#define ARCHLINUXINSTALLER_UTILS_STRINGUTILS_HPP

#include <string>
#include <vector>

namespace archlinuxinstaller {
namespace utils {

class StringUtils
{
public:
	static bool startsWith(const std::string& str, const std::string& prefix);
	static std::vector<std::string> split(const std::string& str, char delim);

	template <typename ForwardIterator, typename GlueT>
	static std::string join(ForwardIterator begin, ForwardIterator end, GlueT glue);

	static std::string trim(const std::string& str, const std::string& whiteChars = " \n\r\t");
};

template <typename ForwardIterator, typename GlueT>
std::string StringUtils::join(ForwardIterator begin, ForwardIterator end, GlueT glue)
{
	std::string str;
	if(begin != end) str = *begin;

	for(auto it = begin + 1; it != end; it++)
	{
		str += glue;
		str += *it;
	}

	return str;
}

}}

#endif // ARCHLINUXINSTALLER_UTILS_STRINGUTILS_HPP
