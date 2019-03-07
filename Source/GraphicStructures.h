#ifndef __GRAPHIC_STRUCTURES__
#define __GRAPHIC_STRUCTURES__

#include "Assimp/aiTypes.h"
#include "Assimp/aiScene.h"

#include "Core.h"

#include "FlexRect.h"
#include "Text.h" // TODO remove

typedef aiMatrix4x4             Matrix;
typedef aiVector3D              Vector;
typedef aiQuaternion            Quaternion;
typedef aiColor4D               Color;
typedef std::vector<Vector>     VectorVec;
typedef std::vector<Quaternion> QuaternionVec;
typedef std::vector<Matrix>     MatrixVec;
typedef std::vector<Matrix*>    MatrixPtrVec;

#ifdef FO_D3D
# include <dxerr.h>
# include <d3dx9.h>

# define COLOR_FIX( c )           (c)

# define Device_                  LPDIRECT3DDEVICE9
# define Surface_                 LPDIRECT3DSURFACE9
# define EffectValue_             D3DXHANDLE
# define Material_                D3DMATERIAL9
# define PresentParams_           D3DPRESENT_PARAMETERS
# define Caps_                    D3DCAPS9
# define VertexBuffer_            LPDIRECT3DVERTEXBUFFER9
# define IndexBuffer_             LPDIRECT3DINDEXBUFFER9
# define PixelShader_             IDirect3DPixelShader9
# define ConstantTable_           ID3DXConstantTable
# define SkinInfo_                LPD3DXSKININFO
# define Light_                   D3DLIGHT9
# define ViewPort_                D3DVIEWPORT9
# define LockRect_                D3DLOCKED_RECT

# define D3D_HR( expr )           { HRESULT hr__ = expr; if( hr__ != D3D_OK ) { WriteLogF( _FUNC_, " - " # expr ", error<%s - %s>.\n", DXGetErrorString( hr__ ), DXGetErrorDescription( hr__ ) ); return 0; } }
# define MATRIX_TRANSPOSE( m )    m.Transpose()
#else
# include <GL/glew.h>
# define GL( expr )               { expr; if( GameOpt.OpenGLDebug ) { GLenum err__ = glGetError(); if( err__ != GL_NO_ERROR ) { WriteLogF( _FUNC_, " - " # expr ", error<0x%08X - %s>.\n", err__, gluErrorString( err__ ) ); ExitProcess( 0 ); } } }
# ifdef FO_WINDOWS
#  include <GL/wglew.h>

#  define WGL( expr )             { if( !(expr) ) { if( GameOpt.OpenGLDebug ) { WriteLogF( _FUNC_, " - " # expr ", error<0x%08X>.\n", GetLastError() ); ExitProcess( 0 ); } } }
# else
#  include <GL/glxew.h>
# endif

# define COLOR_FIX( c )           COLOR_ARGB( ( (uint8*)&(c) )[3], ( (uint8*)&(c) )[0], ( (uint8*)&(c) )[1], ( (uint8*)&(c) )[2] )
# define Device_                  GLuint
# define Surface_                 GLuint
# define EffectValue_             GLint
# define Material_                GLuint
# define PresentParams_           GLuint
# define Caps_                    GLuint
# define VertexBuffer_            GLuint
# define IndexBuffer_             GLuint
# define PixelShader_             GLuint
# define ConstantTable_           GLuint
# define SkinInfo_                GLuint
# define Light_                   GLuint
# define ViewPort_                GLuint
# define LockRect_                GLuint

# define MATRIX_TRANSPOSE( m )
#endif

struct Texture
{
    const char*        Name;

    #ifdef FO_D3D
    LPDIRECT3DTEXTURE9 Instance;
    Texture() : Name( NULL ), Instance( NULL ) {}
    ~Texture() { SAFEREL( Instance ); }
    #else
    GLuint             Id;
    uint8*             Data;
    uint               Size;
    uint               Width;
    uint               Height;
    float              SizeData[4];   // Width, Height, TexelWidth, TexelHeight
    float              Samples;
    # ifdef FO_WINDOWS
    HPBUFFERARB        PBuffer;
    # endif
    Texture();
    ~Texture();
    inline uint& Pixel( uint x, uint y ) { return *( (uint*)Data + y * Width + x ); }
    bool         Update();
    bool         Update( const Rect& r );
    #endif
};

struct EffectDefault
{
    enum EType { String, Floats, Dword };
    char*  Name;
    EType  Type;
    uint8* Data;
    uint   Size;
};

struct EffectInstance
{
    char*          EffectFilename;
    EffectDefault* Defaults;
    uint           DefaultsCount;
};

struct Effect
{
    int            Id;
    const char*    Name;
    const char*    Defines;

    #ifdef FO_D3D
    LPD3DXEFFECT   DXInstance;
    uint           EffectFlags;
    EffectValue_   EffectParams;
    EffectValue_   TechniqueSkinned;
    EffectValue_   TechniqueSkinnedWithShadow;
    EffectValue_   TechniqueSimple;
    EffectValue_   TechniqueSimpleWithShadow;
    EffectValue_   LightDir;
    EffectValue_   LightDiffuse;
    #else
    GLuint         Program;
    uint           Passes;
    GLint          ZoomFactor;
    GLint          ColorMap;
    GLint          ColorMapSize;
    GLint          ColorMapSamples;
    GLint          EggMap;
    GLint          EggMapSize;
    # ifdef SHADOW_MAP
    GLint          ShadowMap;
    GLint          ShadowMapSize;
    GLint          ShadowMapSamples;
    GLint          ShadowMapMatrix;
    # endif
    GLint          SpriteBorder;
    #endif

    EffectDefault* Defaults;
    EffectValue_   ProjectionMatrix;
    EffectValue_   BoneInfluences;
    EffectValue_   GroundPosition;
    EffectValue_   MaterialAmbient;
    EffectValue_   MaterialDiffuse;
    EffectValue_   WorldMatrices;
    EffectValue_   WorldMatrix;

    // Automatic variables
    bool           IsNeedProcess;
    EffectValue_   PassIndex;
    bool           IsTime;
    EffectValue_   Time;
    float          TimeCurrent;
    double         TimeLastTick;
    EffectValue_   TimeGame;
    float          TimeGameCurrent;
    double         TimeGameLastTick;
    bool           IsRandomPass;
    EffectValue_   Random1Pass;
    EffectValue_   Random2Pass;
    EffectValue_   Random3Pass;
    EffectValue_   Random4Pass;
    bool           IsRandomEffect;
    EffectValue_   Random1Effect;
    EffectValue_   Random2Effect;
    EffectValue_   Random3Effect;
    EffectValue_   Random4Effect;
    bool           IsTextures;
    EffectValue_   Textures[EFFECT_TEXTURES];
    #ifndef FO_D3D
    EffectValue_   TexturesSize[EFFECT_TEXTURES];
    #endif
    bool           IsScriptValues;
    EffectValue_   ScriptValues[EFFECT_SCRIPT_VALUES];
    bool           IsAnimPos;
    EffectValue_   AnimPosProc;
    EffectValue_   AnimPosTime;

    // Default effects
    static Effect* Contour, * ContourDefault;
    static Effect* Generic, * GenericDefault;
    static Effect* Critter, * CritterDefault;
    static Effect* Tile, * TileDefault;
    static Effect* Roof, * RoofDefault;
    static Effect* Rain, * RainDefault;
    static Effect* Iface, * IfaceDefault;
    static Effect* Primitive, * PrimitiveDefault;
    static Effect* Light, * LightDefault;
    static Effect* FlushRenderTarget, * FlushRenderTargetDefault;
    static Effect* FlushRenderTargetMS, * FlushRenderTargetMSDefault;
    static Effect* FlushPrimitive, * FlushPrimitiveDefault;
    static Effect* FlushMap, * FlushMapDefault;
    static Effect* Font, * FontDefault;
    static Effect* Simple3d, * Simple3dDefault;
    static Effect* Skinned3d, * Skinned3dDefault;
    static Effect* Simple3dShadow, * Simple3dShadowDefault;
    static Effect* Skinned3dShadow, * Skinned3dShadowDefault;
};
#ifdef FO_D3D
# define IS_EFFECT_VALUE( pos )                 ( (pos) != NULL )
# define SET_EFFECT_VALUE( eff, pos, value )    eff->DXInstance->SetFloat( pos, value )
#else
# define IS_EFFECT_VALUE( pos )                 ( (pos) != -1 )
# define SET_EFFECT_VALUE( eff, pos, value )    GL( glUniform1f( pos, value ) )
#endif

#ifdef FO_D3D
struct MeshContainer
{

    ID3DXMesh*      InitMesh;
    ID3DXMesh*      SkinMesh;
    ID3DXMesh*      SkinMeshBlended;
    Material_*      Materials;
    EffectInstance* Effects;
    uint            NumMaterials;
    uint*           Adjacency;
    SkinInfo_       Skin;
    char**          TextureNames;
    Matrix*         BoneOffsets;
    Matrix**        FrameCombinedMatrixPointer;
    ID3DXBuffer*    BoneCombinationBuf;
    uint            NumAttributeGroups;
    uint            NumPaletteEntries;
    uint            NumInfluences;
};
typedef std::vector<MeshContainer*> MeshContainerVec;

struct Frame
{
    char*          Name;
    Matrix         TransformationMatrix;
    Matrix         CombinedTransformationMatrix;
    MeshContainer* DrawMesh;
    Frame*         Sibling;
    Frame*         FirstChild;

    const char*    GetName()
    {
        return Name;
    }

    Frame* Find( const char* name )
    {
        if( Name && Str::Compare( Name, name ) )
            return this;
        Frame* frame = NULL;
        if( Sibling )
            frame = Sibling->Find( name );
        if( !frame && FirstChild )
            frame = FirstChild->Find( name );
        return frame;
    }
};
typedef std::vector<Frame*> FrameVec;
typedef ID3DXMesh           MeshSubset;
#else
struct Vertex3D
{
    Vector Position;
    float  PositionW;
    Vector Normal;
    float  Color[4];
    float  TexCoord[2];
    float  TexCoord2[2];
    float  TexCoord3[2];
    Vector Tangent;
    Vector Bitangent;
    float  BlendWeights[4];
    float  BlendIndices[4];
    float  Padding[1];
};
static_assert( sizeof(Vertex3D) == 128, "Wrong Vertex3D size." );
typedef std::vector<Vertex3D>    Vertex3DVec;
typedef std::vector<Vertex3DVec> Vertex3DVecVec;

struct MeshSubset
{
    Vertex3DVec    Vertices;
    Vertex3DVec    VerticesTransformed;
    bool           VerticesTransformedValid;
    uint           FacesCount;
    UInt16Vec      Indicies;
    std::string    DiffuseTexture;
    float          DiffuseColor[4];
    float          AmbientColor[4];
    float          SpecularColor[4];
    float          EmissiveColor[4];
    uint           BoneInfluences;
    MatrixVec      BoneOffsets;
    MatrixPtrVec   FrameCombinedMatrixPointer;
    EffectInstance DrawEffect;
    GLuint         VAO, VBO, IBO;
};
typedef std::vector<MeshSubset> MeshSubsetVec;

struct Frame;
typedef std::vector<Frame*>     FrameVec;
struct Frame
{
    std::string   Name;
    Matrix        TransformationMatrix;
    Matrix        CombinedTransformationMatrix;
    MeshSubsetVec Mesh;
    FrameVec      Children;

    const char*   GetName()
    {
        return Name.c_str();
    }

    Frame* Find( const char* name )
    {
        const char* frame_name = Name.c_str();
        if( Str::Compare( frame_name, name ) )
            return this;
        for( uint i = 0; i < Children.size(); i++ )
        {
            Frame* frame = Children[i]->Find( name );
            if( frame )
                return frame;
        }
        return NULL;
    }
};
#endif

typedef std::vector<Texture*> TextureVec;
typedef std::vector<Effect*>  EffectVec;

#endif // __GRAPHIC_STRUCTURES__
