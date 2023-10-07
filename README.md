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


## 2. Hello Module

### 2.1 Building and install

1. Access the folder `hello_module` and run the command `make`.

2. Install the module by typing

```console
$ sudo insmod hello_module.ko
```

3. Run the `dmesg` command to see the `printk` messages
```console
$ sudo dmesg | tail
.
.
.
[25924.125951] Loading hello module...
[25924.125954] Hello, World!
```

### 2.2 Removing the module

1. Remove the module by typing

```console
$ sudo rmmod hello_module
```

2. Run the `dmesg` command to see the `printk` messages
```console
$ sudo dmesg | tail
.
.
.
[25924.125951] Loading hello module...
[25924.125954] Hello, World!
[26104.535066] The module is now unloaded!
[26104.535069] Goodbye, World!
```

The module was removed.

### 2.3 Tracing the hello_module
 Note: To see more detail about ftrace, access the Linux documentation, link https://www.kernel.org/doc/html/v6.1/trace/ftrace.html

1. Change to root user
```console
$ sudo su
```

2. And entry onto the directory
```console
# cd /sys/kernel/debug/tracing
```

3. Check the available tracers
```console
# cat available_tracers 
timerlat osnoise hwlat blk mmiotrace function_graph wakeup_dl wakeup_rt wakeup function nop
```

This holds the different types of tracers that have been compiled into the kernel. The tracers listed here can be configured by echoing their name into current_tracer.

4. Disable the tracer and configure the function graph by echoing to current_tracer
```console
# echo 0 > tracing_on
# echo function_graph > current_tracer
```

5. Empty the `trace` file by echoing nothing to it
```console
# echo > trace
```

6. Enable the function filtering per module and enable the tracer
```console
# echo :mod:hello_module > set_ftrace_filter
# echo 1 > tracing_on
```
7. Check the `trace` file is empty
```console
# cat trace
# tracer: function_graph
#
# CPU  DURATION                  FUNCTION CALLS
# |     |   |                     |   |   |   |
```

8. Install the hello module and check the `trace` file again
```console
# insmod /home/charlesdias/Linux_Driver/code_modules/hello_module/hello_module.ko 
# cat trace
# tracer: function_graph
#
# CPU  DURATION                  FUNCTION CALLS
# |     |   |                     |   |   |   |
 3)   5.027 us    |  hello_start [hello_module]();
```

Notice that the hello_start took nearly 5 us to be executed.


