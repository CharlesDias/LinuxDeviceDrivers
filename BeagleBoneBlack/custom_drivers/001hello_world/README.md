# Example 001hello_world

**Note.** The kernel and modules must be precompiled. See the section [Linux kernel compilation - Version 5.10.](#linux-kernel-compilation---version-510)

1. Access the `LinuxDeviceDrivers/GitHub/BeagleBoneBlack` folder and run the docker image

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

