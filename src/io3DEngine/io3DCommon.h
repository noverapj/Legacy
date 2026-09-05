

#ifndef _io3DCommon_h_
#define _io3DCommon_h_

enum PlaySoundMask
{
	PSM_NONE	= 1,
	PSM_LOOP	= 2,
	PSM_THREAD	= 4,
};

enum VertexComponent
{
	IOFVF_POSITION	= (1<<0),
	IOFVF_POSITION2	= (1<<1),
	IOFVF_POSITIONW = (1<<2),
	IOFVF_WEIGHTS	= (1<<3),
	IOFVF_INDICES	= (1<<4),
	IOFVF_NORMAL	= (1<<5),
	IOFVF_COLOR0	= (1<<6),
	IOFVF_COLOR1	= (1<<7),
	IOFVF_UV0		= (1<<8),
	IOFVF_UV1		= (1<<9),
	IOFVF_UV2		= (1<<10),
	IOFVF_UV3		= (1<<11),
	IOFVF_TANGENT	= (1<<12),
	IOFVF_BINORMAL	= (1<<13),
	IOFVF_END
};

enum LightType
{
	LT_POINT		= 1,
	LT_SPOT			= 2,
	LT_DIRECTIONAL	= 3,
};

enum LightReceiveType
{
	LRT_ALL_REAL_TIME,
	LRT_JUST_PURE_REAL_TIME,
	LRT_ALL_VERTEX_COLOR,
	LRT_NO_LIGHT,
};

enum CullingMode
{
	CM_NO_INIT	    = 0,
	CM_NO			= 1,    
	CM_CW			= 2,
	CM_CCW			= 3,	/* default */
};

enum FillMode
{
	FM_POINT		= 1,
	FM_WIREFRAME	= 2,
	FM_SOLID		= 3,	/* default */
};

enum ShadeType
{
	ST_NONE_INIT	= 0,
	ST_FLAT			= 1,
	ST_GOURAUD		= 2,	/* default */
	ST_PHONG		= 3,
};

enum TextureEffectType
{
	TET_ENVIRONMENT_MAP,
	TET_PROJECTIVE_TEXTURE,
	TET_SCROLL,
	TET_ROTATE,
	TET_ANIMATE,
	TET_TRANSFORM,
	TET_COLOR_TRANSFORM,
};

enum TextureFilterOption
{
	TFO_NEAREST,			/* default */
	TFO_BILINEAR,
	TFO_TRILINEAR,
	TFO_ANISOTROPIC,
	TFO_BILINEAR_NOMIPMAP
};

enum TextureAddressMode
{
	TAM_WRAP		= 1,	/* default */
	TAM_MIRROR		= 2,
	TAM_CLAMP		= 3,
};

enum TextureTransformType
{
	TTT_TRANSLATE_U,
	TTT_TRANSLATE_V,
	TTT_SCALE_U,
	TTT_SCALE_V,
	TTT_ROTATE
};

enum ColorTransformType
{
	CTT_AMBIENT,
	CTT_DIFFUSE,
	CTT_SPECULAR,
	CTT_EMISSIVE
};

enum WaveformType
{
    WFT_SINE,
    WFT_TRIANGLE,
    WFT_SQUARE,
    WFT_SAWTOOTH,
    WFT_INVERSE_SAWTOOTH
};

enum SceneBlendMode
{
	BLEND_NO_ALPHA,
	BLEND_SRCALPHA,
	BLEND_COLORKEY,
	BLEND_ADD,
	BLEND_SCREEN,
	BLEND_LIGHTEN,
	BLEND_DARKEN,
	BLEND_REV_SUBTRACT,
	BLEND_SUBTRACT
};

enum BufferType
{
	BT_STATIC,
	BT_MANAGED,
	BT_DYNAMIC,
	BT_SYSTEM
};

enum BufferUsage
{
	BU_AUTO,
	BU_HARDWARE,
	BU_SOFTWARE
};

enum TextAlignType
{
	TAT_LEFT,
	TAT_CENTER,
	TAT_RIGHT
};

enum TextVertAlign
{
	TVA_TOP,
	TVA_CENTER,
	TVA_BOTTOM
};

enum TextStyle
{
	TS_NORMAL,
	TS_SHADOW,
	TS_OUTLINE,
	TS_OUTLINE_2X,
	TS_OUTLINE_FULL,
	TS_OUTLINE_FULL_2X,
	TS_OUTLINE_FULL_3X,
	TS_OUTLINE_FULL_4X,
	TS_BOLD,
	TS_BOLD_SHADOW,
	TS_BOLD_OUTLINE,
	TS_BOLD_OUTLINE_FULL,
	TS_BOLD_OUTLINE_FULL_2X,
	TS_BOLD_OUTLINE_FULL_3X,
	TS_BOLD_OUTLINE_FULL_4X,
};

enum MeshType
{
	MT_STATIC,
	MT_ANIMATION,
	MT_LIGHTMAP,
	MT_BILLBOARD,
	MT_NORMAL_BILLBOARD,
	MT_ANIMATE_EFFECT,
	MT_STATIC_VERTEX_COLOR,
};

enum ParameterType
{
    PT_BOOL,
    PT_FLOAT,
    PT_INT,
    PT_UNSIGNED_INT,
    PT_SHORT,
    PT_UNSIGNED_SHORT,
    PT_LONG,
    PT_UNSIGNED_LONG,
    PT_STRING,
	PT_VECTOR2,
    PT_VECTOR3,
    PT_MATRIX3,
    PT_MATRIX4,
    PT_QUATERNION,
    PT_COLOURVALUE,
	PT_VECTOR_LIST
};

enum ShadowMapType
{
	SMT_NOT_ENABLE,
	SMT_DEPTH_BUF,
	SMT_R32F_TEX,
};

enum FXRenderType
{
	FRT_ADD,
	FRT_SRCALPHA,
	FRT_SCREEN,
	FRT_LIGHTEN,
	FRT_DARKEN,
	FRT_REV_SUBTRACT,
	FRT_SUBTRACT,
};

enum FXTransformType
{
	FXT_OWNER,
	FXT_DERIVED,
};

enum UIRenderType
{
	UI_RENDER_NORMAL,
	UI_RENDER_SCREEN,
	UI_RENDER_GRAY,
	UI_RENDER_ADD,
	UI_RENDER_NORMAL_ADD_COLOR,
	UI_RENDER_COLOR_ALPHA,
	UI_RENDER_LIGHTEN,
	UI_RENDER_DARKEN,
	UI_RENDER_MULTIPLY,
	UI_RENDER_GRAY_LIGHT,
	UI_RENDER_ADD_ALPHA,
};

enum ErasePolicy
{
	EP_AUTOMATIC,
	EP_MANUAL,
	EP_NO_ERASE,
};

enum ResourceLoadState
{
	RLS_NOT_LOADED,
	RLS_LOADING,
	RLS_LOADED,
	RLS_FAILED,
};

enum TextureQuality
{
	TQ_DEFAULT,	// 매니저의 설정대로
	TQ_LOW,		// 16비트, 1/4 사이즈
	TQ_MIDDLE,	// 16비트
	TQ_HIGH,	// 원본그대로, 32비트
};

struct StageBlendOperation
{
	 D3DTEXTUREOP m_StageOp;
	 DWORD	m_dwSrcFactor;
	 DWORD	m_dwDestFactor;

	 StageBlendOperation()
	 {
		 m_StageOp = D3DTOP_MODULATE;
		 m_dwSrcFactor  = D3DTA_TEXTURE;
		 m_dwDestFactor = D3DTA_DIFFUSE;
	 }

	 StageBlendOperation( D3DTEXTUREOP eOp, DWORD dwSrc, DWORD dwDest )
		 : m_StageOp(eOp), m_dwSrcFactor(dwSrc), m_dwDestFactor(dwDest)
	 {
	 }

	 bool operator!=( const StageBlendOperation &rhs ) const
	 {
		 if( m_StageOp != rhs.m_StageOp ||
			 m_dwSrcFactor != rhs.m_dwSrcFactor ||
			 m_dwDestFactor != rhs.m_dwDestFactor )
			 return true;

		 return false;
	 }

	 bool operator==( const StageBlendOperation &rhs ) const
	 {
		 if( *this != rhs )
			 return false;

		 return true;
	 }
};

enum RenderGroupID
{
	RENDER_BACKGROUND = 0,
	
	RENDER_PRE_SKY = 5,
	RENDER_GROUP_1 = 10,
	RENDER_GROUP_2 = 20,
	RENDER_PRE_WORLD = 25,
	RENDER_GROUP_3 = 30,
	RENDER_GROUP_4 = 40,
	RENDER_MAIN = 50,
	RENDER_GROUP_6 = 60,
	RENDER_GROUP_7 = 70,
	RENDER_LATE_WORLD = 75,
	RENDER_GROUP_8 = 80,
	RENDER_GROUP_9 = 90,
	RENDER_LATE_SKY = 95,
	RENDER_OVERLAY = 100,

	RENDER_GUI = 110,
	RENDER_FINALLY = 120,
};

#define RENDERABLE_DEFAULT_PRIORITY	100

#include "ioSingleton.h"
#include "ioHashString.h"

struct AniEventTime
{
	float	fEventTime;
	ioHashString szEventType;
	ioHashString szSubInfo;
	
	AniEventTime( float fTime, const char *szType, const char *szInfo )
		: fEventTime( fTime ), szEventType( szType ), szSubInfo( szInfo )
	{
	}
	
	bool operator<( const AniEventTime &rhs ) const
	{
		if( fEventTime < rhs.fEventTime )
			return true;
		
		return false;
	}
};

typedef std::vector< AniEventTime > AniEventList;
typedef std::vector< const AniEventTime* > AniEventConstPtrList;

#define MAX_ALPHA_RATE	255

class ioLight;
class ioAxisAlignBox;

typedef std::vector< WORD > WordVec;
typedef std::vector< int > IntVec;
typedef std::vector< ioHashString > ioHashStringVec;
typedef std::vector< D3DXVECTOR2 > Vector2Vec;
typedef std::vector< D3DXVECTOR3 > Vector3Vec;
typedef std::vector< D3DXVECTOR4 > Vector4Vec;
typedef std::vector< ioAxisAlignBox > AABoxList;
typedef std::vector< ioLight* > ioLightList;
typedef std::list< VertexComponent > VertexComponentList;

typedef DWORD UniqueObjID;
typedef DWORD UniqueNodeID;

#endif

