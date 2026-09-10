

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioRotateChargeItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioRotateChargeItem::ioRotateChargeItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iCurMaxCount = 0;
	m_iCurCount = 0;

	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_dwCurMapEffect = -1;
}

ioRotateChargeItem::ioRotateChargeItem( const ioRotateChargeItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAni( rhs.m_AttackReadyAni ),
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
m_vExtendEndAttributeList( rhs.m_vExtendEndAttributeList ),
m_GuidAttribute( rhs.m_GuidAttribute ),
m_GatherAnimation( rhs.m_GatherAnimation ),
m_GatheringEffect( rhs.m_GatheringEffect ),
m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
m_fGatherAniRate( rhs.m_fGatherAniRate ),
m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
m_szRetreatFr( rhs.m_szRetreatFr ),
m_szRetreatBack( rhs.m_szRetreatBack ),
m_szRetreatLt( rhs.m_szRetreatLt ),
m_szRetreatRt( rhs.m_szRetreatRt ),
m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
m_fRetreatBackRate( rhs.m_fRetreatBackRate ),
m_fRetreatLtRate( rhs.m_fRetreatLtRate ),
m_fRetreatRtRate( rhs.m_fRetreatRtRate ),
m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
m_fRange( rhs.m_fRange ),
m_fMoveSpeed( rhs.m_fMoveSpeed ),
m_fHeightGap( rhs.m_fHeightGap ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_EnableCircle( rhs.m_EnableCircle ),
m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_fRevisionRate( rhs.m_fRevisionRate ),
m_dwGuidPeriod( rhs.m_dwGuidPeriod ),
m_bUseGuidAttack( rhs.m_bUseGuidAttack ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fUpLimitRange( rhs.m_fUpLimitRange )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringMaxEffectID = -1;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iCurMaxCount = 0;
	m_iCurCount = 0;

	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;

	m_dwCurMapEffect = -1;
}

ioRotateChargeItem::~ioRotateChargeItem()
{
}

void ioRotateChargeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	rkLoader.LoadString_e( "charge_branch", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	m_vExtendAttributeList.clear();
	m_vExtendEndAttributeList.clear();

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.reserve( iExtendCnt );
	m_vExtendEndAttributeList.reserve( iExtendCnt );

	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );

		AttackAttribute kEndAttribute;
		wsprintf_e( szKey, "extend_end_attack0%d", i+1 );
		LoadAttackAttribute( szKey, kEndAttribute, rkLoader );
		m_vExtendEndAttributeList.push_back( kEndAttribute );
	}

	// Guid
	wsprintf_e( szKey, "guid_attack" );
	LoadAttackAttribute( szKey, m_GuidAttribute, rkLoader );

	m_bUseGuidAttack = rkLoader.LoadBool_e( "guid_attack_use", false );
	m_dwGuidPeriod = rkLoader.LoadInt_e( "guid_attack_period", 0 );


	// Gather
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;


	// Charge max

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_back", "", szBuf, MAX_PATH );
	m_szRetreatBack = szBuf;
	m_fRetreatBackRate = rkLoader.LoadFloat_e( "retreat_animation_back_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_right", "", szBuf, MAX_PATH );
	m_szRetreatRt = szBuf;
	m_fRetreatRtRate = rkLoader.LoadFloat_e( "retreat_animation_right_rate", FLOAT1 );
	rkLoader.LoadString_e( "retreat_animation_left", "", szBuf, MAX_PATH );
	m_szRetreatLt = szBuf;
	m_fRetreatLtRate = rkLoader.LoadFloat_e( "retreat_animation_left_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );


	// MagicCircle
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );

	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "gathering_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "gathering_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "gathering_min_speed", 0.0f );

	m_fRevisionRate = rkLoader.LoadFloat_e( "gathering_revision_rate", 0.0f );

	//
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
}

ioItem* ioRotateChargeItem::Clone()
{
	return new ioRotateChargeItem( *this );
}

ioWeaponItem::WeaponSubType ioRotateChargeItem::GetSubType() const
{
	return WST_ROTATE_CHARGE;
}

void ioRotateChargeItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_fCurChargeRate = 0.0f;

	m_iCurMaxCount = 0;
	m_iCurCount = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

void ioRotateChargeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearState();

	m_dwCurMapEffect = -1;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
	float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_CHARGING;
	m_dwAttackStartTime = FRAMEGETTIME();
	m_iCurCombo = iCurCombo;
	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );

	if( m_bChargeAutoTarget )
	{
		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
	else
	{
		pOwner->StopAutoTargetTracking();
	}
}

void ioRotateChargeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		OnChargeFire( pOwner );
		break;
	case CS_FIRE_END:
		OnFireEnd( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}
}

void ioRotateChargeItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioRotateChargeItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// MagicCircle Update
	if( pOwner->CheckMagicCircle() )
	{
		// 뱡향 변화 체크
		pOwner->CheckChangeDirectionForMagicCircle();

		// MagicCircle 위치 갱신
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, m_fRange, m_fHeightGap, true );
		else
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, m_fRange, m_fHeightGap, false );

		// MagicCircle 위치방향으로 캐릭터 회전
		D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
	}

	// 공격버튼땜, 발사.
	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToAttackFire( pOwner );
		return;
	}

	// 점프키 누름. 점프상태 전환
	if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// SD 스킬 입력 간주.
	if( pOwner->IsDefenseKeyDown() )
	{
		if( pOwner->CheckMagicCircle() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
		{
			pOwner->CheckMagicCircleVisible( false );
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, true, true );
		}

		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
			if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
			{
				if( m_dwMaxChargeTime > dwGapTime )
					m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
				else
					m_fCurChargeRate = FLOAT1;
			}

			m_fCurChargeRateForSkill = m_fCurChargeRate;

			pOwner->SetChargingState( false );

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			if( m_GatheringMaxEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringMaxEffectID, false );
				m_GatheringMaxEffectID = -1;
			}

			ClearState();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}

	// Guid
	CheckGuidAttack( pOwner );

	if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
		m_GatheringMaxEffectID == -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;

		ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
		if( pEffect )
		{
			m_GatheringMaxEffectID = pEffect->GetUniqueID();
		}

		if( !m_GatheringMaxSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
			}
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << m_ChargeState;
			kPacket << m_iCurCombo;
			kPacket << true;		// Max Charge
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRotateChargeItem::OnChargeFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_dwFireMotionEndTime + dwLooseTime < dwCurTime )
	{
		m_iCurCount++;

		if( m_iCurCount >= m_iCurMaxCount )
		{
			//SetFireEndMotion( pOwner );
			ProcessReserve( pOwner );
			return;
		}
		else
		{
			SetFireMotion( pOwner );
			return;
		}
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}
	}
}

void ioRotateChargeItem::OnFireEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireEndMotionEndTime > 0 && m_dwFireEndMotionEndTime < dwCurTime )
	{
		ProcessReserve( pOwner );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}
	}
}

void ioRotateChargeItem::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		ProcessReserve( pOwner );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

void ioRotateChargeItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetActionAni( m_GatherAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate, 0.0f, 0.0f, true );

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
	{
		m_GatheringEffectID = pEffect->GetUniqueID();
	}

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	m_ChargeState = CS_GATHERING;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwGatheringStartTime = dwCurTime;
	m_dwCheckGuidTime = dwCurTime + m_dwGuidPeriod;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRotateChargeItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	pOwner->SetChargingState( false );

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_ChargeState = CS_ATTACK_FIRE;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if ( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRotateChargeItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetChargingState( false );

	m_iCurCount = 0;
	m_dwKeyReserveTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	m_ChargeState = CS_ATTACK_FIRE;
	if( pOwner->IsNeedProcess() )
	{
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;

		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}

		int iExtendCnt = m_vExtendAttributeList.size();
		m_iCurMaxCount = (int)(iExtendCnt * m_fCurChargeRate);
		m_iCurMaxCount = max( m_iCurMaxCount, 1 );
	}

	SetFireMotion( pOwner );

	// MagicCircle
	if( pOwner->CheckMagicCircle() )
	{
		pOwner->CheckMagicCircleVisible( false );
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false, true );
	}

	m_vCirclePos = pOwner->GetMagicCirclePos( m_fMagicCircleOffSet, true );

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}
	else if( pOwner->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	}
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		}
		else
		{
			pMapEffect = pOwner->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
		}
	}

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pOwner->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	CheckWeaponFloatInfo( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurMaxCount;
		kPacket << m_iCurCount;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// 마지막에 제거해야한다
	pOwner->DestroyMagicCircle();
}

void ioRotateChargeItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->DestroyMagicCircle();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	pOwner->SetChargingState( false );

	m_iCurCount = 0;
	m_dwKeyReserveTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwFireEndMotionEndTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = CS_RETREAT;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRotateChargeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetChargingState( false );

		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		if( m_GatheringMaxEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringMaxEffectID, false );
			m_GatheringMaxEffectID = -1;
		}

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );

		ChangeToNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		{
			bool bMaxCharge;
			rkPacket >> bMaxCharge;

			if( bMaxCharge )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;

				if( m_GatheringMaxEffectID == -1 )
				{
					ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
					if( pEffect )
					{
						m_GatheringMaxEffectID = pEffect->GetUniqueID();
					}
				}

				if( !m_GatheringMaxSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
					}
				}
			}
			else
				ChangeToGathering( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
		{
			rkPacket >> m_iCurMaxCount;
			rkPacket >> m_iCurCount;
			rkPacket >> m_fCurChargeRate;

			ChangeToAttackFire( pOwner );
		}
		break;
	case CS_RETREAT:
		{
			ioHashString szAni;
			float fAniRate;
			D3DXVECTOR3 vDir;

			rkPacket >> szAni >> fAniRate >> vDir;

			ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
		}
		break;
	}
}

void ioRotateChargeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurBlowRate;
	float fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurBlowRate = FLOAT1;
	fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}

	ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
	if( !bJumpAttack && pFloatWeapon )
	{
		pFloatWeapon->SetMoveDir( m_vWeaponDir );
		pFloatWeapon->SetMoveSpeed( m_fCurWeaponMoveSpeed );
		pFloatWeapon->SetFloatPower( m_fCurWeaponFloatPower );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioRotateChargeItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

bool ioRotateChargeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioRotateChargeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}
}

void ioRotateChargeItem::GetAutoTargetValue( ioBaseChar *pOwner,
											 float &fRange, float &fMinAngle, float &fMaxAngle,
											 AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

float ioRotateChargeItem::GetChargeDamageRateForSkill()
{
	float fCurDamageRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		fCurDamageRate += fCurMaxDamage * m_fCurChargeRateForSkill;
	}

	return fCurDamageRate;
}

float ioRotateChargeItem::GetChargeForceRateForSkill()
{
	float fCurForceRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		fCurForceRate += fCurMaxForce * m_fCurChargeRateForSkill;
	}

	return fCurForceRate;
}

float ioRotateChargeItem::GetChargeBlowRateForSkill()
{
	float fCurBlowRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRateForSkill;
	}

	return fCurBlowRate;
}

float ioRotateChargeItem::GetChargeWoundedTimeRateForSkill()
{
	float fCurWoundedTimeRate = FLOAT1;
	if( m_fCurChargeRateForSkill > 0.0f )
	{
		float fCurMaxFloat = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		fCurWoundedTimeRate += fCurMaxFloat * m_fCurChargeRateForSkill;
	}

	return fCurWoundedTimeRate;
}

void ioRotateChargeItem::ClearCurChargeRateForSkill()
{
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
}

void ioRotateChargeItem::SetFireMotion( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurChargeAniRate = FLOAT1;
	float fCurForceSlidingRate = FLOAT1;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	SetAttackByAttribute( pOwner, m_vExtendAttributeList[m_iCurCount], false, fCurChargeAniRate, fCurForceSlidingRate );
}

void ioRotateChargeItem::SetFireEndMotion( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_iCurCount--;
	if( !COMPARE( m_iCurCount, 0, (int)m_vExtendEndAttributeList.size() ) )
	{
		m_dwFireEndMotionEndTime = dwCurTime;
		m_ChargeState = CS_FIRE_END;
		return;
	}

	m_ChargeState = CS_FIRE_END;
	SetAttackByAttribute( pOwner, m_vExtendEndAttributeList[m_iCurCount], true );

	pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
}

void ioRotateChargeItem::SetAttackByAttribute( ioBaseChar *pOwner, const AttackAttribute &rkAttr, bool bEnd, float fExtraAniRate, float fExtraForceRate )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAutoTarget( ATT_NORMAL );

	int iAniID	     = pGrp->GetAnimationIdx( rkAttr.m_AttackAnimation );
	float fTimeRate  = rkAttr.m_fAttackAniRate * fExtraAniRate;
	DWORD dwPreDelay = rkAttr.m_dwPreDelay;
	DWORD dwEndDelay = rkAttr.m_dwEndDelay;

	pGrp->ClearAllLoopAni( 100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->RefreshFireTimeList( iAniID, rkAttr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	if( !bEnd )
		m_dwFireMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;
	else
		m_dwFireEndMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay + dwEndDelay;


	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve + dwPreDelay;

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( rkAttr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay, fExtraForceRate );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate + dwPreDelay;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
}

bool ioRotateChargeItem::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
		if( !m_szRetreatFr.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioRotateChargeItem::CheckRetreat - No Front Ani" );
		}
		break;
	case 7:
		if( !m_szRetreatFr.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioRotateChargeItem::CheckRetreat - No Front Ani" );
		}
		break;
	case 1:
		if( iRest > 10 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	case 2:
		if( iRest <= 35 )
		{
			if( !m_szRetreatRt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatRt, m_fRetreatRtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 3:
	case 4:
		if( !m_szRetreatBack.IsEmpty() )
		{
			ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
			return true;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioRotateChargeItem::CheckRetreat - No Back Ani" );
		}
		break;
	case 5:
		if( iRest > 10 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatBack.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatBack, m_fRetreatBackRate, vMoveDir );
				return true;
			}
		}
		break;
	case 6:
		if( iRest <= 35 )
		{
			if( !m_szRetreatLt.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatLt, m_fRetreatLtRate, vMoveDir );
				return true;
			}
		}
		else
		{
			if( !m_szRetreatFr.IsEmpty() )
			{
				ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );
				return true;
			}
		}
		break;
	}

	return false;
}

void ioRotateChargeItem::CheckReserve( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

float ioRotateChargeItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

void ioRotateChargeItem::ProcessReserve( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );

	// 공격키가 눌러져 있으면 공격 상태 처리
	if( pOwner->IsAttackKeyDown() && 
		!pOwner->IsJumpKeyDown() &&
		!pOwner->IsDefenseKeyDown() &&
		!pOwner->IsChatModeState(true) )
	{
		pOwner->NormalAttackOver( false );
		ClearState();

		ioObjectItem *pItem = pOwner->GetObject();
		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}

		return;
	}

	pOwner->NormalAttackOver();
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		ClearState();
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		ClearState();
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		ClearState();
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ClearState();
		ioObjectItem *pItem = pOwner->GetObject();

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
		}
		return;
	}

	ClearState();
	pOwner->SetState( CS_DELAY );
	return;
}

void ioRotateChargeItem::CheckGuidAttack( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCheckGuidTime > dwCurTime )
		return;

	if( !m_bUseGuidAttack )
		return;

	if( m_GuidAttribute.m_vWeaponInfoList.empty() )
		return;

	m_vCirclePos = pOwner->GetMagicCirclePos( m_fMagicCircleOffSet, true );

	CheckWeaponFloatInfo( pOwner );

	pOwner->NormalAttackFireExplicit( m_GuidAttribute.m_vWeaponInfoList.front() );
	m_dwCheckGuidTime = dwCurTime + m_dwGuidPeriod;
}

float ioRotateChargeItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioRotateChargeItem::CheckWeaponFloatInfo( ioBaseChar *pOwner )
{
	// WeaponMove
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	// CharDir
	if( vDir == ioMath::VEC3_ZERO )
	{
		vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
		pOwner->SetWorldOrientation( qtRot );
	}

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( m_fRange > 0.0f )
	{
		fRate = fRange / m_fRange;

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	m_vWeaponDir = vDir;
	m_fCurWeaponMoveSpeed = fCurWeaponMoveSpeed;
	m_fCurWeaponFloatPower = fCurWeaponFloatPower;
}



