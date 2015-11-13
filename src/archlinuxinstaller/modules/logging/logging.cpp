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

#include "archlinuxinstaller/modules/logging/logging.hpp"

#include <boost/filesystem.hpp>

#include "archlinuxinstaller/utils/formatutils.hpp"

namespace archlinuxinstaller {
namespace modules {
namespace logging {

const double Logging::ORDER = 0.01;

Logging::Logging() :
	tempCout(nullptr), logFile(nullptr), logDevice(nullptr), logStream(nullptr)
{
}

Logging::~Logging()
{
	delete logStream;
	delete logDevice;
	delete logFile;
	delete tempCout;
}

bool Logging::runOutsideBefore(const std::function<UIT>&)
{
	tempPath = "log-" + utils::FormatUtils::formatNow("%Y%m%d-%H%M%S") + ".txt";

	delete logStream;
	delete logDevice;
	delete logFile;
	delete tempCout;

	tempCout = new std::ostream(std::cout.rdbuf());
	logFile = new std::fstream(tempPath, std::fstream::in | std::fstream::out | std::fstream::trunc);
	logDevice = new TeeDevice(*tempCout, *logFile);
	logStream = new TeeStream(*logDevice);

	std::cout.rdbuf(logStream->rdbuf());

	cerrBuf = std::cerr.rdbuf(logStream->rdbuf());
	clogBuf = std::clog.rdbuf(logStream->rdbuf());

	return true;
}

bool Logging::runOutsideAfter(const std::map<std::string, UserInputBase*>&, const std::function<UIT>&)
{
	std::cout.rdbuf(tempCout->rdbuf());
	std::cerr.rdbuf(cerrBuf);
	std::clog.rdbuf(clogBuf);
	logStream->close();

	logFile->seekg(std::fstream::beg);

	boost::filesystem::path logDir = boost::filesystem::path(path).parent_path();
	if(!logDir.empty()) boost::filesystem::create_directories(logDir);

	std::ofstream finalLogFile(path, std::ofstream::app);
	finalLogFile << logFile->rdbuf();

	bool status = !finalLogFile.fail();
	if(status)
	{
		logFile->close();
		status &= boost::filesystem::remove(tempPath);
	}

	return status;
}

}}}

namespace YAML {

bool convert<archlinuxinstaller::modules::logging::Logging>::decode(Node node, archlinuxinstaller::modules::logging::Logging& logging)
{
	using archlinuxinstaller::modules::Module;

	logging.log = node["log"].as<bool>(false);
	logging.path = node["path"].as<std::string>("/root/" + Module::PROGRAM_NAME + "/log-" + Module::PROGRAM_NAME + ".txt");

	return true;
}

}
