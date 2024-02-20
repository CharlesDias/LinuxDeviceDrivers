# Example 003pseudo_char_driver_multiple

**Note.** The kernel and modules must be precompiled. See the section [Linux kernel compilation - Version 5.10.](#linux-kernel-compilation---version-510)

1. Access the `LinuxDeviceDrivers/GitHub/BeagleBoneBlack` folder and run the docker image

```console
docker run --rm -it -v ~/.gitconfig:/etc/gitconfig -v $(pwd):/home/ldd charlesdias/device_drivers_bbb:latest
```

2. Access the folder `BeagleBoneBlack/custom_drivers/003pseudo_char_driver_multiple`.

3. Run the commands below do generate the pcd_n.ko file

```console
make clean && make
```

4. Check the file information
```console
file pcd_n.ko
```

The expected output is
```console
root@b6e7f65a3d50:/home/ldd/GitHub/custom_drivers/003pseudo_char_driver_multiple# file pcd_n.ko
pcd_n.ko: ELF 32-bit LSB relocatable, ARM, EABI5 version 1 (SYSV), BuildID[sha1]=6b1ebde68629217ec7513f786b9b779cf81a3a25, not stripped
```

5. Get the information about the module
```console
modinfo pcd_n.ko
```

The expected output is
```console
root@b6e7f65a3d50:/home/ldd/GitHub/custom_drivers/003pseudo_char_driver_multiple# modinfo pcd_n.ko
filename:       /home/ldd/GitHub/custom_drivers/003pseudo_char_driver_multiple/pcd_n.ko
description:    A pseudo character driver which handles N devices. 
author:         Charles Dias
license:        GPL
depends:        
name:           pcd_n
vermagic:       4.14.108 SMP preempt mod_unload modversions ARMv7 p2v8 
```

6. Rum the command below to analyze sections of the kernel object file.

```console
arm-linux-gnueabihf-objdump -h pcd_n.ko
```

The expected output is
```console
oot@b6e7f65a3d50:/home/ldd/GitHub/custom_drivers/003pseudo_char_driver_multiple# arm-linux-gnueabihf-objdump -h pcd_n.ko

pcd_n.ko:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .note.gnu.build-id 00000024  00000000  00000000  00000034  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .text         000004f0  00000000  00000000  00000058  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  2 .init.text    000001e8  00000000  00000000  00000548  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  3 .exit.text    00000070  00000000  00000000  00000730  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  4 .ARM.extab    00000048  00000000  00000000  000007a0  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .ARM.exidx    00000030  00000000  00000000  000007e8  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  6 .ARM.extab.init.text 0000000c  00000000  00000000  00000818  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .ARM.exidx.init.text 00000008  00000000  00000000  00000824  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  8 .ARM.extab.exit.text 0000000c  00000000  00000000  0000082c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 .ARM.exidx.exit.text 00000008  00000000  00000000  00000838  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
 10 .rodata       0000005f  00000000  00000000  00000840  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 .modinfo      000000b5  00000000  00000000  000008a0  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 12 .rodata.str1.4 00000381  00000000  00000000  00000958  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 13 __mcount_loc  00000018  00000000  00000000  00000cdc  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
 14 __versions    00000400  00000000  00000000  00000cf4  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 15 .data         000001bc  00000000  00000000  000010f4  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
 16 .gnu.linkonce.this_module 00000240  00000000  00000000  000012c0  2**6
                  CONTENTS, ALLOC, LOAD, RELOC, DATA, LINK_ONCE_DISCARD
 17 .plt          00000001  00000240  00000240  00001500  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 18 .init.plt     00000001  00000241  00000241  00001501  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 19 .bss          00000c00  00000000  00000000  00001504  2**2
                  ALLOC
 20 .comment      0000004a  00000000  00000000  00001504  2**0
                  CONTENTS, READONLY
 21 .note.GNU-stack 00000000  00000000  00000000  0000154e  2**0
                  CONTENTS, READONLY
 22 .ARM.attributes 00000031  00000000  00000000  0000154e  2**0
                  CONTENTS, READONLY
```

The modules initialization function `pcd_driver_init` is placed in the section `.init.text`. And the module clean up function `pcd_driver_cleanup` is placed in the section `.exit.text`.

7. Connect the Ethernet cable to BBB and send the module.
```console
scp pcd_n.ko debian@<ip-address>:/home/debian/drivers
```

8. Install the module
```console
sudo insmod pcd_n.ko
```

The expected output is
```console
debian@beaglebone:~/drivers$ sudo insmod pcd_n.ko                                                              
[sudo] password for debian: 
[ 1399.423221] pcd_n: loading out-of-tree module taints kernel.
[ 1399.439842] pcd_driver_init : Device number <major>:<minor> = 242:0
[ 1399.452026] pcd_driver_init : Device number <major>:<minor> = 242:1
[ 1399.468034] pcd_driver_init : Device number <major>:<minor> = 242:2
[ 1399.484011] pcd_driver_init : Device number <major>:<minor> = 242:3
[ 1399.498413] pcd_driver_init : Module init was successful
```

9. Check the last dmesg messages by typing `dmesg | tail` or if you want to check just the last 6 kernel message, just run  `dmesg | tail -6`. The expected output is similar to
```console
debian@beaglebone:~/drivers$ dmesg | tail -6
[ 1399.423221] pcd_n: loading out-of-tree module taints kernel.
[ 1399.439842] pcd_driver_init : Device number <major>:<minor> = 242:0
[ 1399.452026] pcd_driver_init : Device number <major>:<minor> = 242:1
[ 1399.468034] pcd_driver_init : Device number <major>:<minor> = 242:2
[ 1399.484011] pcd_driver_init : Device number <major>:<minor> = 242:3
[ 1399.498413] pcd_driver_init : Module init was successful

```

10. Unistall the module by typing
```console
sudo rmmod pcd_n
```

The expected output is
```console
debian@beaglebone:~/drivers$ sudo rmmod pcd_n
[ 1532.824024] pcd_driver_cleanup : module unloaded

```

Check the last dmesg messages by typing `dmesg | tail -5`. Observe the message `module unloaded` as the last message.

```console
debian@beaglebone:~/drivers$ dmesg | tail -5
[ 1399.452026] pcd_driver_init : Device number <major>:<minor> = 242:1
[ 1399.468034] pcd_driver_init : Device number <major>:<minor> = 242:2
[ 1399.484011] pcd_driver_init : Device number <major>:<minor> = 242:3
[ 1399.498413] pcd_driver_init : Module init was successful
[ 1532.824024] pcd_driver_cleanup : module unloaded
```

### Testing the PCD multiple

Be aware that :
- `/dev/pcdev-1` is `RDONLY`;
- `/dev/pcdev-2` is `WRONLY`;
- `/dev/pcdev-3` and `/dev/pcdev-4` are `RDWR`.

1. Connect the Ethernet cable to BBB and send the dev_read.c.
```console
scp dev_read.c debian@<ip-address>:/home/debian/drivers
```

2. Compile the code by typing
```console
gcc dev_read.c -o dev_read
```

3. Check the file information 
```console
file dev_read
```

The expected output is
```console
debian@beaglebone:~/drivers$ file dev_read 
dev_read: ELF 32-bit LSB pie executable, ARM, EABI5 version 1 (SYSV), dynamically linked, interpreter /lib/ld-d
```

4. Install the module if necessary
```console
sudo insmod pcd_n.ko
```

5. Make the login as root
```console
sudo -s
```

6. Write some data into `/dev/pcdev-3`

```console
echo "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum." > /dev/pcdev-3
```

7. Run the dev_read
```console
./dev_read 50
```

The expected output is
```console
root@beaglebone:/home/debian/drivers# ./dev_read 50
read requested = 50
open was successful
read 50 bytes of data 
total_read = 50
Lorem Ipsum is simply dummy text of the printing aroot@beaglebone:/home/debian/drivers# 
```
