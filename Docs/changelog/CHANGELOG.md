# CHANGELOG

Complete list of changes in FOClassic since [FOnline SDK r412](https://github.com/cvet/fonline/commit/3e58c94f1234173d5af234eb1f80ad0af8f3c120).

## [v2]() (WIP)

[FOnline](https://github.com/cvet/fonline) and [FOClassic](https://github.com/rotators/foclassic/) are now fully open-source projects.

- restored loading of worldsaves and characters created by FOnline r412, and possibly older versions
    - support for loading legacy files will be kept **only** until first change of their format (eg. changing format of worldsave will disable loading legacy worldsave **and** characters)
    - wordsave/characters files format changes are **not** planned will during [migration stage](https://github.com/rotators/foclassic/milestone/2)
    - loading has been briefly tested against real server state (thanks Kilgore)
- when [checking server status](https://fodev.net/forum/index.php/topic,2351.msg19771.html#msg19771), number of currently connected players is returned instead of characters ingame; this should give more realistic numbers, especially on servers which uses NoLogout maps
- configuration files / command line rework
    - application settings are loaded from disk only once, and cached in memory
    - all command line options has been renamed to use `--CamelCase` format; those which does not follow this scheme (eg. `--some-weird-option`) are intended for internal use and might be changed/removed without notice
    - all command line options uses same names as in configuration files
    - Client
        - removed setting `UserPass`; character password can still be provided by using `--UserPass` in command line
        - renamed setting `FonlineDataPath` to `DataPath`
        - if setting requires changes at runtime (eg. invalid `Language` in config file), it's done in memory only, without updating config file on disk
        - all boolean settings are false by default
    - Mapper
        - settings moved to `Mapper` section
        - Client/Server settings can be overwritten in Mapper.cfg, see below
    - Server:
        - settings moved to `Server` section
        - added setting `Languages` containing list of all languages supported
            - if `Languages` list is empty/not set, `Language_N` is used; it will continue to work until further notice
            - if `Languages` list is non-empty, `Language_N` entries are ignored
            - see bottom of changelog for config file fragment
    - settings reading logic
        - application starts, config file is parsed, all settings are stored in memory as strings
        - Client/Mapper: detail section is handled; example for GL application using `App` as its main section:
            - detail section (`AppGL`) settings are copied into main section (`App`) overwriting old entries or creating new ones
            - detail section (`AppGL`) is removed from memory
            - unused section (`AppDX`) is removed from memory
        - Mapper
            - Client configuration is loaded and parsed using same logic as in previous point
            - Server configuration is loaded
            - note: as Mapper has been using multiple config files, non-Mapper settings can be now be added inside Mapper.cfg - when loading non-Mapper configuration, setting will **not** be overwritten during loading step, but they **might** be overwritten during sections merge step; consider following scenario:
                - Mapper.cfg contains `[Client]` -> `ScreenWidth = 5000`
                - Client.cfg contains `[Client]` -> `ScreenWidth = 1000`
                - Client.cfg contains `[ClientDX]` -> `ScreenWidth = 2000`
                - Mapper loads Mapper.cfg: `ScreenWidth = 5000` is used
                - Mapper loads Client.cfg: `ScreenWidth = 5000` is used, as such setting already exists
                - Client's detail section is merged into main section: `ScreenWidth = 2000` is used
                - making sure that mapper uses _correct_ values can be enforced by either using detail section(s) in Mapper.cfg for non-Mapper settings, or not using detail section(s) at all in Client/Server config files
        - settings in cache are applied to `GameOptions`
            - default value is applied
            - value in config file is applied, if present
            - value in comand line is applied, if present
            - (integers) value is checked agains allowed minimum/maximum; if it's out of range, default value is applied
        - additional configuration based on setings not stored in `GameOptions`
        - summary: detail sections, together with sections merging, should allows fine-tuning applications to individual developers needs without need for creating or editing multiple files between playing and developing sessions - that change is affecting Mapper the most, as it have very little own settings
    - main section name is the same as original filename, without any suffix
        - exception: Client still uses `Game options`, instead of `Client`, for compatibility with _FOConfig_; it will be changed in future version together with introducing config tool changes
    - detail sections names are always the same as applications original filenames
- extensions
    - `uint8 Critter::Access` - _const_ removed
- scripts
    - added global define `__VERSION`, for compatibility with FOnline
    - `int Critter::GetAccess()` changed to `uint8 Critter::GetAccess()`
    - added `void Critter::SetAccess( uint8 access )` for changing players access level
        - works for player characters only
        - `player_getaccess()` is not called
        - custom access level (`access > ACCESS_ADMIN`) are not allowed to use `RunServerScript()`
- additional info
    - project has been integrated with [AppVeyor](https://www.appveyor.com/) service, and configured to prepare (pre)release packages after succesful compilation; that allows anyone interested to use development version of each and all applications without waiting for official releases - which, from this version, will use binaries compiled by external services  
      https://ci.appveyor.com/project/rotators/foclassic/
        - binaries can be found under `Image: Visual Studio 2013` -> `Artifacts`; despite of the image name, _Visual Studio 2010_ is used for compilation
        - `Image: Visual Studio 2017` is used for checking if engine is ready for migration to VS2017; it most likely will take some time, so it can be safely ignored currently


#### Server.cfg fragment
```ini
# List of all supported languages separated by whitespace; if empty/unset, Language_0/1/... is used
# First language on list is used as default
# Length of each language name must be exactly four letters
Languages = engl russ
```


## [v1](https://github.com/rotators/foclassic/releases/tag/v1/)
- Hello world!

FOClassic is born; see [official announcement](https://fodev.net/forum/index.php/topic,30043.0.html) for detailed information about the project.

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
