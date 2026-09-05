#include "StdAfx.h"
#include "ioCircleMoveAttackSkill9.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"


ioCircleMoveAttackSkill9::ioCircleMoveAttackSkill9(void)
{
	ClearData();
}

ioCircleMoveAttackSkill9::ioCircleMoveAttackSkill9( const ioCircleMoveAttackSkill9 &rhs ):
ioNormalSkill( rhs ),
m_szCircleMoveLoopAni( rhs.m_szCircleMoveLoopAni ),
m_fCircleMoveLoopAniRate( rhs.m_fCircleMoveLoopAniRate ),
m_dwCircleMoveLoopDuration( rhs.m_dwCircleMoveLoopDuration ),
m_fCircleRange( rhs.m_fCircleRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_fCircleOffSet( rhs.m_fCircleOffSet ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_RedGuidCircle( rhs.m_RedGuidCircle ),
m_BlueGuidCircle( rhs.m_BlueGuidCircle ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_szLoopAni( rhs.m_szLoopAni ),
m_fLoopAniRate( rhs.m_fLoopAniRate ),
m_dwLoopDuration( rhs.m_dwLoopDuration ),
m_SkillEndMotion( rhs.m_SkillEndMotion ),
m_fEndMotionRate( rhs.m_fEndMotionRate ),
m_CircleWeaponList( rhs.m_CircleWeaponList ),
m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioCircleMoveAttackSkill9::~ioCircleMoveAttackSkill9(void)
{
}

ioSkill* ioCircleMoveAttackSkill9::Clone()
{
	return new ioCircleMoveAttackSkill9( *this );
}

void ioCircleMoveAttackSkill9::ClearData()
{
	m_SkillState = SS_NONE;

	m_bSetHandMesh = false;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_dwCheckTime = 0;

	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwFireStartTime = 0;

	m_dwSkillProtectStartTime = 0;

	m_dwCircleMapEffectID = -1;
}

void ioCircleMoveAttackSkill9::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH],szKey[MAX_PATH];

	rkLoader.LoadString_e( "skill_circle_move_loop_ani", "", szBuf, MAX_PATH );
	m_szCircleMoveLoopAni = szBuf;
	m_fCircleMoveLoopAniRate = rkLoader.LoadFloat_e( "skill_circle_move_loop_ani_rate", 0.0f );
	m_dwCircleMoveLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_circle_move_loop_duration", 0 );

	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "red_guid_circle", "", szBuf, MAX_PATH );
	m_RedGuidCircle = szBuf;
	rkLoader.LoadString_e( "blud_guid_circle", "", szBuf, MAX_PATH );
	m_BlueGuidCircle = szBuf;

	//gauge
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "skill_loop_ani", "", szBuf, MAX_PATH );
	m_szLoopAni = szBuf;
	m_fLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_ani_rate", 0.0f );
	m_dwLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_SkillEndMotion = szBuf;
	m_fEndMotionRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

	int iCount = rkLoader.LoadInt_e( "circle_attack_type_count", 0 );
	m_CircleWeaponList.clear();
	m_CircleWeaponList.reserve( iCount );
	for ( int i=0 ; i<iCount ; ++i )
	{
		WeaponInfo Info;
		wsprintf_e( szKey, "circle_attack_type%d", i+1 );
		Info.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "circle_attack_type%d_resist", i+1 );
		Info.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "circle_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "circle_attack_type%d_wounded_time", i+1 );
		Info.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "circle_attack_type%d_wounded_loop_ani", i+1 );
		Info.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CircleWeaponList.push_back( Info );
	}

	 m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

bool ioCircleMoveAttackSkill9::OnSkillStart( ioBaseChar *pChar )
{
	if ( !ioNormalSkill::OnSkillStart( pChar ) )
		return false;

	ClearData();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetCameraBuff( pChar );

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

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	

	m_dwCheckTime = FRAMEGETTIME();

	return true;
}

void ioCircleMoveAttackSkill9::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );
				SetCircleMoveState( pChar );
			}
		}
		break;
	case SS_CIRCLE_MOVE:
		{
			if ( m_dwCircleMoveLoopDuration > 0 && m_dwMotionStartTime + m_dwCircleMoveLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				break;
			}

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();
			pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );
			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pChar->GetMagicCirclePos();
			pChar->SetTargetRotToTargetPos( vTargetPos, true, false );

			if ( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetActionState( pChar );
				break;
			}
		}
		break;
	case SS_ACTION:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar );
				break;
			}
			else if( m_dwFireStartTime < dwCurTime )
			{
				CreateWeapon( pChar, pStage );
				break;
			}
		}
		break;
	case SS_LOOP:
		{
			if ( m_dwLoopDuration > 0 && m_dwMotionStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				break;
			}

			// 뱡향 변화 체크
			pChar->CheckChangeDirectionForMagicCircle();
			pChar->UpdateMagicCirclePosAll( 0, m_fCircleRange, m_fCircleHeightGap, false );

			if ( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetActionState( pChar );
				break;
			}
		}
		break;
	case SS_END:
		break;
	}

	//float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	
	//int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	//int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{
			LOG.PrintTimeAndLog( 0, "%s reduce Gauge", __FUNCTION__ );
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				if( m_SkillState != SS_ACTION )
					SetEndState( pChar );
			}
		}
	}
}

void ioCircleMoveAttackSkill9::SetCircleMoveState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	if ( m_SkillState == SS_CIRCLE_MOVE )
		return;

	SetMotionTime( 0, 0 );

	m_SkillState = SS_CIRCLE_MOVE;

	int iAniID = pGrp->GetAnimationIdx( m_szCircleMoveLoopAni );
	if( iAniID == -1 )	
		return;

	float fAniRate = FLOAT1;
	if ( fAniRate > 0.0f )
		fAniRate = m_fCircleMoveLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100,1.0f, FLOAT1/ fAniRate );

	pChar->AttachEffect( m_SkillEffectName );

	m_dwSkillProtectStartTime = m_dwMotionStartTime = FRAMEGETTIME();

	if( !pChar->CheckMagicCircle() )
	{
		// 팀 체크
		if( pChar->GetTeam() == TEAM_RED )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		else if( pChar->GetTeam() == TEAM_BLUE )
			pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_BlueGuidCircle, m_fCircleOffSet );
			else
				pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, m_RedGuidCircle, m_fCircleOffSet );
		}
	}
}

void ioCircleMoveAttackSkill9::SetActionState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;
	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return;
	SetMotionTime( 0, 0 );
	m_SkillState = SS_ACTION;

	if ( pChar->IsNeedProcess() )
		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, m_bCircleNoChangeY );

	SetNormalSkill( pChar );
	
	CreateMapEffect( pChar, pStage );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket << static_cast<BYTE>( m_SkillState );
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioCircleMoveAttackSkill9::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if ( !pGrp )
		return;

	if ( m_SkillState == SS_LOOP )
		return;

	SetMotionTime( 0, 0 );

	m_SkillState = SS_LOOP;

	int iAniID = pGrp->GetAnimationIdx( m_szLoopAni );
	if( iAniID == -1 )	
		return;

	float fAniRate = FLOAT1;
	if ( fAniRate > 0.0f )
		fAniRate = m_fLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100,1.0f, FLOAT1/ fAniRate );

	m_dwMotionStartTime = FRAMEGETTIME();
}

void ioCircleMoveAttackSkill9::SetEndState( ioBaseChar *pChar )
{
	SetMotionTime( 0, 0 );
	m_SkillState = SS_END;
	pChar->DestroyMagicCircle();

	if( m_SkillEndMotion.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_SkillEndMotion );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndMotionRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
}

bool ioCircleMoveAttackSkill9::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_CIRCLE_MOVE:
	case SS_ACTION:
	case SS_LOOP:
		return false;

	case SS_END:
		if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
			return false;
	}

	return true;
}

void ioCircleMoveAttackSkill9::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	pChar->DestroyMagicCircle();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );

	EndCameraBuff( pChar );

	ioPlayStage *pStage = pChar->GetCreator();
	if ( pStage )
	{
		if ( m_dwCircleMapEffectID != -1 )
		{
			pStage->DestroyMapEffect( m_dwCircleMapEffectID );
			m_dwCircleMapEffectID = -1;
		}
	}

	ClearData();
}

void ioCircleMoveAttackSkill9::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = dwCurTime;
	DWORD dwDuration = pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime = m_dwMotionStartTime + dwDuration;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;
}

void ioCircleMoveAttackSkill9::CreateWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_dwFireStartTime = 0;

		
		for ( int i=0; i<(int)m_CircleWeaponList.size() ; ++i )
		{
			pChar->SkillFireExplicit( m_CircleWeaponList[i],
				GetName(),
				m_dwWeaponBaseIndex++,
				m_vCirclePos );
		}
		
	}
}

bool ioCircleMoveAttackSkill9::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	// 스킬 무적 파괴
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		return true;
	case SS_CIRCLE_MOVE:
	case SS_ACTION:
	case SS_LOOP:
		if( m_dwSkillProtectTime > 0  && m_dwSkillProtectStartTime + m_dwSkillProtectTime > dwCurTime )
			return true;
		break;
	}

	return false;
}

void ioCircleMoveAttackSkill9::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioNormalSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}

void ioCircleMoveAttackSkill9::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar )
		return;
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRot( qtRot );

	BYTE btType;
	rkPacket >> btType;

	if ( btType == SS_ACTION )
	{
		rkPacket >> m_vCirclePos;
		SetMotionTime( 0, 0 );
		m_SkillState = SS_ACTION;

		SetNormalSkill( pChar );

		CreateMapEffect( pChar, pStage );
	}
}

void ioCircleMoveAttackSkill9::CreateMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;
	if ( m_dwCircleMapEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwCircleMapEffectID );
		m_dwCircleMapEffectID = -1;
	}
	// team circle
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedGuidCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueGuidCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueGuidCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedGuidCircle, m_vCirclePos );
	}

	if ( pMapEffect )
		m_dwCircleMapEffectID = pMapEffect->GetUniqueID();
}
