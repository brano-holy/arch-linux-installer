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

#ifndef ARCHLINUXINSTALLER_CONFIG_SETTINGS_HPP
#define ARCHLINUXINSTALLER_CONFIG_SETTINGS_HPP

#include <string>
#include <experimental/optional>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace archlinuxinstaller {
namespace config {

class Settings
{
public:
	std::experimental::optional<std::string> keyboard;
	std::experimental::optional<std::string> font;
	std::vector<std::string> locales;
	std::experimental::optional<std::string> lang;
	std::experimental::optional<std::string> timezone;
	std::experimental::optional<std::string> hostname;

	bool setKeyboard(bool permanent = false) const;
	bool setFont(bool permanent = false) const;
	bool setLocales() const;
	bool setLang() const;
	bool setTimezone() const;
	bool setHostname() const;
};

}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::config::Settings>
{
	static bool decode(const Node& node, archlinuxinstaller::config::Settings& settings);
};

}

#endif // ARCHLINUXINSTALLER_CONFIG_SETTINGS_HPP
