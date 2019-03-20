# CHANGELOG


Complete list of changes in FOClassic since [FOnline SDK r412](https://github.com/cvet/fonline/commit/3e58c94f1234173d5af234eb1f80ad0af8f3c120).
- [Latest release](https://github.com/rotators/foclassic/releases/latest/)
- [Nightly builds](https://ci.appveyor.com/project/rotators/foclassic/) (Linux, Windows)


## [v7]() (WIP)

- fixed `FONT_TYPE_DIALOG` handling when it's using different size than `FONT_TYPE_DEFAULT`
- _script.cfg_ changed to INI format
    - all configuration in closed within `[APP scripts]` and `[APP binds]` sections, where `APP` is one of following: `Client`, `Mapper`, `Server`
        - `[APP scripts]` section is optional, `[APP binds]` section is required
        - any other sections added to _scripts.cfg_ are ignored
    - script modules are now defined in `path/to/script = [option(s)]` format
        - currently only two options are available: `load` and `skip` (both can be lowercase or uppercase for better visibility)
        - `load` adds module to list of game scripts
        - `skip` forces engine to ignore given module, even if `load` option is present
    - module options can be defined in any order
    - reserved functions are now defined in `bind_name = [target] <type>` format
        - `bind_name` : name of reserved function
            - special name, `all`, can be used to bind all reserved functions to single target; skips binds which are already configured by previous entries
        - `target` : name of script/extension which will handle reserved function; cannot contain whitespace(s)
        - `type` : defines how given bind will be handled
            - `script` : reserved function is handled by script function; default if `<type>` is not set
            - `extension` : reserved function is handled by extension
    - all configuration sections mentioned ignores empty entries
    - see bottom of changelog for examples
- _dialogs.lst_ renamed to _dialogs.cfg_
- _dialogs.cfg_ changed to INI format
    - all dialogs must be defined in `Dialogs` section; any other sections, if present, are ignored and can be used for game-specific settings
    - key name should be a path to dialog file (without extension), key value defines dialog id; examples: `all_alco = 1`, `hub/keri = 1000`
    - dialogs defined without/with invalid id (not a number, value less or equal 0) are silently ignored
- Server will continue initialization even if _dialogs.cfg_ is missing
- Server option `GameSleep` can be set from command line
- added Server option `ScriptsDir`; default: `scripts`
    - allows to change directory containing game scripts
    - directory name cannot start with `.`, `/`, or `\\` characters
    - can be set from command line
- ASCompiler improvements
    - command line options consitent with other applications (double dash, first letter uppercased)
    - command line options does not need to follow any specific order
    - option `client` renamed to `Client`
    - option `mapper` renamed to `Mapper`
    - option `d` renamed to `Define`; needs to be set only once - to add more than one definition separate their names with comma
    - option `gc` renamed to `GC`
    - option `p` renamed to `Output`
    - option `run` renamed to `Run`; needs to be set only once - to call more than one function separate their names with comma
    - added option `Server`, required to compile server scripts
    - single mode
        - compiling single script requires target-specific option with script filename as argument; examples:  
          `ASCompiler client_main.fos -client` (old) -> (current) `ASCompiler --Client client_main.fos`  
          `ASCompiler main.fos` (old) -> (current) `ASCompiler --Server main.fos`
    - introduced batch mode
        - ASCompiler can now run in batch mode, processing all used scripts for given application
        - added option `ScriptsCfg` which process scripts configuration in same way as Server during initialization
        - option defining compilation target (`Client`/`Mapper`/`Server`) must be present; example:
         `ASCompiler --ScriptsCfg --Client`
        - if `Run` option is present, given function(s) will be executed for all scripts
        - any error spotted during script processing (reported by either AngelScript or ASCompiler itself) results with stopping ASCompiler
    - added option `Time` which enabled more detailed time statistics
        - `Preprocess` reports time consumed by preprocessing script only
        - `Build` reports time consumed by creating and building script module only
        - `Compile` reports total time consumed to create module from source; that includes `Preprocess` and `Build` reports, as well as additional steps required to prepare/validate script; `Compile` report is used when `Time` option is not present (old bahavior)
        - in batch mode, summary of reports mentioned above is displayed (plus some extra info) after all scripts has been processed

Example of scripts configuration
```ini
[Server scripts]
events       =           # ignored - empty key
npc_planes   = load      # loaded  - option 'load' present
cheats       = skip      # ignored - option 'skip' present
events_utils = skip load # ignored - option 'skip' have higher priority than option 'load'
```

Example of using bind 'all'; all reserved functions will point to _client/main_, except `start` and `finish`
```ini
[Client scripts]
client/main = load
client/run  = load

[Client binds]
all    = client/main
start  = client/run
finish = client/run
```

Example of using reserved function with dynamic library (previously: `@ server bind check_look CheckLook.dll`)
```ini
[Server binds]
check_look = CheckLook extension
```


## [v6](https://github.com/rotators/foclassic/releases/tag/v6/)

- Client cache contains more detailed informations about engine version used to compile scripts
    - new cache is requested if cached version info does not match Client
- all players are disconnected when reloading client scripts
- scripts changes
    - import errors slightly more developer-friendly
    - when possible, AngelScript error codes are converted to their names from _angelscript.h_ (`asINVALID_CONFIGURATION`, `asOUT_OF_MEMORY` etc.)
    - [Server] with `VerboseInit` enabled, all imports are displayed in log
    - [Server] dependencies saved in _.fosb_ files are relative to scripts directory
    - [Client, Server] precompiled scripts (cache/_.fosb_ files) are saved with version of AngelScript used to compile them; in case of version mismatch:
        - [Client] new cache is requested
        - [Server] script is compiled from source
    - [Client, Server] importing functions from other modules is done only if no errors has been detected during scripts loading
- temporary changes
    - warnings during scripts compilation are treated as errors (enabled during AngelScript updates, disabled in release)
- additional notes
    - as this release contains more than one AngelScript update, it has been splitted into smaller parts (marked with git tags) which should be easier to handle for everyone
    - table below contains links to official AngelScript announcements (_Version_) as well as script engine repository (_Revision_); other than that, it's recommended to read [more detailed changelog](http://angelcode.com/angelscript/changes.php?ver=2) - especially sections _script language_ and _library interface_ (in case of extensions workin with _angelscript.h_)
    - for long-running servers, it's recommended to update engine tag-by-tag; links to compiled binaries are provided (note that AppVeyor keeps artifacts for about 6 months)

| Version                                                  | Revision                                                             | Tag                                                                                       | Binaries                                                           |
|:--------------------------------------------------------:|:--------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------:|:------------------------------------------------------------------:|
| [2.26.1](https://www.gamedev.net/forums/topic/639686--/) | [r1582](http://svn.code.sf.net/p/angelscript/code/trunk/sdk/?p=1582) | [v6-angelscript-2.26.1](https://github.com/rotators/foclassic/tree/v6-angelscript-2.26.1) | https://ci.appveyor.com/project/rotators/foclassic/builds/21911136 |
| [2.26.2](https://www.gamedev.net/forums/topic/641123--/) | [r1602](http://svn.code.sf.net/p/angelscript/code/trunk/sdk/?p=1602) | [v6-angelscript-2.26.2](https://github.com/rotators/foclassic/tree/v6-angelscript-2.26.2) | https://ci.appveyor.com/project/rotators/foclassic/builds/21912852 |
| [2.26.3](https://www.gamedev.net/forums/topic/642897--/) | [r1633](http://svn.code.sf.net/p/angelscript/code/trunk/sdk/?p=1633) | [v6-angelscript-2.26.3](https://github.com/rotators/foclassic/tree/v6-angelscript-2.26.3) | https://ci.appveyor.com/project/rotators/foclassic/builds/21913947 |
| [2.27.0](https://www.gamedev.net/forums/topic/645977--/) | [r1682](http://svn.code.sf.net/p/angelscript/code/trunk/sdk/?p=1682) | [v6-angelscript-2.27.0](https://github.com/rotators/foclassic/tree/v6-angelscript-2.27.0) | https://ci.appveyor.com/project/rotators/foclassic/builds/21952914 |
| [2.27.1](https://www.gamedev.net/forums/topic/646723--/) | [r1699](http://svn.code.sf.net/p/angelscript/code/trunk/sdk/?p=1699) | [v6](https://github.com/rotators/foclassic/tree/v6)                                       | https://github.com/rotators/foclassic/releases/tag/v6/             |


## [v5](https://github.com/rotators/foclassic/releases/tag/v5/)

- fixed loading of .fosb files when script source cannot be found
    - dependencies are not checked if main .fos file does not exists
- all applications will search for config file in following order:
    - executable name
    - original name, including suffix (ClientDX.cfg, MapperGL.cfg, etc.)
    - original name, excluding suffix (Client.cfg, Mapper.cfg, Server.cfg)
    - FOClassic.cfg
    - FOnline.cfg (temporary, file will be ignored in future)
- added Server setting `VerboseInit` (boolean), false by default
    - if enabled, Server prints more details about some tasks (dialogs/maps/prototypes/scripts loading)
- removed Server setting `Language_N`, use `Languages` instead; see [v2 changelog](https://rotators.fodev.net/foclassic/changelog/CHANGELOG.md#v2) for detailed info
- Client log filename changed to `Client.log`
- extensions changes
    - `GameOptions* FOnline` removed, use `FOClassic` instead
    - `GameOptions::FoDataPath` renamed to `GameOptions::DataPath`
    - `ProtoItem::IsItem()` consistent with engine-side check
- scripts changes
    - [Client] global variables `__Gmap*` renamed to `__Worldmap*`
    - [Server] global function `bool SwapCritters(Critter& cr1, Critter& cr2, bool withInventory, bool withVars)` removed
    - [Client, Mapper] global variable `__FoDataPath` renamed to `__DataPath`
    - [Client, Server] global variable `__GlobalMapMaxGroupCount` renamed to `__WorldmapMaxGroupCount`
- extensions/scripts changes (_FOClassic.fos_)
    - multiple defines has been renamed; see tables below for a complete list
    - added `DIK` defines
    - changed values of `COMMAND` defines
    - changed values of `DRAW_PRIMITIVE` defines
- interface changes
    - `Gmap*` entries renamed to `Worldmap*`
    - `LMenuGmap*` entries renamed to `LMenuWorldmap*`
- additional notes
    - as mentioned in [announcement](https://fodev.net/forum/index.php/topic,30043.0.html), this release end migration stage and starts regular development where bigger and/or more demanding changes can and will be introduced; to avoid "r413+ situation" some of bigger reworks might be introduced across multiple releases, making progress somewhat slower but (hopefully) easier to grasp


| Old prefix       | New prefix          |
|------------------|---------------------|
| `ACTION`         | `CRITTER_ACTION     |
| `CMD`            | `COMMAND`           |
| `CONT`           | `CONTAINER`         |
| `COND`           | `CRITTER_CONDITION` |
| `FCRIT`          | `CRITTER_FLAG`      |
| `FH`             | `HEX_FLAG`          |
| `FOMB`           | `MSGBOX`            |
| `FONT`           | `FONT_TYPE`         |
| `FT`             | `FONT_FLAG`         |
| `GLOBAL_PROCESS` | `WORLDMAP_PROCESS`  |
| `GM_FOG`         | `WORLDMAP_FOG`      |
| `GM_WALK`        | `WORLDMAP_WALK`     |
| `ITEM`           | `ITEM_FLAG`         |
| `ITEMS`          | `ITEM_COLLECTION`   |
| `MESSAGE`        | `SENDMESSAGE`       |
| `PT`             | `PATH`              |
| `SCREEN`         | `SHOW_SCREEN`       |
| `VAR`            | `VAR_TYPE`          |

| Old name                        | New name                      |
|---------------------------------|-------------------------------|
| `CHOSEN_MOVE_TO_CRIT`           | `CHOSEN_MOVE_TO_CRITTER`      |
| `CHOSEN_MOVE_ITEM_CONT`         | `CHOSEN_MOVE_ITEM_CONTAINER`  |
| `CHOSEN_USE_SKL_ON_CRITTER`     | `CHOSEN_USE_SKILL_ON_CRITTER` |
| `CHOSEN_USE_SKL_ON_ITEM`        | `CHOSEN_USE_SKILL_ON_ITEM`    |
| `CHOSEN_USE_SKL_ON_SCEN`        | `CHOSEN_USE_SKILL_ON_SCENERY` |
| `CHOSEN_PICK_CRIT`              | `CHOSEN_PICK_CRITTER`         |
| `CHOSEN_WRITE_HOLO`             | `CHOSEN_WRITE_HOLODISK`       |
| `CLIENT_MAIN_SCREEN_GLOBAL_MAP` | `CLIENT_MAIN_SCREEN_WORLDMAP` |
| `CLIENT_SCREEN_PIP_BOY`         | `CLIENT_SCREEN_PIPBOY`        |
| `CLIENT_SCREEN_FIX_BOY`         | `CLIENT_SCREEN_FIXBOY`        |
| `CLIENT_SCREEN_CHA_NAME`        | `CLIENT_SCREEN_CHAR_NAME`     |
| `CLIENT_SCREEN_CHA_AGE`         | `CLIENT_SCREEN_CHAR_AGE`      |
| `CLIENT_SCREEN_CHA_SEX`         | `CLIENT_SCREEN_CHAR_SEX`      |
| `CLIENT_SCREEN_GM_TOWN`         | `CLIENT_SCREEN_WM_TOWN`       |
| `CLIENT_SCREEN_INPUT_BOX`       | `CLIENT_SCREEN_INPUTBOX`      |
| `CLIENT_SCREEN_TOWN_VIEW`       | `CLIENT_SCREEN_WM_TOWNVIEW`   |
| `CLIENT_SCREEN_SAVE_LOAD`       | `CLIENT_SCREEN_LOADSAVE`      |
| `CRITTER_ONLY_NAME`             | `CRITTER_LOOK_NAME`           |
| `MAX_USES`                      | `USE_MAX`                     |
| `NPC_START_ID`                  | `CRITTER_ID_START_NPC`        |
| `USER_START_ID`                 | `CRITTER_ID_START_PLAYER`     |


## [v4](https://github.com/rotators/foclassic/releases/tag/v4/)

- fixed Client crash when loading _.dat_ files in some scenarios
- fixed ASCompiler crash when receiving warnings/errors from script engine
- language packs are loaded by Server before initializing script system
- Mapper scripts are compiled by Server during initialization
    - errors in Mapper scripts are not considered critical by Server; compilation will be stopped if errors are detected, and initialization will move on to next step
- added `FONT_TYPE_DIALOG`, uses _Dialog.fofnt_
    - font file(s) are not required by default; if _Dialog.fofnt_ cannot be found, _Default.fofnt_ is loaded instead
    - used on dialog/barter screens only, replacing `FONT_TYPE_DEFAULT` usage
    - it's recommended to use `FONT_TYPE_DIALOG` whenever drawing strings over dialog/barter screens (even if game uses default font only) to fully support player-made interfaces
- when reading `WindowName` setting, strings `FOCLASSIC_STAGE` and `FOCLASSIC_VERSION` are converted to real values; affects all core applications which uses this setting
- Mapper window no longer adds engine version to `WindowName` setting
    - to restore previous behaviour use `WindowName = My Game Mapper (vFOCLASSIC_VERSION)`
- reserved functions
    - [Client, Mapper] added `void finish()`, executed before closing application
- extensions changes
    - added `Client::UID[]`
    - added `GameOptionsExt::WallAlpha`
    - `Item::_Data::Rate` added to `FOCLASSIC_EXTENSION_STRIP_CONST` list
- scripts changes
    - preprocessor now understands `#undef` directive
    - added global define `ANGELSCRIPT_VERSION`
      exposes value with same name defined in _angelscript.h_
    - scripts for all targets can be placed in subdirectories
        - path to script (`@ TARGET module` in _scripts.cfg_) can be set using Unix and Windows format, but internally Unix format is used
    - Client:
        - added `void CritterCl::GetNameTextInfo(bool& nameVisible, int& x, int& y, int& w, int& h, int& lines)`  
          returns size and position of name / dialog floater / chat message above critter head
        - added `uint8 CritterCl::Alpha`, `const bool CritterCl::FadingEnable`
    - Server:
        - added `const bool Critter::IsDisconnected` (thanks mojuk)  
          returns true for offline player characters
        - added `uint Critter::GetUID(uint8 index)`  
          returns player's UID0 - UID4
        - added `uint Critter::GetIp()`  
          returns player's IP
        - added `string@ Critter::GetIpString()`  
          returns player's IP in human-readable format
    - Client/Mapper:
        - added global variable `uint8 __WallAlpha`  
          allows to change walls transparency, similiar to `__RoofAlpha`
        - added `void SetZoom(float zoom)`  
          allows to change zoom level easily from scripts


## [v3](https://github.com/rotators/foclassic/releases/tag/v3/)

- dialogs security fix (thanks Skycast)
- ~ban command fix (thanks Skycast)
- information about outdated Client version is always displayed to player
    - [windows] if FOGAME.MSG/1024 cannot be found (eg. cache hasn't been downloaded yet), Client displays [native dialog box](https://docs.microsoft.com/en-us/windows/desktop/api/winuser/nf-winuser-messagebox) with hardcoded message
- extension changes
    - if `FOCLASSIC_EXTENSION_STRIP_CONST` is defined, `const` is removed from selected entries; currently affects only geometry-related variables
- extensions changes (requires Bleeding Edge mode)
    - introduced `GameOptionsExt* FOClassicExt` holding additions to `GameOptions* FOClassic`; this is a temporary change - once migration stage is finished (planned next month) both structures will be merged into one
    - introduced `Ini* FOClassicExt::ConfigFile` containing copy of configuration for currently running application
        - all sections starting with `Client` / `Mapper` / `Server` / `Game Options` are reserved for internal use
        - any additional sections are kept intact, and can be used by servers for game-specific configuration
    - introduced `CmdLine* FOClassicExt::CommandLine` containing copy of command line arguments
    - unlike AngelScript headers, _CommandLine.h_ and _Ini.h_ (required to use `CommandLine`/`ConfigFile`) needs to be included "manually" in extension code
- breaking changes
    - previous Client versions won't be able to communicate with Server properly


## [v2](https://github.com/rotators/foclassic/releases/tag/v2/)

[FOnline](https://github.com/cvet/fonline/) and [FOClassic](https://github.com/rotators/foclassic/) are now fully open-source projects.

- restored loading of worldsaves and characters created by FOnline r412, and possibly older revisions (depends which file format version is used)
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
        - binaries are available for six month since they have been created; see [artifacts retention policy](https://www.appveyor.com/docs/packaging-artifacts/#artifacts-retention-policy)


Server.cfg fragment
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
