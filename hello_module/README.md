# 3. Hello Module

The `static int __init hello_start` method is the entry point and corresponds to the function called when the module is loaded (modprobe or insmod), and the `static void __exit hello_end` is the cleanup and exit
point and corresponds to the function executed at module unloading (at rmmod or modprobe -r).

## 3.1 Building and install

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

## 3.2 Removing the module

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

## 3.3 Tracing the hello_module
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