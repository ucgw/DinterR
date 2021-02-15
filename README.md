# DinterR
A data interaction reporting software. 

The data interactions are represented by adding "locally sourced" extra data within inotify / fsnotify event workflows. This data in turn can be sent over a network using a simple protocol for further data analysis on interactions in close to real time.

## Software Components
* kernel patches (currently version 4.19.143)
* a server
* a simple client
* various build and test scripts

## Extra Data Captured
Below is a diagram of what extra data is captured at various function call points of a filesystem event being queued.

![DinterR Data Sources](/doc/images/kernel_fsnotify_inotify_data_flow.png)





### Third Party Code Used / Acknowledgements:
- [cxxopts](https://github.com/jarro2783/cxxopts)
- [boost-ext/SML](https://github.com/boost-ext/sml)
- [ProtEnc](https://github.com/nitnelave/ProtEnc)
- [modern-c-writing-a-thread-safe-queue](https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue)
