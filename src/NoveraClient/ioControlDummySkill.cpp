
#include "stdafx.h"

#include "ioControlDummySkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "DummyCharDefine.h"

#include "ioZoneEffectWeapon.h"

ioControlDummySkill::ioControlDummySkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_iDummyIndex = -1;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_vCreatePos = ioMath::VEC3_ZERO;

	m_PreActionAttribute.Init();
	m_bRemoveAllGauge = false;
	m_bUseActiveCnt = false;
}

ioControlDummySkill::ioControlDummySkill( const ioControlDummySkill &rhs )
: ioAttackSkill( rhs ),
 m_bDisableGrowth( rhs.m_bDisableGrowth ),
 m_iReduceSkillGuageKey( rhs.m_iReduceSkillGuageKey ),
 m_fGuagePerTic( rhs.m_fGuagePerTic ),
 m_dwTicTime( rhs.m_dwTicTime ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_DummyCharName( rhs.m_DummyCharName ),
 m_vDummyCharOffset( rhs.m_vDummyCharOffset ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_PreActionAttribute( rhs.m_PreActionAttribute ),
 m_bRemoveAllGauge( rhs.m_bRemoveAllGauge ),
 m_bSetUseCount( rhs.m_bSetUseCount )
{
	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwFireStartTime = 0;
	m_iDummyIndex = -1;

	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_vCreatePos = ioMath::VEC3_ZERO;
	m_bUseActiveCnt = false;
}

ioControlDummySkill::~ioControlDummySkill()
{
	m_vDestroyDummyCharList.clear();
}

void ioControlDummySkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_bDisableGrowth		= rkLoader.LoadBool_e( "disable_growth", false );
	m_iReduceSkillGuageKey	= rkLoader.LoadInt_e( "start_reduce_skill_guage_key", RK_NONE );
	m_fGuagePerTic			= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime				= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_vDummyCharOffset.x = rkLoader.LoadFloat_e( "dummy_char_width_offset", 0.0f );
	m_vDummyCharOffset.y = rkLoader.LoadFloat_e( "dummy_char_height_offset", 0.0f );
	m_vDummyCharOffset.z = rkLoader.LoadFloat_e( "dummy_char_offset", 0.0f );

	int iCnt = rkLoader.LoadInt_e( "destroy_dummy_cnt", 0 );
	m_vDestroyDummyCharList.clear();
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "destroy_dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vDestroyDummyCharList.push_back( szBuf );
	}

	LoadPreActionAttribute( rkLoader );
	m_bRemoveAllGauge  = rkLoader.LoadBool_e( "remove_all_gauge", false );
	m_bSetUseCount = rkLoader.LoadBool_e( "set_use_count", false );
}

void ioControlDummySkill::LoadPreActionAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "pre_action_motion", "", szBuf, MAX_PATH );
	m_PreActionAttribute.Init();
	m_PreActionAttribute.m_AttackAnimation = szBuf;
	m_PreActionAttribute.m_fAttackAniRate  = rkLoader.LoadFloat_e( "pre_action_motion_rate", FLOAT1 );
	m_PreActionAttribute.m_dwPreDelay	   = (DWORD)rkLoader.LoadInt_e( "pre_action_pre_delay", 0 );

	char szKey[MAX_PATH];
	m_PreActionAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "pre_action_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_PreActionAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "pre_action_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "pre_action_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "pre_action_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_PreActionAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "pre_action_attack_type_count", 0 );
	m_PreActionAttribute.m_vWeaponInfoList.clear();
	m_PreActionAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "pre_action_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "pre_action_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "pre_action_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "pre_action_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "pre_action_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_PreActionAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

}

ioSkill* ioControlDummySkill::Clone()
{
	return new ioControlDummySkill( *this );
}

bool ioControlDummySkill::OnSkillStart( ioBaseChar *pChar )
{
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_SkillState = SS_NONE;
	m_dwFireStartTime		= 0;
	m_vCreatePos = ioMath::VEC3_ZERO;

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	m_dwCheckTime			= FRAMEGETTIME();
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_bReduceSkillGuage = false;

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
	
	SetCameraBuff( pChar );

	return true;
}

void ioControlDummySkill::AttackSkillFire( ioBaseChar *pChar )
{
	//ioAttackSkill::AttackSkillFire( pChar );

	//////////////////////////////////////////////////////////////////////////
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iAniID,
			GetAttribute().m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;

		if( m_iReduceSkillGuageKey == RK_NONE )
			m_bReduceSkillGuage = true;
	}
	//////////////////////////////////////////////////////////////////////////
	m_dwEnableReserveTime = 0;

	SetSkillActionState( pChar );
}

void ioControlDummySkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

				if ( !m_PreActionAttribute.m_AttackAnimation.IsEmpty() )
					SetPreActionState( pChar );
				else
					AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_PREACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
				AttackSkillFire( pChar );
		}
		break;;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
			CheckDummyChar( pChar, pStage );
		}
		break;
	case SS_LOOP:
		{
			if( !IsDummyCharEnable( pChar ) )
			{
				SetSkillEndState( pChar );
				return;
			}
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				if( fCurTicGauge != 0.0f && m_bReduceSkillGuage )
				{
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						SetSkillEndState( pChar );
						return;
					}
				}
			}

			ProcessLoopRotate( pChar );
		}
		break;
	case SS_END:
		break;
	}
}

void ioControlDummySkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	EndCameraBuff( pChar );

	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}


	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		if( pDummy->GetType() == DCT_OWNER_CONTROL2 )
		{
			ioOwnerControlDummy2 *pOwnerControl2 = ToOwnerControlDummy2( pDummy );
			if( pOwnerControl2 )
				pOwnerControl2->SetLostControl();
			else
				pDummy->SetDieState( true );
		}
		else
			pDummy->SetDieState( true );
	}

	if ( m_bRemoveAllGauge )
		pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.0f );

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
	m_iDummyIndex = -1;
	m_bUseActiveCnt = false;
}

bool ioControlDummySkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioControlDummySkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	m_SkillState = SS_LOOP;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, true, false );

	rkPacket >> m_vCreatePos;
	rkPacket >> m_iDummyIndex;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	m_dwFireStartTime = 0;
	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
											  pChar->GetCharName(),
											  m_vCreatePos,
											  0.0f,
											  0, false );

	if( pDummy )
		pDummy->SetControlSkillName( GetName() );
}

void ioControlDummySkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}

	SetChargingRateForWeapon( pWeapon );
}

bool ioControlDummySkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_dwSkillProtectTime > 0 && m_dwSkillStartTime + m_dwSkillProtectTime > FRAMEGETTIME() )
		return true;

	return ioSkill::IsProtected( iDefenseBreakType );
}

bool ioControlDummySkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_NONE:
	case SS_PREACTION:
	case SS_ACTION:
	case SS_LOOP:
		return false;	
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioControlDummySkill::SetPreActionState( ioBaseChar *pChar )
{
	m_SkillState = SS_PREACTION;
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_PreActionAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	//pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate = FLOAT1;
	if( m_PreActionAttribute.m_fAttackAniRate > 0.0f )
		fTimeRate = m_PreActionAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_PreActionAttribute.m_dwPreDelay;

	
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_PreActionAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );
	

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	pChar->SetReservedSliding( m_PreActionAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;

		if( m_iReduceSkillGuageKey == RK_NONE )
			m_bReduceSkillGuage = true;
	}
	m_dwEnableReserveTime = 0;
}

void ioControlDummySkill::SetSkillActionState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();
	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime = 0;

	m_SkillState = SS_ACTION;
}

void ioControlDummySkill::SetSkillEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_END;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	//m_dwFireMotionEndTime = dwCurTime;
	//m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" );
}

void ioControlDummySkill::CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;
	if( !pStage ) return;

	if( m_DummyCharName.IsEmpty() ) return;
	if( m_dwFireStartTime == 0 ) return;

	if( m_dwFireStartTime >= FRAMEGETTIME() )
		return;

	m_SkillState = SS_LOOP;

	m_dwFireStartTime = 0;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );
	
	m_vCreatePos = pChar->GetWorldPosition() + ( pChar->GetTargetRot() * m_vDummyCharOffset );

	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	m_iDummyIndex = iCurIndex;

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharName, m_iDummyIndex,
												   pChar->GetCharName(),
												   m_vCreatePos,
												   0.0f,
												   0, true );

	if( pDummy )
		pDummy->SetControlSkillName( GetName() );

	m_bUseActiveCnt = m_bSetUseCount;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetTargetRot();
		kPacket << m_vCreatePos;
		kPacket << m_iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioControlDummySkill::ProcessLoopRotate( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return;

	pChar->SetTargetRotToTargetPos( pDummy->GetWorldPosition(), true, false );
}

bool ioControlDummySkill::IsDummyCharControl( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return false;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return false;

	return true;
}

bool ioControlDummySkill::IsDummyCharEnable( ioBaseChar *pChar )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );

	if( !pDummy )	return false;

	if( pDummy->GetState() == DCS_DIE )
		return false;

	return true;
}

D3DXVECTOR3 ioControlDummySkill::GetDummyCharControlPos( ioBaseChar *pChar )
{
	if( !pChar || m_iDummyIndex == -1 )
		return m_vCreatePos;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( pDummy )
	{
		return pDummy->GetCameraPosition();
	}

	return m_vCreatePos;
}

void ioControlDummySkill::StartReduceGuage()
{
	m_bReduceSkillGuage = true;
}

bool ioControlDummySkill::IsUseActiveCnt() const
{
	return m_bUseActiveCnt;
}

int ioControlDummySkill::GetMaxActiveCnt( ioBaseChar *pChar ) const
{
	if ( !pChar )
		return 0;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return 0;

	return pDummy->GetSkillMaxActiveCnt();
}

int ioControlDummySkill::GetCurActiveCnt( ioBaseChar *pChar ) const
{
	if ( !pChar )
		return 0;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iDummyIndex );
	if( !pDummy )
		return 0;

	return pDummy->GetSkillCurActiveCnt();
}