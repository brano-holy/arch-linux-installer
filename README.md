Arch Linux Installer
====================

arch-linux-installer
--------------------
Installer for Arch Linux using config file.

The installer installs clean and minimal Arch Linux installation on whole disk. 
It supports BIOS/EFI and encrypted LVM.

Dependencies
------------
`arch-linux-installer` is using Boost.Program_options library (version 1.53 
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
Config files are using INI-like syntax (see examples). To show all possible 
options, run `./arch-linux-installer --help` or simply `./arch-linux-installer` 
without options.
