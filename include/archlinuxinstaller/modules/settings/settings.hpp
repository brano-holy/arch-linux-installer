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

#ifndef ARCHLINUXINSTALLER_MODULES_SETTINGS_SETTINGS_HPP
#define ARCHLINUXINSTALLER_MODULES_SETTINGS_SETTINGS_HPP

#include "archlinuxinstaller/modules/module.hpp"

#include <experimental/optional>

namespace archlinuxinstaller {
namespace modules {
namespace settings {

class Settings : public Module
{
public:
	static const double ORDER;

	std::experimental::optional<std::string> keyboard;
	std::experimental::optional<std::string> font;
	std::vector<std::string> locales;
	std::experimental::optional<std::string> lang;
	std::experimental::optional<std::string> timezone;
	std::experimental::optional<std::string> hostname;

	virtual inline double getOrder() const { return ORDER; }
	virtual bool runOutsideBefore(const std::function<UIT>& ui);
	virtual bool runInside(const std::function<UIT>& ui);

	bool setKeyboard(bool permanent = false) const;
	bool setFont(bool permanent = false) const;
	bool setLocales() const;
	bool setLang() const;
	bool setTimezone() const;
	bool setHostname() const;
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::settings::Settings>
{
	static bool decode(Node node, archlinuxinstaller::modules::settings::Settings& settings);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_SETTINGS_SETTINGS_HPP
