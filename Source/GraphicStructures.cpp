#include "Core.h"

#include "GameOptions.h"
#include "GraphicStructures.h"
#include "Log.h"

#ifdef FO_D3D
#else

Texture::Texture() : Name( NULL ), Id( 0 ), Data( NULL ), Size( 0 ), Width( 0 ), Height( 0 ), Samples( 0.0f )
{
    # ifdef FO_WINDOWS
    PBuffer = NULL;
    # endif
}

Texture::~Texture()
{
    GL( glDeleteTextures( 1, &Id ) );
    SAFEDELA( Data );
}

bool Texture::Update()
{
    GL( glBindTexture( GL_TEXTURE_2D, Id ) );
    GL( glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, Width, Height, GL_BGRA, GL_UNSIGNED_BYTE, Data ) );
    return true;
}

bool Texture::Update( const Rect& r )
{
    GL( glBindTexture( GL_TEXTURE_2D, Id ) );
    GL( glTexSubImage2D( GL_TEXTURE_2D, 0, 0, r.T, Width, r.H(), GL_BGRA, GL_UNSIGNED_BYTE, (uint*)Data + r.T * Width ) );
    return true;
}

#endif
