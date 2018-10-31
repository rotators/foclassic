#ifndef __CMAKE__
#define __CMAKE__

//
// EDITING THIS FILE WILL BREAK COMPATIBILITY WITH OFFICIAL FOCLASSIC RELEASES
//
// Non-template version of this file is used for generating "global hash",
// which serves as a base for various security checks.
//
// See ?TODO? for details.
//
//

// Version //

// increased on major milestones
#define FOCLASSIC_STAGE                    (@FOCLASSIC_STAGE@)

// increased on release
#define FOCLASSIC_VERSION                  (@FOCLASSIC_VERSION@)

#define FOCLASSIC_STAGE_STRING             "@FOCLASSIC_STAGE@"
#define FOCLASSIC_VERSION_STRING           "@FOCLASSIC_VERSION@"

// VersionInfo.rc //

#define FOCLASSIC_FILEVERSION              1, @FOCLASSIC_STAGE@, @FOCLASSIC_VERSION@, 0
#define FOCLASSIC_PRODUCTVERSION           1, @FOCLASSIC_STAGE@, 0, 0
#define FOCLASSIC_FILEVERSION_STRING       "1.@FOCLASSIC_STAGE@.@FOCLASSIC_VERSION@"
#define FOCLASSIC_PRODUCTVERSION_STRING    "1.@FOCLASSIC_STAGE@ (@FOCLASSIC_CODENAME@)"

#endif // __CMAKE__ //
