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

#ifndef ARCHLINUXINSTALLER_CONFIGURATION_CONFIG_HPP
#define ARCHLINUXINSTALLER_CONFIGURATION_CONFIG_HPP

#include <string>
#include <vector>

namespace archlinuxinstaller {
namespace configuration {

	class ConfigLine;
	class VarConfigLine;

	class Config
	{
	private:
		std::string path;

		std::vector<ConfigLine*> content;
		std::vector<VarConfigLine*> varContent;

		VarConfigLine* getVarConfigLine(const std::string& name);
		VarConfigLine* getOrCreateVarConfigLine(const std::string& name);

	public:
		Config();
		Config(const std::string& configPath);

		bool load(const std::string& configPath);
		bool save();
		bool save(const std::string& configPath);

		std::string getValue(const std::string& name);
		void setValue(const std::string& name, const std::string& value);
		void setValue(const std::string& name, const std::string& value, const std::string& comment);

		std::vector<std::string> getValues(const std::string& name);
		void setValues(const std::string& name, const std::vector<std::string>& values);
		void setValues(const std::string& name, const std::vector<std::string>& values, const std::string& comment);
	};

}}

#endif // ARCHLINUXINSTALLER_CONFIGURATION_CONFIG_HPP
