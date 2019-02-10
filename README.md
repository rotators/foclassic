# FOClassic

## Build status

| Platform | Main branch | Development branch |
|----------|:-----------:|:------------------:|
| Linux    | [![Build Status](https://travis-ci.com/rotators/foclassic.svg?branch=master)](https://travis-ci.com/rotators/foclassic) | [![Build Status](https://travis-ci.com/rotators/foclassic.svg?branch=dev)](https://travis-ci.com/rotators/foclassic) |
| Windows  | [![Build status](https://ci.appveyor.com/api/projects/status/wi1iio98h45iw3nj/branch/master?svg=true)](https://ci.appveyor.com/project/rotators/foclassic/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/wi1iio98h45iw3nj/branch/dev?svg=true)](https://ci.appveyor.com/project/rotators/foclassic/branch/dev) |

## Requirements

* git (required)  
  When working on Windows, use _Git for Windows Portable_ if possible - bundled tools might be used in future.  
  https://git-scm.com/download/

* cmake v3.13.4 (minimum required)  
  https://cmake.org/download/

* Visual Studio 2010 (required)  
  https://visualstudio.microsoft.com/vs/older-downloads/

* Visual Studio 2017 (optional)  
  https://visualstudio.microsoft.com/vs/

## Preparations

* Update dependencies  
  `git submodule update --init --recursive`

* Create a build directory; recommended: SDK*  
  This step can be skipped if using automated build

    * Windows
        * _Visual Studio 2010_: `cmake -G "Visual Studio 10" Path/To/Main/Directory/`
        * _Visual Studio 2017_: `cmake -G "Visual Studio 15" -T v100 Path/To/Main/Directory/`  
          Note that -T option informs cmake to generate solution which uses VS2010 toolchain.  
          Builds using _Visual Studio 2017_ toolchain (without -T switch) will NOT work.


## Automated build

CMake-based script is provided to fully automate building process.  
`cmake -P Build.cmake`

Included tasks:

* Preparing files

    * Source formatting

    * Restoring files modification time (CI only)  
      This step is required for timestamps used in released files.

* Preparing build directory/directories

    * When building on Windows, two different directories are created for two different scenarios:
        * `SDK.VS2017.v100`, used to work with source on modern Visual Studio version
        * `SDK.VS2017`,      used to check if engine is ready for migration to VS2017

    * Other platforms are not supported currently

* Building all targets

* Preparing release package  
  All files are placed in [build.dir]/FOClassic-v[version]/ and compressed to FOClassic-v[version].zip
