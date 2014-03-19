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

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>
#include "configline.hpp"
#include "varconfigline.hpp"

using namespace std;

namespace configuration
{
	class Config
	{
		private:
			string path;

			vector<ConfigLine*> content;
			vector<VarConfigLine*> varContent;

		public:
			Config();
			Config(string configPath);

			~Config();

			bool load(string configPath);
			bool save();
			bool save(string configPath);

			string getValue(string name);
			void setValue(string name, string value);
			void addValue(string name, string value);
			void addValue(string name, string value, string comment);

			vector<string> getValues(string name);
			void setValues(string name, vector<string> values);
			void addValues(string name, vector<string> values);
			void addValues(string name, vector<string> values, string comment);
	};
}

#endif // CONFIG_H
