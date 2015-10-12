# Arch Linux Installer
[![Build Status](https://travis-ci.org/branoholy/archlinux-installer.svg?branch=master)](https://travis-ci.org/branoholy/archlinux-installer)

## archlinux-installer
*An unofficial installer for Arch Linux using a config file.*

`archlinux-installer` installs a clean and minimal Arch Linux installation. It 
supports BIOS/EFI and encrypted LVM.

## Download
You can download the source code and build `archlinux-installer` according to 
the [build instructions](#build) below or download a binary from the 
[Releases](https://github.com/branoholy/archlinux-installer/releases) page.

## Usage
1. Boot Arch Linux installation [image](https://www.archlinux.org/download/) 
(you can use [archlinux-usb-creator](https://github.com/branoholy/archlinux-usb-creator) 
to create a bootable USB).

2. Download a binary from the [Releases](https://github.com/branoholy/archlinux-installer/releases) 
page or [build](#build) your own binary.

3. Edit the [config](#config) file as you need.

4. Run `archlinux-installer`.

```bash
$ ./archlinux-installer <config-path>
```

### Config
Config files use [YAML](http://yaml.org/) syntax (see [examples](https://github.com/branoholy/archlinux-installer/tree/master/examples)). 
When writing your own config files, you can start with 
[config.yaml](https://github.com/branoholy/archlinux-installer/blob/master/examples/config.yaml) 
that contains all available options.

## Dependencies
There are no dependencies for running the binary release (all libraries are 
linked statically). Dependencies are necessary only for building.

`archlinux-installer` is using:

* [yaml-cpp](https://github.com/jbeder/yaml-cpp) 
library (version 0.5.1 or newer).

## Build
Make sure you have installed all [dependencies](#dependencies) before building.

```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

For faster build on a multicore processor, you can use:

```bash
$ cmake --build . -- -j4
```

Where the option `-j4` means you want to use 4 cores for building.

## Examples
Run `archlinux-installer` with a config file:
```bash
$ ./archlinux-installer path/to/config.yaml
```

## License
Arch Linux Installer is licensed under GNU GPL v3 (see 
[LICENSE](https://github.com/branoholy/archlinux-installer/blob/master/LICENSE) 
file).

