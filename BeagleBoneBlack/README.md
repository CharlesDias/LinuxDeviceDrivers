# Linux Device Drivers on BeagleBone Black

- [Host and target setup.](#host-and-target-setup)
- [Custom drivers.](#custom-drivers)

## Host and target setup

### Using a Docker container as a development environment

Start the docker container.

```console
docker run --rm -it -v ~/.gitconfig:/etc/gitconfig -v $(pwd):/home/ldd charlesdias/device_drivers_bbb:latest
```

### Setup a development environment on local machine

1. Install the packages on the host system
```console
sudo apt-get update
```
```console
sudo apt-get install build-essential lzop u-boot-tools net-tools bison flex libssl-dev libncurses5-dev libncursesw5-dev unzip chrpath xz-utils minicom wget git-core
```

2. Download the Linaro toolchain. Access the link https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/ and download the file name `gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz`

![Linaro toolchain](images/fig_1.png)

#### Toolchain installation and path settings

1. Extract the file `gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz` to the Workspace directory. 

2. Access the Home folder and open .bashrc file
```console
cd ~ && vi .bashrc
```

3. And add the export command below to the end of the file
```console
export PATH=~/Study/Udemy/LinuxDeviceDriver/downloads/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/:$PATH
```

4. Save and close the file. To update the modification type the command `source .bashrc` or pefome a logoff and login.

5. To validate if everything is right, type the command `arm` in the console and press the keyboard **TAB**. The output are expected
```console
charlesdias@charlesdias:~/Study/Udemy/LinuxDeviceDriver$ arm
arm2hpdl                           arm-linux-gnueabihf-gcc-7.5.0      arm-linux-gnueabihf-ld
arm-linux-gnueabihf-addr2line      arm-linux-gnueabihf-gcc-ar         arm-linux-gnueabihf-ld.bfd
arm-linux-gnueabihf-ar             arm-linux-gnueabihf-gcc-nm         arm-linux-gnueabihf-ld.gold
arm-linux-gnueabihf-as             arm-linux-gnueabihf-gcc-ranlib     arm-linux-gnueabihf-nm
arm-linux-gnueabihf-c++            arm-linux-gnueabihf-gcov           arm-linux-gnueabihf-objcopy
arm-linux-gnueabihf-c++filt        arm-linux-gnueabihf-gcov-dump      arm-linux-gnueabihf-objdump
arm-linux-gnueabihf-cpp            arm-linux-gnueabihf-gcov-tool      arm-linux-gnueabihf-ranlib
arm-linux-gnueabihf-dwp            arm-linux-gnueabihf-gdb            arm-linux-gnueabihf-readelf
arm-linux-gnueabihf-elfedit        arm-linux-gnueabihf-gdb-add-index  arm-linux-gnueabihf-size
arm-linux-gnueabihf-g++            arm-linux-gnueabihf-gfortran       arm-linux-gnueabihf-strings
arm-linux-gnueabihf-gcc            arm-linux-gnueabihf-gprof          arm-linux-gnueabihf-strip
charlesdias@charlesdias:~/Study/Udemy/LinuxDeviceDriver$ arm
```

### Target setup

1. Download the last Debian image for BeagleBone Black. Acess the link `https://beagleboard.org/latest-images` and check the title **Recommended Debian Images** and download, for example, the [AM335x 11.7 2023-09-02 4GB microSD IoT](https://files.beagle.cc/file/beagleboard-public-2021/images/am335x-debian-11.7-iot-armhf-2023-09-02-4gb.img.xz). Or we can use our own Linux image built through Yocto Project.

2. Open the balenaEtcher software.

3. Select the core-image-minimal-beaglebone.wic.xz file.

4. Select the USB flash driver and click on Flash button.

5. Once the process is complete, unmount and remove the SD card.

6. Insert the SD card into BBB.

7. Press the boot button (S2) and power up the BBB.

8. Release the S2 button after 2 to 5 seconds.

## Linux kernel compilation - Version 5.10

1. Clone the branch 5.10-rt.

```console
git clone -b 5.10-rt https://github.com/beagleboard/linux.git linux_bbb_5.10-rt
```

2. Access the linux_bbb_5.10-rt folder and run the docker image
<!-- ```console
sudo docker run -it -v $PWD:/home/ldd/ charlesdias/ubuntu-bbb:latest
``` -->

```console
docker run --rm -it -v ~/.gitconfig:/etc/gitconfig -v $(pwd):/home/ldd charlesdias/device_drivers_bbb:latest
```

3. If necessary, removes all the temporary folder, object files, images generated during the previous build.
```console
make ARCH=arm distclean
```

4. Creates a .config file by using default config file given by the vendor
```console
make ARCH=arm bb.org_defconfig
```

5. Run this command to update the .config file settings before compile.​
```console
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

6. Kernel source code compilation. This stage creates a kernel image "uImage" also all the device tree source files will be compiled, and dtbs will be generated. ​

```console
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage dtbs LOADADDR=0x80008000 -j8
```

7. Building and generating in-tree loadable(M) kernel modules(.ko)

```console
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j8
```

8. Extract the modules.

```console
mkdir -p ../../kernel_modules && make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=../../kernel_modules modules_install -j $nproc
```

**Attention: If it runs the `sudo make ARCH=arm modules_install` command, these module will be installed on the host machine into /lib/modules/<kernel_version> folder.**

## Update the boot images and modules in SD card

1. Connect the SD card to computer

2. Access the host console terminal and copy the uImage from linux_bbb_4.14/arch/boot to the boot partition

```console
cp BeagleBoneBlack/linux_source/linux_bbb_5.10-rt/arch/arm/boot/zImage /media/<user-name>/<boot-partition-name>/
```

3. Copy module files root partition

```console
sudo cp -a BeagleBoneBlack/kernel_modules/lib/modules/5.10.162/ /media/<user-name>/<root-partition-name>/lib/modules/5.10.162
```

4. Run the sync command

```console
sync
```

5. Removes the SD card and connect it to the BBB.

6. After login, run the command 

```console
uname -r
```

7. The expected result is

```console
root@beaglebone:~# uname -r
5.10.162
```

## Custom drivers

-[Example 001hello_world.](#example-001hello_world)

### Example 001hello_world

**Note.** The kernel and modules must be precompiled. See the section [Linux kernel compilation - Version 5.10.](#linux-kernel-compilation---version-510)

1. Access the BeagleBoneBlack folder and run the docker image

```console
docker run --rm -it -v ~/.gitconfig:/etc/gitconfig -v $(pwd):/home/ldd charlesdias/device_drivers_bbb:latest
```

2. Access the folder `BeagleBoneBlack/custom_drivers/001hello_world`.

3. Run the commands below do generate the main.ko file

```console
make clean && make
```

4. Check the file information

```console
file main.ko
```

The expected output is

```console
linuxdev@372b4e1b2ea5:/home/ldd/custom_drivers/001hello_world$ file main.ko 
main.ko: ELF 32-bit LSB relocatable, ARM, EABI5 version 1 (SYSV), BuildID[sha1]=f8149e92c5439a7d9511c5cbaae68d304c977778, not stripped
```

5. Get the information about the module

```console
modinfo main.ko
```

The expected output is

```console
linuxdev@372b4e1b2ea5:/home/ldd/custom_drivers/001hello_world$ modinfo main.ko 
filename:       /home/ldd/custom_drivers/001hello_world/main.ko
board:          Beaglebone Black Revision A5
description:    A simple hello world kernel module.
author:         Charles Dias
license:        GPL
depends:        
name:           main
vermagic:       5.10.162 SMP preempt_rt mod_unload modversions ARMv7 p2v8
```

6. Rum the command below to analyze sections of the kernel object file.

```console
arm-linux-gnueabihf-objdump -h main.ko
```

The expected output is

```console
linuxdev@372b4e1b2ea5:/home/ldd/custom_drivers/001hello_world$ arm-linux-gnueabihf-objdump -h main.ko

main.ko:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .note.gnu.build-id 00000024  00000000  00000000  00000034  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .text         00000000  00000000  00000000  00000058  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .init.text    00000018  00000000  00000000  00000058  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  3 .exit.text    0000000c  00000000  00000000  00000070  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  4 .ARM.extab.init.text 00000000  00000000  00000000  0000007c  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .ARM.exidx.init.text 00000008  00000000  00000000  0000007c  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  6 .ARM.extab.exit.text 00000000  00000000  00000000  00000084  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .ARM.exidx.exit.text 00000008  00000000  00000000  00000084  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  8 .modinfo      000000ca  00000000  00000000  0000008c  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 .rodata.str1.4 00000023  00000000  00000000  00000158  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 10 .note.Linux   00000018  00000000  00000000  0000017c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 __versions    000000c0  00000000  00000000  00000194  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 12 .data         00000000  00000000  00000000  00000254  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 13 .gnu.linkonce.this_module 00000240  00000000  00000000  00000280  2**6
                  CONTENTS, ALLOC, LOAD, RELOC, DATA, LINK_ONCE_DISCARD
 14 .plt          00000001  00000240  00000240  000004c0  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 15 .init.plt     00000001  00000241  00000241  000004c1  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 16 .bss          00000000  00000000  00000000  000004c2  2**0
                  ALLOC
 17 .note.GNU-stack 00000000  00000000  00000000  000004c2  2**0
                  CONTENTS, READONLY
 18 .comment      0000004a  00000000  00000000  000004c2  2**0
                  CONTENTS, READONLY
 19 .note.GNU-stack 00000000  00000000  00000000  0000050c  2**0
                  CONTENTS, READONLY
 20 .ARM.attributes 00000031  00000000  00000000  0000050c  2**0
                  CONTENTS, READONLY
```

The modules initialization function `hello_world_init` is placed in the section `.init.text`. And the module clean up function `hello_world_cleanup` is placed in the section `.exit.text`. `.data` section is zero because the kernel module does not contain any data.

7. Connect the Ethernet cable to BBB and send the module.

```console
scp main.ko root@<ip-address>:/home/root
```

8. Install the module

```console
sudo insmod main.ko
```

The expected output is

```console
root@beaglebone:~# insmod main.ko 
[  102.526514] main: loading out-of-tree module taints kernel.
root@beaglebone:~# [  102.527378] Hello World
```

9. Check the last dmesg messages by typing `dmesg | tail` or if you want to check just the last 5 kernel message, just run  `dmesg | tail -5`. The expected output is similar to
```console
root@beaglebone:~# dmesg | tail
[    6.012443] FAT-fs (mmcblk0p1): codepage cp437 not found
[    6.265657] udevd[192]: starting version 3.2.10
[    6.329632] udevd[193]: starting eudev-3.2.10
[    7.756813] EXT4-fs (mmcblk0p2): re-mounted. Opts: (null)
[   28.951280] cpsw 4a100000.ethernet: initializing cpsw version 1.12 (0)
[   29.016456] SMSC LAN8710/LAN8720 4a101000.mdio:00: attached PHY driver [SMSC LAN8710/LAN87)
[   31.071423] cpsw 4a100000.ethernet eth0: Link is Up - 100Mbps/Full - flow control off
[   31.071528] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
[  102.526514] main: loading out-of-tree module taints kernel.
[  102.527378] Hello World
```

10. Unistall the module by typing `sudo rmmod main`.

```console
root@beaglebone:~# rmmod main
[  121.300420] Good bye World!
```

Check the last dmesg messages by typing `dmesg | tail`. Observe the message `Good bye World`

```console
root@beaglebone:~# dmesg | tail
[    6.265657] udevd[192]: starting version 3.2.10
[    6.329632] udevd[193]: starting eudev-3.2.10
[    7.756813] EXT4-fs (mmcblk0p2): re-mounted. Opts: (null)
[   28.951280] cpsw 4a100000.ethernet: initializing cpsw version 1.12 (0)
[   29.016456] SMSC LAN8710/LAN8720 4a101000.mdio:00: attached PHY driver [SMSC LAN8710/LAN87)
[   31.071423] cpsw 4a100000.ethernet eth0: Link is Up - 100Mbps/Full - flow control off
[   31.071528] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
[  102.526514] main: loading out-of-tree module taints kernel.
[  102.527378] Hello World
[  121.300420] Good bye World!
```

