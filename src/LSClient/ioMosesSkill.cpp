#include "StdAfx.h"

#include "ioMosesSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioMosesSkill::ioMosesSkill(void)
{
	ClearData();
}

ioMosesSkill::ioMosesSkill( const ioMosesSkill& rhs )
: ioAttackSkill( rhs ),
m_dwAddDuration( rhs.m_dwAddDuration ),
m_dwEnableKeyTime( rhs.m_dwEnableKeyTime ),
m_dwAddDurationMax( rhs.m_dwAddDurationMax ),
m_dwEnableKeyTimeMax( rhs.m_dwEnableKeyTimeMax ),
m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_fScaleRate( rhs.m_fScaleRate ),
m_fMaxScaleRate( rhs.m_fMaxScaleRate ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioMosesSkill::~ioMosesSkill(void)
{
}

void ioMosesSkill::ClearData()
{
	m_SkillState = SS_PRE;
	m_bAddKey = false;
	m_dwCheckStartTime = 0;
	m_fZoneRangeTemp = 0.0f;
	m_fZoneWidthTemp = 0.0f;
	m_fAreaRangeTemp = 0.0f;
	m_fAreaWidthTemp = 0.0f;
	m_fScaleRateTemp = 0.0f;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_bCheckEffctScale = true;
}

void ioMosesSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// key
	m_dwAddDuration = rkLoader.LoadInt_e( "key_add_duration", 0 );
	m_dwEnableKeyTime = rkLoader.LoadInt_e( "key_enable_time", 0 );

	m_dwAddDurationMax = rkLoader.LoadInt_e( "key_add_duration_max", 0 );
	m_dwEnableKeyTimeMax = rkLoader.LoadInt_e( "key_enable_time_max", 0 );
	
	m_fScaleRate = rkLoader.LoadFloat_e( "skill_scale_rate", 0.0f );
	m_fMaxScaleRate = rkLoader.LoadFloat_e( "skill_max_rate", 0.0f );
	
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
	
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_fTimeRate = rkLoader.LoadFloat_e( "loop_animation_time_rate",1.0f );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

}

ioSkill* ioMosesSkill::Clone()
{
	return new ioMosesSkill( *this );
}

bool ioMosesSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( !pChar ) return false;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;
	
	if( !HasSkillMotion() )
		return false;

	ClearData();
	pChar->AttachEffect( m_SkillEffectName );

	m_SkillState = SS_PRE;
	m_dwCheckTime = FRAMEGETTIME();
	pChar->SetSkillProtection();
	
	SetCameraBuff( pChar );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	ioPlayStage *pStage = pChar->GetCreator();

	if( !m_bDisableGrowth )
	{		
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);
		}
	}

	return true;
}

void ioMosesSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{

	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
	}
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;
	
	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		{	
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				CreateAreaWeapon( pChar, pChar->GetWorldPosition() );
				m_dwFireStartTime = 0;
			}

			DWORD dwCheck1 = 0;
			DWORD dwCheck2 = 0;
			
			if( m_fMaxScaleRate > m_fScaleRateTemp )
			{
				dwCheck1 = m_dwCheckStartTime + m_dwEnableKeyTime;
				dwCheck2 = m_dwCheckStartTime + m_dwAddDuration;
			}
			else
			{
				dwCheck1 = m_dwCheckStartTime + m_dwEnableKeyTimeMax;
				dwCheck2 = m_dwCheckStartTime + m_dwAddDurationMax;
			}

			if( pChar->IsNeedProcess() )
			{
				// 키연타 체크
				if( !m_bAddKey && dwCheck1 < dwCheck2 )
				{
					if( m_dwCheckStartTime > 0 && COMPARE( dwCurTime, dwCheck1, dwCheck2 ) )
					{
						if( pChar->IsAttackKey() )
						{
							m_bAddKey = true;
							
							if( pChar->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
								kPacket << pChar->GetCharName();
								kPacket << GetName();
								kPacket << static_cast<int>(m_SkillState);
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}
						}
					}
				}
			}

			if( dwCheck2 < dwCurTime )
			{
				if( m_bAddKey )
				{
					ioEntityGroup *pGrp = pChar->GetGroup();
					if ( pGrp )
					{
						pGrp->ClearAllLoopAni( FLOAT100, true );
						pGrp->ClearAllActionAni( FLOAT100, true );
						pGrp->SetActionAni( m_iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/ m_fTimeRate );
					}

					m_fScaleRateTemp += m_fScaleRate;
					m_bAddKey = false;
					m_dwCheckStartTime = dwCurTime;
					ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIdx );
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					
					if ( pZone )
					{
						m_fZoneRangeTemp += m_fAreaRangeRate * 2.0f;
						m_fZoneWidthTemp += m_fAreaWidthRate * 2.0f;
						
						if ( m_fMaxScaleRate >= m_fScaleRateTemp )
						{
							pZone->SetZoneRange( m_fZoneRangeTemp );
							pZone->SetZoneWidth( m_fZoneWidthTemp );
						}
					}
					ioAreaWeapon *pArea = FindAreaWeapon( pChar );
					if ( pArea )
					{
						m_fAreaRangeTemp += m_fAreaRangeRate;
						m_fAreaWidthTemp += m_fAreaWidthRate;
						D3DXVECTOR3 vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
						D3DXVec3Normalize( &vAttackDir, &vAttackDir );

						pArea->SetAreaDirection( vAttackDir );

						if ( m_fMaxScaleRate >= m_fScaleRateTemp )
						{
							D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
							D3DXVECTOR3 vAreaPos( 0.0f, 0.0f, 0.0f );
							vAreaPos = ( m_fZoneRangeTemp * FLOAT05 ) * vAttackDir;
							vAreaPos = vCharPos + vAreaPos;
							D3DXVECTOR3 vAreaEffectScale;
							if ( m_bCheckEffctScale )
							{
								vAreaEffectScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
								m_bCheckEffctScale = false;
							}
							else
								vAreaEffectScale = D3DXVECTOR3( FLOAT1 + m_fScaleRate, FLOAT1, FLOAT1 + m_fScaleRate );
							
							pArea->SetAreaEffectScale( pStage, vAreaEffectScale  );
							pArea->SetAreaRange( m_fAreaRangeTemp );
							pArea->SetAreaWidth( m_fAreaWidthTemp );
							pArea->SetAreaPosition( vAreaPos, pStage);
						}
					}
				}
				else
				{
					SetEndState( pChar );
				}
			}
		}
		break;
	case SS_END:
		{
			
		}
		break;
	}
	if( pChar->IsNeedProcess() )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pChar );
				}
			}
		}
	}	
}

void ioMosesSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}
	
	m_bSetHandMesh = false;
	
	ioWeapon *pWeapon  = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIdx );
	if ( pWeapon )
	{
		pWeapon->SetWeaponDead();
	}
	
	EndCameraBuff( pChar );
	
	EndPreDelayEffect( pChar );
	
	DestroyAreaWeapon( pChar );

	pChar->EndEffect( m_SkillEffectName );
	GetOwnerItem()->ClearTarget();

	ClearData();
}

void ioMosesSkill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	
	if ( m_SkillState == SS_LOOP )
		return;

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();
	if ( !pGrp )
		return;
	m_iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( m_iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	m_fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( m_iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/ m_fTimeRate );
	
	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;


	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			m_iCurSkillAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			m_fTimeRate,
			dwPreDelay );
	}

	m_bAddKey = false;
	m_dwCheckStartTime = FRAMEGETTIME();

	m_dwFireStartTime = m_dwFireMotionEndTime = FRAMEGETTIME();

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( m_iCurSkillAniID, "fire_s" ) * m_fTimeRate;
}

void ioMosesSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );

    m_dwWeaponIdx = pWeapon->GetWeaponIndex();
	
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if ( pZone )
	{
		D3DXVECTOR3 vAttackDir = pWeapon->GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vAttackDir, &vAttackDir );
		pZone->SetCollisionDir( vAttackDir );
		m_fZoneRange = pZone->GetZoneRange();
		m_fZoneWidth = pZone->GetZoneWidth();

		m_fZoneRangeTemp = m_fZoneRange;
		m_fZoneWidthTemp = m_fZoneWidth;
	}
}

void ioMosesSkill::CreateAreaWeapon( ioBaseChar* pChar, const D3DXVECTOR3& vPos )
{
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		"",
		GetName(),
		m_AreaWeaponName,
		vPos,
		pChar->GetTargetRot(),
		ioAreaWeapon::CT_NORMAL );
	
	if ( pArea )
	{
		m_dwCreatedAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		m_fAreaRange = pArea->GetAreaRange();
		m_fAreaWidth = pArea->GetAreaWidth();
		pArea->SetAreaEffectScale( pChar->GetCreator(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );

		m_fAreaRangeTemp = m_fAreaRange;
		m_fAreaWidthTemp = m_fAreaWidth;

		m_fAreaRangeRate = m_fAreaRange * m_fScaleRate; 
		m_fAreaWidthRate = m_fAreaWidth * m_fScaleRate;
	}
}

ioAreaWeapon* ioMosesSkill::FindAreaWeapon( ioBaseChar *pChar )
{
	if( m_dwCreatedAreaWeaponIndex == 0 )
		return NULL;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwCreatedAreaWeaponIndex );
	if ( !pArea )
		return NULL;

	return pArea ;
}

void ioMosesSkill::DestroyAreaWeapon( ioBaseChar *pChar )
{
	if( m_dwCreatedAreaWeaponIndex )
	{
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwCreatedAreaWeaponIndex );
	}

	m_dwCreatedAreaWeaponIndex = 0;
}

bool ioMosesSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
		return false;
		break;
	case SS_END:
		return true;
		break;
	}
	return true;
}	

void ioMosesSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_LOOP:
		m_bAddKey = true;
		break;
	case SS_END:
		m_SkillState = SS_END;
		break;
	}
	
}

void ioMosesSkill::SetEndState( ioBaseChar *pChar )
{
	m_SkillState = SS_END;
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<int>( m_SkillState );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMosesSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwCheckStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioAttackSkill::IsProtected( iDefenseBreakType );
}
