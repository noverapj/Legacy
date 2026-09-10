

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioSummonChargeItem.h"

#include "WeaponDefine.h"

ioSummonChargeItem::ioSummonChargeItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_iCurSummonCnt = 1;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioSummonChargeItem::ioSummonChargeItem( const ioSummonChargeItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAni( rhs.m_AttackReadyAni ),
  m_vSummonAttackInfoList( rhs.m_vSummonAttackInfoList ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_iMaxSummonCnt( rhs.m_iMaxSummonCnt ),
  m_szRetreatFr( rhs.m_szRetreatFr ),
  m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
  m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
  m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
  m_iLimitSummonCnt( rhs.m_iLimitSummonCnt ),
  m_fSkillGaugeRate( rhs.m_fSkillGaugeRate )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_iCurSummonCnt = 1;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioSummonChargeItem::~ioSummonChargeItem()
{
}

void ioSummonChargeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "attack_move_ani", "", szBuf, MAX_PATH );
	m_ChargingMoveAnimation = szBuf;

	//
	m_iLimitSummonCnt = rkLoader.LoadInt_e( "summon_limit_cnt", 0 );

	m_iMaxSummonCnt = rkLoader.LoadInt_e( "summon_max_cnt", 0 );
	m_vSummonAttackInfoList.clear();
	m_vSummonAttackInfoList.reserve( m_iMaxSummonCnt );

	for( i=0 ; i<m_iMaxSummonCnt ; i++ )
	{
		SummonAttackInfo kInfo;

		wsprintf_e( szKey, "summon_time0%d", i+1 );
		kInfo.m_dwCreateTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "summon_effect0%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_SummonEffect = szBuf;

		wsprintf_e( szKey, "summon_sound0%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_SummonSound = szBuf;

		AttackAttribute kAttribute;
		wsprintf_e( szKey, "summon_attack0%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );

		kInfo.m_SummonAttribute = kAttribute;

		m_vSummonAttackInfoList.push_back( kInfo );
	}

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );
}

ioItem* ioSummonChargeItem::Clone()
{
	return new ioSummonChargeItem( *this );
}

ioWeaponItem::WeaponSubType ioSummonChargeItem::GetSubType() const
{
	return WST_SUMMON_CHARGE;
}

void ioSummonChargeItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_iCurSummonCnt = 1;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_vSummonAttackInfoList.empty() )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		//pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_bSetChargingMoveAni = false;
	}
	else
	{
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
		//pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioSummonChargeItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}

	if( m_ChargeState == CS_GATHERING )
		CheckChargingMoveState( pOwner );
}

void ioSummonChargeItem::OnCharging( ioBaseChar *pOwner )
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
		ChangeToAttackFire( pOwner, false );
	}
}

void ioSummonChargeItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 공격버튼땜, 발사.
	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToAttackFire( pOwner, true );
		return;
	}

	// 점프키 누름. 점프상태 전환
	if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// SD 스킬 입력.
	if( pOwner->IsDefenseKeyDown() )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			//pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_bSetChargingMoveAni = false;

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			m_ChargeState = CS_NONE;
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << m_iCurSummonCnt;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else if( pOwner->IsDirKeyDoubleClick() )
	{
		CheckRetreat( pOwner, true );
		return;
	}

	if( CheckNextSummon( pOwner ) && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurSummonCnt;
		kPacket << true;		// next summon
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSummonChargeItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	//pOwner->SetAttackMoveEnable( true );

	m_iCurSummonCnt = 1;
	m_dwNextSummonTime = 0;

	// NextTime
	CheckNextSummonTime();

	// Effect
	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	ioHashString szCurEffect = GetCurSummonEffect();
	if( !szCurEffect.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
		if( pEffect )
			m_GatheringEffectID = pEffect->GetUniqueID();
	}

	// Sound
	ioHashString szCurSound = GetCurSummonSound();
	if( !szCurSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
		}
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurSummonCnt;
		kPacket << false;		// next summon
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSummonChargeItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	//pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
	//

	if( bCharged )
	{
		float fCurChargeAniRate = FLOAT1;
		float fCurForceSlidingRate = FLOAT1;

		int iIndex = m_iCurSummonCnt - 1;
		if( COMPARE( iIndex, 0, (int)m_vSummonAttackInfoList.size() ) )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
			pOwner->SetNormalAttackByAttribute( m_vSummonAttackInfoList[iIndex].m_SummonAttribute, true, fCurChargeAniRate, fCurForceSlidingRate );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurSummonCnt;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSummonChargeItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;
	rkPacket >> m_iCurSummonCnt;

	bool bCharged;

	switch( iState )
	{
	case CS_NONE:
		//pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_bSetChargingMoveAni = false;

		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		m_ChargeState = CS_NONE;
		break;
	case CS_GATHERING:
		{
			bool bNext;
			rkPacket >> bNext;

			if( bNext )
			{
				// Effect
				if( m_GatheringEffectID != -1 )
				{
					pOwner->EndEffect( m_GatheringEffectID, false );
					m_GatheringEffectID = -1;
				}

				ioHashString szCurEffect = GetCurSummonEffect();
				if( !szCurEffect.IsEmpty() )
				{
					ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
					if( pEffect )
						m_GatheringEffectID = pEffect->GetUniqueID();
				}

				// Sound
				ioHashString szCurSound = GetCurSummonSound();
				if( !szCurSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
					{
						g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
					}
				}
			}
			else
			{
				ChangeToGathering( pOwner );
			}
		}
		break;
	case CS_ATTACK_FIRE:
		rkPacket >> bCharged;
		ChangeToAttackFire( pOwner, bCharged );
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

void ioSummonChargeItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

	ioSummonMissileWeapon *pSummon = ToSummonMissileWeapon( pWeapon );
	if( pSummon && m_iLimitSummonCnt > 0 )
	{
		g_WeaponMgr.CheckMaxSummonWeaponCnt( pWeapon, m_iLimitSummonCnt );
	}
}

bool ioSummonChargeItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioSummonChargeItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !bCheck ) return false;

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

bool ioSummonChargeItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
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

void ioSummonChargeItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_ChargingMoveAnimation);
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_GatherAnimation );
			if( pAni )
			{
				dwPlayTime = pAni->GetCurTime();
			}

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetChargingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
			if( iAniID == -1 )
				return;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = pGrp->GetLoopInPlay( m_ChargingMoveAnimation );
			if( pAni )
				dwPlayTime = pAni->GetCurTime();

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetChargingMoveAni = false;
		}
	}
}

void ioSummonChargeItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	//pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}
}

void ioSummonChargeItem::GetAutoTargetValue( ioBaseChar *pOwner,
											 float &fRange, float &fMinAngle, float &fMaxAngle,
											 AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

ioHashString ioSummonChargeItem::GetCurSummonEffect()
{
	ioHashString szName;

	if( m_vSummonAttackInfoList.empty() )
		return szName;

	int iIndex = m_iCurSummonCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_vSummonAttackInfoList.size() ) )
		szName = m_vSummonAttackInfoList[iIndex].m_SummonEffect;

	return szName;
}

ioHashString ioSummonChargeItem::GetCurSummonSound()
{
	ioHashString szName;

	if( m_vSummonAttackInfoList.empty() )
		return szName;

	int iIndex = m_iCurSummonCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_vSummonAttackInfoList.size() ) )
		szName = m_vSummonAttackInfoList[iIndex].m_SummonSound;

	return szName;
}

bool ioSummonChargeItem::CheckNextSummon( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( m_dwNextSummonTime <= 0 ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwNextSummonTime < dwCurTime )
	{
		m_iCurSummonCnt++;

		// Effect
		if( m_GatheringEffectID != -1 )
		{
			pOwner->EndEffect( m_GatheringEffectID, false );
			m_GatheringEffectID = -1;
		}

		ioHashString szCurEffect = GetCurSummonEffect();
		if( !szCurEffect.IsEmpty() )
		{
			ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
			if( pEffect )
				m_GatheringEffectID = pEffect->GetUniqueID();
		}

		// Sound
		ioHashString szCurSound = GetCurSummonSound();
		if( !szCurSound.IsEmpty() )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				g_SoundMgr.PlaySound( szCurSound, pGrp->GetParentSceneNode() );
			}
		}

		if( !CheckNextSummonTime() )
		{
			m_dwNextSummonTime = 0;
		}

		return true;
	}

	return false;
}

bool ioSummonChargeItem::CheckNextSummonTime()
{
	m_dwGatheringStartTime;

	if( m_vSummonAttackInfoList.empty() )
		return false;

	int iIndex = m_iCurSummonCnt;
	if( COMPARE( iIndex, 0, (int)m_vSummonAttackInfoList.size() ) )
	{
		m_dwNextSummonTime = m_dwGatheringStartTime + m_vSummonAttackInfoList[iIndex].m_dwCreateTime;
		return true;
	}

	return false;
}

void ioSummonChargeItem::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
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
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
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

void ioSummonChargeItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );

	//pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	m_dwKeyReserveTime = 0;

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

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iCurSummonCnt;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSummonChargeItem::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	//pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
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

	ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );

	return true;
}

void ioSummonChargeItem::CheckReserve( ioBaseChar *pOwner )
{
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

void ioSummonChargeItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioSummonChargeItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioSummonChargeItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioSummonChargeItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioSummonChargeItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioSummonChargeItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
		pOwner->GetState() == CS_VIEW ||
		pOwner->GetState() == CS_LOADING ||
		pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioSummonChargeItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}
