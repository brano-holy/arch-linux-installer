Arch Linux Installer
====================

arch-linux-installer
--------------------
*Installer for Arch Linux using config file.*

`arch-linux-installer` installs clean and minimal Arch Linux installation 
on whole disk. 
It supports BIOS/EFI and encrypted LVM.

Download
--------
You can download source code and build `arch-linux-installer` according to build 
instruction below or download binaries from [Release](https://github.com/brano-holy/arch-linux-installer/releases) 
page.

Dependencies
------------
There are no dependencies for running binary release (all libraries are linked 
staticly).

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

Config syntax
-------------
Config files are using INI-like syntax (see [examples](https://github.com/brano-holy/arch-linux-installer/tree/master/examples)).

To show all possible options, simply run `./arch-linux-installer` without options.
