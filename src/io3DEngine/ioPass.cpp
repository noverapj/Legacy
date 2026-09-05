

#include "stdafx.h"

#include "ioPass.h"
#include "ioTechnique.h"
#include "ioTextureUnitState.h"

#include "ioShader.h"
#include "ioShaderGroup.h"
#include "ioShaderManager.h"
#include "ioAutoShaderParamSource.h"
#include "ioTextureManager.h"

#include "HelpFunc.h"

using namespace std;

ioPass::ioPass( ioTechnique *pParent, int iIndex )
{
	m_pParent = pParent;
	m_pShaderGrp = NULL;

	m_iPassIndex = iIndex;
	m_dwPassHash = 0;

	SetDefaultSetting();
	ReCalculateHash();

	m_bLoadDone = false;
	m_bNeedUpdatedAmbientEmissiveCache = true;
}

ioPass::ioPass( ioTechnique *pParent, int iIndex, const ioPass &rhs )
: m_pParent(pParent), m_iPassIndex(iIndex)
{
	m_pShaderGrp = NULL;
	
	m_iPassIndex = iIndex;
	m_dwPassHash = 0;

	ClearColorTransformRate();

	// ReCalculateHash() 는 operator=()에서 호출된다.
	*this = rhs;

	m_bLoadDone = false;
	m_bNeedUpdatedAmbientEmissiveCache = true;
	m_bEnablePass = true;
}

ioPass::~ioPass()
{
	TextureUnitStateList::iterator iter;
	for( iter=m_TextureUnitStateList.begin() ; iter!=m_TextureUnitStateList.end() ; ++iter )
	{
		delete *iter;
	}
	m_TextureUnitStateList.clear();

	if( m_pShaderGrp )
	{
		ioShaderManager::GetSingleton().UnLoadShaderGroup( m_pShaderGrp );
		m_pShaderGrp = NULL;
	}
}

void ioPass::CopyExceptTextureUnitStates( const ioPass &rhs )
{
	m_dwPassProperty  = rhs.m_dwPassProperty;
	m_dwPassLightType = rhs.m_dwPassLightType;

	m_BlendOp     = rhs.m_BlendOp;
	m_SourceBlend = rhs.m_SourceBlend;
	m_DestBlend   = rhs.m_DestBlend;

	m_dwTextureFactor   = rhs.m_dwTextureFactor;
	m_bUseTextureFactor = rhs.m_bUseTextureFactor;

	m_DepthFunc   = rhs.m_DepthFunc;
	m_fDepthBias  = rhs.m_fDepthBias;
	m_fBiasSlope  = rhs.m_fBiasSlope;

	m_AlphaTestFunc = rhs.m_AlphaTestFunc;
	m_iAlphaTestRef = rhs.m_iAlphaTestRef;

	m_CullMode    = rhs.m_CullMode;
	m_ShadeType   = rhs.m_ShadeType;
	m_FillMode	  = rhs.m_FillMode;

	m_RenderTarget = rhs.m_RenderTarget;
	m_iRTWidth  = rhs.m_iRTWidth;
	m_iRTHeight = rhs.m_iRTHeight;

	m_bDepthCheck = rhs.m_bDepthCheck;
	m_bDepthWrite = rhs.m_bDepthWrite;
	m_bLightingEnable = rhs.m_bLightingEnable;
	m_bColorWrite = rhs.m_bColorWrite;

	m_bUseQuadBuffer = rhs.m_bUseQuadBuffer;
	m_bDisableShader = rhs.m_bDisableShader;

	if( rhs.m_pShaderGrp )
	{
		SetShaderGroup( rhs.m_pShaderGrp->GetName() );
	}
	else
	{
		SetShaderGroup( "" );
	}

	m_CustomParameters = rhs.m_CustomParameters;
}

ioPass& ioPass::operator=( const ioPass &rhs )
{
	CopyExceptTextureUnitStates( rhs );

	RemoveAllTextureUnitStates();

	TextureUnitStateList::const_iterator iter;
	for( iter=rhs.m_TextureUnitStateList.begin() ; iter!=rhs.m_TextureUnitStateList.end() ; ++iter )
	{
		ioTextureUnitState *t = new ioTextureUnitState( this, *(*iter) );
		if( t )
		{
			m_TextureUnitStateList.push_back( t );
		}
	}

	ReCalculateHash();

	return *this;
}

bool ioPass::operator==( const ioPass &rhs ) const
{
	if( m_pShaderGrp != rhs.m_pShaderGrp )
		return false;

	if( m_iPassIndex != rhs.m_iPassIndex ||
		m_dwPassHash != rhs.m_dwPassHash ||
		m_dwPassProperty != rhs.m_dwPassProperty ||
		m_dwPassLightType != rhs.m_dwPassLightType )
		return false;

	if( m_BlendOp != rhs.m_BlendOp ||
		m_SourceBlend != rhs.m_SourceBlend ||
		m_DestBlend != rhs.m_DestBlend )
		return false;

	if( m_dwTextureFactor != rhs.m_dwTextureFactor ||
		m_bUseTextureFactor != rhs.m_bUseTextureFactor )
		return false;

	if( m_DepthFunc != rhs.m_DepthFunc ||
		m_fDepthBias != rhs.m_fDepthBias ||
		m_fBiasSlope != rhs.m_fBiasSlope )
		return false;

	if( m_AlphaTestFunc != rhs.m_AlphaTestFunc ||
		m_iAlphaTestRef != rhs.m_iAlphaTestRef )
		return false;

	if( m_CullMode != rhs.m_CullMode ||
		m_ShadeType != rhs.m_ShadeType ||
		m_FillMode != rhs.m_FillMode )
		return false;

	if( m_RenderTarget != rhs.m_RenderTarget ||
		m_iRTWidth != rhs.m_iRTWidth ||
		m_iRTHeight != rhs.m_iRTHeight )
		return false;

	if( m_bDepthCheck != rhs.m_bDepthCheck ||
		m_bDepthWrite != rhs.m_bDepthWrite ||
		m_bLightingEnable != rhs.m_bLightingEnable ||
		m_bColorWrite != rhs.m_bColorWrite )
		return false;

	if( m_bUseQuadBuffer != rhs.m_bUseQuadBuffer )
		return false;

	if( m_bDisableShader != rhs.m_bDisableShader )
		return false;

	if( m_CustomParameters != rhs.m_CustomParameters )
		return false;

	if( m_TextureUnitStateList.size() != rhs.m_TextureUnitStateList.size() )
		return false;

	int iTexUnitCnt = m_TextureUnitStateList.size();
	for( int i=0 ; i<iTexUnitCnt ; i++ )
	{
		if( *m_TextureUnitStateList[i] != *rhs.m_TextureUnitStateList[i] )
			return false;
	}

	return true;
}

bool ioPass::operator!=( const ioPass &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

void ioPass::SetDefaultSetting()
{
	m_dwPassProperty = 0;
	m_dwPassLightType = PLT_DIRECTIONAL | PLT_POINT | PLT_SPOT;

	m_BlendOp     = D3DBLENDOP_ADD;
	m_SourceBlend = D3DBLEND_ONE;
	m_DestBlend   = D3DBLEND_ZERO;

	m_dwTextureFactor   = 0xffffffff;
	m_bUseTextureFactor = false;

	m_DepthFunc  = D3DCMP_LESSEQUAL;
	m_fDepthBias = 0.0f;
	m_fBiasSlope = 0.0f;

	m_AlphaTestFunc = D3DCMP_NEVER;
	m_iAlphaTestRef = 0;

	m_CullMode  = CM_CCW;
	m_ShadeType = ST_GOURAUD;
	m_FillMode  = FM_SOLID;

	m_iRTWidth = 0;
	m_iRTHeight = 0;

	m_bDepthCheck = true;
	m_bDepthWrite = true;
	m_bLightingEnable = true;
	m_bColorWrite = true;

	m_bUseQuadBuffer = false;
	m_bDisableShader = false;

	ClearColorTransformRate();

	m_bEnablePass = true;
}

void ioPass::Load()
{
	m_bLoadDone = false;

	if( HasRenderTarget() )
	{
		if( !g_TextureMgr.GetByName( m_RenderTarget ) )
		{
			g_TextureMgr.CreateRTT( m_RenderTarget,
									m_iRTWidth,
									m_iRTHeight,
									D3DFMT_R5G6B5,
									D3DFMT_D16,
									false );
		}
	}

	for_each( m_TextureUnitStateList.begin(),
			  m_TextureUnitStateList.end(),
			  bind2nd( mem_fun( &ioTextureUnitState::Load ), true ) );
}

bool ioPass::IsLoaded() const
{
	return m_pParent->IsLoaded();
}

bool ioPass::IsLoadDone()
{
	if( m_bLoadDone )
		return true;

	TextureUnitStateList::iterator iter;
	for( iter=m_TextureUnitStateList.begin() ; iter!=m_TextureUnitStateList.end() ; ++iter )
	{
		ioTextureUnitState *pState = *iter;
		if( !pState->IsUseLightTexture() && !pState->IsLoadDone() )
		{
			m_bLoadDone = false;
			return false;
		}
	}

	m_bLoadDone = true;

	return true;
}

ErasePolicy ioPass::GetTextureErasePolicy() const
{
	return m_pParent->GetTextureErasePolicy();
}

void ioPass::SetPassIndex( int iIndex )
{
	m_iPassIndex = iIndex;
}

bool ioPass::HasShader() const
{
	if( m_pShaderGrp )
		return true;

	return false;
}

bool ioPass::IsTransparent() const
{
	if( m_DestBlend != D3DBLEND_ZERO )
		return true;

	if( m_SourceBlend != D3DBLEND_SRCCOLOR &&
		m_SourceBlend != D3DBLEND_INVDESTCOLOR &&
		m_SourceBlend != D3DBLEND_DESTALPHA &&
		m_SourceBlend != D3DBLEND_INVDESTALPHA )
		return false;

	return true;
}

bool ioPass::IsCanCastShadow() const
{
	if( !HasProperty( PP_SHADOW_CAST_PASS ) )
		return false;

	if( m_pShaderGrp && m_pShaderGrp->GetGenShadowShader() )
		return true;

	return false;
}

bool ioPass::HasRenderShadowShader() const
{
	if( m_pShaderGrp && m_pShaderGrp->GetRenderShadowShader() )
		return true;

	return false;
}

bool ioPass::IsLightAvailable( ioLight *pLight ) const
{
	PassLightType eLightType;

	switch( pLight->GetType() )
	{
	case LT_DIRECTIONAL:
		eLightType = PLT_DIRECTIONAL;
		break;
	case LT_POINT:
		eLightType = PLT_POINT;
		break;
	case LT_SPOT:
		eLightType = PLT_SPOT;
		break;
	default:
		return false;
	}

	return IsLightAvailableType( eLightType );
}

void ioPass::SetShaderGroup( const ioHashString &name )
{
	if( m_pShaderGrp )
	{
		ioShaderManager::GetSingleton().UnLoadShaderGroup( m_pShaderGrp );
		m_pShaderGrp = NULL;
	}

	if( !name.IsEmpty() )
	{
		m_pShaderGrp = ioShaderManager::GetSingleton().LoadShaderGroup( name );
	}
}

const ioHashString* ioPass::GetShaderGroupName() const
{
	if( m_pShaderGrp )
		return &m_pShaderGrp->GetName();

	return NULL;
}

ShaderGroupType ioPass::GetShaderGroupType() const
{
	if( m_pShaderGrp )
		return m_pShaderGrp->GetGroupType();

	return SGT_NONE;
}

void ioPass::ConvertShaderStaticToVertexColor()
{
	if( !m_pShaderGrp )	return;

	const char *szOrgName = m_pShaderGrp->GetName().c_str();

	char szPreName[MAX_PATH];
	strcpy( szPreName, szOrgName );
	strlwr( szPreName );

	if( strstr( szPreName, "staticvcolor" ) )
		return;	// not static

	if( !strstr( szPreName, "static" ) )
		return;

	char szNewName[MAX_PATH];
	const char *szHalfTag = szOrgName + strlen( "static" );
	wsprintf( szNewName, "StaticVColor%s", szHalfTag );

	SetShaderGroup( ioHashString(szNewName) );
}

void ioPass::ConvertShaderAnimateToStatic()
{
	if( !m_pShaderGrp )	return;

	const char *szOrgName = m_pShaderGrp->GetName().c_str();

	char szPreName[MAX_PATH];
	strcpy( szPreName, szOrgName );
	strlwr( szPreName );

	if( !strstr( szPreName, "animate" ) )
		return;

	char szNewName[MAX_PATH];
	const char *szHalfTag = szOrgName + strlen( "animate" );
	wsprintf( szNewName, "Static%s", szHalfTag );

	SetShaderGroup( ioHashString(szNewName) );
}

void ioPass::ConvertShaderStaticToAnimate()
{
	if( !m_pShaderGrp )	return;

	const char *szOrgName = m_pShaderGrp->GetName().c_str();

	char szPreName[MAX_PATH];
	strcpy( szPreName, szOrgName );
	strlwr( szPreName );

	if( strstr( szPreName, "staticvcolor" ) )
		return;	// not static

	if( !strstr( szPreName, "static" ) )
		return;

	char szNewName[MAX_PATH];
	const char *szHalfTag = szOrgName + strlen( "static" );
	wsprintf( szNewName, "Animate%s", szHalfTag );

	SetShaderGroup( ioHashString(szNewName) );
}

void ioPass::SetRenderTarget( const ioHashString &szName, int iWidth, int iHeight )
{
	m_RenderTarget = szName;
	m_iRTWidth  = iWidth;
	m_iRTHeight = iHeight;
}

void ioPass::ClearRenderTarget()
{
	m_RenderTarget.Clear();
	m_iRTWidth = 0;
	m_iRTHeight = 0;
}

ioShader* ioPass::BindActiveShader( const ioLight *pLight, bool bGenShadow, bool bRecvShadow )
{
	if( !m_pShaderGrp || m_bDisableShader )
		return NULL;

	ioShader *pShader = NULL;

	if( !IsTransparent() )
	{
		if( HasProperty( PP_SHADOW_RECV_PASS ) && bRecvShadow )
		{
			pShader = m_pShaderGrp->GetRenderShadowShader();
		}
		else if( HasProperty( PP_SHADOW_CAST_PASS ) && bGenShadow )
		{
			pShader = m_pShaderGrp->GetGenShadowShader();
		}
	}

	if( !pShader )
	{
		if( m_bLightingEnable )
			pShader = m_pShaderGrp->GetShader( pLight );
		else
			pShader = m_pShaderGrp->GetShader( LC_NOLIGHT );
	}

	return pShader;
}

ioShader* ioPass::BindActiveShader( const ioLightList &rkLightList, bool bGenShadow, bool bRecvShadow )
{
	if( !m_pShaderGrp || m_bDisableShader )
		return NULL;

	ioShader *pShader = NULL;

	if( !IsTransparent() )
	{
		if( HasProperty( PP_SHADOW_RECV_PASS ) && bRecvShadow )
		{
			pShader = m_pShaderGrp->GetRenderShadowShader();
		}
		else if( HasProperty( PP_SHADOW_CAST_PASS ) && bGenShadow )
		{
			pShader = m_pShaderGrp->GetGenShadowShader();
		}
	}

	if( !pShader )
	{
		if( m_bLightingEnable )
			pShader = m_pShaderGrp->GetShader( rkLightList );
		else
			pShader = m_pShaderGrp->GetShader( LC_NOLIGHT );
	}

	return pShader;
}

D3DMATERIAL9 ioPass::GetDXMaterial() const
{
	D3DMATERIAL9 kMaterial;
	kMaterial.Ambient = GetAmbient();
	kMaterial.Diffuse = GetDiffuse();
	kMaterial.Specular = GetSpecular();
	kMaterial.Emissive = GetEmissive();
	kMaterial.Power = m_pParent->GetSpecularPower();

	return kMaterial;
}

D3DCOLORVALUE ioPass::GetAmbient() const
{
	if( !m_bColorXformed )
		return m_pParent->GetAmbient();

	return m_pParent->GetAmbient() * m_fAmbientXformRate;
}

D3DCOLORVALUE ioPass::GetDiffuse() const
{
	if( !m_bColorXformed )
		return m_pParent->GetDiffuse();

	return m_pParent->GetDiffuse() * m_fDiffuseXformRate;
}

D3DCOLORVALUE ioPass::GetSpecular() const
{
	if( !m_bColorXformed )
		return m_pParent->GetSpecular();

	return m_pParent->GetSpecular() * m_fSpecularXformRate;
}

D3DCOLORVALUE ioPass::GetEmissive() const
{
	if( !m_bColorXformed )
		return m_pParent->GetEmissive();

	return m_pParent->GetEmissive() * m_fEmissiveXformRate;
}

D3DXVECTOR4 ioPass::GetSpecularParam() const
{
	return m_pParent->GetSpecularParam();
}

D3DCOLORVALUE ioPass::GetAmbientEmissiveColor( const D3DCOLORVALUE &rkAmbientLight, bool bNeedUpdate ) const
{
	if( bNeedUpdate || m_bColorXformed || m_bNeedUpdatedAmbientEmissiveCache )
	{
		m_CachedAmbientEmissiveColor = GetAmbient() * rkAmbientLight + GetEmissive();
		m_CachedAmbientEmissiveColor.r = max( 0.0f, min( m_CachedAmbientEmissiveColor.r, 1.0f ) );
		m_CachedAmbientEmissiveColor.g = max( 0.0f, min( m_CachedAmbientEmissiveColor.g, 1.0f ) );
		m_CachedAmbientEmissiveColor.b = max( 0.0f, min( m_CachedAmbientEmissiveColor.b, 1.0f ) );
		m_CachedAmbientEmissiveColor.a = max( 0.0f, min( m_CachedAmbientEmissiveColor.a, 1.0f ) );
		m_bNeedUpdatedAmbientEmissiveCache = false;
	}

	return m_CachedAmbientEmissiveColor;
}

ioTextureUnitState* ioPass::CreateTextureUnitState()
{
	ioTextureUnitState *t = new ioTextureUnitState(this);
	if( t )
	{
		t->SetTextureCoordSet( m_TextureUnitStateList.size() );
		m_TextureUnitStateList.push_back( t );
	}

	return t;
}

ioTextureUnitState* ioPass::CreateTextureUnitState( const ioHashString &szTexName )
{
	ioTextureUnitState *t = new ioTextureUnitState(this);
	if( t )
	{
		t->SetTextureName( szTexName );
		t->SetTextureCoordSet( m_TextureUnitStateList.size() );
		m_TextureUnitStateList.push_back( t );
	}

	return t;
}

void ioPass::AddTextureUnitState( ioTextureUnitState *pState )
{
	if( pState )
	{
		m_TextureUnitStateList.push_back( pState );
		ReCalculateHash();
	}
}

void ioPass::SwapTextureUnitStateOrder( int iSrc, int iDest )
{
	int iNumStats = GetNumTextureUnitStates();
	if( !COMPARE( iSrc, 0, iNumStats ) || !COMPARE( iDest, 0, iNumStats ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPass::SwapTextureUnitStateOrder Failed - Src:%d, Dest:%d, Total:%d",
								iSrc, iDest, iNumStats );
		return;
	}

    ioTextureUnitState *pTemp = m_TextureUnitStateList[iSrc];
	m_TextureUnitStateList[iSrc] = m_TextureUnitStateList[iDest];
	m_TextureUnitStateList[iDest] = pTemp;

	ReCalculateHash();
}

ioTextureUnitState* ioPass::GetTextureUnitState( int iIndex ) const
{
	if( !COMPARE( iIndex, 0, GetNumTextureUnitStates() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPass::GetTextureUnitState - overflow( %d / %d )", iIndex, GetNumTextureUnitStates() );
		return NULL;
	}

	return m_TextureUnitStateList[iIndex];
}

ioPass::TextureUnitStateIterator ioPass::GetTextureUnitStateIterator()
{
	return TextureUnitStateIterator(m_TextureUnitStateList.begin(), m_TextureUnitStateList.end() );
}

void ioPass::RemoveTextureUnitState( int iIndex )
{
	if( !COMPARE( iIndex, 0, GetNumTextureUnitStates() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioPass::RemoveTextureUnitState - overflow" );
		return;
	}

	TextureUnitStateList::iterator iter = m_TextureUnitStateList.begin() + iIndex;
	delete *iter;
	m_TextureUnitStateList.erase( iter );

	ReCalculateHash();
}

void ioPass::RemoveAllTextureUnitStates()
{
	TextureUnitStateList::iterator iter;
	for( iter=m_TextureUnitStateList.begin() ; iter!=m_TextureUnitStateList.end() ; ++iter )
	{
		delete *iter;
	}
	m_TextureUnitStateList.clear();

	ReCalculateHash();
}

void ioPass::SetColorTransformRate( ColorTransformType ctType, float fRate )
{
	switch( ctType )
	{
	case CTT_AMBIENT:
		m_fAmbientXformRate = fRate;
		break;
	case CTT_DIFFUSE:
		m_fDiffuseXformRate = fRate;
		break;
	case CTT_SPECULAR:
		m_fSpecularXformRate = fRate;
		break;
	case CTT_EMISSIVE:
		m_fEmissiveXformRate = fRate;
		break;
	}

	m_bColorXformed = true;
}

void ioPass::ClearColorTransformRate()
{
	m_fAmbientXformRate = 1.0f;
	m_fDiffuseXformRate = 1.0f;
	m_fSpecularXformRate = 1.0f;
	m_fEmissiveXformRate = 1.0f;
	m_bColorXformed = false;
}

void ioPass::SetSceneBlending( SceneBlendMode eMode )
{
	switch( eMode )
	{
	case BLEND_NO_ALPHA:
		SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );
		break;
	case BLEND_SRCALPHA:
		SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		break;
	case BLEND_COLORKEY:
		SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		break;
	case BLEND_ADD:
		SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		break;
	case BLEND_SCREEN:
		SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_INVDESTCOLOR, D3DBLEND_ONE );
		break;
	case BLEND_LIGHTEN:
		SetSceneBlending( D3DBLENDOP_MAX, D3DBLEND_ONE, D3DBLEND_ONE );
		break;
	case BLEND_DARKEN:
		SetSceneBlending( D3DBLENDOP_MIN, D3DBLEND_ONE, D3DBLEND_ONE );
		break;
	case BLEND_REV_SUBTRACT:
		SetSceneBlending( D3DBLENDOP_REVSUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		break;
	case BLEND_SUBTRACT:
		SetSceneBlending( D3DBLENDOP_SUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		break;
	}
}

void ioPass::SetSceneBlending( D3DBLENDOP eOp, D3DBLEND eSource, D3DBLEND eDest )
{
	m_BlendOp     = eOp;
	m_SourceBlend = eSource;
	m_DestBlend   = eDest;
}

void ioPass::SetSceneBlendOp( D3DBLENDOP eOp )
{
	m_BlendOp = eOp;
}

void ioPass::SetSceneBlendSource( D3DBLEND eSource )
{
	m_SourceBlend = eSource;
}

void ioPass::SetSceneBlendDest( D3DBLEND eDest )
{
	m_DestBlend = eDest;
}

D3DBLENDOP ioPass::GetAlphaBlendOp() const
{
	return m_BlendOp;
}

D3DBLEND ioPass::GetAlphaSourceBlendFactor() const
{
	if( m_SourceBlend == D3DBLEND_ONE )
		return D3DBLEND_SRCALPHA;

	return m_SourceBlend;
}

D3DBLEND ioPass::GetAlphaDestBlendFactor() const
{
	if( m_DestBlend == D3DBLEND_ZERO )
		return D3DBLEND_INVSRCALPHA;

	return m_DestBlend;
}

void ioPass::SetTextureFactor( DWORD dwFactor )
{
	m_dwTextureFactor = dwFactor;
	m_bUseTextureFactor = true;
}

void ioPass::SetDepthBias( float fDepthBias, float fBiasSlope )
{
	m_fDepthBias = fDepthBias;
	m_fBiasSlope = fBiasSlope;
}

void ioPass::SetAlphaTestSetting( D3DCMPFUNC eFunc, int iRef )
{
	m_AlphaTestFunc = eFunc;
	m_iAlphaTestRef = iRef;
}

void ioPass::SetTextureFiltering( TextureFilterOption filter )
{
	for_each( m_TextureUnitStateList.begin(),
			  m_TextureUnitStateList.end(),
			  bind2nd( mem_fun( &ioTextureUnitState::SetTextureFiltering ), filter ) );
}

void ioPass::SetTextureMipMapBias( float fBias )
{
	for_each( m_TextureUnitStateList.begin(),
			  m_TextureUnitStateList.end(),
			  bind2nd( mem_fun( &ioTextureUnitState::SetTextureMipMapBias ), fBias ) );
}

void ioPass::SetTextureAnisotropy( int iMaxAniso )
{
	for_each( m_TextureUnitStateList.begin(),
			  m_TextureUnitStateList.end(),
			  bind2nd( mem_fun( &ioTextureUnitState::SetTextureAnisotropy ), iMaxAniso ) );
}

void ioPass::StopAllTextureEffect( bool bStop )
{
	for_each( m_TextureUnitStateList.begin(),
			  m_TextureUnitStateList.end(),
			  bind2nd( mem_fun( &ioTextureUnitState::StopAllTextureEffect ), bStop ) );
}

void ioPass::ReCalculateHash()
{
	m_dwPassHash = 0;

	int iNumTex = GetNumTextureUnitStates();
	if( iNumTex > 0 && !m_TextureUnitStateList[0]->GetTextureName().IsEmpty() )
	{
		m_dwPassHash += ( m_TextureUnitStateList[0]->GetTextureHashCode() & 0xffff ) << 16;
	}

	if( iNumTex > 1 && !m_TextureUnitStateList[1]->GetTextureName().IsEmpty() )
	{
		m_dwPassHash += ( m_TextureUnitStateList[1]->GetTextureHashCode() & 0xffff );
	}
}
