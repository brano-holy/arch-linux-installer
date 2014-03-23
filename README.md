Arch Linux Installer
====================

arch-linux-installer
--------------------
*Unofficial installer for Arch Linux using config file.*

`arch-linux-installer` installs clean and minimal Arch Linux installation 
on whole disk. It supports BIOS/EFI and encrypted LVM.

Download
--------
You can download source code and build `arch-linux-installer` according to build 
instruction below or download binaries from [Releases](https://github.com/brano-holy/arch-linux-installer/releases) 
page.

Usage
-----
1. Boot Arch Linux installation [image](https://www.archlinux.org/download/) (you 
can use [arch-linux-usb-creator](https://github.com/brano-holy/arch-linux-usb-creator) 
to create bootable USB).

2. Download binary from [Releases](https://github.com/brano-holy/arch-linux-installer/releases) 
page or build and download your own binary.

3. Edit config file as you need (you may omit this step if you specify options 
as command line arguments).

4. Run `arch-linux-installer`.

With config file:
```bash
$ ./arch-linux-installer -c path/to/config.conf
```

Or with command line arguments:
```bash
$ ./arch-linux-installer --disk-device /dev/sda --locales "en_US.UTF-8 UTF-8" --lang en_US --hostname my-arch --timezone Europe/Prague
```

Dependencies
------------
There are no dependencies for running binary release (all libraries are linked 
statically). Dependencies are necessary only for building.

`arch-linux-installer` is using:

* Boost.Program_options library (version 1.53 
or newer) - [http://www.boost.org/doc/libs/1\_53\_0/doc/html/program\_options.html](http://www.boost.org/doc/libs/1_53_0/doc/html/program_options.html).

Building
--------
Make sure you have installed all dependencies before building.

```bash
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

For faster build on multicore processor, you can use:

```bash
$ cmake --build . -- -j4
```

Where option `-j4` means you want to use 4 cores for building.

Config syntax
-------------
Config files are using INI-like syntax (see [examples](https://github.com/brano-holy/arch-linux-installer/tree/master/examples)).

To show all possible options, simply run `arch-linux-installer` without options.

License
-------
Arch Linux Installer is licensed under GNU GPL v3 (see 
[COPYING](https://github.com/brano-holy/arch-linux-installer/blob/master/COPYING) 
file).
