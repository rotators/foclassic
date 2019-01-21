#ifndef __CMAKE__
#define __CMAKE__

//
// EDITING THIS FILE WILL BREAK COMPATIBILITY WITH OFFICIAL FOCLASSIC RELEASES
//
// Non-template version of this file is used for generating "global seed"; see Customization.md for details.
//

// Version //

// increased on major milestones
#define FOCLASSIC_STAGE                    (@FOCLASSIC_STAGE@)

// increased on release
#define FOCLASSIC_VERSION                  (@FOCLASSIC_VERSION@)

// game-specific version
#define FOCLASSIC_VERSION_CUSTOM           (@FOCLASSIC_VERSION_CUSTOM@)

// strings
#define FOCLASSIC_STAGE_STRING             "@FOCLASSIC_STAGE@"
#define FOCLASSIC_VERSION_STRING           "@FOCLASSIC_VERSION@"
#define FOCLASSIC_VERSION_CUSTOM_STRING    "@FOCLASSIC_VERSION_CUSTOM@"

// VersionInfo.rc //

#define FOCLASSIC_FILEVERSION              1, @FOCLASSIC_STAGE@, @FOCLASSIC_VERSION@, @FOCLASSIC_VERSION_CUSTOM@
#define FOCLASSIC_PRODUCTVERSION           1, @FOCLASSIC_STAGE@, 0, 0
#define FOCLASSIC_FILEVERSION_STRING       "1.@FOCLASSIC_STAGE@.@FOCLASSIC_VERSION@.@FOCLASSIC_VERSION_CUSTOM@"
#define FOCLASSIC_PRODUCTVERSION_STRING    "1.@FOCLASSIC_STAGE@ (@FOCLASSIC_CODENAME@)"

#endif // __CMAKE__ //
