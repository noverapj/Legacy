

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioScabSkill.h"

#include "WeaponDefine.h"
#include "BuffDefine.h"


ioScabSkill::ioScabSkill()
{
	ClearData();
}

ioScabSkill::ioScabSkill( const ioScabSkill &rhs )
	//: ioNormalSkill( rhs ),
	: ioCountSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_TargetAimAttackBuffList( rhs.m_TargetAimAttackBuffList ),
	m_bSkillZeroAimAttackGauge( rhs.m_bSkillZeroAimAttackGauge ),
	m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
	m_SkillMapEffect( rhs.m_SkillMapEffect ),
	m_fCharEffectOffset( rhs.m_fCharEffectOffset ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_StartAttribute( rhs.m_StartAttribute ),
	m_LoopFireAttribute( rhs.m_LoopFireAttribute ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_stTargetCheckBuffName( rhs.m_stTargetCheckBuffName ),
	m_fLoopFireLookOffSet( rhs.m_fLoopFireLookOffSet ),
	m_fLoopFireHeightOffSet( rhs.m_fLoopFireHeightOffSet ),
	m_dwMaxLoopDelayTime( rhs.m_dwMaxLoopDelayTime ),
	m_stFailAttackEndMotion( rhs.m_stFailAttackEndMotion ),
	m_fFailAttackEndMotionRate( rhs.m_fFailAttackEndMotionRate )

{
	ClearData();
}

ioScabSkill::~ioScabSkill()
{
}

void ioScabSkill::ClearData()
{
	m_bSetHandMesh = false;

	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;


	m_dwCurSkillZeroAimAttackCnt = 0;

	m_dwStartSkillProtectedTime = 0;

	m_dwStartFireEndMotionTime = 0;
	m_dwLoopFireEndMotionTime = 0;

	m_dwEndLoopTime = 0;

	m_vAddTargetList.clear();
	m_stZoneWeaponTargetName.Clear();
}

void ioScabSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioCountSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	rkLoader.LoadString_e( "skill_zero_aim_attack_motion", "", szBuf, MAX_PATH );
	m_SkillZeroAimMotion = szBuf;
	m_fSkillZeroAimMotionLate = rkLoader.LoadFloat_e( "skill_zero_aim_attack_motion_rate", FLOAT1 );

	m_dwSkillZeroAimAttackCnt = rkLoader.LoadInt_e( "skill_zero_aim_attack_cnt", 1 );
	m_bSkillZeroAimAttackGauge = rkLoader.LoadBool_e( "skill_zero_aim_attack_reduce_gauge", 1 );

	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_protect_time", 0 );

	//////////////////////////////////////////////////////////////////////////
	wsprintf_e( szBuf, "start_attack");
	LoadAttackAttribute( szBuf, m_StartAttribute, rkLoader );

	wsprintf_e( szBuf, "loop_attack");
	LoadAttackAttribute( szBuf, m_LoopFireAttribute, rkLoader );
	//////////////////////////////////////////////////////////////////////////

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	m_fCharEffectOffset = rkLoader.LoadFloat_e( "skill_map_effect_offset", 0.0f );

	rkLoader.LoadString_e( "action_fire_target_checkbuff", "", szBuf, MAX_PATH );
	m_stTargetCheckBuffName = szBuf;

	// target search range
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );

	//////////////////////////////////////////////////////////////////////////
	m_fLoopFireLookOffSet = rkLoader.LoadFloat_e( "loop_attack_target_look_offset", 0.f );
	m_fLoopFireHeightOffSet = rkLoader.LoadFloat_e( "loop_attack_target_height_offset", 0.f );
	//////////////////////////////////////////////////////////////////////////
	m_iMaxUseCount = rkLoader.LoadInt_e( "max_use_count", 100 );
	m_iNeedCount = rkLoader.LoadInt_e( "need_count", 1 );

	m_fDefaultRecoveryCount = rkLoader.LoadFloat_e( "default_recovery_count", 0.1f );
	m_fDelayRunRecoveryCountRate = rkLoader.LoadFloat_e( "delayrun_recovery_count_rate", FLOAT1 );
	m_fEtcRecoveryCountRate = rkLoader.LoadFloat_e( "etc_recovery_count_rate", FLOAT1 );
	m_iDefaultRecoveryCountTic = rkLoader.LoadFloat_e( "default_recovery_count_tic", 100 );
	//////////////////////////////////////////////////////////////////////////

	m_dwMaxLoopDelayTime = rkLoader.LoadFloat_e( "loop_delay_max_time", 0 );

	rkLoader.LoadString_e( "fail_attack_motion", "", szBuf, MAX_PATH );
	m_stFailAttackEndMotion	= szBuf;
	m_fFailAttackEndMotionRate = rkLoader.LoadFloat_e( "fail_attack_motion_time_rate", FLOAT1 );

	LoadBuffList( rkLoader );
}

void ioScabSkill::LoadBuffList( ioINILoader &rkLoader )
{
	ioBuffInfo BuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuff;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioScabSkill::Clone()
{
	return new ioScabSkill( *this );
}

bool ioScabSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioScabSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ioCountSkill::OnSkillStart( pChar );

	AddOwnerBuff( pChar );

	DWORD dwPreDelay = GetPreDelayTime();

	DWORD dwTrackingTime = 0;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
	{
		ioHashString szPreDelay = GetPreDelayAniName();

		int iAniID = pGrp->GetAnimationIdx( szPreDelay );
		if( iAniID != -1 )
			dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
	}

	if( dwTrackingTime > 0 )
		dwTrackingTime += FRAMEGETTIME();
	else
		dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	ClearData();

	m_SkillState = SS_PRE;
	m_dwCheckTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	//m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillMoveEnable( false, false );

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
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

	/*if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}*/

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		//SetLoopState( pChar, pStage );
		SetStartFireState( pChar, pStage );
	}	

	return true;
}

void ioScabSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioCountSkill::OnSkillEnd( pChar );

	EraseOwnerBuff( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );		
	}

	ClearData();
}

void ioScabSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetStartFireState( pChar, pStage );
			}
		}
		break;
	case SS_START_FIRE:
		{
			if( pChar->IsNeedProcess() )
			{
				if( dwCurTime > m_dwStartFireEndMotionTime )
				{
					m_dwEndLoopTime = dwCurTime + m_dwMaxLoopDelayTime;
					if( m_vAddTargetList.size() )
						SetLoopState( pChar, pStage );
					else
						SetEndState( pChar , pStage );
				}
			}
		}
		break;
	case SS_LOOP:
		{	
			CheckNormalMoveAni( pChar );

			//동기화 처리
			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
				SetLoopFireState( pChar, pStage );
		}
		break;
	case SS_LOOP_FIRE:
		{
			if( pChar->IsNeedProcess() )
			{
				if( dwCurTime > m_dwLoopFireEndMotionTime )
				{
					if( m_vAddTargetList.size() )
						SetLoopState( pChar,pStage );
					else
						SetEndState( pChar, pStage );
				}
			}
		}
		break;
	case SS_END:
		{

		}
		break;
	}

	if( pChar->IsNeedProcess() && m_SkillState == SS_LOOP && m_dwEndLoopTime < dwCurTime )
	{
		SetEndState( pChar, pStage );
	}
}


void ioScabSkill::SetStartFireState( ioBaseChar *pChar,ioPlayStage *pStage )
{
	if( !pChar )
		return;

	m_SkillState = SS_START_FIRE;
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_StartAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_StartAttribute.m_fAttackAniRate * m_fExtraAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_dwStartFireEndMotionTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	//m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_StartAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	pChar->SetReservedSliding( m_StartAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	pChar->SetSkillMoveEnable( false, false );
}


void ioScabSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_LOOP;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SS_LOOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioScabSkill::SetLoopFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SS_LOOP_FIRE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	int iAniID = 0;

	if( m_vAddTargetList.empty() )
	{
		iAniID = pGrp->GetAnimationIdx( m_stFailAttackEndMotion );
		if( iAniID == -1 )
		{
			SetEndState( pChar, pStage );
			return;
		}

		float fTimeRate = m_fFailAttackEndMotionRate;
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

		m_dwEndLoopTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID );
		m_dwLoopFireEndMotionTime = m_dwEndLoopTime;

		m_SkillState = SS_LOOP_FIRE;
		return;
	}

	m_SkillState = SS_LOOP_FIRE;	

	iAniID = pGrp->GetAnimationIdx( m_LoopFireAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_LoopFireAttribute.m_fAttackAniRate * m_fExtraAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_dwLoopFireEndMotionTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwWeaponBaseIndex++;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_LoopFireAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );

	pChar->SetReservedSliding( m_LoopFireAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->SetSkillMoveEnable( false, false );
	//////////////////////////////////////////////////////////////////////////
	for( int i = 0; i < (int)m_vAddTargetList.size(); i++ )
	{
		ioHashString stTargetName = m_vAddTargetList[i];
		ioBaseChar *pTarget = pStage->GetBaseChar( stTargetName );
		if( !pTarget )
			continue;

		ioBuff *pTargetBuff = pTarget->GetBuff( m_stTargetCheckBuffName, pChar->GetCharName() );
		ioPositionBuff2 *pPosition2Buff = ToPositionBuff2( pTargetBuff );
		if( pPosition2Buff )
		{
			m_stZoneWeaponTargetName = m_vAddTargetList[i];

			m_vAddTargetList.erase( m_vAddTargetList.begin() + i );
			pPosition2Buff->SetSafeCreate( false );
			pPosition2Buff->SetReserveEndBuff();

			D3DXVECTOR3 vWorldPos = pChar->GetWorldPosition();
			D3DXVECTOR3 vLook = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vTargetPos = vWorldPos + vLook*m_fLoopFireLookOffSet;
			FomationTarget( pTarget, pStage->GetMapHeight( vWorldPos.x, vWorldPos.z ), pStage->GetMapHeight( vTargetPos.x, vTargetPos.z ), vTargetPos );

			return;
		}
	}

	//여기 오면 멀쩡한 타겟이 없다는 거다.
	//m_SkillState = SS_END;
	SetEndState( pChar, pStage );
}


void ioScabSkill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}


void ioScabSkill::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << (int)SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioScabSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( !pStage )
		return;

	ioZoneEffectWeapon *pZoneWeapon = ToZoneEffectWeapon( pWeapon );
	if( pWeapon )
	{
		ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
		if( pWeaponOwner )
		{
			ioBaseChar *pTarget = pWeaponOwner->GetBaseChar( m_stZoneWeaponTargetName );
			if( pTarget )
			{
				pZoneWeapon->SetIgnoreCollsionChar( m_stZoneWeaponTargetName );
				pZoneWeapon->SetZoneValue( pTarget->GetWorldPosition(), true );
				pZoneWeapon->SetAttachTargetName( m_stZoneWeaponTargetName );
			}
			
		}
	}
}

void ioScabSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SS_LOOP:
		SetLoopState( pChar, pStage );
		break;
	case SS_LOOP_FIRE:
		SetLoopFireState( pChar, pStage );
		break;
	case SS_END:
		SetEndState( pChar, pStage );
		break;
	case SS_WEAPON_WOUNED:
		{
			ioHashString stWoundedName;
			rkPacket >> stWoundedName;

			ioBaseChar *pWoundTarget = pChar->GetBaseChar( stWoundedName );
			if( pWoundTarget )
				SetNextSkillState( pWoundTarget );
		}
		break;
	}
}


bool ioScabSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioScabSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_LOOP:
		{
			if( 0 < m_dwStartSkillProtectedTime && 
				dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
				return true;
		}
		break;			
	}

	return false;
}

bool ioScabSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_START_FIRE:
	case SS_LOOP:
	case SS_LOOP_FIRE:
		return false;
	case SS_END:
		return true;
	}

	return true;
}


void ioScabSkill::SetNextSkillState( ioBaseChar *pWounder , ioWeapon *pWeapon /* = NULL */ )
{
	if( pWounder->IsNeedProcess() )
	{
		if( !pWounder->HasBuff( BT_POSITION2 ) )
			return;
		
		ioItem *pItem = GetOwnerItem();
		if( pItem && !(pItem->GetOwnerName().IsEmpty() ) )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pItem->GetOwnerName();
			kPacket << GetName();
			kPacket << (int)SS_WEAPON_WOUNED;
			kPacket << pWounder->GetCharName();

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
	for( size_t i = 0; i < m_vAddTargetList.size(); i++ )
	{
		if( m_vAddTargetList[i] == pWounder->GetCharName() )
			return;
	}

	m_vAddTargetList.push_back( pWounder->GetCharName() );
}


void ioScabSkill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}


void ioScabSkill::EraseOwnerBuff( ioBaseChar *pChar )
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage && pChar )
	{
		int iCnt = m_OwnerBuffList.size();
		for( int i=0; i<iCnt; ++i )
		{
			ioBuff *pBuff = pChar->GetBuff( m_OwnerBuffList[i].m_BuffName );
			if( pBuff )
				pBuff->SetReserveEndBuff();
				//pBuff->EndBuff();
		}
	}
}


void ioScabSkill::FomationTarget( ioBaseChar *pChar, float fSourceHeight, float fTargetHeight, D3DXVECTOR3 vTargetPos )
{
	if( fSourceHeight >= fTargetHeight )
		vTargetPos.y += m_fLoopFireHeightOffSet;
	else	
		vTargetPos.y = fTargetHeight + m_fLoopFireHeightOffSet;

	pChar->SetWorldPosition( vTargetPos );
}