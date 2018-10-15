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
    VS2010: `cmake -G "Visual Studio 10" Path/To/Main/Directory/`
    VS2017: `cmake -G "Visual Studio 15" -T v100 Path/To/Main/Directory/`

* Automated build
  `cmake -P Build.cmake`