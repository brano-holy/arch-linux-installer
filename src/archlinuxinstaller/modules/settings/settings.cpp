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

#include "archlinuxinstaller/modules/settings/settings.hpp"

#include "archlinuxinstaller/utils/systemutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace settings {

Settings::Settings() : Module("settings", 0.2)
{
}

bool Settings::decode(const YAML::Node& node)
{
	return YAML::convert<Settings>::decode(node, *this);
}

bool Settings::runOutsideBefore(const std::function<UIT>& ui)
{
	bool status = true;

	if(keyboard) status &= ui("Setting keyboard (" + *keyboard + ')', setKeyboard());
	if(font) status &= ui("Setting font", setFont());

	return status;
}

bool Settings::runInside(const std::function<UIT>& ui)
{
	bool status = true;

	if(keyboard) status &= ui("Setting keyboard", setKeyboard(true));
	if(font) status &= ui("Setting font", setFont(true));
	if(!locales.empty()) status &= ui("Setting locales", setLocales());
	if(lang) status &= ui("Setting lang", setLang());
	if(timezone) status &= ui("Setting timezone", setTimezone());
	if(hostname) status &= ui("Setting hostname", setHostname());

	return status;
}

bool Settings::setKeyboard(bool permanent) const
{
	bool status = true;
	if(keyboard)
	{
		status = utils::SystemUtils::system("loadkeys " + *keyboard);
		if(permanent) status &= utils::SystemUtils::system("echo \"KEYMAP=" + *keyboard + "\" >> /etc/vconsole.conf");
	}

	return status;
}

bool Settings::setFont(bool permanent) const
{
	bool status = true;
	if(font)
	{
		status = utils::SystemUtils::system("setfont " + *font + " -m 8859-2");
		if(permanent) status &= utils::SystemUtils::system("echo \"FONT=" + *font + "\" >> /etc/vconsole.conf");
	}

	return status;
}

bool Settings::setLocales() const
{
	bool status = true;
	if(!locales.empty())
	{
		for(const std::string& locale : locales)
		{
			status &= utils::SystemUtils::system("echo \"" + locale + "\" >> /etc/locale.gen");
		}
		status &= utils::SystemUtils::system("locale-gen");
	}

	return status;
}

bool Settings::setLang() const
{
	bool status = true;
	if(lang)
	{
		status = utils::SystemUtils::system("echo LANG=" + *lang + " > /etc/locale.conf");
		status &= utils::SystemUtils::system("export LANG=" + *lang);
	}

	return status;
}

bool Settings::setTimezone() const
{
	if(timezone) return utils::SystemUtils::system("ln -s /usr/share/zoneinfo/" + *timezone + " /etc/localtime");
	return true;
}

bool Settings::setHostname() const
{
	if(hostname) return utils::SystemUtils::system("echo " + *hostname + " > /etc/hostname");
	return true;
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::settings::Settings>::decode(const Node& node, archlinuxinstaller::modules::settings::Settings& settings)
{
	if(node.IsMap())
	{
		if(node["keyboard"]) settings.keyboard = node["keyboard"].as<std::string>();
		if(node["font"]) settings.font = node["font"].as<std::string>();
		if(node["locales"]) settings.locales = node["locales"].as<std::vector<std::string>>();
		if(node["lang"]) settings.lang = node["lang"].as<std::string>();
		if(node["timezone"]) settings.timezone = node["timezone"].as<std::string>();
		if(node["hostname"]) settings.hostname = node["hostname"].as<std::string>();

		return true;
	}

	return false;
}

}
