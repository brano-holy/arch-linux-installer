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

#include "archlinuxinstaller/configuration/config.hpp"

#include <fstream>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "archlinuxinstaller/configuration/textconfigline.hpp"
#include "archlinuxinstaller/configuration/varconfigline.hpp"

namespace archlinuxinstaller {
namespace configuration {

Config::Config()
{
}

Config::Config(const std::string& configPath)
{
	load(configPath);
}

bool Config::load(const std::string& configPath)
{
	std::ifstream file(configPath);

	if(!file.is_open()) return false;
	path = configPath;

	std::string line;
	while(std::getline(file, line))
	{
		std::string trimLine = boost::algorithm::trim_copy(line);
		std::size_t hashPos = trimLine.find('#');

		std::size_t signPos;
		if(hashPos == std::string::npos) signPos = trimLine.find("=\"");
		else
		{
			std::string varPart = trimLine.substr(0, hashPos);
			signPos = varPart.find("=\"");
		}

		ConfigLine *configLine;
		if(signPos != std::string::npos)
		{
			configLine = new VarConfigLine();
			varContent.push_back((VarConfigLine*)configLine);
		}
		else if(hashPos != std::string::npos) configLine = new CommentConfigLine();
		else configLine = new TextConfigLine();

		configLine->parseLine(line);
		content.push_back(configLine);
	}

	return true;
}

bool Config::save()
{
	return save(path);
}

bool Config::save(const std::string& configPath)
{
	std::ofstream file(configPath);

	if(!file.is_open()) return false;

	for(ConfigLine *line : content)
	{
		file << *line << std::endl;
	}

	return true;
}

VarConfigLine* Config::getVarConfigLine(const std::string& name)
{
	for(VarConfigLine *line : varContent)
	{
		if(line->getName() == name) return line;
	}

	return nullptr;
}

VarConfigLine* Config::getOrCreateVarConfigLine(const std::string& name)
{
	VarConfigLine *line = getVarConfigLine(name);
	if(line == nullptr)
	{
		line = new VarConfigLine(name);
		varContent.push_back(line);
		content.push_back(line);
	}

	return line;
}

std::string Config::getValue(const std::string& name)
{
	for(const VarConfigLine *line : varContent)
	{
		if(line->getName() == name) return line->getValue();
	}

	return "";
}

void Config::setValue(const std::string& name, const std::string& value)
{
	VarConfigLine *line = getOrCreateVarConfigLine(name);
	line->setValue(value);
}

void Config::setValue(const std::string& name, const std::string& value, const std::string& comment)
{
	VarConfigLine *line = getOrCreateVarConfigLine(name);
	line->setValue(value);
	line->setComment(comment);
}

std::vector<std::string> Config::getValues(const std::string& name)
{
	std::vector<std::string> values;
	std::string nameValue = getValue(name);

	return boost::algorithm::split(values, nameValue, boost::is_space());
}

void Config::setValues(const std::string& name, const std::vector<std::string>& values)
{
	setValue(name, boost::algorithm::join(values, " "));
}

void Config::setValues(const std::string& name, const std::vector<std::string>& values, const std::string& comment)
{
	setValue(name, boost::algorithm::join(values, " "), comment);
}

}}
