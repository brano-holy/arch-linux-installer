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

#ifndef ARCHLINUXINSTALLER_MODULES_LOGGING_LOGGING_HPP
#define ARCHLINUXINSTALLER_MODULES_LOGGING_LOGGING_HPP

#include <iostream>
#include <fstream>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include "archlinuxinstaller/modules/module.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace logging {

class Logging : public Module
{
private:
	typedef boost::iostreams::tee_device<std::ostream, std::ostream> TeeDevice;
	typedef boost::iostreams::stream<TeeDevice> TeeStream;

	std::string tempPath;
	std::ostream *tempCout;
	std::fstream *logFile;

	TeeDevice *logDevice;
	TeeStream *logStream;

	std::streambuf *cerrBuf;
	std::streambuf *clogBuf;

public:
	bool log;
	std::string path;

	Logging();
	virtual ~Logging();

	virtual bool decode(const YAML::Node& node);

	virtual bool runOutsideBefore(const std::function<UIT>& ui);
	virtual bool runOutsideAfter(const std::map<std::string, UserInputBase*>& userInputs, const std::function<UIT>& ui);
};

}}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::modules::logging::Logging>
{
	static bool decode(const Node& node, archlinuxinstaller::modules::logging::Logging& logging);
};

}

#endif // ARCHLINUXINSTALLER_MODULES_LOGGING_LOGGING_HPP
