
#include "stdafx.h"

#include "ioWeaponControlSkill2.h"

#include "ioGrowthUpInfo.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioWeaponControlSkill2::ioWeaponControlSkill2()
{
	ClearData();
}

ioWeaponControlSkill2::ioWeaponControlSkill2( const ioWeaponControlSkill2 &rhs )
	: ioNormalSkill( rhs ),
	m_AttackAttribute( rhs.m_AttackAttribute ),
	m_LoopAnimation( rhs.m_LoopAnimation ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_StartHandMesh( rhs.m_StartHandMesh ),
	m_LoopHandMesh( rhs.m_LoopHandMesh ),
	m_EndHandMesh( rhs.m_EndHandMesh ),
	m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
	m_EndAnimation( rhs.m_EndAnimation ),
	m_fEndAniRate( rhs.m_fEndAniRate ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime )
{
	ClearData();
}

ioWeaponControlSkill2::~ioWeaponControlSkill2()
{
}

void ioWeaponControlSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	// Animation
	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	LoadAttackAttribute_e( "control_attack", m_AttackAttribute, rkLoader );

	// HandMesh
	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "loop_hand_mesh", "", szBuf, MAX_PATH );
	m_LoopHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );
}

ioSkill* ioWeaponControlSkill2::Clone()
{
	return new ioWeaponControlSkill2( *this );
}

void ioWeaponControlSkill2::ClearData()
{
	m_ControlState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_vCreatePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwProtectTime = 0;
	m_bSetHandMesh = false;
	m_bReduceGauge = false;
}

bool ioWeaponControlSkill2::IsProtected( int iDefenseBreakType ) const
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

	if( m_ControlState == RS_ACTION )
		return true;

	if( m_ControlState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}

	return false;
}

bool ioWeaponControlSkill2::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioWeaponControlSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_ControlState != RS_END )
		return false;

	if( m_dwEndAniEndTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioWeaponControlSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();
	ClearData();

	m_dwCheckTime = FRAMEGETTIME();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();
	
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

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

	DWORD dwPreDelayTime = GetPreDelayTime();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioWeaponControlSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_ControlState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}

		// SetLoopState가 발생했을때도 한다
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;

			m_vCreatePos = pChar->GetMidPositionByRate();
			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
				GetName(),
				m_dwWeaponBaseIndex,
				m_vCreatePos );
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( !CheckControlWeapon( pChar ) )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		break;
	case RS_END:
		break;
	}

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				SetEndState( pChar );
				return;
			}
		}
	}
}

void ioWeaponControlSkill2::OnSkillEnd( ioBaseChar *pOwner )
{
	g_WeaponMgr.CheckSkillDependencyWeaponDead( pOwner->GetCharName(), GetName() );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );

	// Ani
	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniEndTime = 0;

	ioNormalSkill::OnSkillEnd( pOwner );	
}

void ioWeaponControlSkill2::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioWeaponControlSkill2::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioWeaponControlSkill2::SetActionState( ioBaseChar *pChar )
{
	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );


	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate;

	DWORD dwCreateTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = m_dwMotionStartTime + dwCreateTime;

	m_ControlState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
		m_bReduceGauge = true;
	}
}

void ioWeaponControlSkill2::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_LoopAnimation.IsEmpty() )
	{
		m_dwLoopStartTime = 0;
		return;
	}

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_ControlState = RS_LOOP;
}

void ioWeaponControlSkill2::SetEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	m_ControlState = RS_END;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwEndAniEndTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	if( iCurSkillAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );

	m_dwEndAniEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * m_fEndAniRate;

	if( m_bSetHandMesh )
	{
		if( !m_LoopHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pChar->GetCharName(), GetName() );
}

void ioWeaponControlSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

const D3DXVECTOR3& ioWeaponControlSkill2::GetWeaponPos( ioBaseChar *pChar )
{
	if( !pChar )
		return m_vCreatePos;

	if( m_dwWeaponBaseIndex < 0 )
		return pChar->GetWorldPosition();

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	ioControlSwordWeapon *pSword = ToControlSwordWeapon( pWeapon );
	ioControlMeshWeapon *pMesh = ToControlMeshWeapon( pWeapon );

	if( pSword )
	{
		return pSword->GetPosition();
	}

	if ( pMesh )
	{
		return pMesh->GetPosition();
	}

	return m_vCreatePos;
}

bool ioWeaponControlSkill2::CheckControlWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToControlSwordWeapon( pWeapon ) || ToControlMeshWeapon( pWeapon ) )
	{		
		return true;
	}	
	return false;
}

bool ioWeaponControlSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioWeaponControlSkill2::IsWeaponControl( ioBaseChar *pChar )
{
	switch( m_ControlState )
	{
	case RS_LOOP:
	case RS_END:
		if( CheckControlWeapon( pChar ) )
			return true;
		break;
	}

	return false;
}


