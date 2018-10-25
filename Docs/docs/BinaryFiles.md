# BinaryFiles

## SIGNATURE
All binary files created by engine share same signature which contains information about their type and version.

```cpp
#define BINARY_SIGNATURE(name,type,ver)   const unsigned char name[6] = { 'F', 'O', type, ((ver) >> 8) & 0xFF, (ver) & 0xFF, 0 }
#define BINARY_SIGNATURE_VALID(sig1,sig2) (sig1[0] == sig2[0] && sig1[1] == sig2[1] && sig1[2] == sig2[2] && sig1[5] == sig2[5])
#define BINARY_SIGNATURE_VERSION(sig)     ((sig[3] << 8) | sig[4])
```
### Signature types

```cpp
#define BINARY_TYPE_CLIENTSAVE   'C'
#define BINARY_TYPE_MAPSAVE      'M'
#define BINARY_TYPE_PROFILERSAVE 'P'
#define BINARY_TYPE_SCRIPTSAVE   'S'
#define BINARY_TYPE_WORLDSAVE    'W'
#define BINARY_TYPE_CACHE        'c' // reserved 
```

### Validating signature
- Add signature template with current version.
```cpp
#define FILE_FORMAT_V1   (1)
#define FILE_FORMAT_V2   (2)
// ...
#define FILE_FORMAT_LAST (FILE_FORMAT_V10)
BINARY_SIGNATURE( EngineSignature, BINARY_TYPE_XXX, FILE_FORMAT_LAST );
```
- Open file and store first N bytes; if it fails, processing **must** be stopped
```cpp
unsigned char file_signature[sizeof( EngineSignature )];
if( !ReadFile( file, file_signature, sizeof( file_signature )))
	return;
```
- Global version check
```cpp
if( memcmp( EngineSignature, file_signature, sizeof( file_signature )) != 0 )
	return;
```
- Format version check
```cpp
if( memcmp( EngineSignature, file_signature, sizeof( file_signature )) != 0 )
{
	if( !BINARY_SIGNATURE_VALID( EngineSignature, file_signature ))
		return;
	// extract version number
	unsigned char version = BINARY_SIGNATURE_VERSION( file_signature );
	// check supported versions
	if( !version || version < FILE_FORMAT_V5 || version > FILE_FORMAT_LAST )
		return;
	// handle version mismatch
}
```
### Version types
As stated above, engine saves binary files with global version, or format version.
- Global version, also know as _application_ version, is used for files which can be easily recreated during initialization.
- Format version is used for all other scenarios.

In some cases, additional version check is performed by adding version number at the end of file, to make sure it's saved correctly.

|                     | Format             | Global             | EOF                |
|:--------------------|:------------------:|:------------------:|:------------------:|
| BINARY_CLIENTSAVE   | :heavy_check_mark: |                    |                    |
| BINARY_MAPSAVE      |                    | :heavy_check_mark: |                    |
| BINARY_PROFILERSAVE | :heavy_check_mark: |                    |                    |
| BINARY_SCRIPTSAVE   |                    | :heavy_check_mark: |                    |
| BINARY_WORLDSAVE    | :heavy_check_mark: |                    | :heavy_check_mark: |
