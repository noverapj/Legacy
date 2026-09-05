

#include "stdafx.h"

#include "ioStringConverter.h"

#include "ioTexture.h"
#include "ioTextureManager.h"

#include "ioTextureEffect.h"
#include "ioTextureScrollEffect.h"
#include "ioTextureRotateEffect.h"
#include "ioTextureTransformEffect.h"
#include "ioTextureAnimationEffect.h"
#include "ioTextureEffectManager.h"

#include "ioTextureUnitState.h"
#include "ioPass.h"

ioTextureUnitState::ioTextureUnitState( ioPass *pParent )
{
	m_pParent = pParent;

	for( int i=0 ; i<MAX_ANI_TEX_FRAMES ; i++ )
	{
		m_FrameTex[i] = NULL;
	}

	m_iNumFrames = 0;
	m_iCurFrame  = 0;
	m_iTextureCoordSetIdx = 0;
	m_iTextureCustomValue = 0;
	m_AddressMode = TAM_WRAP;

	m_fAnimDuration  = 0.0f;
	m_fUMod = m_fVMod = 0.0f;
	m_fUScale = m_fVScale = 1.0f;
	m_fRotate = 0.0f;

	m_TexFilterOption = TFO_BILINEAR;
	m_fTextureMipMapBias = 0.0f;
	m_iMaxAniso = 1;

	D3DXMatrixIdentity( &m_TexModMatrix );
	m_bRecalcTexMatrix = false;
	m_bIdentityTexMatrix = true;

	m_bUseLightTexture = false;
	m_bDefaultFiltering = true;
	m_bRenderTarget = false;
	m_bScaleUVOnCenter = false;

	m_pParent->ReCalculateHash();
}

ioTextureUnitState::ioTextureUnitState( ioPass *pParent,
									    const ioTextureUnitState &rhs )
{
	m_pParent = pParent;

	for( int i=0 ; i<MAX_ANI_TEX_FRAMES ; i++ )
	{
		m_FrameTex[i] = NULL;
	}

	*this = rhs;
}

ioTextureUnitState::ioTextureUnitState( ioPass *pParent,
									    const ioHashString &szTexName,
										int iTexCoordSet )
{
	m_pParent = pParent;

	for( int i=0 ; i<MAX_ANI_TEX_FRAMES ; i++ )
	{
		m_FrameTex[i] = NULL;
	}

	m_iNumFrames = 0;
	m_iCurFrame  = 0;
	m_iTextureCoordSetIdx = 0;
	m_iTextureCustomValue = 0;
	m_AddressMode = TAM_WRAP;

	m_fAnimDuration  = 0.0f;
	m_fUMod = m_fVMod = 0.0f;
	m_fUScale = m_fVScale = 1.0f;
	m_fRotate = 0.0f;

	m_TexFilterOption = TFO_BILINEAR;
	m_fTextureMipMapBias = 0.0f;
	m_iMaxAniso = 1;

	D3DXMatrixIdentity( &m_TexModMatrix );
	m_bRecalcTexMatrix = false;
	m_bIdentityTexMatrix = true;

	m_bUseLightTexture = false;
	m_bDefaultFiltering = true;
	m_bRenderTarget = false;
	m_bScaleUVOnCenter = false;

	SetTextureName( szTexName );
	SetTextureCoordSet( iTexCoordSet );

	m_pParent->ReCalculateHash();
}

ioTextureUnitState::~ioTextureUnitState()
{
	UnLoad();
}

ioTextureUnitState& ioTextureUnitState::operator=( const ioTextureUnitState &rhs )
{
	for( int i=0 ; i<rhs.m_iNumFrames ; i++ )
	{
		m_Frames[i] = rhs.m_Frames[i];
	}

	// 내가 가진 프레임이 rhs보다 큰 경우를 대비
	for( ; i<MAX_ANI_TEX_FRAMES ; i++ )
	{
		if( !m_Frames[i].IsEmpty() )
		{
			m_Frames[i].Clear();
		}
	}

	m_iNumFrames = rhs.m_iNumFrames;
	m_iCurFrame  = rhs.m_iCurFrame;

	m_iTextureCoordSetIdx = rhs.m_iTextureCoordSetIdx;
	m_iTextureCustomValue = rhs.m_iTextureCustomValue;
	m_AddressMode = rhs.m_AddressMode;

	m_ColorOp = rhs.m_ColorOp;
	m_AlphaOp = rhs.m_AlphaOp;

	RemoveAllEffects();

	if( !rhs.m_TexEffectList.empty() )
	{
		m_TexEffectList = rhs.m_TexEffectList;

		// Controller는 공유되지 않음, Load시 생성됨
		TexEffectList::iterator iter=m_TexEffectList.begin();
		for( ; iter!=m_TexEffectList.end() ; ++iter )
		{
			iter->m_pController = NULL;
		}
	}

	m_fAnimDuration = rhs.m_fAnimDuration;
	m_fUMod = rhs.m_fUMod;
	m_fVMod = rhs.m_fVMod;
	m_fUScale = rhs.m_fUScale;
	m_fVScale = rhs.m_fVScale;
	m_fRotate = rhs.m_fRotate;

	m_TexFilterOption = rhs.m_TexFilterOption;
	m_fTextureMipMapBias = rhs.m_fTextureMipMapBias;
	m_iMaxAniso = rhs.m_iMaxAniso;

	m_TexModMatrix = rhs.m_TexModMatrix;
	m_bRecalcTexMatrix = rhs.m_bRecalcTexMatrix;
	m_bIdentityTexMatrix = rhs.m_bIdentityTexMatrix;

	m_bUseLightTexture = rhs.m_bUseLightTexture;
	m_bDefaultFiltering = rhs.m_bDefaultFiltering;
	m_bRenderTarget = rhs.m_bRenderTarget;
	m_bScaleUVOnCenter = rhs.m_bScaleUVOnCenter;

	if( IsLoaded() )
	{
		Load();
	}

	m_pParent->ReCalculateHash();

	return *this;
}

bool ioTextureUnitState::operator==( const ioTextureUnitState &rhs ) const
{
	if( m_iNumFrames != rhs.m_iNumFrames )
		return false;

	for( int i=0 ; i<m_iNumFrames ; i++ )
	{
		if( m_Frames[i] != rhs.m_Frames[i] )
			return false;
	}

	if( m_iTextureCoordSetIdx != rhs.m_iTextureCoordSetIdx )
		return false;
    
	if( m_iTextureCustomValue != rhs.m_iTextureCustomValue )
		return false;

	if( m_AddressMode != rhs.m_AddressMode ||
		m_ColorOp != rhs.m_ColorOp ||
		m_AlphaOp != rhs.m_AlphaOp )
		return false;

	if( m_fAnimDuration != rhs.m_fAnimDuration ||
		m_fUMod != rhs.m_fVMod ||
		m_fVMod != rhs.m_fVMod ||
		m_fUScale != rhs.m_fUScale ||
		m_fVScale != rhs.m_fVScale ||
		m_fRotate != rhs.m_fRotate )
		return false;

	if( m_TexFilterOption != rhs.m_TexFilterOption ||
		m_fTextureMipMapBias != rhs.m_fTextureMipMapBias ||
		m_iMaxAniso != rhs.m_iMaxAniso )
		return false;

/*	skip..
	D3DXMATRIX m_TexModMatrix;
	bool m_bRecalcTexMatrix;
	bool m_bIdentityTexMatrix;

	bool m_bDefaultFiltering;
*/

	if( m_bRenderTarget != rhs.m_bRenderTarget )
		return false;

	if( m_bScaleUVOnCenter != rhs.m_bScaleUVOnCenter )
		return false;

	if( m_bUseLightTexture != rhs.m_bUseLightTexture )
		return false;

	if( m_TexEffectList.size() != rhs.m_TexEffectList.size() )
		return false;

	TexEffectList::const_iterator iSrc, iDest;
	iSrc = m_TexEffectList.begin();
	iDest = rhs.m_TexEffectList.begin();
	for( ; iSrc != m_TexEffectList.end() ; ++iSrc, ++iDest )
	{
		TextureEffect kSrcEff, kDestEff;
		kSrcEff = *iSrc;
		kSrcEff.m_pController = NULL;

		kDestEff = *iDest;
		kDestEff.m_pController = NULL;

		if( 0 != memcmp( &kSrcEff, &kDestEff, sizeof(TextureEffect) ) )
			return false;
	}

	return true;
}


bool ioTextureUnitState::operator!=( const ioTextureUnitState &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

void ioTextureUnitState::SetTextureName( const ioHashString &kName )
{
	m_Frames[0] = kName;

	m_iNumFrames = 1;
	m_iCurFrame  = 0;

	m_pParent->ReCalculateHash();
}

void ioTextureUnitState::SetTextureCoordSet( int iCoordSet )
{
	m_iTextureCoordSetIdx = iCoordSet;
}

void ioTextureUnitState::SetTextureCustomValue( int iCustomValue )
{
	m_iTextureCustomValue = iCustomValue;
}

void ioTextureUnitState::Load( bool bLoadThread )
{
	UnLoad();

	ErasePolicy eTexturePolicy = GetTextureErasePolicy();

	for( int i=0 ; i<m_iNumFrames ; i++ )
	{
		if( !m_Frames[i].IsEmpty() )
		{
			m_FrameTex[i] = g_TextureMgr.CreateTexture( m_Frames[i],
														0,
														D3DX_DEFAULT,
														TQ_DEFAULT,
														bLoadThread );

			if( !m_bRenderTarget && m_FrameTex[i] )
			{
				m_FrameTex[i]->SetErasePolicy( eTexturePolicy );
			}
		}
	}

	TexEffectList::iterator iter=m_TexEffectList.begin();
	for( ; iter!=m_TexEffectList.end() ; ++iter )
	{
		g_TextureEffectMgr.CreateEffect( this, *iter );
	}
}

void ioTextureUnitState::UnLoad()
{
	for( int i=0 ; i<MAX_ANI_TEX_FRAMES ; i++ )
	{
		DESTROY_TEXTURE( m_FrameTex[i] );
	}

	if( !m_TexEffectList.empty() )
	{
		TexEffectList::iterator iter=m_TexEffectList.begin();
		for( ; iter!=m_TexEffectList.end() ; ++iter )
		{
			g_TextureEffectMgr.UnLoadEffect( iter->m_pController );
			iter->m_pController = NULL;
		}
	}
}

bool ioTextureUnitState::IsLoaded() const
{
	return m_pParent->IsLoaded();
}

bool ioTextureUnitState::IsLoadDone()
{
	for( int i=0 ; i<m_iNumFrames ; i++ )
	{
		if( !m_FrameTex[i] || !m_FrameTex[i]->IsLoadDone() )
			return false;
	}

	return true;
}

ErasePolicy ioTextureUnitState::GetTextureErasePolicy() const
{
	return m_pParent->GetTextureErasePolicy();
}

void ioTextureUnitState::SetTextureAnimation( int iNumFrames, float fDuration, bool bOnce )
{
	if( m_Frames[0].IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureUnitState::SetTextureAnimation - TexName Empty" );
		return;
	}
	if( iNumFrames > MAX_ANI_TEX_FRAMES )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureUnitState::SetTextureAnimation - over(%d/%d)",
								iNumFrames, MAX_ANI_TEX_FRAMES );
		return;
	}

	std::string szTemp( m_Frames[0].c_str() );

	int iPos = szTemp.find_last_of( "." );
	std::string szBaseName, szExt;
	szBaseName = szTemp.substr( 0, iPos );
	szExt = szTemp.substr(iPos);

	m_iNumFrames = iNumFrames;
	m_iCurFrame = 0;
	m_fAnimDuration = fDuration;

	char szTexName[MAX_PATH];
	for( int i=1 ; i<iNumFrames ; i++ )
	{
		sprintf( szTexName, "%s%d%s", szBaseName.c_str(), i, szExt.c_str() );
		m_Frames[i] = szTexName;
	}

	RemoveEffect( TET_ANIMATE );

	TextureEffect kEffect;
	kEffect.m_Type = TET_ANIMATE;
	kEffect.m_fArg1 = (float)iNumFrames;
	kEffect.m_fArg2 = fDuration;

	if( bOnce )
		kEffect.m_dwOnce = 1;
	else
		kEffect.m_dwOnce = 0;

	AddTexEffect( kEffect );

	if( IsLoaded() )
	{
		Load();
	}
	
	m_pParent->ReCalculateHash();
}

D3DXVECTOR2 ioTextureUnitState::GetTextureDimensions( int iFrame ) const
{
	ioTexture *pTex = g_TextureMgr.GetTextureByName( m_Frames[iFrame] );
	if( !pTex )
	{
		LOG.PrintTimeAndLog( 0, "ioTextureUnitState::GetTextureDimensions - %s Not Exist",
								 m_Frames[iFrame].c_str() );

		return D3DXVECTOR2( 0.0f, 0.0f );
	}

	return D3DXVECTOR2( pTex->GetWidth(), pTex->GetHeight() );
}

void ioTextureUnitState::SetCurFrameByRate( float fRate )
{
	SetCurFrame( m_iNumFrames * fRate );
}

void ioTextureUnitState::SetCurFrame( int iFrameNum )
{
	if( COMPARE( iFrameNum, 0, m_iNumFrames ) )
	{
		m_iCurFrame = iFrameNum;
	}
}

void ioTextureUnitState::AddTexEffect( TextureEffect &rkEffect )
{
	rkEffect.m_pController = NULL;

	if( IsLoaded() )
	{
		g_TextureEffectMgr.CreateEffect( this, rkEffect );
	}

	m_TexEffectList.push_back( rkEffect );
}

void ioTextureUnitState::RemoveAllEffects()
{
	if( !m_TexEffectList.empty() )
	{
		TexEffectList::iterator iter=m_TexEffectList.begin();
		for( ; iter!=m_TexEffectList.end() ; ++iter )
		{
			g_TextureEffectMgr.UnLoadEffect( iter->m_pController );
		}

		m_TexEffectList.clear();
	}

	ClearTextureModifyValues();
}

void ioTextureUnitState::RemoveEffect( const TextureEffectType tType )
{
	TexEffectList::iterator iter = m_TexEffectList.begin();
	while( iter!=m_TexEffectList.end() )
	{
		if( iter->m_Type == tType )
		{
			g_TextureEffectMgr.UnLoadEffect( iter->m_pController );
			iter = m_TexEffectList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	ClearTextureModifyValues();
}

void ioTextureUnitState::RemoveEffectByIndex( int iIndex )
{
	if( !COMPARE( iIndex, 0, GetTextureEffectCnt() ) )
		return;

	TexEffectList::iterator iter = m_TexEffectList.begin();
	std::advance( iter, iIndex );
	
	g_TextureEffectMgr.UnLoadEffect( iter->m_pController );
	m_TexEffectList.erase( iter );

	ClearTextureModifyValues();
}

void ioTextureUnitState::SetUseLightTexture()
{
	m_bUseLightTexture = true;
}

void ioTextureUnitState::SetTextureAddressMode( TextureAddressMode eMode )
{
	m_AddressMode = eMode;
}

void ioTextureUnitState::SetTextureAnisotropy( int iMaxAniso )
{
	m_iMaxAniso = iMaxAniso;
}

void ioTextureUnitState::SetTextureFiltering( TextureFilterOption eOption )
{
	m_TexFilterOption = eOption;
	m_bDefaultFiltering = false;
}

void ioTextureUnitState::SetTextureMipMapBias( float fBias )
{
	m_fTextureMipMapBias = fBias;
}

void ioTextureUnitState::SetColorStageBlendOp( D3DTEXTUREOP eOp, DWORD dwSrc, DWORD dwDest )
{
	m_ColorOp.m_StageOp = eOp;
	m_ColorOp.m_dwSrcFactor  = dwSrc;
	m_ColorOp.m_dwDestFactor = dwDest;
}

void ioTextureUnitState::SetColorStageBlendOp( D3DTEXTUREOP eOp )
{
	m_ColorOp.m_StageOp = eOp;
}

void ioTextureUnitState::SetColorStageBlendSource( DWORD dwSrc )
{
	m_ColorOp.m_dwSrcFactor = dwSrc;
}

void ioTextureUnitState::SetColorStageBlendDest( DWORD dwDest )
{
	m_ColorOp.m_dwDestFactor = dwDest;
}

void ioTextureUnitState::SetAlphaStageBlendOp( D3DTEXTUREOP eOp, DWORD dwSrc, DWORD dwDest )
{
	m_AlphaOp.m_StageOp = eOp;
	m_AlphaOp.m_dwSrcFactor  = dwSrc;
	m_AlphaOp.m_dwDestFactor = dwDest;
}

void ioTextureUnitState::SetAlphaStageBlendOp( D3DTEXTUREOP eOp )
{
	m_AlphaOp.m_StageOp = eOp;
}

void ioTextureUnitState::SetAlphaStageBlendSource( DWORD dwSrc )
{
	m_AlphaOp.m_dwSrcFactor = dwSrc;
}

void ioTextureUnitState::SetAlphaStageBlendDest( DWORD dwDest )
{
	m_AlphaOp.m_dwDestFactor = dwDest;
}

void ioTextureUnitState::SetTextureTransform( const D3DXMATRIX &kMat )
{
	m_TexModMatrix = kMat;
	m_bRecalcTexMatrix = false;

	if( D3DXMatrixIsIdentity( &m_TexModMatrix ) )
		m_bIdentityTexMatrix = true;
	else
		m_bIdentityTexMatrix = false;
}

void ioTextureUnitState::SetTextureScroll( float fU, float fV )
{
	m_fUMod = fU;
	m_fVMod = fV;

	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureScale( float fU, float fV )
{
	m_fUScale = fU;
	m_fVScale = fV;

	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureRotate( float fDegrees )
{
	m_fRotate = fDegrees;

	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::RecalcTextureMatrix()
{
	D3DXMATRIX xForm;
	D3DXMatrixIdentity( &xForm );

	if( m_fUScale != 0.0f || m_fVScale != 0.0f )
	{
		xForm._11 = 1.0f / m_fUScale;
		xForm._22 = 1.0f / m_fVScale;

		if( m_bScaleUVOnCenter )
		{
			xForm._31 = ( -FLOAT05 * xForm._11 ) + FLOAT05;
			xForm._32 = ( -FLOAT05 * xForm._22 ) + FLOAT05;
		}
	}

	if( m_fUMod != 0.0f || m_fVMod != 0.0f )
	{
		D3DXMATRIX matTrans;
		D3DXMatrixIdentity( &matTrans );

		matTrans._31 = m_fUMod;
		matTrans._32 = m_fVMod;

		D3DXMatrixMultiply( &xForm, &matTrans, &xForm );
	}

	if( m_fRotate != 0.0f )
	{
		D3DXMATRIX matRot;
		D3DXMatrixIdentity( &matRot );

		float fTheta = DEGtoRAD( m_fRotate );
		float fCosTheta = cosf( fTheta );
		float fSinTheta = sinf( fTheta );

		matRot._11 = fCosTheta;
		matRot._21 = -fSinTheta;
		matRot._12 = fSinTheta;
		matRot._22 = fCosTheta;

		matRot._31 = FLOAT05 + ( ( -FLOAT05 * fCosTheta ) - ( -FLOAT05 * fSinTheta ) );
		matRot._32 = FLOAT05 + ( ( -FLOAT05 * fSinTheta ) + ( -FLOAT05 * fCosTheta ) );

		D3DXMatrixMultiply( &xForm, &matRot, &xForm );
	}

	m_TexModMatrix = xForm;	
	m_bRecalcTexMatrix = false;
	m_bIdentityTexMatrix = false;
}

void ioTextureUnitState::ClearTextureModifyValues()
{
	m_fUMod = m_fVMod = 0.0f;
	m_fUScale = m_fVScale = 1.0f;
	m_fRotate = 0.0f;
	
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureUScroll( float fValue )
{
	m_fUMod = fValue;
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureVScroll( float fValue )
{
	m_fVMod = fValue;
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureUScale( float fValue )
{
	m_fUScale = fValue;
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureVScale( float fValue )
{
	m_fVScale = fValue;
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetTextureScaleOnCenter( bool bCenter )
{
	m_bScaleUVOnCenter = bCenter;
	m_bRecalcTexMatrix = true;
}

void ioTextureUnitState::SetScrollAnimation( float fUSpeed, float fVSpeed, bool bOnce )
{
	TextureEffect kEffect;
	kEffect.m_Type = TET_SCROLL;
	kEffect.m_fArg1 = fUSpeed;
	kEffect.m_fArg2 = fVSpeed;

	if( bOnce )
		kEffect.m_dwOnce = 1;
	else
		kEffect.m_dwOnce = 0;

	AddTexEffect( kEffect );
}

void ioTextureUnitState::SetRotateAnimation( float fSpeed, bool bOnce )
{
	TextureEffect kEffect;
	kEffect.m_Type  = TET_ROTATE;
	kEffect.m_fArg1 = fSpeed;
	
	if( bOnce )
		kEffect.m_dwOnce = 1;
	else
		kEffect.m_dwOnce = 0;

	AddTexEffect( kEffect );
}

void ioTextureUnitState::SetTransformAnimation( TextureTransformType ttType,
											    WaveformType waveType,
												float fBase,
												float fFrequence,
												float fPhase,
												float fAmplitude,
												bool bOnce )
{
	TextureEffect kEffect;
	kEffect.m_Type = TET_TRANSFORM;
	kEffect.m_XformType = ttType;
	kEffect.m_WaveType = waveType;
	kEffect.m_fBase = fBase;
	kEffect.m_fFrequency = fFrequence;
	kEffect.m_fPhase = fPhase;
	kEffect.m_fAmplitude = fAmplitude;

	if( bOnce )
		kEffect.m_dwOnce = 1;
	else
		kEffect.m_dwOnce = 0;

	AddTexEffect( kEffect );
}

void ioTextureUnitState::SetColorTransformRate( ColorTransformType ctType, float fRate )
{
	m_pParent->SetColorTransformRate( ctType, fRate );
}

void ioTextureUnitState::SetColorTransform( ColorTransformType ctType,
											WaveformType eWaveType,
											float fBase,
											float fFrequence,
											float fPhase,
											float fAmplitude,
											bool bOnce )
{
	TextureEffect kEffect;
	kEffect.m_Type = TET_COLOR_TRANSFORM;
	kEffect.m_ColorXformType = ctType;
	kEffect.m_WaveType = eWaveType;
	kEffect.m_fBase = fBase;
	kEffect.m_fFrequency = fFrequence;
	kEffect.m_fPhase = fPhase;
	kEffect.m_fAmplitude = fAmplitude;

	if( bOnce )
		kEffect.m_dwOnce = 1;
	else
		kEffect.m_dwOnce = 0;

	AddTexEffect( kEffect );
}

void ioTextureUnitState::StopAllTextureEffect( bool bStop )
{
	TexEffectList::iterator iter = m_TexEffectList.begin();
	for( ; iter!=m_TexEffectList.end() ; ++iter )
	{
		if( iter->m_pController )
		{
			iter->m_pController->StopUpdate( bStop );
		}
	}
}

TextureEffect* ioTextureUnitState::GetTextureEffect( int iIndex )
{
	if( COMPARE( iIndex, 0, GetTextureEffectCnt() ) )
		return &m_TexEffectList[iIndex];

	LOG.PrintTimeAndLog( 0, "ioTextureUnitState::GetTextureEffect - overflow(%d/%d)",
							iIndex, GetTextureEffectCnt() );

	return NULL;
}

