

#include "stdafx.h"

#include "ioResource.h"
#include "ioRcAnimateFX.h"
#include "ioRcAniTrack.h"
#include "ioMeshData.h"

#include "ioMaterial.h"
#include "ioTechnique.h"
#include "ioPass.h"
#include "ioTextureUnitState.h"
#include "ioMaterialManager.h"
#include "ioAnimateFXManager.h"

ioRcAnimatePlane::ioRcAnimatePlane()
{
	m_pMeshData = new ioMeshData;
	m_pTrack = NULL;
	m_pMtl = NULL;
	
	m_fStartTime = 0.0f;
	m_fDuration = 1.0f;

	m_RenderType = FRT_ADD;
	m_TransformType = FXT_OWNER;
}

ioRcAnimatePlane::~ioRcAnimatePlane()
{
	SAFEDELETE( m_pMtl );
	SAFEDELETE( m_pTrack );

	m_pMeshData->DeleteAllBuffers();
	SAFEDELETE( m_pMeshData );
}

ioRcAniTrack* ioRcAnimatePlane::CreateTrack()
{
	SAFEDELETE( m_pTrack );
	m_pTrack = new ioRcAniTrack;

	return m_pTrack;
}

bool ioRcAnimatePlane::GetCurFrame( float fCurTime, D3DXVECTOR3 &vTrans, D3DXQUATERNION &qtRot )
{
	if( m_pTrack )
	{
		m_pTrack->GetInterpolatedFrame( fCurTime, 0, vTrans, qtRot );
		return true;
	}

	return false;
}

void ioRcAnimatePlane::AddAlphaRate( float fTimeRate, float fAlphaRate )
{
	m_AlphaRateMap.AddTimeFactor( fTimeRate, fAlphaRate );
}

void ioRcAnimatePlane::SetAlphaRate( int iIdx, float fAlphaRate )
{
	m_AlphaRateMap.SetFactor( iIdx, fAlphaRate );
}

float ioRcAnimatePlane::GetAlphaRateCnt() const
{
	return m_AlphaRateMap.GetFactorCnt();
}

float ioRcAnimatePlane::GetPlaneAlphaRate( float fTimeRate )
{
	return m_AlphaRateMap.GetCurFactor( fTimeRate );
}

float ioRcAnimatePlane::GetTimeRate( int iIdx )
{
	return m_AlphaRateMap.GetTimeRate( iIdx );
}

float ioRcAnimatePlane::GetAlphaRate( int iIdx )
{
	return m_AlphaRateMap.GetFactor( iIdx );
}

void ioRcAnimatePlane::RemoveAlphaRate( int iIdx )
{
	m_AlphaRateMap.RemoveFactor( iIdx );
}

void ioRcAnimatePlane::UpdateAlphaRateByDuration( float fNewDuration )
{
	if( m_fDuration == fNewDuration )
		return;

	int iAlphaRateCnt = GetAlphaRateCnt();
	if( m_fDuration > fNewDuration )	// 시간이 줄었음 뒤에서부터 잘라나간다.
	{
		float fLimitRate = fNewDuration / m_fDuration;
		for( int i=iAlphaRateCnt-1 ; i>0 ; i-- )
		{
			float fTimeRate = GetTimeRate( i );

			if( fTimeRate > fLimitRate )
			{
				RemoveAlphaRate( i );
			}
			else
			{
				break;
			}
		}

	}
	else	// 시간이 늘었음
	{
		std::vector< float > vTimeList, vAlphaList;
		vTimeList.reserve( iAlphaRateCnt );
		vAlphaList.reserve( iAlphaRateCnt );
		for( int i=0 ; i<iAlphaRateCnt ; i++ )
		{
			vTimeList.push_back( m_AlphaRateMap.GetTimeRate(i) );
			vAlphaList.push_back( m_AlphaRateMap.GetFactor(i) );
		}

		m_AlphaRateMap.RemoveAllFactor();

		float fScaleRate = m_fDuration / fNewDuration;
		for( i=0 ; i<iAlphaRateCnt ; i++ )
		{
			float fNewTimeRate = vTimeList[i] * fScaleRate;
			m_AlphaRateMap.AddTimeFactor( fNewTimeRate, vAlphaList[i] );
		}
	}

	SetDuration( fNewDuration );
}

void ioRcAnimatePlane::SetWorldTMInverse( const D3DXMATRIX &matInv )
{
	m_InvWorldTM = matInv;
}

void ioRcAnimatePlane::SetTextureName( const char *szName )
{
	SAFEDELETE( m_pMtl );

	m_TextureName = szName;

	m_pMtl = g_MaterialMgr.CreateMaterial();

	ioPass *pPass = m_pMtl->CreateTechnique()->CreatePass();
	pPass->SetDepthCheckEnable( true );
	pPass->SetDepthWriteEnable( false );
	pPass->SetLightingEnable( false );
	pPass->SetCullingMode( CM_NO );
	pPass->SetShaderGroup( "AnimateFX" );
	pPass->CreateTextureUnitState( m_TextureName );
	pPass->SetAlphaTestSetting( D3DCMP_GREATER, 0 );
	ApplyRenderTypeToMaterial( m_RenderType );

	m_pMtl->Load( EP_MANUAL );
}

void ioRcAnimatePlane::ApplyRenderTypeToMaterial( FXRenderType eType )
{
	if( !m_pMtl )	return;

	ioPass *pPass = m_pMtl->GetBestTechnique()->GetPass(0);
	if( !pPass )	return;

	ioTextureUnitState *pState = pPass->GetTextureUnitState(0);
	if( !pState )	return;

	switch( eType )
	{
	case FRT_ADD:
		pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_SRCALPHA:
		pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
		pState->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_SCREEN:
		pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_INVDESTCOLOR, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_LIGHTEN:
		pPass->SetSceneBlending( D3DBLENDOP_MAX, D3DBLEND_ONE, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_DARKEN:
		pPass->SetSceneBlending( D3DBLENDOP_MIN, D3DBLEND_ONE, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_REV_SUBTRACT:
		pPass->SetSceneBlending( D3DBLENDOP_REVSUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	case FRT_SUBTRACT:
		pPass->SetSceneBlending( D3DBLENDOP_SUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_ONE );
		pState->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		break;
	}
}

void ioRcAnimatePlane::SetLinkBipedName( const char *szName )
{
	m_LinkBipedName = szName;
}

void ioRcAnimatePlane::SetRenderType( FXRenderType eType )
{
	m_RenderType = eType;
	ApplyRenderTypeToMaterial( eType );
}

void ioRcAnimatePlane::SetTransformType( FXTransformType eType )
{
	m_TransformType = eType;
}

void ioRcAnimatePlane::SetStartTime( float fStartTime )
{
	m_fStartTime = fStartTime;
}

void ioRcAnimatePlane::SetDuration( float fDuration )
{
	m_fDuration = fDuration;
}

ioRcAnimateFX::ioRcAnimateFX( const ioHashString &name )
: ioResource( name )
{
	m_fBoundRadius = 0.0f;
}

ioRcAnimateFX::~ioRcAnimateFX()
{
	RcAnimatePlaneList::iterator iter;
	for( iter=m_AnimatePlaneList.begin() ; iter!=m_AnimatePlaneList.end() ; ++iter )
	{
		delete *iter;
	}
	m_AnimatePlaneList.clear();

	g_AnimateFXMgr.RemoveMem( GetName() );
}

ioRcAnimatePlane* ioRcAnimateFX::CreatePlane()
{
	ioRcAnimatePlane *pPlane = new ioRcAnimatePlane;
	m_AnimatePlaneList.push_back( pPlane );

	return pPlane;
}

ioRcAnimatePlane* ioRcAnimateFX::GetPlane( int i )
{
	if( COMPARE( i, 0, GetPlaneCnt() ) )
		return m_AnimatePlaneList[i];

	LOG.PrintTimeAndLog( 0, "ioRcAnimateFX::GetPlane - overflow (%d/%d)", i, GetPlaneCnt() );

	return NULL;
}

int ioRcAnimateFX::GetPlaneCnt() const
{
	return m_AnimatePlaneList.size();
}

void ioRcAnimateFX::SetBoundBox( const D3DXVECTOR3 &vMinPos, const D3DXVECTOR3 &vMaxPos )
{
	m_BoundBox.SetMinMaxPos( vMinPos, vMaxPos );
}

void ioRcAnimateFX::SetBoundRadius( float fRadius )
{
	m_fBoundRadius = fRadius;
}
