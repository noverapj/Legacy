

#include "stdafx.h"

#include "ioNode.h"
#include "ioSceneNode.h"

#include "ioMeshTrailer.h"
#include "ioEntityGroup.h"

#include "ioAnimation.h"

#include "ioTextureUnitState.h"
#include "ioPass.h"
#include "ioTechnique.h"

#include "ioMaterial.h"
#include "ioMaterialManager.h"
#include "ioMeshControlPoint.h"

#include "ioVertexBufferInstance.h"
#include "ioIndexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderOperation.h"
#include "ioCurveGenerator.h"
#include "ioFrameTimer.h"

#include "ioMeshTrailDataManager.h"
#include "ioRenderSystem.h"
#include "ErrorReport.h"

float ioMeshTrailer::m_fTrailSegmentRate = 1.0f;
bool  ioMeshTrailer::m_bUseGlowTrail = false;

ioMeshTrailer::ioMeshTrailer( ioEntityGroup *pGrp )
{
	m_pOwner = pGrp;
	m_pMeshData = NULL;
	m_pMtl = NULL;
	m_RenderType = FRT_ADD;

	m_iMaxTickSegments = 10;
	m_fMaxTickTime = 200.0f;

	m_fGlowness = 2.0f;
	m_fIntensity = 5.0f;

	m_pTrailData  = NULL;
	m_pTrailAni	  = NULL;
	m_fTrailStartTime = 0.0f;
	m_fAniTimeFactor = 1.0f;

	m_fGraseCloseSetTime = 0.0f;
	m_fGraceCloseAniTime = 0.0f;
	m_bGraceClose = false;

	m_iCurVertexIndex = 0;
	m_bUpdateBufferOK = false;

	m_bMaintain = false;
	m_fMaintainTime = 200.0f;
	m_fMaintainDepth= 0.0f;
	m_fMaintainStartPoint = 0.0f;
	m_fMaintainEndPoint   = 0.0f;
	m_fMaintainGapPoint   = 0.0f;
}

ioMeshTrailer::~ioMeshTrailer()
{
	if( m_pMeshData )
	{
		m_pMeshData->DeleteAllBindingBuffers();
		m_pMeshData->m_pIndexBufferInstance = NULL;
		SAFEDELETE( m_pMeshData );
	}

	SAFEDELETE( m_pMtl );
}

void ioMeshTrailer::SetTexture( const ioHashString &szName )
{
	m_TextureName = szName;
}

void ioMeshTrailer::SetRenderType( FXRenderType eType )
{
	m_RenderType = eType;
}

void ioMeshTrailer::SetMaxTickTime( float fTime )
{
	m_fMaxTickTime = fTime;
}

void ioMeshTrailer::SetMaxSegment( int iMaxSegment )
{
	m_iMaxTickSegments = iMaxSegment;
}

void ioMeshTrailer::SetDepthCheck( bool bDepthCheck )
{
	if( m_pMtl )
	{
		ioTechnique *pTech = m_pMtl->GetBestTechnique();
		ioPass *pPass = NULL;

		if( m_bUseGlowTrail )
		{
			pPass = pTech->GetPass( 3 );
			if( pPass )
				pPass->SetDepthCheckEnable( bDepthCheck );

			pPass = pTech->GetPass( 4 );
			if( pPass )
				pPass->SetDepthCheckEnable( bDepthCheck );
		}
		else
		{
			pPass = pTech->GetPass( 0 );
			if( pPass )
				pPass->SetDepthCheckEnable( bDepthCheck );
		}
	}
}

void ioMeshTrailer::SetGlowness( float fGlow )
{
	m_fGlowness = fGlow;
}

void ioMeshTrailer::SetIntensity( float fIntensity )
{
	m_fIntensity = fIntensity;
}

void ioMeshTrailer::SetStartEndPoint( const ioMeshControlPoint *pStart,
									  const ioMeshControlPoint *pEnd )
{
	m_StartLinkBiped = pStart->GetLinkedBipedName();
	m_vStartPoint = pStart->GetPoint();

	m_EndLinkBiped = pEnd->GetLinkedBipedName();
	m_vEndPoint = pEnd->GetPoint();
}

bool ioMeshTrailer::GenerateTrailData( const ioHashString &rkSkeleton,
									   const ioHashString &rkMeshName,
								       const ioAnimation *pTrailAni,
									   int iCtrlPointIdx,
									   float fSampleTick,
									   float fEventStartTime,
									   float fMaintainTime )
{
	m_pTrailAni = pTrailAni;

	m_pTrailData = ioMeshTrailDataManager::GetSingleton().GenerateTrailData( rkSkeleton,
																			 rkMeshName,
																		     this,
																		     iCtrlPointIdx,
																		     fSampleTick,
																		     fEventStartTime );

	if( !m_pTrailData )
		return false;

	m_fTrailStartTime = fEventStartTime;
	m_fAniTimeFactor  = pTrailAni->GetTimeFactor();

	m_bMaintain       = false;
	m_fMaintainTime   = fMaintainTime;
	m_matMaintainTransform = *m_pOwner->GetTransform();
	int iMaxCtrlPoints = ( m_fMaxTickTime + fSampleTick ) / fSampleTick;
	m_vCtrlPointList.reserve( iMaxCtrlPoints * 2 + 4 );
	return true;
}

void ioMeshTrailer::BuildMaterial()
{
	if( m_bUseGlowTrail )
	{
		BuildGlowMaterial();
	}
	else
	{
		BuildNormalMaterial();
	}
}

void ioMeshTrailer::BuildNormalMaterial()
{
	SAFEDELETE( m_pMtl );

	m_pMtl = g_MaterialMgr.CreateMaterial();
	m_pMtl->SetEmissive( 1.0f, 1.0f, 1.0f );

	ioPass *pPass = m_pMtl->CreateTechnique()->CreatePass();
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "DecalToon" );
	SetPassRenderType( pPass, m_RenderType );

	ioTextureUnitState *pState = pPass->CreateTextureUnitState( m_TextureName );
	pState->SetTextureAddressMode( TAM_CLAMP );
	m_pMtl->Load( EP_MANUAL );
}

#define RTT_SIZE	128

void ioMeshTrailer::BuildGlowMaterial()
{
	SAFEDELETE( m_pMtl );

	m_pMtl = g_MaterialMgr.CreateMaterial();
	m_pMtl->SetEmissive( 1.0f, 1.0f, 1.0f );

	ioTechnique *pTech = m_pMtl->CreateTechnique();

	// 1 Pass Render To RTT
	ioPass *pPass = pTech->CreatePass();
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "GlowToRTT" );
	pPass->SetRenderTarget( "_TrailGlowRTT1", RTT_SIZE, RTT_SIZE );

	ioShaderCustomArg kIntensity( "fIntensity" );
	kIntensity.SetValue( m_fIntensity );
	pPass->AddCustomParameter( kIntensity );

	ioTextureUnitState *pState = pPass->CreateTextureUnitState( m_TextureName );
	pState->SetTextureAddressMode( TAM_CLAMP );

	// 2 Horizon Blur
	pPass = pTech->CreatePass();
	pPass->SetUseQuadBuffer( true );
	pPass->SetDepthCheckEnable( false );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "GlowHorzBlur" );
	pPass->SetRenderTarget( "_TrailGlowRTT2", RTT_SIZE, RTT_SIZE );

	pState = pPass->CreateTextureUnitState( "_TrailGlowRTT1" );
	pState->SetTextureAddressMode( TAM_CLAMP );
	pState->SetRenderTarget( true );

	// 3 Vertical Blur
	pPass = pTech->CreatePass();
	pPass->SetUseQuadBuffer( true );
	pPass->SetDepthCheckEnable( false );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "GlowVertBlur" );
	pPass->SetRenderTarget( "_TrailGlowRTT1", RTT_SIZE, RTT_SIZE );

	ioShaderCustomArg kGlowness( "fGlowness" );
	kGlowness.SetValue( m_fGlowness );
	pPass->AddCustomParameter( kGlowness );

	pState = pPass->CreateTextureUnitState( "_TrailGlowRTT2" );
	pState->SetTextureAddressMode( TAM_CLAMP );
	pState->SetRenderTarget( true );

	// 4 Original Mesh.
	pPass = pTech->CreatePass();
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "GlowNo" );
	SetPassRenderType( pPass, m_RenderType );

	pState = pPass->CreateTextureUnitState( m_TextureName );
	pState->SetTextureAddressMode( TAM_CLAMP );

	// 5 Add Blurred Texture
	pPass = pTech->CreatePass();
	pPass->SetUseQuadBuffer( true );
	pPass->SetDepthCheckEnable( false );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "GlowQuad" );

	switch( m_RenderType )
	{
	case FRT_REV_SUBTRACT:
		pPass->SetSceneBlending( D3DBLENDOP_REVSUBTRACT, D3DBLEND_ONE, D3DBLEND_ONE );
		break;
	case FRT_SUBTRACT:
		pPass->SetSceneBlending( D3DBLENDOP_SUBTRACT, D3DBLEND_ONE, D3DBLEND_ONE );
		break;
	default:
		pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ONE );
		break;
	}

	pState = pPass->CreateTextureUnitState( "_TrailGlowRTT1" );
	pState->SetTextureAddressMode( TAM_CLAMP );
	pState->SetRenderTarget( true );

	pState->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	pState->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );

	m_pMtl->Load( EP_MANUAL );
}

void ioMeshTrailer::SetPassRenderType( ioPass *pPass, FXRenderType eType )
{
	if( !pPass )	return;

	switch( eType )
	{
	case FRT_ADD:
		pPass->SetSceneBlending( BLEND_ADD );
		break;
	case FRT_SRCALPHA:
		pPass->SetSceneBlending( BLEND_SRCALPHA );
		break;
	case FRT_SCREEN:
		pPass->SetSceneBlending( BLEND_SCREEN );
		break;
	case FRT_LIGHTEN:
		pPass->SetSceneBlending( BLEND_LIGHTEN );
		break;
	case FRT_DARKEN:
		pPass->SetSceneBlending( BLEND_DARKEN );
		break;
	case FRT_REV_SUBTRACT:
		pPass->SetSceneBlending( BLEND_REV_SUBTRACT );
		break;
	case FRT_SUBTRACT:
		pPass->SetSceneBlending( BLEND_SUBTRACT );
		break;
	}
}

void ioMeshTrailer::SetGraceClose()
{
	if( m_bGraceClose )	return;

	m_vCtrlPointList.erase( m_vCtrlPointList.begin(), m_vCtrlPointList.end() );

	m_fGraseCloseSetTime = FRAMEGETTIME();
	m_fGraceCloseAniTime = m_pTrailAni->GetCurTime();

	m_pTrailAni = NULL;
	m_bGraceClose = true;
	m_matMaintainTransform = *m_pOwner->GetTransform();
}

bool ioMeshTrailer::UpdateCtrlPoints( float fTime )
{
	if( !m_pTrailData )	return false;
	if( !m_bGraceClose && !m_pTrailAni )
		return false;

	if( m_bGraceClose )
	{
		float fPastTime = ( FRAMEGETTIME() - m_fGraseCloseSetTime ) * m_fAniTimeFactor;
		float fMaxTickTime = min( m_fMaxTickTime, m_fGraceCloseAniTime - m_fTrailStartTime );
		if( fMaxTickTime < fPastTime )
		{
			if( m_fMaintainTime == 0.0f )
				return false;

			if( fMaxTickTime + m_fMaintainTime < fPastTime )
				return false;
			else
			{
				float fGapTime = fPastTime - fMaxTickTime;
				float fOriRate = (float)min( 1.0f, fGapTime / m_fMaintainTime );
				m_fMaintainStartPoint = ( m_fMaintainEndPoint - m_fMaintainGapPoint ) + ( m_fMaintainGapPoint * fOriRate );
				if( m_fMaintainStartPoint > m_fMaintainEndPoint )
				{
					m_fMaintainStartPoint = m_fMaintainEndPoint;
					return false;
				}
			}
			m_bMaintain = true;
		}
	}
	return true;
}

void ioMeshTrailer::NotifyCurrentCamera( ioCamera *pCamera )
{
	m_bUpdateBufferOK = true;

	if( !CreateBuffers() )
	{
		m_bUpdateBufferOK = false;
		return;
	}

	if( !m_bMaintain )
	{
		float fTrailStart, fTrailEnd;
		if( !m_bGraceClose && m_pTrailAni )
		{
			fTrailEnd = m_pTrailAni->GetCurTime();
			fTrailStart = max( fTrailEnd - m_fMaxTickTime, 0.0f );
		}
		else
		{
			fTrailEnd = m_fGraceCloseAniTime;
			fTrailStart = max( fTrailEnd - m_fMaxTickTime, 0.0f );

/*			float fPastCloseTime = ( FRAMEGETTIME() - m_fGraseCloseSetTime ) * m_fAniTimeFactor;
			float fMaxTickTime   = min( m_fMaxTickTime, m_fGraceCloseAniTime - m_fTrailStartTime );

			if( fMaxTickTime > fPastCloseTime )
				fTrailStart = m_fGraceCloseAniTime - ( fMaxTickTime - fPastCloseTime );
			else
				fTrailStart = m_fGraceCloseAniTime;
*/
		}

		fTrailStart = max( fTrailStart, m_fTrailStartTime );
		
		m_fMaintainStartPoint = fTrailStart;
		m_fMaintainEndPoint   = fTrailEnd;
		m_fMaintainGapPoint   = fTrailEnd - fTrailStart;
	}

	m_vCtrlPointList.erase( m_vCtrlPointList.begin(), m_vCtrlPointList.end() );
	m_pTrailData->GetTrailPointRange( m_fMaintainStartPoint, m_fMaintainEndPoint, m_vCtrlPointList );	

	if( m_vCtrlPointList.size() <= 4 )
	{
		m_pMeshData->m_iVertexCount = 0;
		m_pMeshData->m_iIndexCount  = 0;
		m_bUpdateBufferOK = false;
		return;
	}

	int iCtrlCnt = m_vCtrlPointList.size();
	m_GenStart.InitBuffer( iCtrlCnt / 2 + 1 );
	m_GenEnd.InitBuffer( iCtrlCnt / 2 + 1 );

	for( int i=0 ; i<iCtrlCnt ; i+=2 )
	{
		m_GenStart.AddPoint( m_vCtrlPointList[i] );
		m_GenEnd.AddPoint( m_vCtrlPointList[i+1] );
	}

	m_GenStart.BuildSpline();
	m_GenEnd.BuildSpline();

	DWORD dwLockFlags;
	int iLockStartBytes, iLockSizeBytes;

	int iNeedVtxCount = (int)( m_iMaxTickSegments * m_fTrailSegmentRate ) * 2;
	int iMaxVtxCount  = m_iMaxTickSegments * 4;

	if( m_iCurVertexIndex + iNeedVtxCount <= iMaxVtxCount )
	{
		iLockStartBytes = m_iCurVertexIndex * sizeof(ioVtxTex);
		iLockSizeBytes  = iNeedVtxCount * sizeof(ioVtxTex);
		dwLockFlags     = D3DLOCK_NOOVERWRITE;

		m_iCurVertexIndex += iNeedVtxCount;
	}
	else
	{
		iLockStartBytes = 0;
		iLockSizeBytes  = iNeedVtxCount * sizeof(ioVtxTex);
		dwLockFlags     = D3DLOCK_DISCARD;

		m_iCurVertexIndex = iNeedVtxCount;
	}

	ioVertexBufferInstance *pVtxBuf = m_pMeshData->GetVtxInstance( 0 );
	if( !pVtxBuf->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
	{
		m_bUpdateBufferOK = false;
		return;
	}

//	__try
//	{
//		FillBuffers( pVtxBuf->GetBuffer(), iNeedVtxCount / 2 );
//	}
//	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
//	{
//		m_bUpdateBufferOK = false;
//	}

	FillBuffers( pVtxBuf->GetBuffer(), iNeedVtxCount / 2 );

	pVtxBuf->UnLock();
    
	if( m_bUpdateBufferOK )
	{
		m_pMeshData->m_iVertexCount = iNeedVtxCount;
		m_pMeshData->m_iIndexCount  = iNeedVtxCount;
	}
}

void ioMeshTrailer::FillBuffers( void *pBuf, int iCurMaxSegment )
{
	if( !pBuf )
		return;

	ioVtxTex *pVtxBuf = (ioVtxTex*)pBuf;

	float fCurRate = 0.0f;
	float fRateGap = 1.0f / ( iCurMaxSegment - 1 );

	float fCurU = 0.0f;
	float fDivOff = 1.0f / (float)( iCurMaxSegment - 1 );	// 1.0f이 끝에 오도록

	for( int i=0 ; i<iCurMaxSegment ; i++ )
	{
		pVtxBuf->vPos = m_GenStart.GetPosition( fCurRate );
		pVtxBuf->u = fCurU;
		pVtxBuf->v = 1.0f;
		pVtxBuf++;

		pVtxBuf->vPos = m_GenEnd.GetPosition( fCurRate );
		pVtxBuf->u = fCurU;
		pVtxBuf->v = 0.0f;
		pVtxBuf++;

		fCurRate += fRateGap;
		fCurU += fDivOff;
	}
}

bool ioMeshTrailer::CreateBuffers()
{
	if( m_pMeshData )
		return true;

	int iOneTimeVertex = m_iMaxTickSegments * 2;
	ioVertexBufferInstance *pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC,
																				IOFVF_POSITION | IOFVF_UV0,
																				iOneTimeVertex * 2 );

	if( !pVtxInstance )
		return false;

	m_pMeshData = new ioMeshData;
	m_pMeshData->InsertStreamSource( 0, IOFVF_POSITION | IOFVF_UV0 );
	m_pMeshData->SetBinding( 0, pVtxInstance );

	m_pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyStripSharedBuffer( iOneTimeVertex );

	m_pMeshData->m_iVertexCount = iOneTimeVertex;
	m_pMeshData->m_iIndexCount  = iOneTimeVertex;

	m_iCurVertexIndex = 0;

	return true;
}

const ioHashString& ioMeshTrailer::GetTrailAniName() const
{
	return m_pTrailAni->GetName();
}

ioMaterial* ioMeshTrailer::GetMaterial() const
{
	return m_pMtl;
}

float ioMeshTrailer::GetSquaredViewDepth( const ioCamera *pCamera )
{
	if( m_bMaintain )
	{
		if( m_fMaintainDepth == 0.0f )
			m_fMaintainDepth = m_pOwner->GetSquaredViewDepth( pCamera );
		return m_fMaintainDepth;
	}
	return m_pOwner->GetSquaredViewDepth( pCamera );
}

void ioMeshTrailer::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	m_pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyStripSharedBuffer( m_pMeshData->m_iIndexCount );

	rkRenderOp.m_pMeshData = m_pMeshData;
	rkRenderOp.m_PrimitiveType = D3DPT_TRIANGLESTRIP;

	rkRenderOp.m_iMinIndex    = 0;
	rkRenderOp.m_iVertexCount = m_pMeshData->m_iVertexCount;
	rkRenderOp.m_iBaseVertexIndex = m_iCurVertexIndex - m_pMeshData->m_iVertexCount;

	rkRenderOp.m_iIndexStart = 0;
	rkRenderOp.m_iFaceCount  = m_pMeshData->m_iIndexCount - 2;
	rkRenderOp.m_bUseIndex = true;
}

void ioMeshTrailer::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	if( m_bGraceClose )
	{
		*pMatWorld = m_matMaintainTransform;
	}
	else
	{
		*pMatWorld = *m_pOwner->GetTransform();
	}
}

bool ioMeshTrailer::SetUseGlowTrail( bool bUse )
{
	if( RenderSystem().GetPixelShaderVersion() < 20 )
	{
		m_bUseGlowTrail = false;
	}
	else
	{
		m_bUseGlowTrail = bUse;
	}

	return m_bUseGlowTrail;
}

void ioMeshTrailer::SetTrailSegmentRate( float fRate )
{
	m_fTrailSegmentRate = max( 0.3f, min( fRate, 1.0f ) );
}
