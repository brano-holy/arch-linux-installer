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

#ifndef ARCHLINUXINSTALLER_MODULES_USERINPUT_HPP
#define ARCHLINUXINSTALLER_MODULES_USERINPUT_HPP

#include <iostream>
#include <limits>

namespace archlinuxinstaller {
namespace modules {

enum UserInputType { Text, Password };

class UserInputBase
{
protected:
	std::string key;
	std::string name;
	UserInputType type;

public:
	UserInputBase(const std::string& key, const std::string& name, UserInputType type);
	virtual ~UserInputBase();

	inline const std::string& getKey() const { return key; }
	inline const std::string& getName() const { return name; }
	inline UserInputType getType() const { return type; }

	virtual std::istream& read(std::istream& in) = 0;
	virtual std::ostream& write(std::ostream& out) const = 0;

	friend std::istream& operator>>(std::istream& in, UserInputBase& userInput);
	friend std::ostream& operator<<(std::ostream& out, const UserInputBase& userInput);
};

template<typename T>
class UserInput : public UserInputBase
{
protected:
	T value;

public:
	UserInput(const std::string& name, UserInputType type);
	UserInput(const std::string& key, const std::string& name, UserInputType type);

	inline const T& getValue() const { return value; }

	virtual std::istream& read(std::istream& in);
	virtual std::ostream& write(std::ostream& out) const;
};

template<typename T>
UserInput<T>::UserInput(const std::string& name, UserInputType type) : UserInputBase(name, name, type)
{
}

template<typename T>
UserInput<T>::UserInput(const std::string& key, const std::string& name, UserInputType type) : UserInputBase(key, name, type)
{
}

template<typename T>
std::istream& UserInput<T>::read(std::istream& in)
{
	in >> value;
	in.clear();
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return in;
}

template<typename T>
std::ostream& UserInput<T>::write(std::ostream& out) const
{
	return out << value;
}

}}

#endif // ARCHLINUXINSTALLER_MODULES_USERINPUT_HPP
