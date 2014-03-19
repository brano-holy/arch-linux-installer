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

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include <boost/program_options.hpp>

#include "configuration/config.hpp"
#include "utils.hpp"

using namespace std;
using namespace configuration;
using namespace utils;

namespace po = boost::program_options;

int main(int argc, char **argv) {
	string programName = "arch-linux-installer";
	string programPath = string(argv[0]);

	po::options_description genOptions("General options");
	genOptions.add_options()
		("config,c", po::value<string>()->value_name("<path>"), "Specify path to config file.")
		("disk-device", po::value<string>()->value_name("<device>")->required(), "Specify disk device, where will be Arch Linux installed.")
		("efi", po::bool_switch(), "Use EFI.")
		("swap-size", po::value<double>()->value_name("<percent>")->default_value(1.05), "Specify size of swap partition according to RAM size (eg.: 1.05 means 105% of RAM size).")
		("erase-disk", po::bool_switch(), "Erase whole disk before installation (write zeros).")
		("help,h", po::bool_switch(), "Print this help.")
	;

	po::options_description envOptions("Enviroment options");
	envOptions.add_options()
		("keyboard", po::value<string>()->value_name("<layout>")->default_value("en-qwerty"), "Specify layout of keyboard.")
		("font", po::value<string>()->value_name("<font>")->default_value("Lat2-Terminus16"), "Specify font of console.")
		("locales", po::value<vector<string>>()->value_name("<locale locale...>")->multitoken(), "Specify locales \nEg.: \"en_US.UTF-8 UTF-8\" \"cs_CZ.UTF-8 UTF-8\"\nOr in config:\nlocales = en_US.UTF-8 UTF-8\nlocales = cs_CZ.UTF-8 UTF-8\n")
		("lang", po::value<string>()->value_name("<lang>")->required(), "Specify system language.")
		("hostname", po::value<string>()->value_name("<arg>")->required(), "Specify hostname.")
		("timezone", po::value<string>()->value_name("<arg>")->required(), "Specify timezone (eg.: Europe/Prague).")
	;

	po::options_description encryptOptions("Encrypted LVM options");
	encryptOptions.add_options()
		("enc-lvm", po::bool_switch(), "Encrypt whole drive and use LVM.")
		("enc-iters", po::value<int>()->value_name("<n>")->default_value(1000), "Number of iterations for generating key.")
		("lvm-vg-name", po::value<string>()->value_name("<arg>")->default_value("arch_vg"), "Specify name of LVM volume group.")
	;

	po::options_description hiddenOptions("Hidden options");
	hiddenOptions.add_options()
		("chrooted", po::bool_switch(), "This option is added, when installer is running in newly installed system.")
	;

	po::options_description cmdOptions;
	cmdOptions.
		add(genOptions).
		add(envOptions).
		add(encryptOptions)
	;

	po::options_description allOptions;
	allOptions.
		add(cmdOptions).
		add(hiddenOptions)
	;

	po::variables_map args;
	po::store(po::parse_command_line(argc, argv, allOptions), args);

	if(argc == 1 || args["help"].as<bool>())
	{
		cout << "Usage: " + programName + " [options]" << endl;
		cout << cmdOptions << endl;
		return 0;
	}

	string configPath;
	if(args.count("config"))
	{
		configPath = args["config"].as<string>();

		ifstream configFile(configPath);
		if(configFile.is_open())
		{
			po::store(po::parse_config_file(configFile, cmdOptions), args);
		}
		else
		{
			cout << "Cannot open config file \"" << configPath << "\"." << endl;
			return -1;
		}
	}

	po::notify(args);

	// ############################################
	string lvmPartition = args["disk-device"].as<string>() + "3";

	// ############################################
	if(!args["chrooted"].as<bool>())
	{
		cout << "Setting keyboard to '" << args["keyboard"].as<string>() << "'..." << endl;
		SystemUtils::csystem("loadkeys " + args["keyboard"].as<string>());

		cout << "Setting font to '" << args["font"].as<string>() << "'..." << endl;
		SystemUtils::csystem("setfont " + args["font"].as<string>() + " -m 8859-2");

		// ############################################
		if(args["erase-disk"].as<bool>())
		{
			cout << "Erasing " << args["disk-device"].as<string>() << "..." << endl;
			SystemUtils::csystem("dd if=/dev/zero of=" + args["disk-device"].as<string>() + " bs=1M");
		}
		else
		{
			cout << "Destroying old partition table..." << endl;
			SystemUtils::csystem("sgdisk -Z " + args["disk-device"].as<string>());
		}

		// ############################################
		string efiPartition;
		string biosPartition;

		cout << "Creating partitions (+FS) for ";
		if(args["efi"].as<bool>())
		{
			cout << "EFI mode..." << endl;

			efiPartition = args["disk-device"].as<string>() + "1";
			cout << "Creating EFI partition (" + efiPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 1:0:+100M -t 1:EF00 " + args["disk-device"].as<string>());

			cout << "Creating FS for EFI partition (" + efiPartition + ")..." << endl;
			SystemUtils::csystem("mkfs.fat " + efiPartition);
		}
		else
		{
			cout << "BIOS mode..." << endl;

			biosPartition = args["disk-device"].as<string>() + "1";
			cout << "Creating BIOS partition (" + biosPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 1:0:+2M -t 1:EF02 " + args["disk-device"].as<string>());
		}

		// ############################################
		char ramSizeUnit;
		int ramSize = SystemUtils::getRAMSize(ramSizeUnit);
		char swapSizeUnit = ramSizeUnit;
		int swapSize = SystemUtils::alignSize(ramSize * args["swap-size"].as<double>());

		cout << "Creating and mounting partitions (+FS) for ";
		if(args["enc-lvm"].as<bool>())
		{
			cout << "boot and encrypted LVM (root and swap)..." << endl;

			string bootPartition = args["disk-device"].as<string>() + "2";
			cout << "Creating boot partition (" + bootPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 2:0:+300M -t 2:8300 " + args["disk-device"].as<string>());
			cout << "Creating FS for boot partition (" + bootPartition + ")..." << endl;
			SystemUtils::csystem("mkfs.ext2 " + bootPartition);

			cout << "Creating root LVM partition (" + lvmPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 3:0:0 -t 3:8E00 " + args["disk-device"].as<string>());

			string diskMapperCrypt = lvmPartition.substr(lvmPartition.find_last_of("/") + 1) + "_crypt";
			cout << "Setting up encryption (" + diskMapperCrypt + ")..." << endl;

			cout << fixed << setprecision(2) << "Please wait few seconds to generate the key after pressing enter..." << endl;
			cout << "Keyboard layout: " << args["keyboard"].as<string>() << endl;
			SystemUtils::csystem("cryptsetup -y -q -i " + to_string(args["enc-iters"].as<int>()) + " -h sha512 -s 512 luksFormat " + lvmPartition);
			SystemUtils::csystem("cryptsetup open --type luks " + lvmPartition + " " + diskMapperCrypt);

			cout << "Creating root and swap logical volumes (RAM: " << ramSize << ramSizeUnit << ", SWAP: " << swapSize << swapSizeUnit << ")..." << endl;
			SystemUtils::csystem("pvcreate /dev/mapper/" + diskMapperCrypt);
			SystemUtils::csystem("vgcreate " + args["lvm-vg-name"].as<string>() + " /dev/mapper/" + diskMapperCrypt);
			SystemUtils::csystem("lvcreate -L " + to_string(swapSize) + swapSizeUnit + " " + args["lvm-vg-name"].as<string>() + " -n swap");
			SystemUtils::csystem("lvcreate -l 100%FREE " + args["lvm-vg-name"].as<string>() + " -n root");

			string swapVolume = "/dev/" + args["lvm-vg-name"].as<string>() + "/swap";
			cout << "Creating FS for swap volume (" + swapVolume + ")..." << endl;
			SystemUtils::csystem("mkswap " + swapVolume);

			string rootVolume = "/dev/" + args["lvm-vg-name"].as<string>() + "/root";
			cout << "Creating FS for root volume (" + rootVolume + ")..." << endl;
			SystemUtils::csystem("mkfs.ext4 " + rootVolume);

			cout << "Mounting root volume (" + rootVolume + " => /mnt)..." << endl;
			SystemUtils::csystem("mount " + rootVolume + " /mnt");

			cout << "Mounting boot partition (" + bootPartition + " => /mnt/boot)..." << endl;
			SystemUtils::csystem("mkdir /mnt/boot");
			SystemUtils::csystem("mount " + args["disk-device"].as<string>() + "2 /mnt/boot");

			if(args["efi"].as<bool>())
			{
				cout << "Mounting EFI partition (" + efiPartition + " => /mnt/boot/efi)..." << endl;
				SystemUtils::csystem("mkdir /mnt/boot/efi");
				SystemUtils::csystem("mount " + efiPartition + " /mnt/boot/efi");
			}

			cout << "Turning SWAP on (" + swapVolume + ")..." << endl;
			SystemUtils::csystem("swapon " + swapVolume);
		}
		else
		{
			cout << "root and swap..." << endl;

			string swapPartition = args["disk-device"].as<string>() + "2";
			cout << "Creating swap partition (" + swapPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 2:0:+" + to_string(swapSize) + swapSizeUnit + " -t 2:8300 " + args["disk-device"].as<string>());
			cout << "Creating FS for swap partition..." << endl;
			SystemUtils::csystem("mkswap " + args["disk-device"].as<string>() + "2");

			string rootPartition = args["disk-device"].as<string>() + "3";
			cout << "Creating root partition (" + rootPartition + ")..." << endl;
			SystemUtils::csystem("sgdisk -n 3:0:0 -t 3:8300 " + args["disk-device"].as<string>());
			cout << "Creating FS for root partition (" + rootPartition + ")..." << endl;
			SystemUtils::csystem("mkfs.ext4 " + rootPartition);

			cout << "Mounting root partition (" + rootPartition + " => /mnt)..." << endl;
			SystemUtils::csystem("mount " + rootPartition + " /mnt");

			if(args["efi"].as<bool>())
			{
				cout << "Mounting EFI partition (" + efiPartition + " => /mnt/boot/efi)..." << endl;
				SystemUtils::csystem("mkdir -p /mnt/boot/efi");
				SystemUtils::csystem("mount " + efiPartition + " /mnt/boot/efi");
			}

			cout << "Turning SWAP on (" + swapPartition + ")..." << endl;
			SystemUtils::csystem("swapon " + swapPartition);
		}

		// ############################################
		cout << "Install Arch Linux base..." << endl;
		SystemUtils::csystem("pacstrap /mnt base");

		cout << "Generate fstab..." << endl;
		SystemUtils::csystem("genfstab -U -p /mnt >> /mnt/etc/fstab");

		cout << "Chroot new installation..." << endl;
		SystemUtils::csystem("cp " + programPath + " /mnt/root/" + programName);
		if(args.count("config"))
		{
			SystemUtils::csystem("cp " + configPath + " /mnt/root/" + programName + ".conf");
		}

		SystemUtils::csystem("arch-chroot /mnt /root/" + programName + " -c /root/" + programName + ".conf --chrooted");

		SystemUtils::csystem("rm /mnt/root/" + programName + " /mnt/root/" + programName + ".conf");

		SystemUtils::csystem("umount -R /mnt");
		SystemUtils::csystem("reboot");
	}
	else
	{
		// Running in newly installed system.
		// ############################################
		vector<string> locales = args["locales"].as<vector<string>>();

		cout << "Set locales and lang..." << endl;
		for(size_t i = 0; i < locales.size(); i++)
		{
			SystemUtils::csystem("echo \"" + locales.at(i) + "\" >> /etc/locale.gen");
		}
		SystemUtils::csystem("locale-gen");

		// ############################################
		SystemUtils::csystem("echo LANG=" + args["lang"].as<string>() + " > /etc/locale.conf");
		SystemUtils::csystem("export LANG=" + args["lang"].as<string>());

		cout << "Setting keyboard to '" << args["keyboard"].as<string>() << "'..." << endl;
		SystemUtils::csystem("loadkeys " + args["keyboard"].as<string>());

		cout << "Setting font to '" << args["font"].as<string>() << "'..." << endl;
		SystemUtils::csystem("setfont " + args["font"].as<string>() + " -m 8859-2");

		SystemUtils::csystem("echo \"KEYMAP=" + args["keyboard"].as<string>() + "\nFONT=" + args["font"].as<string>() + "\" >> /etc/vconsole.conf");

		// ############################################
		cout << "Setting timezone..." << endl;
		SystemUtils::csystem("ln -s /usr/share/zoneinfo/" + args["timezone"].as<string>() + " /etc/localtime");

		cout << "Setting HW clock to UTC..." << endl;
		SystemUtils::csystem("hwclock --systohc --utc");

		// ############################################
		cout << "Enable wired network..." << endl;
		SystemUtils::csystem("systemctl enable dhcpcd.service");

		cout << "Setting hostname (" << args["hostname"].as<string>() << ")..." << endl;
		SystemUtils::csystem("echo " + args["hostname"].as<string>() + " > /etc/hostname");

		// ############################################
		if(args["enc-lvm"].as<bool>())
		{
			cout << "Create an initial ramdisk environment..." << endl;

			Config mkinitcpio("/etc/mkinitcpio.conf");
			// HOOKS="base udev autodetect modconf block filesystems keyboard fsck"
			mkinitcpio.setValue("HOOKS", "base udev autodetect modconf block keyboard keymap consolefont encrypt lvm2 filesystems fsck shutdown");
			mkinitcpio.save();

			SystemUtils::csystem("mkinitcpio -p linux");
		}

		// ############################################
		cout << "Setting root password..." << endl;
		SystemUtils::csystem("passwd");

		// ############################################
		cout << "Installing and configuring GRUB..." << endl;
		if(args["efi"].as<bool>())
		{
			SystemUtils::csystem("pacman -S --noconfirm grub dosfstools efibootmgr");
			SystemUtils::csystem("grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=grub --recheck");
		}
		else
		{
			SystemUtils::csystem("pacman -S --noconfirm grub");
			SystemUtils::csystem("grub-install --target=i386-pc --recheck " + args["disk-device"].as<string>());
		}

		if(args["enc-lvm"].as<bool>())
		{
			Config grubConfig("/etc/default/grub");
			grubConfig.setValue("GRUB_CMDLINE_LINUX", "cryptdevice=" + lvmPartition + ":" + args["lvm-vg-name"].as<string>());
			// grub.setValue("GRUB_DISABLE_SUBMENU", "y");
			grubConfig.save();
		}

		SystemUtils::csystem("chmod -x /etc/grub.d/10_linux");
		SystemUtils::csystem("grub-mkconfig -o /boot/grub/grub.cfg");
	}

	return 0;
}
