# CHANGELOG

Complete list of changes in FOClassic since [FOnline r412](https://xp-dev.com/sc/76003/412/).

## [v1](SOON) (WIP)
- Hello world!

  `TODO` short intro with link to announcement on FOdev

- engine building process has been migrated to [CMake](https://cmake.org/) and heavily integrated with it
    - many tasks has been automated to avoid human mistakes when preparing release
    - parts of cmake setup which might be useful for developers will become part of release and/or public repository; that includes more or less complex scripts, where cmake will be favored over _.bat_/_.sh_ files
- changed generated binary files signatures, files created by FOnline are explicitly ignored
    - results in enforcing clean state on all servers willing to use applications in this project; migrating from r412 keeping saves, while technically possible (by hexediting files), is not adviced, supported, or good idea at all
    - see [binary files documentation](https://rotators.fodev.net/foclassic/docs/BinaryFiles.md) for detailed description
- changed protocol used for Client <-> Server communication
    - protocol versioning has been removed, version of Client and Server must match; adjusting FOGAME.MSG/1024 is advised
    - multiple changes breaking compatibility with FOnline
    - [checking server status](https://fodev.net/forum/index.php/topic,2351.msg19771.html#msg19771) is not affected by protocol changes
- changed naming convention for released applications
    - `FOnline` -> `ClientGL`
    - `FOnlineD3D` -> `ClientDX`
    - `Mapper` -> `MapperGL`
    - `MapperD3D` -> `MapperDX`
    - `FOnlineServer` -> `Server`
    - [windows] reminder: while _.exe_ files can be renamed, associated _.pdb_ files must keep original name, and be placed in same directory as _.exe_ file
- crashdumps created by Client/Mapper includes information about rendering mode
- verbose server initialization log has been added and enabled by default; option to disable will follow in future version
- security fix for registration parameters
- hidden trade crash fix
- simplified logic for detecting outdated scripts bytecode
    - scripts are recompiled from source whenever any mismatch is detected (older version, source newer than bytecode, etc.)
    - removed warning message
- server simulates binding reserved functions after compilation of client scripts
- all provided applications now share global version number
    - version contains two parts - `STAGE` and `VERSION`
        - `STAGE` - internal version number, increased on completion of major milestones
        - `VERSION` - public version number, increased on each release; visible in Client/Mapper/Server log
    - both values are defined for all scripts as `FOCLASSIC_STAGE` and `FOCLASSIC_VERSION`
    - [windows] version information is added to executables as [VERSIONINFO](https://docs.microsoft.com/en-us/windows/desktop/menurc/versioninfo-resource) in `1.STAGE.VERSION.*` format
- _magic numbers_ coming from engine - previously kept in multiple _.fos_ files - has been merged into _FOClassic.fos_
    - it is strongly adviced to use provided file as read-only header, both for scripts and and extensions; long-term goal is to eliminate need for editing _FOClassic.fos_ and _FOClassic.h_
    - whenever value from engine might be used for server-specific values (like custom item types), marker is added with suffix `LAST` and pointing at last value used by engine; please report if any of markers is missing
        - example: `#define ITEM_TYPE_FOOD (ITEM_TYPE_LAST+1)`
    - names of many values has been changed, to elimate cryptic prefixes such as `FCRIT` / `FH` / `FT` and more
        - old names are provided for each renamed value - developers can decide if they want to follow naming used in engine code right away, or keep old names and delay adaptation process; when old names are removed from header, developers can copy compatibility macros and maintain them in their own codebase
    - in specific cases values/macros are provided even if they're either not used by engine at all, or have no use in extensions/scripts code
        - first include macros which are widely used in scripts, and really shouldn't be separated (example: flags for finding critters)
        - second include informations about inner engine logic which can be indirect help when working on more complex features (example: sprite layers)
    - it's adviced to check that file at least once to have a general idea what and when is planned to be changed; due to way how project is released, in future versions developers should check for changes using tools provided by their source control system (`git diff` / `svn diff`)
- extensions changes
    - header name changed to _FOClassic.h_
    - StlPort is no longer used, and must be removed from all extensions
    - added `GameOptions* FOClassic` which will replace `GameOptions* FOnline` in future version
    - removed functions related to item deterioration flags not used by engine
        - `bool Item::IsNoResc();`
        - `bool Item::IsService();`
        - `bool Item::IsServiceExt();`
        - `bool Item::IsEternal();`
    - fixed macro redefinition warnings; all values defined in header are undefined first
    - many values/macros has been moved to _FOClassic.fos_, which is now required dependency
    - added dummy extensions which serves as an example of recommended code structure; includes cmake setup
        - other than being an example, provided extensions are part of engine building process, using only files prepared for release
- scripts changes
    - disabled `bool SwapCritters(Critter& cr1, Critter& cr2, bool withInventory, bool withVars)` (function does nothing and always returns _false_); will be removed in future version
    - default date changed to 08.09.2078, 09:44
- temporary changes; will be reverted in future version
    - script string debugging disabled
    - profiler disabled
- numeric commands removed
- GenerateWorld.cfg no longer required during initialization
- additional notes
    - it's very much adviced to delete or move r412 executables to avoid launching them by muscle memory or missclick; while in most cases it ends with crash only (which is expected), FOnline r412 Server attempting to load state created by FOClassic might result with **heavy** RAM/HDD usage and/or making operating system completely unresponsive
    - both headers are still work in progress, and has been only briefly tested against extensions - all missing entries, candidates for inclusion, and additional tuning should be posted on [issue tracker](https://github.com/rotators/foclassic/issues)
