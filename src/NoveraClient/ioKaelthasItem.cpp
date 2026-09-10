

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioKaelthasItem.h"
#include "WeaponDefine.h"
#include "ioPlayStage.h"

ioKaelthasItem::ioKaelthasItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_FullChargeEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioKaelthasItem::ioKaelthasItem( const ioKaelthasItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAni( rhs.m_AttackReadyAni ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_ChargingMoveAnimation( rhs.m_ChargingMoveAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
  m_ExtendAttribute( rhs.m_ExtendAttribute ),
  m_dwChargeFullTime( rhs.m_dwChargeFullTime ),
  m_stFullChargeEffect( rhs.m_stFullChargeEffect ),
  m_dwDecFullGatherGaugeTime( rhs.m_dwDecFullGatherGaugeTime ),
  m_fDecFullGatherGauge( rhs.m_fDecFullGatherGauge ),
  m_fFullGatherCheckRange( rhs.m_fFullGatherCheckRange ),
  m_stFullGatherCheckBuff( rhs.m_stFullGatherCheckBuff ),
  m_stFullGatherAttackBuff( rhs.m_stFullGatherAttackBuff ),
  m_fExtraNeedGauge( rhs.m_fExtraNeedGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_FullChargeEffectID = -1;

	m_bSetChargingMoveAni = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
}

ioKaelthasItem::~ioKaelthasItem()
{
}

void ioKaelthasItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

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
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );


	//////////////////////////////////////////////////////////////////////////
	LoadAttackAttribute( "extend_attack", m_ExtendAttribute, rkLoader );


	m_dwChargeFullTime = (DWORD)rkLoader.LoadInt_e( "charge_full_time", 10000 );
	rkLoader.LoadString_e( "full_charge_effect", "", szBuf, MAX_PATH );
	m_stFullChargeEffect = szBuf;

	m_dwDecFullGatherGaugeTime = (DWORD)rkLoader.LoadInt_e( "dec_full_gauge_by_tic_time", 1 );
	m_fDecFullGatherGauge = rkLoader.LoadFloat_e( "dec_full_gauge_by_tic", FLOAT0001 );

	m_fFullGatherCheckRange = rkLoader.LoadFloat_e( "full_charge_range", FLOAT1 );

	rkLoader.LoadString_e( "full_charge_attack_check_buff", "", szBuf, MAX_PATH );
	m_stFullGatherCheckBuff = szBuf;

	rkLoader.LoadString_e( "full_charge_attack_buff", "", szBuf, MAX_PATH );
	m_stFullGatherAttackBuff = szBuf;

	m_fExtraNeedGauge = rkLoader.LoadFloat( "extra_need_gauge", 30000.f );
}

ioItem* ioKaelthasItem::Clone()
{
	return new ioKaelthasItem( *this );
}

ioWeaponItem::WeaponSubType ioKaelthasItem::GetSubType() const
{
	return WST_KAELTHAS_ITEM;
}

void ioKaelthasItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_dwKeyReserveTime = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();

	if( m_FullChargeEffectID != -1 )
	{
		pOwner->EndEffect( m_FullChargeEffectID, false );
		m_FullChargeEffectID = -1;
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
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( true );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

}

void ioKaelthasItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_FULL_CHARGE:
		OnFullCharge( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner && pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;;
	}
}

void ioKaelthasItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && GetCurBullet() >= GetNeedBullet() )
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

void ioKaelthasItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

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
			pOwner->SetAttackMoveEnable( false );
			pOwner->SetChargingState( false );
			m_bSetChargingMoveAni = false;

			if( m_FullChargeEffectID != -1 )
			{
				pOwner->EndEffect( m_FullChargeEffectID, false );
				m_FullChargeEffectID = -1;
			}

			m_ChargeState = CS_NONE;
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

	if( m_dwGatheringStartTime+m_dwChargeFullTime  < dwCurTime )
	{
		ChangeFullCharge( pOwner );
		return;
	}
}


void ioKaelthasItem::OnFullCharge( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() && GetCurBullet() <= 0.f )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	CheckChargingMoveState( pOwner );
	
	if( pOwner->IsNeedProcess() )
		CheckTarget( pOwner );	

	CheckKeyInput( pOwner );
}


void ioKaelthasItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = dwCurTime;
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioKaelthasItem::ChangeFullCharge( ioBaseChar *pOwner )
{
	m_ChargeState = CS_FULL_CHARGE;
	pOwner->SetAttackMoveEnable( true );

	// Effect
	if( m_FullChargeEffectID != -1 )
	{
		pOwner->EndEffect( m_FullChargeEffectID, false );
		m_FullChargeEffectID = -1;
	}

	ioHashString szCurEffect = m_stFullChargeEffect;
	if( !szCurEffect.IsEmpty() )
	{
		ioEffect *pEffect = pOwner->AttachEffect( szCurEffect );
		if( pEffect )
			m_FullChargeEffectID = pEffect->GetUniqueID();
	}

	if(  pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKaelthasItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	//
	pOwner->InitExtendAttackTagInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	if( m_FullChargeEffectID != -1 )
	{
		pOwner->EndEffect( m_FullChargeEffectID, false );
		m_FullChargeEffectID = -1;
	}
	//

	m_dwKeyReserveTime = 0;
	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;
	if( bCharged )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

		float fCurChargeAniRate = FLOAT1;
		float fCurForceSlidingRate = FLOAT1;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}

		DWORD dwPreDelay = m_ExtendAttribute.m_dwPreDelay;

		pOwner->SetAutoTarget( ATT_NORMAL );
		pOwner->SetNormalAttackByAttribute( m_ExtendAttribute,
			true,
			fCurChargeAniRate,
			fCurForceSlidingRate );

		int iAniID = pGrp->GetAnimationIdx( m_ExtendAttribute.m_AttackAnimation );
		float fTimeRate = m_ExtendAttribute.m_fAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fKeyReserve > 0.0f )
			m_dwKeyReserveTime = dwCurTime + fKeyReserve;

		m_szCurAni = m_ExtendAttribute.m_AttackAnimation;
		m_fCurAniRate = fTimeRate;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		
		m_ChargeState = CS_ATTACK_FIRE;

		DecExtraGauge();
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioKaelthasItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	bool bCharged;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
		m_bSetChargingMoveAni = false;

		if( m_FullChargeEffectID != -1 )
		{
			pOwner->EndEffect( m_FullChargeEffectID, false );
			m_FullChargeEffectID = -1;
		}

		m_ChargeState = CS_NONE;
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_FULL_CHARGE:
		ChangeFullCharge( pOwner );
		break;
	case CS_ATTACK_FIRE:
	case CS_NORMAL_ATTACK:
		rkPacket >> bCharged;
		ChangeToAttackFire( pOwner, bCharged );
		break;
	case CS_ADD_ATTACK_BUFF:
		{
			ioHashString stTargetName;
			rkPacket >> stTargetName;
			AddTargetBuff( pOwner, stTargetName );
		}
		break;
	}
}

void ioKaelthasItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

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

bool ioKaelthasItem::IsChargeAttackState( ioBaseChar *pOwner )
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


bool ioKaelthasItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
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


bool ioKaelthasItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) 
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	return true;
}

void ioKaelthasItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		// 점프키 누름. 점프상태 전환
		if( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}else if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
		{
			pOwner->ChangeDirectionByInputDir( false );
			pOwner->SetDashState( false );
			pOwner->SendDashState( false );
			return;
		}
		else if( !pOwner->IsAttackKeyDown() )
		{
			pOwner->SetAttackMoveEnable( false );
			pOwner->SetState( CS_DELAY );
		}
	}
}

void ioKaelthasItem::CheckTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fTargetRangeSq = m_fFullGatherCheckRange * m_fFullGatherCheckRange;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 거리 체크
		if( fDiffRangeSq > fTargetRangeSq )
			continue;

		if( !pTarget->HasBuff( m_stFullGatherCheckBuff ) )
			continue;

		if( pTarget->HasBuff( m_stFullGatherAttackBuff ) )
			continue;

		AddTargetBuff( pOwner, pTarget->GetCharName() );
	}
}


void ioKaelthasItem::AddTargetBuff( ioBaseChar *pOwner, const ioHashString& stTargetName )
{
	if( !pOwner )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar( stTargetName );
	if( !pTarget )
		return;

	if( pTarget->HasBuff( m_stFullGatherAttackBuff ) )
		return;

	pTarget->AddNewBuff( m_stFullGatherAttackBuff, pOwner->GetCharName(), GetName(), NULL );

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_ADD_ATTACK_BUFF;
		kPacket << m_iCurCombo;
		kPacket << stTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioKaelthasItem::CheckChargingMoveState( ioBaseChar *pOwner )
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

void ioKaelthasItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;
	m_fCurChargeRate = 0.0f;

	if( m_FullChargeEffectID != -1 )
	{
		pOwner->EndEffect( m_FullChargeEffectID, false );
		m_FullChargeEffectID = -1;
	}
}

void ioKaelthasItem::GetAutoTargetValue( ioBaseChar *pOwner,
											  float &fRange, float &fMinAngle, float &fMaxAngle,
											  AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

void ioKaelthasItem::CheckReserve( ioBaseChar *pOwner )
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

void ioKaelthasItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioKaelthasItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioKaelthasItem::SetJumpAttack - AttackAttribute Not Exist" );
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

void ioKaelthasItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioKaelthasItem::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
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

bool ioKaelthasItem::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

void ioKaelthasItem::CheckChargeInfo( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon ) return;

	float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
	float fCurScaleRate, fCurExplosionRate, fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
	fCurScaleRate = fCurExplosionRate = fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
		float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );

		float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );

		float fCurMaxScale = max( 0.0f, (m_fMaxScaleRate - FLOAT1) );
		float fCurMaxExplosion = max( 0.0f, (m_fMaxExplosionRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
		fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;

		fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

		fCurScaleRate += fCurMaxScale * m_fCurChargeRate;
		fCurExplosionRate += fCurMaxExplosion * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
		pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );

		pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );

		pWeapon->SetOwnerChargeScaleRate( fCurScaleRate );
		pWeapon->SetOwnerChargeExplosionRate( fCurExplosionRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}
}



void ioKaelthasItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
		case CS_DELAY:
		case CS_DASH:
		case CS_RUN:
			if( m_fCurExtraGauge < m_fMaxExtraGauge )
			{
				float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

				m_fCurExtraGauge += fGauge;
				m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
			}
			return;
		case CS_ATTACK:
			if( m_ChargeState == CS_FULL_CHARGE )
			{
				float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
				m_fCurExtraGauge = max( 0.f, m_fCurExtraGauge - fGauge );
			}
			return;
	}
}

int	ioKaelthasItem::GetNeedBullet()
{
	return (int)m_fExtraNeedGauge;
}


int ioKaelthasItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

int ioKaelthasItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

void ioKaelthasItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}


float ioKaelthasItem::DecExtraGauge()
{
	m_fCurExtraGauge = max( 0.f, m_fCurExtraGauge - m_fExtraNeedGauge );
	return m_fCurExtraGauge;
}

void ioKaelthasItem::OnReleased( ioBaseChar *pOwner )
{
	__super::OnReleased( pOwner );
	m_fCurExtraGauge = 0.f;
}


bool ioKaelthasItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) 
		return false;

	if( pOwner->GetState() == CS_ATTACK && m_ChargeState == CS_FULL_CHARGE )
		return true;

	return false;
}