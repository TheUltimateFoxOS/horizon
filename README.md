# Horizon

Horizon is planed to become the main FoxOS kernel at some point. We decided to rewrite the kernel with some stricter design principles and to make it more modular.  

The project structure will look something like this:

- /core -> contains the core kernel stuff like paging and memory management it will also contain some very basic drivers like the VGA/serial and ps2 drivers.
- /devices/\<device name\> -> contains the drivers for the device as a loadable module.
- /filesystems/\<fs name\> -> contains the drivers for the filesystem as a loadable module.

## The strict design principles

- every code file needs to be in a location that makes sense (not putting a header file for a list in the net folder)
- the core kernel contains the vfs/network stack but no actual device drivers the device drivers are in the /\<device name\> as loadable modules (does not apply for the stivale2 bootmodules vfs driver)
- the loaded drivers can manually scan for the device in case of for example ata or call a register_pci_driver function to register a function witch gets called when a device is found
- the modules get loaded as early as possible
- the vfs is going to be as simple as possible on the kernel side. The vfs is not going to implement the following functions:
- - `file_t* open(char* path)`
- - `void close(file_t* file)`
- - `void read(file_t* file, void* buffer, size_t size, size_t offset)`
- - `void write(file_t* file, void* buffer, size_t size, size_t offset)`
- - `void delete(file_t* file) // also closes file`
- - `void mkdir(char* path)`
- - `dir_t dir_at(int idx, char* path) // reurn is_none in struct`
- A module can specify the following function and they fill be called in that order by the kernel:
- - init (directly called after loading)
- - device_init (called in the device init phase of the kernel)
- - fs_init (called after kernel started all known devices should scan disks for the filesystem and then register a vfs node with \<fs_name\>-\<id\> (the id is just a internal number in the driver counting up)
- The rules described [here](https://github.com/TheUltimateFoxOS/FoxOS/blob/main/CONTRIBUTING.md) still apply.
- Syscall's are implemented by id's. there will be always id 0 (get_syscall_id) witch returns the id for a syscall searched by name
