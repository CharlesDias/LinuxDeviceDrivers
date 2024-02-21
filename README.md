# Linux Device Drivers

Notes: Study notes from:
- LinkedIn Learning [Linux Device Drivers.](https://www.linkedin.com/learning/linux-device-drivers)
- Udemy [Linux Device Driver Programming Using Beaglebone Black(LDD1).](https://www.udemy.com/course/linux-device-driver-programming-using-beaglebone-black/)
- [Linux Device Driver Development](https://www.amazon.com/Linux-Device-Driver-Development-development/dp/1803240067) book.

## Index

- [1. Device Drivers for BeagleBone Black.](BeagleBoneBlack/README.md)
- [2. Hello Module.](hello_module/README.md)
- [3. Linux Kernel Module (LKM).](#linux-kernel-module-lkm)
- [Best practices for writing efficient and reliable kernel modules](#best-practices-for-writing-efficient-and-reliable-kernel-modules)
- [Debugging techniques for kernel modules in Linux](#debugging-techniques-for-kernel-modules-in-linux)
- [Profiling and optimizing the performance of a kernel module in Linux](#profiling-and-optimizing-the-performance-of-a-kernel-module-in-linux)
- [Handling errors and exceptions in kernel modules](#handling-errors-and-exceptions-in-kernel-modules)
- [Techniques for optimizing the memory usage of a kernel module in Linux](#techniques-for-optimizing-the-memory-usage-of-a-kernel-module-in-linux)
- [Concurrency in kernel modules](#concurrency-in-kernel-modules)
- [Interrupts in kernel modules](#interrupts-in-kernel-modules)
- [Power management in kernel modules](#power-management-in-kernel-modules)
- [Handling I/O operations in kernel modules](#handling-io-operations-in-kernel-modules)
- [Synchronization and locking in kernel modules](#synchronization-and-locking-in-kernel-modules)
- [Handling memory allocation and deallocation in kernel modules](#handling-memory-allocation-and-deallocation-in-kernel-modules)
- [Testing frameworks for device drivers](#testing-frameworks-for-device-drivers)

Please note that these links will work correctly if the titles are used as anchors in the same Markdown document.

## Linux Kernel Module (LKM)

Linux Kernel Modules enable features such as device drivers, filesystems, and system calls to be dynamically added to or removed from the running kernel. Essentially, they are pieces of code that can be loaded and unloaded into the kernel upon demand. They extend the functionality of the kernel without the need to reboot the system.

A kernel module can be a device driver, in which case it would control and manage a particular hardware device, hence the name device driver. A module can also add a framework support (for example IIO, the Industrial Input Output framework), extend an existing framework, or even a new filesystem or an extension of it. The thing to keep in mind is that kernel modules are not always device drivers, whereas device drivers are always kernel modules.

To support module loading, the kernel must have been built with the following option enabled:
```text
CONFIG_MODULES=y
```

Unloading modules is a kernel feature that can be enabled or disabled according to the `CONFIG_MODULE_UNLOAD` kernel configuration option. Thus, to be able to unload modules, the following feature must be enabled:
```text
CONFIG_MODULE_UNLOAD=y
```

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


The modules are installed in `/lib/modules/$(uname -r)/kernel/`. In addition to the kernel directory that is shipped with modules, the following files are installed in `/lib/modules/<version>` as well:

- `modules.builtin`: This lists all the kernel objects (.ko) that are built into the kernel.
- `modules.alias`: This contains the aliases for module loading utilities, which are used to match drivers and devices.
- `modules.dep`: This lists modules, along with their dependencies.
- `modules.symbols`: This tells us which module a given symbol belongs to.

## Best practices for writing efficient and reliable kernel modules

Writing efficient and reliable kernel modules in Linux requires a good understanding of the kernel, careful programming, and thorough testing. Here are some best practices:

1. **Understand the Kernel**: Before you start writing a kernel module, make sure you understand how the kernel works, especially the parts that your module will interact with. Read the kernel documentation and source code, and use tools like `objdump` and `nm` to inspect the kernel and other modules.

2. **Follow Coding Standards**: The Linux kernel has a specific coding style that all kernel code should follow. This includes things like indentation, naming conventions, and comment style. Following these standards makes your code easier to read and understand.

3. **Use Kernel APIs**: The kernel provides a wide range of APIs for tasks like memory management, file operations, process management, etc. Always use these APIs instead of trying to implement these tasks yourself. This makes your code more reliable and efficient.

4. **Handle Errors Carefully**: Kernel code runs in a privileged mode, so a bug can crash the entire system. Always check the return values of functions and handle errors appropriately. Use the `BUG_ON` macro to catch programming errors during development.

5. **Avoid Blocking Operations**: Kernel code should not block or wait for resources. This can cause the entire system to hang. Use non-blocking operations whenever possible.

6. **Minimize Memory Usage**: Kernel memory is a limited resource, so use it sparingly. Avoid large static data structures, and use dynamic memory allocation carefully to avoid memory leaks.

7. **Use Synchronization Primitives**: If your module is accessed by multiple processes or threads at the same time, use synchronization primitives like spinlocks or mutexes to protect shared data.

8. **Test Thoroughly**: Test your module thoroughly before deploying it. Use tools like `kmemleak` and `kasan` to detect memory leaks and other bugs. Test your module under different conditions and loads to make sure it works correctly in all situations.

9. **Keep Up with Kernel Changes**: The Linux kernel is constantly changing, and new versions may introduce changes that affect your module. Keep up with these changes and update your module as necessary.

10. **Provide Documentation**: Document your code and provide a README file that explains what your module does, how to use it, and any known issues or limitations. This makes it easier for others to use and maintain your module.

## Debugging techniques for kernel modules in Linux

Debugging kernel modules in Linux can be challenging due to the nature of kernel space. Here are some common techniques:

1. **Printk**: The `printk` function is the kernel equivalent of `printf`. It can be used to print debug messages to the kernel log, which can be viewed with the `dmesg` command. This is a simple and effective way to trace the execution of your code and print the values of variables.

2. **Kernel Debugger (KDB)**: KDB is a simple debugger that is included in the Linux kernel. It can be used to inspect the state of the system, set breakpoints, step through code, etc. However, it is a bit limited compared to full-featured debuggers.

3. **Kernel GNU Debugger (KGDB)**: KGDB is a more powerful debugger for the Linux kernel. It allows you to use the GNU Debugger (GDB) to debug the kernel. You can set breakpoints, step through code, inspect variables, etc. However, setting up KGDB can be a bit complex, as it requires two machines connected via a serial cable or a network connection.

4. **QEMU and GDB**: If you're writing a device driver or other hardware-related code, you can use the QEMU emulator to run your code in a controlled environment. You can then use GDB to debug your code as it runs inside QEMU.

5. **Static Analysis Tools**: Tools like `sparse` and `coccinelle` can analyze your code and find potential bugs without even running the code.

6. **Dynamic Analysis Tools**: Tools like `kmemleak` can detect memory leaks in your kernel code, and `kasan` (KernelAddressSanitizer) can detect out-of-bounds and use-after-free bugs.

Remember that debugging kernel code can be risky, as a bug can crash the entire system. It's a good idea to do your debugging on a separate test system or a virtual machine, not on a production system.

## Profiling and optimizing the performance of a kernel module in Linux

Profiling and optimizing the performance of a kernel module in Linux involves identifying bottlenecks and areas of the code that can be improved. Here are some common techniques:

1. **Perf**: Perf is a powerful tool for profiling Linux applications, including kernel modules. It can measure CPU cycles, cache hits and misses, context switches, and other performance-related events. You can use it to identify the parts of your code that consume the most CPU time.

2. **OProfile**: OProfile is a system-wide profiler for Linux systems. It uses the hardware performance counters of the CPU to identify performance bottlenecks in the code. It can profile the kernel, kernel modules, libraries, and applications.

3. **Ftrace**: Ftrace is a tracing framework for the Linux kernel. It can trace function calls, interrupts, context switches, and other events in the kernel. You can use it to understand the flow of execution in your module and identify areas that need optimization.

4. **LTTng (Linux Trace Toolkit Next Generation)**: LTTng is a high-performance logging tool that can trace the kernel, system libraries, and applications. It's useful for identifying performance issues and understanding the behavior of the system.

5. **Optimization Techniques**: Once you've identified the bottlenecks in your code, you can use various optimization techniques to improve performance. This could include algorithmic optimizations, efficient use of data structures, minimizing I/O operations, and taking advantage of specific hardware features.

6. **Kernel Tuning**: The Linux kernel has many tunable parameters that can affect the performance of your module. For example, you can adjust the scheduler parameters to give more CPU time to your module, or adjust the memory management parameters to reduce page faults.

Remember that optimization should be done carefully and methodically. Always measure the performance before and after each optimization, to make sure that it's actually improving. Also, be aware that optimization can sometimes lead to more complex code, which can increase the risk of bugs.

## Handling errors and exceptions in kernel modules

Handling errors and exceptions in kernel modules is crucial for the stability and reliability of the system. Here are some common techniques:

1. **Error Return Codes**: Kernel functions often return an error code to indicate success or failure. These error codes are defined in `<linux/errno.h>`. Always check the return value of a function and handle errors appropriately.

2. **BUG_ON and WARN_ON Macros**: These macros can be used to check for conditions that should never happen. If the condition is true, `BUG_ON` will cause a kernel panic and `WARN_ON` will print a warning message and a stack trace. These macros are useful for catching programming errors.

3. **Panic and Oops**: If a serious error occurs, you can use the `panic` function to halt the system, or the `oops` function to generate a crash report and optionally halt the system. These functions should only be used in extreme situations where the system can no longer operate safely.

4. **Exception Handling**: The kernel has mechanisms to handle exceptions such as page faults, divide-by-zero errors, and illegal instructions. If your module performs operations that could cause exceptions, make sure it handles them correctly.

5. **Error Recovery**: In some cases, your module may be able to recover from an error and continue operating. This could involve freeing resources, rolling back changes, or retrying an operation. Design your code to be resilient to errors.

6. **Logging Errors**: Use the `printk` function to log error messages. These messages can help you diagnose and fix problems. You can use different log levels to indicate the severity of the error.

7. **Error Injection**: Tools like `fault-inject` can deliberately cause errors to occur, to test how your module handles them. This can help you find and fix bugs before they occur in a real system.

Remember that error handling is a critical part of kernel programming. A bug in a kernel module can cause the entire system to crash, so it's important to handle errors carefully and thoroughly.

## Techniques for optimizing the memory usage of a kernel module in Linux

Optimizing memory usage in a kernel module is crucial as kernel memory is a limited resource and mismanagement can lead to system instability. Here are some common techniques:

1. **Efficient Data Structures**: Use the most efficient data structure for your needs. The Linux kernel provides a variety of data structures like linked lists, queues, maps, etc. Choosing the right one can significantly reduce memory usage.

2. **Memory Pools**: If your module frequently allocates and deallocates small blocks of memory, consider using a memory pool. Memory pools can reduce the overhead of memory allocation and prevent fragmentation.

3. **Slab Allocator**: The slab allocator is a memory management mechanism intended for the efficient memory allocation of kernel objects which may be frequently allocated and deallocated. It eliminates fragmentation caused by allocations and deallocations.

4. **Avoid Large Static Variables**: Large static variables can consume a significant amount of memory. If possible, allocate large data structures dynamically and free them when they're no longer needed.

5. **Use kmalloc and kfree**: The `kmalloc` and `kfree` functions are used to allocate and free memory in the kernel. They're similar to `malloc` and `free` in user space, but they're designed to be safe and efficient in a kernel environment.

6. **Page Allocator**: For larger allocations, consider using the page allocator functions (`__get_free_page`, `__get_free_pages`, `free_page`, `free_pages`) which work with memory in terms of pages rather than bytes.

7. **Use of RCU (Read-Copy-Update)**: RCU allows for read-only concurrent access to the data structure as well as a lock-free method for synchronization.

8. **Vmalloc and Vfree**: For large contiguous allocations where physical continuity is not required, `vmalloc` and `vfree` can be used. They allocate memory that is virtually contiguous.

Remember, always free any memory you allocate once it's no longer needed to prevent memory leaks.

## Concurrency in kernel modules

Handling concurrency in kernel modules is crucial for the stability and performance of the system. Here are some common techniques:

1. **Spinlocks**: Spinlocks are a simple and efficient locking mechanism that can be used to protect critical sections of code. They're suitable for short sections of code where the overhead of context switching would be high.

2. **Semaphores and Mutexes**: Semaphores and mutexes are more advanced locking mechanisms that can put a process to sleep if the lock is not available. They're suitable for longer sections of code where the overhead of spinning would be high.

3. **Read-Copy-Update (RCU)**: RCU is a synchronization mechanism that allows reads to proceed concurrently with updates. It's suitable for data structures that are read often and updated rarely.

4. **Per-CPU Variables**: Per-CPU variables are a way to avoid contention by giving each CPU its own instance of a variable. They're suitable for data that is used frequently by all CPUs.

5. **Atomic Operations**: Atomic operations are operations that complete in a single step without being interrupted. They're suitable for simple updates to shared data.

6. **Seqlocks**: Seqlocks are a type of lock that allows readers to proceed concurrently with a single writer. They're suitable for data that is read often and updated occasionally.

7. **Lock-Free Data Structures**: Lock-free data structures use atomic operations to manage concurrency without the need for locks. They're complex to implement, but can provide excellent performance in multi-threaded environments.

Remember that concurrency control is a complex topic, and the best technique depends on the specific requirements of your module. Always test your code thoroughly to ensure that it works correctly in all situations.

## Interrupts in kernel modules

Handling interrupts in kernel modules is a common task, especially for device drivers. Here are some common techniques:

1. **Requesting an Interrupt Line**: Use the `request_irq` function to register an interrupt handler function. This function will be called when an interrupt occurs on the specified interrupt line.

2. **Freeing an Interrupt Line**: When you're done with an interrupt line, use the `free_irq` function to unregister the interrupt handler.

3. **Interrupt Handler Function**: This is the function that gets called when an interrupt occurs. It should be short and fast, to avoid delaying other interrupts. If you need to do a lot of work in response to an interrupt, consider using a bottom half.

4. **Disabling and Enabling Interrupts**: You can disable interrupts on the current processor using the `local_irq_disable` function, and enable them again with `local_irq_enable`. Be careful with these functions, as disabling interrupts for too long can cause the system to become unresponsive.

5. **Interrupt Context**: Code running in interrupt context has several restrictions. It can't sleep, it can't access user space memory, and it must be reentrant. Keep these restrictions in mind when writing your interrupt handler.

6. **Bottom Halves**: A bottom half is a mechanism to defer some work to be executed later, outside of the interrupt context. This is useful if your interrupt handler needs to do a lot of work or needs to call functions that can sleep. The Linux kernel provides several mechanisms for bottom halves, including tasklets and work queues.

7. **Threaded Interrupt Handlers**: In some cases, you can use a threaded interrupt handler. This is a special type of interrupt handler that runs in a separate kernel thread, not in interrupt context. This allows the handler to sleep, access user space memory, and do other things that are not normally allowed in interrupt context.

## Power management in kernel modules

Power management is an important aspect of kernel module development, especially for device drivers. Here are some common techniques:

1. **Power Management Callbacks**: The kernel provides several callbacks that your module can implement to handle power management events. These include `.suspend`, `.resume`, `.freeze`, and `.thaw`.

2. **Runtime Power Management**: Runtime power management allows devices to be put into a low-power state when they're not in use, and woken up when they're needed again. You can use the `pm_runtime_get` and `pm_runtime_put` functions to manage the runtime power state of a device.

3. **System Sleep States**: The kernel supports several system sleep states, such as standby, suspend-to-RAM, and suspend-to-disk. Your module should be able to handle these states and recover gracefully when the system wakes up.

4. **Device Power States**: Many devices support multiple power states, such as on, off, and various levels of sleep or low-power operation. Your module should be able to put the device into the appropriate state depending on the system's power state and the device's usage.

5. **Power Management QoS (Quality of Service)**: The PM QoS framework allows drivers to express their performance needs in terms of latency, throughput, etc. This can be used to optimize the system's power management decisions.

6. **Energy-Efficient Scheduling**: The kernel's scheduler can take power efficiency into account when deciding which tasks to run. Your module can influence this by setting the appropriate scheduling parameters.

Remember that power management is a complex topic, and the best techniques depend on the specific requirements of your module and the capabilities of the hardware it's controlling. Always test your power management code thoroughly to ensure that it works correctly in all situations.

## Handling I/O operations in kernel modules

Handling I/O operations is a common task in kernel module development, especially for device drivers. Here are some common techniques:

1. **I/O Ports and I/O Memory**: Use the `inb`, `inw`, `inl`, `outb`, `outw`, and `outl` functions to read from and write to I/O ports. Use the `ioread8`, `ioread16`, `ioread32`, `iowrite8`, `iowrite16`, and `iowrite32` functions to read from and write to I/O memory.

2. **Memory-Mapped I/O**: Use the `ioremap` and `iounmap` functions to map device memory into the kernel's address space. This allows you to access device memory as if it were regular memory.

3. **DMA (Direct Memory Access)**: DMA allows a device to access memory directly, without involving the CPU. This can greatly improve performance for large data transfers. The kernel provides a DMA API to manage DMA transfers.

4. **Interrupts**: Many devices use interrupts to signal the completion of an I/O operation. Use the `request_irq` and `free_irq` functions to handle interrupts.

5. **Polling**: For simple or slow devices, you might use polling instead of interrupts. This involves repeatedly checking the device's status until the I/O operation is complete.

6. **Block I/O**: For block devices like hard drives, the kernel provides a block I/O layer that handles buffering, scheduling, and other tasks. You can create a request queue and a request function to handle block I/O operations.

7. **Network I/O**: For network devices, the kernel provides a network layer that handles packet transmission, reception, and other tasks. You can create a network device and implement the `ndo_start_xmit` function to handle network I/O operations.

Remember that I/O operations can be slow, so it's important to design your module to handle I/O efficiently. This might involve using DMA for large data transfers, using interrupts to avoid busy-waiting, and using buffering to smooth out I/O bursts.

## Synchronization and locking in kernel modules

Synchronization and locking are crucial in kernel modules, especially when dealing with shared resources or concurrent operations. Here are some common techniques:

1. **Spinlocks**: Spinlocks are a simple and efficient locking mechanism that can be used to protect critical sections of code. They're suitable for short sections of code where the overhead of context switching would be high.

2. **Semaphores and Mutexes**: Semaphores and mutexes are more advanced locking mechanisms that can put a process to sleep if the lock is not available. They're suitable for longer sections of code where the overhead of spinning would be high.

3. **Read-Copy-Update (RCU)**: RCU is a synchronization mechanism that allows reads to proceed concurrently with updates. It's suitable for data structures that are read often and updated rarely.

4. **Seqlocks**: Seqlocks are a type of lock that allows readers to proceed concurrently with a single writer. They're suitable for data that is read often and updated occasionally.

5. **Per-CPU Variables**: Per-CPU variables are a way to avoid contention by giving each CPU its own instance of a variable. They're suitable for data that is used frequently by all CPUs.

6. **Atomic Operations**: Atomic operations are operations that complete in a single step without being interrupted. They're suitable for simple updates to shared data.

7. **Lock-Free Data Structures**: Lock-free data structures use atomic operations to manage concurrency without the need for locks. They're complex to implement, but can provide excellent performance in multi-threaded environments.

8. **Completion Variables**: Completion variables are a synchronization mechanism that allows one process to wait for another process to finish a task. They're useful for coordinating tasks between different parts of your module.

Remember that synchronization and locking can have a significant impact on performance, so it's important to use them judiciously. Always release locks as soon as possible to minimize contention, and consider using lock-free techniques where appropriate.

## Handling memory allocation and deallocation in kernel modules

Memory management is a crucial aspect of kernel module development. Here are some common techniques:

1. **kmalloc and kfree**: The `kmalloc` and `kfree` functions are used to allocate and free memory in the kernel. They're similar to `malloc` and `free` in user space, but they're designed to be safe and efficient in a kernel environment.

2. **vmalloc and vfree**: For large contiguous allocations where physical continuity is not required, `vmalloc` and `vfree` can be used. They allocate memory that is virtually contiguous.

3. **get_free_pages and free_pages**: For larger allocations, consider using the page allocator functions (`__get_free_page`, `__get_free_pages`, `free_page`, `free_pages`) which work with memory in terms of pages rather than bytes.

4. **Memory Pools**: If your module frequently allocates and deallocates small blocks of memory, consider using a memory pool. Memory pools can reduce the overhead of memory allocation and prevent fragmentation.

5. **Slab Allocator**: The slab allocator is a memory management mechanism intended for the efficient memory allocation of kernel objects which may be frequently allocated and deallocated. It eliminates fragmentation caused by allocations and deallocations.

6. **Per-CPU Variables**: Per-CPU variables are a way to avoid contention by giving each CPU its own instance of a variable. They're suitable for data that is used frequently by all CPUs.

7. **Avoid Large Static Variables**: Large static variables can consume a significant amount of memory. If possible, allocate large data structures dynamically and free them when they're no longer needed.

Remember, always free any memory you allocate once it's no longer needed to prevent memory leaks. Also, be aware that memory allocation can fail, especially for large allocations, so always check the return value of `kmalloc`, `vmalloc`, and `get_free_pages`.

## Testing frameworks for device drivers

### Linux Test Project (LTP)

LTP is a project that aims to deliver test suites to the open source community that validate the reliability, robustness, and stability of Linux. It includes a set of tools for testing different aspects of the kernel such as system calls, networking, and filesystems. For device driver testing, you can use LTP's device driver framework (DDT).

### KUnit

KUnit is a lightweight unit testing and mocking framework for the Linux kernel. Unlike other unit testing frameworks, KUnit is a part of the Linux kernel itself. This makes it capable of testing any part of the kernel, including device drivers.

### Kernel Test Robot

Kernel Test Robot is an automated testing system that can build and boot kernels, and run tests on them. It's particularly useful for regression testing and can help catch issues early in the development cycle.

### Autotest

Autotest is a framework for automated testing and includes a number of tests specifically for the kernel. It's designed to be portable and extensible, making it a good choice for complex testing scenarios.

### Trinity

Trinity is a system call fuzz tester which generates random system calls. It's particularly useful for stress testing and finding obscure bugs in device drivers.
