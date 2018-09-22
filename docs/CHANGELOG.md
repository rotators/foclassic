## CHANGELOG

### [v1](SOON) (WIP)
- Hello world!
  
  `TODO` short intro with link to announcement on FOdev
  
- changed generated binary files signatures, files created by FOnline are explicitly ignored
  - results in enforcing clean state on all servers willing to use applications in this project; migrating from r412 keeping saves, while technically possible, is not adviced, support, or good idea at all
  - see [binary files documentation](BinaryFiles.md#signature) for detailed description
- security fix for registration parameters
- removed StlPort support
- simplified logic for detecting outdated scripts bytecode
  scripts are silently recompiled from source whenever any mismatch is detected
- core applications (ASCompiler, Client, Mapper, Server) now share global version number
  - version of Client and Server **must** match; adjusting FOGAME.MSG/1024 is advised
  - [windows] version information is added to executables as [VERSIONINFO](https://docs.microsoft.com/en-us/windows/desktop/menurc/versioninfo-resource) in `1.STAGE.VERSION.*` format
    - STAGE - internal version number, increased on completion of major milestones
    - VERSION - public version number, increased on each release; visible in Client/Mapper/Server log
