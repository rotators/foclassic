# CHANGELOG

## [v1](SOON) (WIP)

- Hello world!
  
  `TODO` short intro with link to announcement on FOdev
- changed generated binary files signatures, files created by FOnline are explicitly ignored
  - results in enforcing clean state on all servers willing to use applications in this project; migrating from r412 keeping saves, while technically possible, is not adviced, supported, or good idea at all
  - see [binary files documentation](https://github.com/rotators/foclassic/blob/master/Docs/docs/BinaryFiles.md#signature) for detailed description
- removed StlPort support
- security fix for registration parameters
- hidden trade crash fix
- numeric commands removed
- simplified logic for detecting outdated scripts bytecode
  - recompile scripts from source whenever any mismatch is detected (older version, source newer than bytecode, etc.)
  - removed warning message
- core applications (ASCompiler, Client, Mapper, Server) now share global version number
  - version of Client and Server **must** match; adjusting FOGAME.MSG/1024 is advised
  - [windows] version information is added to executables as [VERSIONINFO](https://docs.microsoft.com/en-us/windows/desktop/menurc/versioninfo-resource) in `1.STAGE.VERSION.*` format
    - STAGE - internal version number, increased on completion of major milestones
    - VERSION - public version number, increased on each release; visible in Client/Mapper/Server log
- _magic numbers_ coming from engine, previously kept in \_defines.fos, has been moved to separated file (_foclassic.fos_)
  - it is strongly adviced to use provided file as read-only header, both for scripts and and extensions; long-term goal is to eliminate need for editing _foclassic.fos_ and _foclassic.h_, server-specific customizations should be taken into account
  - whenever value from engine might be used for server-specific values (like custom item types), marker is added with suffix `LAST` and pointing at last value used by engine; please report if any of markers is missing
    - example: `#define ITEM_TYPE_FOOD (ITEM_TYPE_LAST+1)`
- extensions changes
  - removed functions related to item deterioration flags not used by engine
    - `bool Item::IsNoResc();`
    - `bool Item::IsService();`
    - `bool Item::IsServiceExt();`
    - `bool Item::IsEternal();`
- scripts changes
  - disabled `bool SwapCritters(Critter& cr1, Critter& cr2, bool withInventory, bool withVars)`
  - default date changed to 08.09.2078, 09:44
- temporary changes; will be reverted in future versions
  - script string debugging disabled
  - profiler disabled
