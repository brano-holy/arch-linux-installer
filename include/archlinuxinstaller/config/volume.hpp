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

#ifndef ARCHLINUXINSTALLER_CONFIG_VOLUME_HPP
#define ARCHLINUXINSTALLER_CONFIG_VOLUME_HPP

#include <string>
#include <experimental/optional>

#include <yaml-cpp/yaml.h>

namespace archlinuxinstaller {
namespace config {

class Device;
class VolumeGroup;

class Volume
{
private:
	std::string path;

public:
	std::string name;
	std::string size;
	std::experimental::optional<std::string> filesystem;
	std::experimental::optional<std::string> mountPoint;

	inline const std::string& getPath() const { return path; }
	inline bool isSwap() const { return (filesystem && *filesystem == "swap"); }

	virtual bool hasRoot() const;
	bool create(const VolumeGroup& volumeGroup) const;
	bool isMountable() const;
	bool mount() const;
	void fillMountables(std::vector<std::reference_wrapper<const Volume>>& mountables) const;

	friend struct YAML::convert<archlinuxinstaller::config::Device>;
	friend struct YAML::convert<archlinuxinstaller::config::VolumeGroup>;
};

}}

namespace YAML {

template<>
struct convert<archlinuxinstaller::config::Device>;

template<>
struct convert<archlinuxinstaller::config::VolumeGroup>;

template<>
struct convert<archlinuxinstaller::config::Volume>
{
	static bool decode(Node node, archlinuxinstaller::config::Volume& volume);
};

}

#endif // ARCHLINUXINSTALLER_CONFIG_VOLUME_HPP
