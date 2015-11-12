# Specification of configuration files
This file extends the brief description of configuration files mentioned in `config-specification.yaml`.


## Modules
Arch Linux Installer consists of modules for easy future extension. The only required module is `devices`.


### Module `general` <sup><sub><sup>`optional`</sup></sub></sup>
The `general` module specifies general settings of the program.

**Properties:**
* debug: `bool` (default: `false`) <sup>`optional`</sup>

  Print all commands to `stdin` (and to a log file if specified) and do not execute them if `true`. This option can be useful if you want to see all commands that will be run.

* keepConfig: `bool` (default: `false`) <sup>`optional`</sup>

  Set to `true` if you want to keep your configuration file in the newly installed system. It will be saved to `/root/archlinux-installer/config.yaml`.

* keepProgram: `bool` (default: `false`) <sup>`optional`</sup>

  Set to `true` if you want to keep this program (Arch Linux Installer) in the newly installed system. It will be saved to `/root/archlinux-installer/archlinux-installer`.


### Module `logging` <sup><sub><sup>`optional`</sup></sub></sup>
This module enables logging of all run commands. The log file can be found in `/root/archlinux-installer/log-archlinux-installer.txt` (unless a different `path` is specified).

**Properties:**
* log: `bool` (default: `false`) <sup>`optional`</sup>

  Set to `true` to enable logging.

* path: `string` (default: `/root/archlinux-installer/log-archlinux-installer.txt`) <sup>`optional`</sup>

  Change the path of log file. You can save it to your account.


### Module `devices` <sup><sub><sup>`required`</sup></sub></sup>
This module formats selected devices. You can specify more devices in a sequence.

**Properties:**
* **\- device**: `map` <sup>`required`</sup>

  * name: `string` <sup>`optional`</sup>

    Name of the device. It is used only for printing.

  * **path**: `string` <sup>`required`</sup>

    Path of the device in system (e.g. `/dev/sda/`).

  * erase: `string` (default: `none`) <sup>`optional`</sup>

    Erase mode for creating partitions. `none` only deletes the current partition table and create a new one, `zero` fills the whole device with `/dev/zero`, and `random` fills the device with `/dev/random/`.

  * **partitions**: `sequence<partition>` <sup>`required`</sup>

    * **\- partition**: `map` <sup>`required`</sup>

      * name: `string` <sup>`optional`</sup>

        Name of the partition. It is used only for printing.

      * **size**: `string` <sup>`required`</sup>

        Size of the partition. You can specify the size with unit (e.g. `100M`, `50G`), with percent (e.g. `105%RAM`), or with keywords (e.g. `rest`).

      * **type**: `string` <sup>`required`</sup>

        Type of the partition (e.g. `ef00`, `8300`).

      * filesystem: `string` <sup>`optional`</sup>

        Filesystem to create (e.g. `fat`, `ext4`), or `swap` for the swap partition.

      * mount: `string` <sup>`optional`</sup>

        Full path of a mount point (e.g. `/boot/efi`). This option is optional but one partition (or volume) needs to have the root mount point (`/`).

      * encryption: `map` <sup>`optional`</sup>

        Encrypt this partition with LVM on LUKS. `cryptsetup` utility is used to encrypt the partition.

        * **name**: `string` <sup>`required`</sup>

          Name of the physical volume (e.g. `lvm_pv`). It is used for `pvcreate /dev/mapper/<name>`.

        * hash: `string` <sup>`optional`</sup>

          Specifies the hash used in the LUKS key setup scheme and volume key digest for luksFormat (e.g. `sha512`). It is the `--hash` parameter in the `cryptsetup` utility. IF it is not specified, the default `--hash` option of the `cryptsetup` utility is used.

        * keySize: `int` <sup>`optional`</sup>

          Sets key size in bits (e.g. `512`). The argument has to be a multiple of 8. The possible key-sizes  are limited by the cipher and mode used. It is the `--key-size` parameter in the `cryptsetup` utility. If it is not specified, the default `--key-size` option of the `cryptsetup` utility is used.

        * iterTime: `int` <sup>`optional`</sup>

          The number of milliseconds to spend with PBKDF2 passphrase processing (e.g. `1000`). It is the `--iter-time` parameter in the `cryptsetup` utility. If it is not specified, the default `--iter-time` option of the `cryptsetup` utility is used. Specifying 0 as parameter selects the compiled-in default (of `cryptsetup`).

        * **volumeGroups**: `sequence<volumeGroup>` <sup>`required`</sup>

          * **\- volumeGroup**: `map` <sup>`required`</sup>

            * **name**: `string` <sup>`required`</sup>

              Name of the volume group (e.g. `lvm_vg`)

            * **volumes**: `sequence<volume>` <sup>`required`</sup>

              * **\- volume**: `map` <sup>`required`</sup>

                * **name**: `string` <sup>`required`</sup>

                  Name of the logical volume (e.g. "root", "swap").

                * **size**: `string` <sup>`required`</sup>

                  Same as the `size` parameter of `partition`.

                * filesystem: `string` <sup>`optional`</sup>

                  Same as the `filesystem` parameter of `partition`.

                 * mount: `string` <sup>`optional`</sup>

                   Same as the `mount` parameter of `partition`.

        * sshDecrypt: `map` <sup>`optional`</sup>

          * **ip**: `string` <sup>`required`</sup>

            The `ip` parameter of GRUB (e.g. `:::::eth0:dhcp`).

          * **network**: `string` <sup>`required`</sup>

            A `mkinitcpio` hook that configures a network interface, such as `netconf` or `ppp`.

          * **sshServer**: `string` <sup>`required`</sup>

            An SSH server in initrd. You have the option of using either `dropbear` or `tinyssh`.

          * **sshKey**: `string` <sup>`required`</sup>

            A path to the SSH public key that will be used to decrypt the partition over SSH. It can be a local path (if you downloaded your key before installation) or http(s) address where the key can be downloaded from.


### Module `settings` <sup><sub><sup>`optional`</sup></sub></sup>
User settings for keyboard, font, etc.

**Properties:**
* keyboard: `string` <sup>`optional`</sup>

  Set a keyboard layout for the console (e.g. `cz-qwertz`).

* font: `string` <sup>`optional`</sup>

  Set a console font (e.g. `Lat2-Terminus16`).

* locales: `sequence<string>` <sup>`optional`</sup>

  A list of locales (e.g. `[en_US.UTF-8 UTF-8, cs_CZ.UTF-8 UTF-8]`).

* lang: `string` <sup>`optional`</sup>

  The default language (e.g. `en_US.UTF-8`).

* timezone: `string` <sup>`optional`</sup>

  The default timezone (e.g. `Europe/Prague`).

* hostname: `string` <sup>`optional`</sup>

  The default hostname of your machine (e.g. `my-arch-machine`).


### Module `users` <sup><sub><sup>`optional`</sup></sub></sup>
This module specifies a list of additional users. The `root` user is created (and you will be asked for the password) even when you omit the `users` module.

**Properties:**
* \- user: `map` <sup>`optional`</sup>

  * **name**: `string` <sup>`required`</sup>

    Username of the user.

  * comment: `string` <sup>`optional`</sup>

    The `--comment` parameter of the `useradd` utility. It is generally a short description of the login, and is currently used as the field for the user's full name.

  * createHome: `bool` (default: `true`) <sup>`optional`</sup>

    The `--home` parameter of the `useradd` utility.

  * groups: `sequence<string>` <sup>`optional`</sup>

    The `--groups` parameter of the `useradd` utility (e.g. `[wheel, ssh, irc]`). A list of supplementary groups which the user is also a member of.

  * sshKey: `string` <sup>`optional`</sup>

    A path to the SSH public key that will be used to decrypt the partition over SSH. It can be a local path (if you downloaded your key before installation) or http(s) address where the key can be downloaded from.


### Module `pacman` <sup><sub><sup>`optional`</sup></sub></sup>
This module is used to install additional packages from the default repositories (using `pacman`) or from AUR repositories.

**Properties:**
* packages: `sequence<string>` <sup>`optional`</sup>

  A list of packages to install using `pacman` (e.g. `[vim-minimal, wget]`).

* aurPackages: `sequence<string>` <sup>`optional`</sup>

  A list of AUR packages to isntall. No AUR helper is required but you can install it (e.g. `[packer]`).


### Module `afterInstall` <sup><sub><sup>`optional`</sup></sub></sup>
The `afterInstall` module can be used to run any command you want when the installation is finished.

**Properties:**
* commands: `sequence<string>` <sup>`optional`</sup>

  A list of commands to run in newly installed system at the end of the installation.

