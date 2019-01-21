# Customization

Build process can be customized in following places:
- [FOClassic directory]/CMakeFOClassic.txt
- [top-level directory]/CMakeFOClassic.txt
- any CMakeLists.txt, before calling `add_subdirectory( [FOClassic directory] )`

Last two entries are relevant only when _FOClassic_ is built as a [sub-project](https://cmake.org/cmake/help/latest/command/add_subdirectory.html).

## FOCLASSIC_COPY_CLIENT_DIR
Type:    path
Default: not set
List of directories where Client executables should be copied after compilation.

## FOCLASSIC_COPY_MAPPER_DIR
Type:    path
Default: not set
List of directories where Mapper executables should be copied after compilation.

## FOCLASSIC_COPY_SERVER_DIR
Type:    path
Default: not set
List of directories where Server executable should be copied after compilation.

## FOCLASSIC_COPY_ASCOMPILER_DIR
Type:    path
Default: not set
List of directories where ASCompiler executable should be copied after compilation.

## FOCLASSIC_CUSTOM_VERSION
Type:    unsigned number
Default: 0
Allows to change version string from "`version FOCLASSIC_VERSION`" to "`version FOCLASSIC_VERSION.FOCLASSIC_CUSTOM_VERSION`" in logfiles, crash dumps and Server UI.  
On Windows, it will become part of FILEVERSION, making it visible in file properties.

## FOCLASSIC_FOLDER_PREFIX
Type:    string
Default: `FOClassic`
Allows to group all _FOClassic_ targets when using IDE. Useful only if engine and game content are part of the same CMake project.  
See [FOLDER property documentation](https://cmake.org/cmake/help/v3.12/prop_tgt/FOLDER.html).

## Hashing
When building _FOClassic_ from sources, it's possible to generate global seed (basing on string and/or file(s) content provided by developers) which will be used in engine to create various values which should be different from those used in _vanilla_ engine (like network protocol). Other than values provided by developers (if any), global seed changes automagically with each new version.  
Note that this will **not** prevent _rogue players_ from experimenting with network protocol, nor will make server 100% secure.

### FOCLASSIC_HASH_STRING
Type:    string
Default: not set
String used to initialize global seed. Can be used together with `FOCLASSIC_HASH_FILES`.

### FOCLASSIC_HASH_FILES
Type:    list of paths
Default: not set
Files used to initialize global seed. Can be used together with `FOCLASSIC_HASH_STRING`.
