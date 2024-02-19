# Example 002pseudo_char_driver

**Note.** The kernel and modules must be precompiled. See the section [Linux kernel compilation - Version 5.10.](#linux-kernel-compilation---version-510)

1. Access the `LinuxDeviceDrivers/GitHub/BeagleBoneBlack` folder and run the docker image

```console
docker run --rm -it -v ~/.gitconfig:/etc/gitconfig -v $(pwd):/home/ldd charlesdias/device_drivers_bbb:latest
```

2. Access the folder `BeagleBoneBlack/custom_drivers/002pseudo_char_driver`.

3. Run the commands below do generate the pcd.ko file

```console
make clean && make
```

4. Check the file information

```console
file pcd.ko
```

The expected output is

```console
linuxdev@24b6a0b5265a:/home/ldd/custom_drivers/002pseudo_char_driver$ file pcd.ko 
pcd.ko: ELF 32-bit LSB relocatable, ARM, EABI5 version 1 (SYSV), BuildID[sha1]=61d32828e2bacd257b3fb2dbc0c55047ada14cde, not stripped
```

5. Get the information about the module

```console
modinfo pcd.ko
```

The expected output is

```console
linuxdev@24b6a0b5265a:/home/ldd/custom_drivers/002pseudo_char_driver$ modinfo pcd.ko 
filename:       /home/ldd/custom_drivers/002pseudo_char_driver/pcd.ko
board:          BeagleBone Black Revision A5
description:    A pseudo character driver
author:         Charles Dias
license:        GPL
depends:        
name:           pcd
vermagic:       5.10.162+ SMP preempt_rt mod_unload modversions ARMv7 p2v8 
```

6. Rum the command below to analyze sections of the kernel object file.

```console
arm-linux-gnueabihf-objdump -h pcd.ko
```

The expected output is

```console
linuxdev@24b6a0b5265a:/home/ldd/custom_drivers/002pseudo_char_driver$ arm-linux-gnueabihf-objdump -h pcd.ko

pcd.ko:     file format elf32-littlearm

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .note.gnu.build-id 00000024  00000000  00000000  00000034  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .text         000003f0  00000000  00000000  00000058  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  2 .init.text    00000188  00000000  00000000  00000448  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  3 .exit.text    0000004c  00000000  00000000  000005d0  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  4 .ARM.extab    00000000  00000000  00000000  0000061c  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  5 .ARM.exidx    00000028  00000000  00000000  0000061c  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  6 .ARM.extab.init.text 00000000  00000000  00000000  00000644  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  7 .ARM.exidx.init.text 00000008  00000000  00000000  00000644  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
  8 .ARM.extab.exit.text 00000000  00000000  00000000  0000064c  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  9 .ARM.exidx.exit.text 00000008  00000000  00000000  0000064c  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA
 10 .rodata       0000005f  00000000  00000000  00000654  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 11 .modinfo      000000c0  00000000  00000000  000006b3  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 12 .rodata.str1.4 0000039c  00000000  00000000  00000774  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 13 .note.Linux   00000018  00000000  00000000  00000b10  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 14 __versions    00000440  00000000  00000000  00000b28  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
 15 .data         00000088  00000000  00000000  00000f68  2**2
                  CONTENTS, ALLOC, LOAD, RELOC, DATA
 16 .data.once    00000001  00000000  00000000  00000ff0  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 17 .gnu.linkonce.this_module 00000240  00000000  00000000  00001000  2**6
                  CONTENTS, ALLOC, LOAD, RELOC, DATA, LINK_ONCE_DISCARD
 18 .plt          00000001  00000240  00000240  00001240  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 19 .init.plt     00000001  00000241  00000241  00001241  2**0
                  CONTENTS, ALLOC, LOAD, DATA
 20 .bss          00000248  00000000  00000000  00001244  2**2
                  ALLOC
 21 .note.GNU-stack 00000000  00000000  00000000  00001244  2**0
                  CONTENTS, READONLY
 22 .comment      0000004a  00000000  00000000  00001244  2**0
                  CONTENTS, READONLY
 23 .note.GNU-stack 00000000  00000000  00000000  0000128e  2**0
                  CONTENTS, READONLY
 24 .ARM.attributes 00000031  00000000  00000000  0000128e  2**0
                  CONTENTS, READONLY
```

The module's initialization function, `pcd_driver_init`, is located in the `.init.text` section. Meanwhile, the module's cleanup function, `pcd_driver_cleanup`, is found in the `.exit.text` section. Note that the `.data` section is no longer zero (136 bytes or 0x88 in hexadecimal) once the kernel module contains some data.

7. Check the symbol table of the object `pcd.ko`. This can be useful for debugging or understanding the structure of a compiled program. The symbol table includes information such as:

- Function names
- Global variables
- Constants
- Other identifiers used in your program

```console
nm pcd.ko
```

The expected output is

```console
linuxdev@24b6a0b5265a:/home/ldd/custom_drivers/002pseudo_char_driver$ nm pcd.ko 
00000000 t $a
00000040 t $a
000001d4 t $a
00000298 t $a
00000000 t $a
00000000 t $a
00000000 r $d
0000003c t $d
000001d0 t $d
00000294 t $d
000003ec t $d
00000184 t $d
00000000 r $d
00000048 t $d
00000000 r $d
00000000 r $d
00000000 d $d
00000000 b $d
00000000 d $d
00000000 r $d
00000000 d $d
00000000 r $d
00000000 r $d
00000000 r $d
00000069 r $d
00000000 r .LANCHOR0
00000000 b .LANCHOR1
00000000 d .LANCHOR2
00000000 d .LANCHOR3
00000000 r .LC0
00000028 r .LC1
000001e0 r .LC10
00000204 r .LC11
00000250 r .LC12
00000278 r .LC13
00000284 r .LC14
000002a0 r .LC15
000002d0 r .LC16
000002dc r .LC17
000002fc r .LC18
00000314 r .LC19
00000050 r .LC2
00000318 r .LC20
00000338 r .LC21
0000035c r .LC22
00000378 r .LC23
0000008c r .LC3
000000b0 r .LC4
000000d0 r .LC5
0000011c r .LC6
00000144 r .LC7
00000180 r .LC8
000001a4 r .LC9
00000049 r __UNIQUE_ID_author208
00000000 r __UNIQUE_ID_board210
00000069 r __UNIQUE_ID_depends113
00000023 r __UNIQUE_ID_description209
0000005d r __UNIQUE_ID_license207
00000072 r __UNIQUE_ID_name112
0000007b r __UNIQUE_ID_vermagic111
00000000 r ____versions
         U __aeabi_unwind_cpp_pr0
         U __check_object_size
         U __class_create
00000024 r __func__.28254
00000030 r __func__.28266
00000018 r __func__.28273
00000000 r __func__.28278
0000000c r __func__.28283
0000003c r __func__.28291
0000004c r __func__.28301
00000204 b __key.28293
00000000 D __this_module
00000000 d __warned.8244
00000000 r _note_7
         U alloc_chrdev_region
         U arm_copy_from_user
         U arm_copy_to_user
         U cdev_add
         U cdev_del
         U cdev_init
         U class_destroy
00000204 B class_pcd
00000000 T cleanup_module
00000000 B device_buffer
         U device_create
         U device_destroy
00000200 B device_number
00000244 B device_pcd
00000000 T init_module
         U memset
00000208 B pcd_cdev
00000000 t pcd_driver_cleanup
00000000 t pcd_driver_init
00000000 D pcd_fops
000001d4 T pcd_llseek
00000000 T pcd_open
00000298 T pcd_read
00000020 T pcd_release
00000040 T pcd_write
         U printk
         U unregister_chrdev_region
         U warn_slowpath_fmt
```

If you want to see more information, such as the size of the symbols and the section they belong to, you can use the -S and -t options:

```console
nm -S -t d pcd.ko
```

The expected output is

```console
linuxdev@24b6a0b5265a:/home/ldd/custom_drivers/002pseudo_char_driver$ nm -S -t d pcd.ko
00000000 t $a
00000064 t $a
00000468 t $a
00000664 t $a
00000000 t $a
00000000 t $a
00000000 r $d
00000060 t $d
00000464 t $d
00000660 t $d
00001004 t $d
00000388 t $d
00000000 r $d
00000072 t $d
00000000 r $d
00000000 r $d
00000000 d $d
00000000 b $d
00000000 d $d
00000000 r $d
00000000 d $d
00000000 r $d
00000000 r $d
00000000 r $d
00000105 r $d
00000000 r .LANCHOR0
00000000 b .LANCHOR1
00000000 d .LANCHOR2
00000000 d .LANCHOR3
00000000 r .LC0
00000040 r .LC1
00000480 r .LC10
00000516 r .LC11
00000592 r .LC12
00000632 r .LC13
00000644 r .LC14
00000672 r .LC15
00000720 r .LC16
00000732 r .LC17
00000764 r .LC18
00000788 r .LC19
00000080 r .LC2
00000792 r .LC20
00000824 r .LC21
00000860 r .LC22
00000888 r .LC23
00000140 r .LC3
00000176 r .LC4
00000208 r .LC5
00000284 r .LC6
00000324 r .LC7
00000384 r .LC8
00000420 r .LC9
00000073 00000020 r __UNIQUE_ID_author208
00000000 00000035 r __UNIQUE_ID_board210
00000105 00000009 r __UNIQUE_ID_depends113
00000035 00000038 r __UNIQUE_ID_description209
00000093 00000012 r __UNIQUE_ID_license207
00000114 00000009 r __UNIQUE_ID_name112
00000123 00000069 r __UNIQUE_ID_vermagic111
00000000 00001088 r ____versions
         U __aeabi_unwind_cpp_pr0
         U __check_object_size
         U __class_create
00000036 00000011 r __func__.28254
00000048 00000009 r __func__.28266
00000024 00000010 r __func__.28273
00000000 00000009 r __func__.28278
00000012 00000012 r __func__.28283
00000060 00000016 r __func__.28291
00000076 00000019 r __func__.28301
00000516 b __key.28293
00000000 00000576 D __this_module
00000000 00000001 d __warned.8244
00000000 00000024 r _note_7
         U alloc_chrdev_region
         U arm_copy_from_user
         U arm_copy_to_user
         U cdev_add
         U cdev_del
         U cdev_init
         U class_destroy
00000516 00000004 B class_pcd
00000000 00000076 T cleanup_module
00000000 00000512 B device_buffer
         U device_create
         U device_destroy
00000512 00000004 B device_number
00000580 00000004 B device_pcd
00000000 00000392 T init_module
         U memset
00000520 00000060 B pcd_cdev
00000000 00000076 t pcd_driver_cleanup
00000000 00000392 t pcd_driver_init
00000000 00000136 D pcd_fops
00000468 00000196 T pcd_llseek
00000000 00000032 T pcd_open
00000664 00000344 T pcd_read
00000032 00000032 T pcd_release
00000064 00000404 T pcd_write
         U printk
         U unregister_chrdev_region
         U warn_slowpath_fmt
```

The output you're seeing is from the `nm` command, which displays the symbol table of an object file. 

- The first column is the symbol's value. For functions and variables, this is the address where the symbol is located. For relocatable files like this one, the value is the offset from the start of the section that contains the symbol.
- The second column, when present, is the size of the symbol. This is displayed because the -S option was used with the nm command.
- The third column is the symbol's type. It can be one of many values, but the most common are:
  - t or T for text (code) symbols
  - d or D for data symbols
  - b or B for uninitialized data symbols
  - r or R for read-only data symbols
  - U for undefined symbols The lowercase letters mean the symbol is local (not visible outside the object file where it's defined), and the uppercase letters mean the symbol is global (visible to other object files).
- The fourth column is the symbol's name.

In the output, we can see several symbols related to the `pcd` (pseudo character driver) module, such as `pcd_driver_init`, `pcd_driver_cleanup`, `pcd_open`, `pcd_read`, `pcd_write`, and `pcd_release`. These are likely the functions that implement the character driver's operations. There are also symbols for variables like `device_buffer`, `device_number`, and `pcd_cdev`.

The symbols starting with `__UNIQUE_ID_` are metadata about the module, such as its author, description, and license. The symbols starting with `__func__` are compiler-generated symbols for function names. The symbols starting with . are likely compiler-generated symbols for string literals or other constants.

The symbols marked with `U` are undefined: they're used in this module but defined elsewhere. These are typically functions from the Linux kernel or other modules that this module calls.

8. Connect the Ethernet cable to BBB and send the module.

```console
scp pcd.ko root@<ip-address>:/home/root
```

9. Install the module

```console
insmod pcd.ko
```

10. Check the last dmesg messages by typing `dmesg | tail` or if you want to check just the last 5 kernel message, just run  `dmesg | tail -5`. The expected output is similar to

```console
root@beaglebone:~# dmesg | tail -5
[   10.915484] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready
[ 3165.869219] pcd: loading out-of-tree module taints kernel.
[ 3165.870039] pcd_driver_init : Pseudo Char Driver Initialized
[ 3165.870059] pcd_driver_init : Device number <major>:<minor> = 239:0
[ 3165.882814] pcd_driver_init : Module init was successful.
```

The <major> number can change from install to install.

11. Write some data on the `pcd` device.

```console
echo "Write some data to test the pcd driver." > /dev/pcd 
```

And check the dmesg.

```console
dmesg
. . . 
[ 3475.781283] pcd_open : Open operation was successful
[ 3475.781458] pcd_write : Write requested with count = 40 and f_pos = 0
[ 3475.781480] pcd_write : Number of bytes successfully written = 40 and updated f_pos = 40
[ 3475.781505] pcd_release : Close operation was successful
```

12. Read the data from `pcd` device.

```console
root@beaglebone:~# cat /dev/pcd 
Write some data to test the pcd driver.
```

And check the dmesg.

```console
root@beaglebone:~# cat /dev/pcd
. . . 
[ 3643.288103] pcd_open : Open operation was successful
[ 3643.288166] pcd_read : Read requested with count = 4096 and f_pos = 0
[ 3643.288184] pcd_read : Number of bytes successfully read = 512 and updated f_pos = 512
[ 3643.288526] pcd_read : Read requested with count = 4096 and f_pos = 512
[ 3643.288542] pcd_read : Number of bytes successfully read = 0 and updated f_pos = 512
[ 3643.288560] pcd_release : Close operation was successful
```

13. Send a file to the device and use the `dd` command.

Send the dummy_text.txt to the BeableBone Black target.

```console
scp dummyt_text.txt root@<ip-address>:/home/root
```

```console
dd if=dummy_text.txt of=/dev/pcd
```

```console
root@beaglebone:~# dmesg
. . . 
[ 4080.238456] pcd_open : Open operation was successful
[ 4080.243406] pcd_write : Write requested with count = 510 and f_pos = 0
[ 4080.243439] pcd_write : Number of bytes successfully written = 510 and updated f_pos = 510
[ 4080.243476] pcd_release : Close operation was successful
```

14. Unistall the module by typing `rmmod pcd`.

```console
rmmod pcd
```

Check the last dmesg messages by typing `dmesg`. Observe the message `pcd_driver_cleanup : Pseudo Char Driver unloaded`.

```console
root@beaglebone:~# dmesg
. . . 
[ 3317.127110] pcd_driver_cleanup : Pseudo Char Driver unloaded
```