# rt_pthread_tutorial
The pthread tutorial for real-time control. Test in ubuntu 20.04 5.15.65-rt49 and ubuntu 18.04 5.4.19-rt11.

## Isolate CPU
The isolated CPUs are not used by user space processes, only by the kernel. To improve the realtime performance, so you can run your task in the isolated CPU by setting CPU affinity.

```console
$ sudo vim /etc/default/grub 
```

Write the current line
```
GRUB_CMDLINE_LINUX="isolcpus=0,1,2"
```

Then update the grub
```console
$ sudo update-grub
```

Reboot and check the grub parameters
```console
$ cat /proc/cmdline
```

## Set CPU affinity
```c++
#include <pthread.h> // -lpthread

CPU_ZERO(&_mask); // reset the CPU set
CPU_SET(CPU_id, &_mask); // add the CPU core to the set, CPU_id from 0 to x (x base on your CPU)
pthread_setaffinity_np(pthread_self(), sizeof(_mask), &_mask);
```

## Disable the E-cores in NUC12(i7-1260P 4P+8E)
Due to some unknown reasons, you may need to disable the E-cores.

Click F2 when start up, then enter the BIOS.

(1) "power,performance and cooling" -> "External Ambient Temperature Tolerance" -> set "user defined"

(2) "performance" -> "processor" -> "Active Atom Cores" -> set "0"

(3) Click F10 to save and reboot
