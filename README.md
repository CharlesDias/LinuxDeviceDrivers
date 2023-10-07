# Linux Device Drivers

## 1. Linux Kernel Module (LKM)

Linux Kernel Modules enable features such as device drivers, filesystems, and system calls to be dynamically added to or removed from the running kernel. Essentially, they are pieces of code that can be loaded and unloaded into the kernel upon demand. They extend the functionality of the kernel without the need to reboot the system.

### Advantages of LKMs

- Flexibility: Users can add or remove specific functionalities per their needs without disturbing the core kernel operations.
- Development: Developers can write and test new kernel functionalities or drivers without rebooting every time. This speeds up the development process.
- Optimization: Systems can run with a lean kernel, loading only required modules. This is particularly useful for embedded systems with limited resources.


#### Key concepts:

- `/lib/modules/$(uname -r)/` : Directory where modules are usually stored, sorted by kernel version.
- `insmod` : Command to insert/load a module into the kernel.
- `rmmod` : Command to remove/unload a module from the kernel.
- `modprobe` : Command to add or remove modules, but with the added capability of handling dependencies (can load or unload multiple interdependent modules). Uses dependency file to load dependencies first and then module.
- `lsmod` : Command to list the modules currently loaded into the kernel. Does not list statically linked modules. The most recently inserted module is listed first.
- `/proc/modules` : A file that lists all the loaded modules.
- `modinfo` : Command to show information about a module. Extracts info from the module file.
- `modconf` : A module configuration tool to enable or disable module configurations.
- `depmod` : Generate module dependencies for modprobe.


