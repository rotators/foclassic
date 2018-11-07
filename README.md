# FOClassic

## Build status

| Platform | Main branch | Development branch |
|----------|:-----------:|:------------------:|
| Windows  | [![Build status](https://ci.appveyor.com/api/projects/status/wi1iio98h45iw3nj/branch/master?svg=true)](https://ci.appveyor.com/project/rotators/foclassic/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/wi1iio98h45iw3nj/branch/dev?svg=true)](https://ci.appveyor.com/project/rotators/foclassic/branch/dev) |

## Requirements

* Visual Studio 2010 (required)  
  https://visualstudio.microsoft.com/vs/older-downloads/

* Visual Studio 2017 (optional)  
  https://visualstudio.microsoft.com/vs/

* cmake -- v3.12.1 (minimum required)  
  https://cmake.org/download/

* git (required)  
  when working on windows, use _Git for Windows Portable_ if possible (bundled tools might be used in future)  
  https://git-scm.com/download/

## Preparations

* Create a build directory; recommended: SDK*

    * Windows
        * VS2010: `cmake -G "Visual Studio 10" Path/To/Main/Directory/`
        * VS2017: `cmake -G "Visual Studio 15" -T v100 Path/To/Main/Directory/`  
          Note that -T option informs cmake to generate solution which uses VS2010 toolchain.  
          Builds using VS2017 toolchain will NOT work.

## Automated build

CMake-based script is provided to fully automate building process.  
`cmake -P Build.cmake`

Included tasks:

* Preparing files

    * Source formatting

    * Restoring files modification time (CI only)  
      This step is required for timestamps used in released files.

* Preparing build directory/directories

    * For Windows, three different directories are created for different scenarios:
        * `SDK.VS2010`,      used to release binaries
        * `SDK.VS2017.v100`, used to work with source on modern Visual Studio version
        * `SDK.VS2017`,      used to check if engine is ready for migration to VS2017

    * Other platforms are not supported currently

* Building all targets

* Preparing release package  
  All files are placed in [build.dir]/FOClassic-v[version]/ and compressed to FOClassic-v[version].zip
