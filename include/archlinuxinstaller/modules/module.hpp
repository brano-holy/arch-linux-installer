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

#ifndef ARCHLINUXINSTALLER_MODULES_MODULE_HPP
#define ARCHLINUXINSTALLER_MODULES_MODULE_HPP

#include <yaml-cpp/yaml.h>

#include <functional>
#include <map>
#include <vector>

namespace archlinuxinstaller {
namespace modules {

class UserInputBase;

class Module
{
private:
	std::vector<UserInputBase*> userInputs;

protected:
	std::string tagName;
	double order;
	bool required;

	virtual void addUserInputs(std::vector<UserInputBase*>& userInputs) const;

public:
	using UIT = bool(const std::string&, bool);

	static std::string PROGRAM_NAME;

	Module(const std::string& tagName, double order, bool required = false);
	virtual ~Module();

	virtual inline std::string getTagName() const final { return tagName; }
	virtual inline double getOrder() const final { return order; }
	virtual inline bool isRequired() const final { return required; }

	virtual bool decode(const YAML::Node& node) = 0;

	virtual bool loadConfig(const YAML::Node& config);

	virtual std::vector<UserInputBase*> getUserInputs() final;

	virtual bool runOutsideBefore(const std::function<UIT>& ui);
	virtual bool runOutside(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);
	virtual bool runInsideBefore(const std::function<UIT>& ui);
	virtual bool runInside(const std::function<UIT>& ui);
	virtual bool runInsideAfter(const std::function<UIT>& ui);
	virtual bool runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);
};

}}

#endif // ARCHLINUXINSTALLER_MODULES_MODULE_HPP
