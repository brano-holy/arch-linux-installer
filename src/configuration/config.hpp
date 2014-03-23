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

#ifndef CONFIGURATION_CONFIG_HPP
#define CONFIGURATION_CONFIG_HPP

#include <vector>
#include <string>

#include "configline.hpp"
#include "varconfigline.hpp"

namespace configuration {
	class Config
	{
		private:
			std::string path;

			std::vector<ConfigLine*> content;
			std::vector<VarConfigLine*> varContent;

		public:
			Config();
			Config(std::string configPath);

			~Config();

			bool load(std::string configPath);
			bool save();
			bool save(std::string configPath);

			std::string getValue(std::string name);
			void setValue(std::string name, std::string value);
			void addValue(std::string name, std::string value);
			void addValue(std::string name, std::string value, std::string comment);

			std::vector<std::string> getValues(std::string name);
			void setValues(std::string name, std::vector<std::string> values);
			void addValues(std::string name, std::vector<std::string> values);
			void addValues(std::string name, std::vector<std::string> values, std::string comment);
	};
}

#endif // CONFIGURATION_CONFIG_HPP
