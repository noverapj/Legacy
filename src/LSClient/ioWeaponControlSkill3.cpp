
#include "stdafx.h"

#include "ioWeaponControlSkill3.h"


#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCreateWeaponBuff2.h"

#include "WeaponDefine.h"

ioWeaponControlSkill3::ioWeaponControlSkill3()
{
	m_ControlState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;
}

ioWeaponControlSkill3::ioWeaponControlSkill3( const ioWeaponControlSkill3 &rhs )
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
	m_AreaWeaponName( rhs.m_AreaWeaponName ),
	m_NoneTargetAttackWeaponInfoList( rhs.m_NoneTargetAttackWeaponInfoList ),
	m_stCheckBuffName( rhs.m_stCheckBuffName ),
	m_fNoneTargetCreateWeaponHeightOffSet( rhs.m_fNoneTargetCreateWeaponHeightOffSet ),
	m_bNotUseAttackKeyInput( rhs.m_bNotUseAttackKeyInput ),
	m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
	m_bConveyerDisable( rhs.m_bConveyerDisable )
{
	m_ControlState = RS_NONE;

	m_dwLoopStartTime = 0;
	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_dwAreaWeaponIndex = 0;
}

ioWeaponControlSkill3::~ioWeaponControlSkill3()
{
}

void ioWeaponControlSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

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

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "attach_area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	int iTicWeaponCnt = rkLoader.LoadInt_e( "tic_weapon_count", 0 );

	if( 0 < iTicWeaponCnt )
		m_NoneTargetAttackWeaponInfoList.reserve( iTicWeaponCnt );

	for( int i=0 ; i<iTicWeaponCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "tic_weapon_index_%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "tic_weapon_index_resistance_%d", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "tic_weapon_time_%d", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "tic_weapon_wound_ani_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "tic_weapon_wound_duration_%d", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "tic_weapon_wound_loop_ani_%d", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_NoneTargetAttackWeaponInfoList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "check_buff_name", "", szBuf, MAX_PATH );
	m_stCheckBuffName = szBuf;

	m_fNoneTargetCreateWeaponHeightOffSet = rkLoader.LoadFloat_e( "none_target_height_offset", 0.f );

	m_bNotUseAttackKeyInput = rkLoader.LoadBool_e( "not_use_attack_key_input", false );
	LoadAttackAttribute_e( "end_attack", m_EndAttackAttribute, rkLoader );

	m_bConveyerDisable = rkLoader.LoadBool_e( "conveyer_disable", false );
	
}

ioSkill* ioWeaponControlSkill3::Clone()
{
	return new ioWeaponControlSkill3( *this );
}

bool ioWeaponControlSkill3::IsProtected( int iDefenseBreakType ) const
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

bool ioWeaponControlSkill3::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
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

bool ioWeaponControlSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_ControlState != RS_END && m_ControlState != RS_END_ATTACK )
		return false;

	if( m_dwEndAniEndTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioWeaponControlSkill3::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();
	if ( m_bConveyerDisable)
		pChar->SetConveyerDisable( true );

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_ControlState = RS_NONE;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	m_dwAreaWeaponIndex = 0;

	m_dwMotionStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwFireStartTime = 0;
	m_dwEndAniEndTime = 0;

	m_bAttackActive = false;

	m_vCreatePos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_dwProtectTime = 0;
	m_bSetHandMesh = false;

	m_bReduceGauge = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
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


bool ioWeaponControlSkill3::CheckBuff( ioBaseChar *pChar, ioPlayStage *pStage )
{
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) continue;
		if( pTarget->GetCharName() == pChar->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( pTarget->GetBuff( m_stCheckBuffName, pChar->GetCharName() ) )
			return true;
	}

	return false;
}


void ioWeaponControlSkill3::CreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_AreaWeaponName.IsEmpty() ) 
		return;

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pChar->GetCharName(),
		"",
		GetName(),
		m_AreaWeaponName,
		m_vCreatePos,
		qtRot,
		ioAreaWeapon::CT_SKILL_DEPENDENCY );

	if( pArea )
		m_dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
}


void ioWeaponControlSkill3::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	m_dwAreaWeaponIndex = 0;
}


void ioWeaponControlSkill3::NoneTargetAttack( ioBaseChar *pChar, ioPlayStage *pStage, D3DXVECTOR3 vPos )
{
	m_bAttackActive = true;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( pWeapon )
	{
		D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
		vWeaponPos.y += m_fNoneTargetCreateWeaponHeightOffSet;
		DWORD dwCurTime = FRAMEGETTIME();
		for( int i = 0; i < (int)m_NoneTargetAttackWeaponInfoList.size(); i++ )
		{

			const WeaponInfo& info = m_NoneTargetAttackWeaponInfoList[i];

			if( pChar->IsNeedProcess() )
			{
				g_WeaponMgr.SetReserveCallWeaponInfo( pWeapon->GetOwner()->GetCharName(),
					info,
					vWeaponPos,
					D3DXVECTOR3( 0.f, -FLOAT1, 0.f ),
					dwCurTime + info.m_dwCallTime,
					m_dwWeaponBaseIndex + 1 + i );
			}
			else
			{
				g_WeaponMgr.SetReserveCallWeaponInfo( pWeapon->GetOwner()->GetCharName(),
					info,
					vPos,
					D3DXVECTOR3( 0.f, -FLOAT1, 0.f ),
					dwCurTime + info.m_dwCallTime,
					m_dwWeaponBaseIndex + 1 + i );
			}
		}

		//SendPacket
		if( pChar->IsNeedProcess() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SS_NONE_TARGET;
			kPacket << vWeaponPos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

}

void ioWeaponControlSkill3::BuffTargetAttack( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_bAttackActive = true;
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) 
			continue;

		if( pTarget->GetTeam() == pChar->GetTeam() ) continue;
		if( pTarget->GetCharName() == pChar->GetCharName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		ioBuff *pCheckBuff = pTarget->GetBuff( m_stCheckBuffName, pChar->GetCharName() );
		if( pCheckBuff )
		{
			ioCreateWeaponBuff2 *pCreateWeaponBuff2 = ToCreateWeaponBuff2( pCheckBuff );
			if( pCreateWeaponBuff2 )
				pCreateWeaponBuff2->StartFireCheckTime();
		}
	}

	//SendPacket
	if( pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_FIND_TARGET;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}


void ioWeaponControlSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelayTime = GetPreDelayTime();

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
		// SetLoopState가 발생했을때도 한다
		if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
		{
			m_dwFireStartTime = 0;

			m_vCreatePos = pChar->GetMidPositionByRate();
			if ( !m_AttackAttribute.m_vWeaponInfoList.empty() )
			{
				pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList.front(),
					GetName(),
					m_dwWeaponBaseIndex,
					m_vCreatePos );
			}
			CreateAreaWeapon( pChar, pStage );
		}

		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		{
			if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < dwCurTime )
			{
				m_dwLoopStartTime = 0;
				SetEndState( pChar );
				return;
			}
			else if( !CheckControlWeapon( pChar ) )
			{
				m_dwLoopStartTime = 0;
				SetEndState( pChar );
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			if( !m_bNotUseAttackKeyInput && pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				if( CheckBuff( pChar, pStage ) )
					BuffTargetAttack( pChar, pStage );
				else
					NoneTargetAttack( pChar, pStage, D3DXVECTOR3(0.f, 0.f, 0.f) );

				SetEndState( pChar );
				return;
			}

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
			ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
			if( pWeapon && pAreaWeapon )
			{
				D3DXVECTOR3 vPos1 = pWeapon->GetPosition();
				pAreaWeapon->SetAreaPosition( vPos1, pStage );
			}
			else
			{
				SetEndState( pChar );
				return;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		break;
	case RS_END:
		break;
	case RS_END_ATTACK:
		{
			if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
			{
				m_dwFireStartTime = 0;
				if( CheckBuff( pChar, pStage ) )
					BuffTargetAttack( pChar, pStage );
				else
					NoneTargetAttack( pChar, pStage, D3DXVECTOR3(0.f, 0.f, 0.f) );
			}
		}
		break;
	}
}

void ioWeaponControlSkill3::OnSkillEnd( ioBaseChar *pOwner )
{
	//스킬이 시간이 되어서든 맞아서든 종료가 된상황에는 버프를 삭제한다.
	ioPlayStage* pStage = pOwner->GetCreator();
	if( pStage && m_bAttackActive == false )
	{
		const BaseCharList &rkCharList = pStage->GetBaseCharList();
		BaseCharList::const_iterator iter;

		for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
		{
			ioBaseChar *pTarget = *iter;
			if( !pTarget ) 
				continue;

			if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
			if( pTarget->GetCharName() == pOwner->GetCharName() ) continue;
			if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

			ioBuff *pCheckBuff = pTarget->GetBuff( m_stCheckBuffName, pOwner->GetCharName() );
			if( pCheckBuff )
				pCheckBuff->SetReserveEndBuff();
		}
	}



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

	if ( m_bConveyerDisable)
		pOwner->SetConveyerDisable( false );

	ioNormalSkill::OnSkillEnd( pOwner );

	g_WeaponMgr.CheckSkillDependencyWeaponDead( pOwner->GetCharName(), GetName() );
}

void ioWeaponControlSkill3::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );
}

void ioWeaponControlSkill3::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );
}

void ioWeaponControlSkill3::SetActionState( ioBaseChar *pChar )
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
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioWeaponControlSkill3::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioWeaponControlSkill3::SetEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	if ( m_EndAttackAttribute.m_AttackAnimation.IsEmpty() )
	{
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
	}
	else
	{
		m_ControlState = RS_END_ATTACK;
		DWORD dwCurTime = FRAMEGETTIME();
		
		ioEntityGroup *pGrp = pChar->GetGroup();
		int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAttackAttribute.m_AttackAnimation );
		if( iCurSkillAniID == -1 )	
			return;

		DWORD dwPreDelay = m_EndAttackAttribute.m_dwPreDelay;
		float fTimeRate = FLOAT1;
		if ( m_EndAttackAttribute.m_fAttackAniRate > 0.0f )
			fTimeRate = m_EndAttackAttribute.m_fAttackAniRate;

		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate , 0.0f, dwPreDelay );
		
		m_dwEndAniEndTime = dwCurTime;
		m_dwEndAniEndTime += dwPreDelay + m_EndAttackAttribute.m_dwEndDelay;
		m_dwEndAniEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

		DWORD dwCreateTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate + dwPreDelay;
		m_dwFireStartTime = dwCurTime + dwCreateTime;

		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iCurSkillAniID,
			m_EndAttackAttribute.m_vWeaponInfoList,
			FTT_SKILL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

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

	//SendPacket
	if( bSendNet && pChar->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioWeaponControlSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	if( iSyncState == SS_NONE_TARGET )
	{
		D3DXVECTOR3 vWeaponPos;
		rkPacket >> vWeaponPos;
		NoneTargetAttack( pChar, pStage, vWeaponPos );
	}
	else if( iSyncState == SS_FIND_TARGET )
	{
		BuffTargetAttack( pChar, pStage );
	}
	else if( iSyncState == SS_END )
	{
		SetEndState( pChar );
	}
}

const D3DXVECTOR3& ioWeaponControlSkill3::GetWeaponPos( ioBaseChar *pChar )
{
	if( !pChar )
		return m_vCreatePos;

	if( m_dwWeaponBaseIndex < 0 )
		return pChar->GetWorldPosition();

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );

	if( ToBombMissileWeapon2( pWeapon ) || ToBombMissileWeapon4( pWeapon ) || ToBombMissileWeapon5( pWeapon ) )
	{
		return pWeapon->GetPosition();
	}

	return m_vCreatePos;
}

bool ioWeaponControlSkill3::CheckControlWeapon( ioBaseChar *pChar )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex );
	if( ToBombMissileWeapon2( pWeapon ) || ToBombMissileWeapon4( pWeapon ) || ToBombMissileWeapon5( pWeapon ) )
		return true;

	return false;
}

bool ioWeaponControlSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioWeaponControlSkill3::IsWeaponControl( ioBaseChar *pChar )
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

bool ioWeaponControlSkill3::UseActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return false;

	if( !pOwner )
		return false;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 && pBombM4->GetMaxExplosionCnt() > 0 )
		return true;

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	//if( pBombM5 && pBombM5->GetMaxExplosionCnt() > 0 )
	//return true;
	if( pBombM5 )
		return false;

	return false;
}

int ioWeaponControlSkill3::GetMaxActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return 0;

	if( !pOwner )
		return 0;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 )
		return pBombM4->GetMaxExplosionCnt();

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	if( pBombM5 )
		return pBombM5->GetMaxExplosionCnt();

	return 0;
}

int ioWeaponControlSkill3::GetCurActiveCount( ioBaseChar *pOwner )
{
	if( m_ControlState != RS_LOOP )
		return 0;

	if( !pOwner )
		return 0;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
	ioBombMissileWeapon4 *pBombM4 = ToBombMissileWeapon4( pWeapon );
	if( pBombM4 )
		return pBombM4->GetCurExplosionCnt();

	ioBombMissileWeapon5 *pBombM5 = ToBombMissileWeapon5( pWeapon );
	if( pBombM5 )
		return pBombM5->GetCurExplosionCnt();

	return 0;
}


void ioWeaponControlSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pOwner = pZone->GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}
}