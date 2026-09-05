

#include "stdafx.h"

#include "../io3DEngine/ioVertexBufferManager.h"
#include "../io3DEngine/ioMeshData.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMagicCircle.h"

ioMagicCircle::ioMagicCircle()
{
	m_dwReadyEffect = -1;
	m_dwEnableEffect = -1;

	m_dwCurGalaxyEffect = -1;

	m_vStartPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCurPos   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vPrePos   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_bEnableSkill = false;
	m_bEnableRender = true;
	m_bOtherVisible = false;

	m_dwVisibleTime = 0;

	m_MagicCircleState = MCS_NONE;
}

ioMagicCircle::~ioMagicCircle()
{
}

void ioMagicCircle::DestroyCircle( ioPlayStage *pStage )
{
	if( !pStage ) return;

	if( m_dwReadyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwReadyEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwReadyEffect = -1;
	}

	if( m_dwEnableEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwEnableEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwEnableEffect = -1;
	}

	if( m_dwCurGalaxyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurGalaxyEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwCurGalaxyEffect = -1;
	}
}

void ioMagicCircle::InitResource( const ioHashString &ReadyCircle,
								  const ioHashString &EnableCircle,
								  const ioHashString &GuidCircle,
								  const D3DXVECTOR3 &vPos,
								  ioPlayStage *pStage )
{
	m_ReadyCircle  = ReadyCircle;
	m_EnableCircle = EnableCircle;
	m_GuidCircle = GuidCircle;
	m_dwVisibleTime = 0;
	m_vCurPos = vPos;

	m_MagicCircleState = MCS_NONE;

	if( !pStage ) return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, vPos, vScale );
	
	if( pMapEffect )
	{
		m_dwReadyEffect = pMapEffect->GetUniqueID();
	}
}

void ioMagicCircle::SetVisible( bool bVisible )
{
	m_bOtherVisible = bVisible;
}

void ioMagicCircle::SetEnableRender( bool bEnable )
{
	m_bEnableRender = bEnable;
}

void ioMagicCircle::SetVisibleTime( ioPlayStage *pStage )
{
	if( !pStage ) return;

	m_dwVisibleTime = FRAMEGETTIME();

	if( !m_GuidCircle.IsEmpty() )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_GuidCircle, m_vCurPos, vScale );
	}
}

void ioMagicCircle::UpdatePos( bool bEnableSkill, const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage )
{
	switch( m_MagicCircleState )
	{
	case MCS_NONE:
		UpdateNonePos( bEnableSkill, vPos, pOwner, pStage );
		break;
	case MCS_NORMAL:
	case MCS_NORMAL_FIX:
	case MCS_SPECIAL:
	case MCS_SPECIAL_FIX:
		UpdateGalaxyPos( vPos, pOwner, pStage );
		break;
	}
}

void ioMagicCircle::SetTargetRot( D3DXQUATERNION qtRotate )
{
	m_TargetRotate = qtRotate;
}

UniqueObjID ioMagicCircle::GetReadyCircelID() const
{
	return m_dwReadyEffect;
}

void ioMagicCircle::UpdateNonePos( bool bEnableSkill, const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_vPrePos = m_vCurPos;

	D3DXVECTOR3 vNewPos = vPos;
	if( pStage )
		pStage->CheckAxisLimit( &vNewPos );

	m_vCurPos = vNewPos;

	D3DXVECTOR3 vStartPos( 0.0f, 0.0f, 0.0f );
	if( vStartPos == m_vStartPos )
		m_vStartPos = m_vCurPos;

	m_bEnableSkill = bEnableSkill;

	if( m_bEnableSkill && m_dwEnableEffect == -1 && pStage )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_EnableCircle, m_vCurPos, vScale );
		
		if( pMapEffect )
		{
			m_dwEnableEffect = pMapEffect->GetUniqueID();
		}
	}

	// 이펙트 위치 갱신 및 출력 갱신
	if( m_dwReadyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwReadyEffect );
		if( pEff )
		{
			pEff->SetWorldPosition( m_vCurPos );

			if( m_bEnableRender )
			{
				if( m_bOtherVisible )
					pEff->SetVisible( true );
				else if( pOwner && pOwner->IsOwnerChar() )
					pEff->SetVisible( true );
				else
				{
					pEff->SetVisible( false );
					/*
					DWORD dwCurTime = FRAMEGETTIME();
					if( m_dwVisibleTime > 0 && m_dwVisibleTime+100 >= dwCurTime )
					{
						pEff->SetVisible( true );
					}
					else
					{
						m_dwVisibleTime = 0;
						pEff->SetVisible( false );
					}
					*/
				}
			}
			else
			{
				pEff->SetVisible( false );
			}
		}
	}

	if( m_dwEnableEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwEnableEffect );
		if( pEff )
		{
			pEff->SetWorldPosition( m_vCurPos );

			if( m_bEnableRender )
			{
				if( m_bOtherVisible )
					pEff->SetVisible( true );
				else if( pOwner && pOwner->IsOwnerChar() )
					pEff->SetVisible( true );
				else
					pEff->SetVisible( false );
			}
			else
			{
				pEff->SetVisible( false );
			}
		}
	}
}

void ioMagicCircle::UpdateGalaxyPos( const D3DXVECTOR3 &vPos, ioBaseChar *pOwner, ioPlayStage *pStage )
{
	m_vPrePos = m_vCurPos;

	D3DXVECTOR3 vNewPos = vPos;
	if( pStage )
		pStage->CheckAxisLimit( &vNewPos );

	m_vCurPos = vNewPos;

	D3DXVECTOR3 vStartPos( 0.0f, 0.0f, 0.0f );
	if( vStartPos == m_vStartPos )
		m_vStartPos = m_vCurPos;

	// 이펙트 위치 갱신 및 출력 갱신
	if( m_dwCurGalaxyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurGalaxyEffect );
		if( pEff )
		{
			pEff->SetWorldPosition( m_vCurPos );

			if( m_bEnableRender )
			{
				if( m_bOtherVisible )
					pEff->SetVisible( true );
				else if( pOwner && pOwner->IsOwnerChar() )
					pEff->SetVisible( true );
				else
					pEff->SetVisible( false );
			}
			else
			{
				pEff->SetVisible( false );
			}
		}
	}
}

void ioMagicCircle::SetGalaxyCircle( const ioHashString &szGalaxyNormal,
									 const ioHashString &szGalaxyNormalFix,
									 const ioHashString &szGalaxySpecial,
									 const ioHashString &szGalaxySpecialFix,
									 ioPlayStage *pStage )
{
	if( !pStage ) return;

	m_GalaxyNormal = szGalaxyNormal;
	m_GalaxyNormalFix = szGalaxyNormalFix;
	m_GalaxySpecial = szGalaxySpecial;
	m_GalaxySpecialFix = szGalaxySpecialFix;

	m_MagicCircleState = MCS_NORMAL;

	if( m_dwReadyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwReadyEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwReadyEffect = -1;
	}

	if( m_dwEnableEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwEnableEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwEnableEffect = -1;
	}

	if( m_dwCurGalaxyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurGalaxyEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwCurGalaxyEffect = -1;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_GalaxyNormal, m_vCurPos, vScale );
	
	if( pMapEffect )
	{
		m_dwCurGalaxyEffect = pMapEffect->GetUniqueID();
	}
}

void ioMagicCircle::ChangeGalaxyCircle( MagicCircleState eState, ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_MagicCircleState == eState )
		return;

	if( m_dwCurGalaxyEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurGalaxyEffect );
		if( pEff )
			pEff->EndEffectForce();

		m_dwCurGalaxyEffect = -1;
	}

	m_MagicCircleState = eState;

	ioHashString szCurCircle;
	switch( m_MagicCircleState )
	{
	case MCS_NORMAL:
		szCurCircle = m_GalaxyNormal;
		break;
	case MCS_NORMAL_FIX:
		szCurCircle = m_GalaxyNormalFix;
		break;
	case MCS_SPECIAL:
		szCurCircle = m_GalaxySpecial;
		break;
	case MCS_SPECIAL_FIX:
		szCurCircle = m_GalaxySpecialFix;
		break;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurCircle, m_vCurPos, vScale );
	
	if( pMapEffect )
	{
		m_dwCurGalaxyEffect = pMapEffect->GetUniqueID();
	}
}

