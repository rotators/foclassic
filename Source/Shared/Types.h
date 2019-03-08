#ifndef ___TYPES___
#define ___TYPES___

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <string>
#include <strstream>
#include <vector>

#include "PlatformSpecific.h"

#if defined (FO_WINDOWS)
# define NOMINMAX
# include <windows.h>
#endif

#if defined (FO_MSVC)
typedef signed char      int8;
typedef unsigned char    uint8;
typedef signed short     int16;
typedef unsigned short   uint16;
typedef signed int       int32;
typedef unsigned int     uint32;
typedef __int64          int64;
typedef unsigned __int64 uint64;
typedef unsigned int     uint;
#elif defined (FO_GCC)
# include <inttypes.h>
typedef signed char      int8;
typedef unsigned char    uint8;
typedef signed short     int16;
typedef unsigned short   uint16;
typedef signed int       int32;
typedef unsigned int     uint32;
typedef int64_t          int64;
typedef uint64_t         uint64;
typedef unsigned int     uint;
#endif

#define PAIR( k, v )    pair<decltype(k), decltype(v)>( k, v )

typedef std::map<std::string, uint8>       StrUInt8Map; //
typedef std::map<uint8, std::string>       UInt8StrMap; //
typedef std::map<std::string, std::string> StrMap;
typedef std::map<uint, std::string>        UIntStrMap;
typedef std::map<std::string, uint16>      StrUInt16Map;
typedef std::map<std::string, uint>        StrUIntMap;
typedef std::map<std::string, void*>       StrPtrMap;
typedef std::map<uint16, std::string>      UInt16StrMap; //
typedef std::map<std::string, uint>        StrUIntMap;
typedef std::map<uint, uint>               UIntMap;
typedef std::map<int, int>                 IntMap;
typedef std::map<int, float>               IntFloatMap;
typedef std::map<int, void*>               IntPtrMap;
typedef std::map<uint, float>              UIntFloatMap;
typedef std::map<uint16, uint>             UInt16UIntMap;

typedef std::multimap<uint, std::string>   UIntStrMulMap;

typedef std::vector<void*>                 PtrVec;
typedef std::vector<int>                   IntVec;
typedef std::vector<uint8>                 UInt8Vec;
typedef std::vector<int16>                 Int16Vec; //
typedef std::vector<uint16>                UInt16Vec;
typedef std::vector<uint>                  UIntVec;
typedef std::vector<char>                  CharVec;
typedef std::vector<std::string>           StrVec;
typedef std::vector<char*>                 PCharVec;
typedef std::vector<uint8*>                PUInt8Vec;
typedef std::vector<float>                 FloatVec;
typedef std::vector<uint64>                UInt64Vec;
typedef std::vector<bool>                  BoolVec;

typedef std::set<std::string>              StrSet;
typedef std::set<uint8>                    UInt8Set; //
typedef std::set<uint16>                   UInt16Set;
typedef std::set<uint>                     UIntSet;
typedef std::set<int>                      IntSet;

typedef std::pair<int, int>                IntPair;
typedef std::pair<uint16, uint16>          UInt16Pair;
typedef std::pair<uint, uint>              UIntPair;
typedef std::pair<char, char>              CharPair;  //
typedef std::pair<char*, char*>            PCharPair; //
typedef std::pair<uint8, uint8>            UInt8Pair; //

typedef std::vector<UInt16Pair>            UShortPairVec;
typedef std::vector<IntPair>               IntPairVec;
typedef std::vector<UIntPair>              UIntPairVec;
typedef std::vector<PCharPair>             PCharPairVec; //
typedef std::vector<UInt8Pair>             UInt8PairVec; //

#endif // ___TYPES___ //
