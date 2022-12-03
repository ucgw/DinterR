# DinterR
A data interaction reporting software. 

The data interactions are represented by adding "locally sourced" extra data within inotify / fsnotify event workflows. This data in turn can be sent over a network using a simple protocol for further data analysis on interactions in close to real time.

## Software Components
* kernel patches (currently version 4.19.143)
* a server (_ddtp-serve_)
* a simple client (_ddtp-cli_)
* various build and test scripts

## Extra Data Captured
Below is a diagram of what extra data is captured at various function call points of a filesystem event being queued.

![DinterR Data Sources](/doc/images/kernel_fsnotify_inotify_data_flow.png)

## Kernel Build Instructions
In order for the _ddtp-serve_ and _ddtp-cli_ to work, the supported kernel needs to be patched using patches provided in the repo. In this section I will provide instructions for using the provided automation for applying those patches and building the kernel. 

**NOTE:** these instructions have only been tested on CentOS 7.

1. download source code for the linux kernel version 4.19.143
2. configure `DinterR/utils/kern/dinterbuild.env` for:
  - KERNEL_DIR (full path of your kernel source.)
  - PATCH_DIR (full path to the patches directory within the repo)

```
KERNEL_DIR="<path to source code>/linux-$KERNEL_VER"
PATCH_DIR="<path to DinterR checkout>/src/kern/patches/$KERNEL_VER"
```
3. manually change the `$KERNEL_DIR/.config` to match `$LOCAL_VERSION_INIT_SUBSTRING`
4. apply patches via script
```
% cd DinterR/utils/kern
% ./apply_latest_patches.sh
```
5. inspect output to ensure that patches were applied successfully.
6. build patched kernel source
```
% ./newbuild.sh
```

## Dinterr Data Transport Protocol (DDTP)
The server and client implement a rudimentary protocol to communicate inotify extra data from source host (server) to remote host (client). Below are diagrams outlining the protocol.

### DDTP (Server)
![DinterR Server](/doc/images/server_fsm.png)

### DDTP (Client)
![DinterR Client](/doc/images/client_fsm.png)


## Userland Build Instructions
The server and client are written in C++. Because of dependencies on some [third party](#third-party-code-used--acknowledgements) software, a version of gcc supporting C++20 standard (**-std=c++2a**). Also, **zlib** is a dependency for the build.

### Building ddtp-serve
```
% cd DinterR/src/user/dinterrd/server
% make
```

### Building ddtp-cli
```
% cd DinterR/src/user/dinterrd/client
% make
```

## Usage / Examples
_ddtp-serve_ help output:

```
% ./ddtp-serve -h
A DinterR data tranport protocol server
Usage:
  ddtp-serve [OPTION...]

  -i, --ipaddr arg  ip address to bind socket to (default: ALL)
  -p, --port arg    remote ddtp server port to connect to (default: 8992)
  -v, --verbose     verbose flag
  -h, --help        Print usage
```

_ddtp-cli_ help output:

```
% ./ddtp-cli 
Error parsing: Option ‘’ has no value
A DinterR data tranport protocol client
Usage:
  ddtp-cli [OPTION...]

  -f, --file arg        remote file to watch via inotify for extra data
  -c, --csv arg         csv file to write/append inotify extra data
  -s, --server arg      remote ddtp server hostname/ip to connect to
  -i, --iterations arg  number of poll interations on socket (default: 10)
  -p, --port arg        remote ddtp server port to connect to (default: 8992)
  -v, --verbose         verbose flag
      --header          write csv header line in csv file
  -h, --help            Print usage
```

### Third Party Code Used / Acknowledgements:
- [cxxopts](https://github.com/jarro2783/cxxopts)
- [boost-ext/SML](https://github.com/boost-ext/sml)
- [ProtEnc](https://github.com/nitnelave/ProtEnc)
- [modern-c-writing-a-thread-safe-queue](https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue)
