== First steps ==

* Requirements
  * Visual Studio 2010 (required)
    https://visualstudio.microsoft.com/vs/older-downloads/

  * Visual Studio 2017 (optional)
    https://visualstudio.microsoft.com/vs/

  * cmake -- v3.12.2 (at least)
    https://cmake.org/download/

  * git -- v2.17.1
    when working on windows, use *Git for Windows Portable* if possible (bundled tools might be used in future)
    https://git-scm.com/download/win

* Preparations
  * Create a build directory; recommended: SDK*
    * Windows
      * VS2010: `cmake -G "Visual Studio 10" Path/To/Main/Directory/`
      * VS2017: `cmake -G "Visual Studio 15" -T v100 Path/To/Main/Directory/`
  	    Note that -T option informs cmake to generate solution which uses VS2010 toolchain.
	    Builds using VS2017 toolchain will NOT work.

== Automated build ==

CMake-base script is provided to fully automate building process.
`cmake -P Build.cmake`

Included tasks:

* Source formatting

* Preparing build directory/directories
  * For Windows, three different directories are created for different scenarios:
	* `SDK.VS2010`,      used to release binaries
	* `SDK.VS2017.v100`, used to work with source on modern Visual Studio version
    * `SDK.VS2017`,      used to check if engine is ready for migration to VS2017
  * Other platforms are not supported currently

* Building all targets

* Preparing release package
  All files are placed in [build directory]/FOClassic-v[version]/ and compressed to FOClassic-v[version].zip
 