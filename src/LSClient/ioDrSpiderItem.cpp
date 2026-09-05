
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioDrSpiderItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioDrSpiderItem::ioDrSpiderItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_fCurChangeDamage = 0.0f;
	m_dwChangeDamageEffect = -1;
	m_iRidingDummyCharIndex = 0;

	m_bProtectCharByDummyChar = false;

	m_iDummyCharHPCount = 1;
	m_iCheckDummyCharHPCount = 0;
}

ioDrSpiderItem::ioDrSpiderItem( const ioDrSpiderItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_fMaxChangeDamage( rhs.m_fMaxChangeDamage ),
  m_fNeedChangeDamage( rhs.m_fNeedChangeDamage ),
  m_fIncreaseChangeDamage( rhs.m_fIncreaseChangeDamage ),
  m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
  m_fDownDecreaseChangeDamage( rhs.m_fDownDecreaseChangeDamage ),
  m_fChangeDamageRateMin( rhs.m_fChangeDamageRateMin ),
  m_fChangeDamageRateMax( rhs.m_fChangeDamageRateMax ),
  m_MaxChangeDamageEffect( rhs.m_MaxChangeDamageEffect ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
  m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
  m_stFullChargeAni( rhs.m_stFullChargeAni ),
  m_fFullChargeAniRate( rhs.m_fFullChargeAniRate ),
  m_RidingDummyCharName( rhs.m_RidingDummyCharName ),
  m_bProtectCharByDummyChar( rhs.m_bProtectCharByDummyChar ),
  m_iDummyCharHPCount( rhs.m_iDummyCharHPCount )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_fCurChangeDamage = 0.0f;
	m_dwChangeDamageEffect = -1;
	m_iRidingDummyCharIndex = 0;
}

ioDrSpiderItem::~ioDrSpiderItem()
{
}

void ioDrSpiderItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// DrSpider
	m_fMaxChangeDamage = rkLoader.LoadFloat_e( "max_change_damage", 0.0f );
	m_fNeedChangeDamage = rkLoader.LoadFloat_e( "need_change_damage", 0.0f );
	
	m_fIncreaseChangeDamage = rkLoader.LoadFloat_e( "increase_change_damage", 0.0f );
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );
	m_fDownDecreaseChangeDamage = rkLoader.LoadFloat_e( "down_decrease_change_damage", 0.0f );

	m_fChangeDamageRateMin = rkLoader.LoadFloat_e( "change_damage_rate_min", 0.0f );
	m_fChangeDamageRateMax = rkLoader.LoadFloat_e( "change_damage_rate_max", 0.0f );

	rkLoader.LoadString_e( "max_change_damage_effect", "", szBuf, MAX_PATH );
	m_MaxChangeDamageEffect = szBuf;

	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );
	
	// dummy char
	rkLoader.LoadString_e( "riding_dummy_char_name", "", szBuf, MAX_PATH );
	m_RidingDummyCharName = szBuf;

	m_bProtectCharByDummyChar = rkLoader.LoadBool_e( "protect_char_by_dummy_char", false );

	m_iDummyCharHPCount = rkLoader.LoadInt_e( "dummy_char_hp_count", 1 );
}

ioItem* ioDrSpiderItem::Clone()
{
	return new ioDrSpiderItem( *this );
}

ioWeaponItem::WeaponSubType ioDrSpiderItem::GetSubType() const
{
	return WST_DR_SPIDER_ITEM;
}

void ioDrSpiderItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;

	int iReadyCnt = m_AttackReadyAniList.size();
	if( !COMPARE( iCurCombo, 0, iReadyCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioDrSpiderItem::CheckNormalAttack( ioBaseChar *pOwner )
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
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioDrSpiderItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if(m_fCurChangeDamage >= m_fMaxChangeDamage)
				ChangeToRiding( pOwner );
			else
				ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioDrSpiderItem::ChangeToGathering( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_ChargeState = CS_GATHERING;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrSpiderItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			
			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		}
		else
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
		}
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
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
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDrSpiderItem::ChangeToRiding( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( pOwner->CheckChangeWereWolfState( m_ExtendChargeType ) && CheckChangeWereWolfState( pOwner ) )
	{
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		if( !CreateDummyChar( pOwner, iCurIndex ) )
			iCurIndex = 0;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEREWOLF_STATE );
			kPacket << pOwner->GetCharName();
			kPacket << DR_SPIDER_RIDING_SET;
			kPacket << GetCurChangeDamage();
			kPacket << iCurIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioDrSpiderItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_GATHERING:
		{
			ChangeToGathering( pOwner );
		}
		break;
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	}
}

void ioDrSpiderItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	if( pChar && pZone )
	{
		if( bJumpAttack )
		{
			ioFlashJump *pFlashJump = ToFlashJump( GetCurExtendJump() );
			if( pFlashJump && pFlashJump->IsFlashJumpState() )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, false );
			}
		}
		else
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioDrSpiderItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioDrSpiderItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

void ioDrSpiderItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioDrSpiderItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;
			
			if( m_bCharged && pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );
				
				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioDrSpiderItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioDrSpiderItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioDrSpiderItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioDrSpiderItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

//void ioDrSpiderItem::ProcessDrSpiderJumpAttack( ioBaseChar *pOwner, float fHeightGap )
//{
//	if( !pOwner )
//		return;
//
//	DWORD dwCurTime = FRAMEGETTIME();
//
//	if( pOwner->IsAttackKeyDown() )
//	{
//		if( pOwner->IsNeedProcess() && pOwner->GetState() == CS_JUMP && (dwCurTime > m_dwAttackKeyReservTime) )
//		{
//			if( Check_D_Attack_Max_Weapon_Cnt(pOwner) )
//				ChangeJump_D_Attack( pOwner );
//		}
//	}
//
//	if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) )
//	{
//		pOwner->SetJumpBoostAttackState(false);
//	}
//}

void ioDrSpiderItem::DecreaseDummyCharHP( ioBaseChar* pOwner )
{
	m_iCheckDummyCharHPCount--;
	if( m_iCheckDummyCharHPCount <= 0 )
		m_fCurChangeDamage = 0.0f;
}

int ioDrSpiderItem::GetMaxBullet()
{
	return (int)m_fMaxChangeDamage;
}

int ioDrSpiderItem::GetNeedBullet()
{
	return (int)m_fNeedChangeDamage;
}

int ioDrSpiderItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurChangeDamage;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxChangeDamage ) );

	return (int)fCurCoolTime;
}

void ioDrSpiderItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxChangeDamage ) );

	m_fCurChangeDamage = fCurBullet;
}

void ioDrSpiderItem::InitChangeDamage()
{
	m_fCurChangeDamage = 0.0f;
}

void ioDrSpiderItem::MaxChangeDamage()
{
	m_fCurChangeDamage = m_fMaxChangeDamage;
}

void ioDrSpiderItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	//switch( iType )
	//{
	//case SSGT_NONE:
	//	break;
	//case SSGT_WOLF:
	//	if( m_ExtendChargeType != ECT_DrSpider )
	//		return;
	//	break;
	//case SSGT_RAPTOR:
	//	if( m_ExtendChargeType != ECT_RIDER )
	//		return;
	//	break;
	//case SSGT_GRIFFIN:
	//	if( m_ExtendChargeType != ECT_GRIFFIN )
	//		return;
	//	break;
	//default:
	//	return;
	//}

	//if( bBuff )
	//{
	//	m_fCurChangeDamage += fDamage;
	//}
	//else
	//{
	//	IORandom random;
	//	random.Randomize();

	//	int iRate = random.Random( 101 );
	//	float fRate = (float)iRate / FLOAT100;

	//	float fGapRate = fabsf(m_fChangeDamageRateMax - m_fChangeDamageRateMin);
	//	fGapRate *= fRate;

	//	float fCurChangeDamageRate = m_fChangeDamageRateMin + fGapRate;

	//	m_fCurChangeDamage += fDamage * fCurChangeDamageRate;
	//}

	//m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
}

void ioDrSpiderItem::IncreaseChangeDamageByWeapon( float fDamage )
{
	if( !m_bUsedWereWolf )
		return;

	if( fDamage <= 0.0f )
		return;

	m_fCurChangeDamage += fDamage;
	m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
}

void ioDrSpiderItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	//if( m_bUsedWereWolf )
	//	return;

	//if( m_fCurChangeDamage >= m_fMaxChangeDamage )
	//	return;

	//float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	//float fCurIncrease = m_fIncreaseChangeDamage * fTimePerSec;

	//float fBuffRate = 0.0f;
	//switch( m_ExtendChargeType )
	//{
	//case ECT_DrSpider:
	//	fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_WOLF );
	//	break;
	//case ECT_RIDER:
	//	fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_RAPTOR );
	//	break;
	//case ECT_GRIFFIN:
	//	fBuffRate = pOwner->GetSpecialGaugeBuffRate( SSGT_GRIFFIN );
	//	break;
	//case ECT_DR_SPIDER:
	//	return;
	//}

	//if( fBuffRate > 0.0f )
	//{
	//	fCurIncrease += fCurIncrease * fBuffRate;
	//}

	//m_fCurChangeDamage += fCurIncrease;
	//m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
}

void ioDrSpiderItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !m_bUsedWereWolf )
		return;

	CharState eState = pOwner->GetState();
	switch( eState )
	{
	case CS_FROZEN:
	case CS_ICE_STATE:
	case CS_STOP_MOTION:
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwChangeWereWolfTime > 0 )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fCurDecrease = 0.0f;

		if( CheckOwnerDownState(pOwner) )
		{
			m_fCurChangeDamage = 0.0f;
			return;
		}
		else
			fCurDecrease = m_fDecreaseChangeDamage * fTimePerSec;

		m_fCurChangeDamage -= fCurDecrease;
		m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
	}
	else
	{
		m_fCurChangeDamage = 0.0f;
	}
}

void ioDrSpiderItem::ReleseWereWolfState( ioBaseChar *pChar )
{
	ioAttackableItem::ReleseWereWolfState( pChar );

	if( pChar && m_dwWereWolfObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pChar->GetObject();
		if( !pObjectItem )
			return;

		if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
		{
			//애니메이션 초기화
			if( pChar->GetState() == CS_DELAY && m_fCurChangeDamage < 1.0f )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			}
			if( pChar->GetState() == CS_RUN && m_fCurChangeDamage < 1.0f )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			}
			else if( pChar->GetState() == CS_JUMP && !m_bUsedWereWolf && m_fCurChangeDamage < 1.0f )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_FALL );
			}
			else if( pChar->GetState() == CS_FALL && m_fCurChangeDamage <= 1.0f )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_FALL );
			}
			else
			{
				pChar->ReleaseObjectItem( __FUNCTION__ );
			}
		}
	}

	if( pChar && m_iRidingDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = 0;
	}

	float fCurChangeDamage = m_fCurChangeDamage;
	// 신규용병은 S키 눌러서 더미 릴리즈시키고 나서 release_attack을 발동
	if( m_fCurChangeDamage >= 1.0f )
	{
		// 릴리즈 될때 게이지 초기화
		m_fCurChangeDamage = 0.0f;
		InitExtraGauge();
	}
	else
		pChar->SetState( CS_DELAY );
}

void ioDrSpiderItem::ReleseWereWolfStateByOwner( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );

	// add buff
	int iBuffCnt = m_vReleaseWereWolfBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vReleaseWereWolfBuffList[i];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// change state & motion
	pChar->SetWereWolfState( m_ReleaseWereWolfAttack.m_AttackAnimation, m_ReleaseWereWolfAttack.m_fAttackAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReleaseWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ReleaseWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ReleaseWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ReleaseWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
}

void ioDrSpiderItem::CheckEnableChangeDamage( ioBaseChar *pChar )
{
	if( m_fCurChangeDamage >= m_fNeedChangeDamage )
		SetEnableChangeDamageEffect( pChar );
}

bool ioDrSpiderItem::CheckChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;
	if( !m_bEnableWereWolf )
		return false;
	if( m_bUsedWereWolf )
		return false;

	// check bullet
	if( m_fCurChangeDamage < m_fNeedChangeDamage )
		return false;

	SetChangeWereWolfState( pChar );
	return true;
}

void ioDrSpiderItem::SetChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ReleaseEnableChangeDamageEffect( pChar );

	// change state & motion
	pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ChangeWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ChangeWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->AddEntity( szMeshName );
		}
	}

	// add buff
	int iBuffCnt = m_vWereWolfBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vWereWolfBuffList[j];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// create DrSpider objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwWereWolfObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_dwWereWolfObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwWereWolfObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();			

				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pChar->EquipItem( pItem );
			}
		}
	}

	m_iCheckDummyCharHPCount = m_iDummyCharHPCount;
	m_bUsedWereWolf = true;
	m_dwChangeWereWolfTime = FRAMEGETTIME();
}

bool ioDrSpiderItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	
	CharState eState = pOwner->GetState();

	//if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
	//	return true;
	//else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
	//	return true;
	//else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
	//	return true;
	//else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
	//	return true;
	//else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
	//	return true;
	if( eState == CS_DROP_ZONE_DOWN )
		return true;
	else if( eState == CS_DIE )
		return true;
	//else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
	//	return true;

	

	return false;
}

float ioDrSpiderItem::GetCurChangeDamage()
{
	return m_fCurChangeDamage;
}

void ioDrSpiderItem::SetCurChangeDamage( float fDamage )
{
	m_fCurChangeDamage = fDamage;
	m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
}

void ioDrSpiderItem::OnReleased( ioBaseChar *pOwner )
{
	ReleseWereWolfState( pOwner );
	ReleaseEnableChangeDamageEffect( pOwner );
	m_fCurChangeDamage = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioDrSpiderItem::SetEnableChangeDamageEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwChangeDamageEffect == -1 && !m_MaxChangeDamageEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_MaxChangeDamageEffect );
		if( pEffect )
		{
			m_dwChangeDamageEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEREWOLF_STATE );
			kPacket << pChar->GetCharName();
			kPacket << WEREWOLF_ENABLE_GAUGE;
			kPacket << m_fCurChangeDamage;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioDrSpiderItem::ReleaseEnableChangeDamageEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwChangeDamageEffect != -1 )
		pChar->EndEffect( m_dwChangeDamageEffect, false );

	m_dwChangeDamageEffect = -1;
}

//void ioDrSpiderItem::ChangeToGathering( ioBaseChar *pOwner )
//{
//	ioEntityGroup *pGrp = pOwner->GetGroup();
//	pGrp->ClearAllActionAni( FLOAT100, true );
//	pGrp->ClearAllLoopAni( FLOAT100, true );
//
//	float fCurRate = FLOAT1;
//	if( m_fGatherAniRate > 0.0f )
//		fCurRate = m_fGatherAniRate;
//
//	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
//
//	m_ChargeState = CS_GATHERING;
//	m_dwGatheringStartTime = FRAMEGETTIME();
//
//	pOwner->SetAttackMoveEnable( false );
//
//	if( pOwner->IsNeedSendNetwork() )
//	{
//		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
//		FillItemGaugePacketHeader( pOwner, kPacket );
//		kPacket << m_ChargeState;
//		kPacket << m_iCurCombo;
//		P2PNetwork::SendToAllPlayingUser( kPacket );
//	}
//}

void ioDrSpiderItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if(m_fCurChangeDamage < m_fMaxChangeDamage)
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fCurIncrease = m_fIncreaseChangeDamage * fTimePerSec;
			m_fCurChangeDamage += fCurIncrease;
			m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
		}
	}
	else
		pOwner->SetState( CS_DELAY );
}

void ioDrSpiderItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioDrSpiderItem::ApplyChangeDrSpiderState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ReleaseEnableChangeDamageEffect( pChar );

	// change state & motion
	pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ChangeWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ChangeWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->AddEntity( szMeshName );
		}
	}

	/*
	// add buff
	int iBuffCnt = m_vDrSpiderBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vDrSpiderBuffList[j];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}
	*/

	// create DrSpider objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwWereWolfObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_dwWereWolfObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwWereWolfObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();
				
				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pChar->EquipItem( pItem );
			}
		}
	}

	m_bUsedWereWolf = true;
	m_dwChangeWereWolfTime = FRAMEGETTIME();
}

bool ioDrSpiderItem::CreateDummyChar( ioBaseChar *pOwner, int iIndex )
{
	if( !pOwner ) return false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return false;

	if( pOwner && m_iRidingDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = 0;
	}

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_RidingDummyCharName, iIndex,
												   pOwner->GetCharName(),
												   vTargetPos,
												   0.0f,
												   0, true );

	if( pDummy )
	{
		g_DummyCharMgr.SetCurLocalIndex( iIndex );
		m_iRidingDummyCharIndex = iIndex;
		return true;
	}

	return false;
}

void ioDrSpiderItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if(m_bProtectCharByDummyChar && m_bUsedWereWolf )
		DecreaseDummyCharHP( pOwner );
	else
	{
		float fTypeRate = 0.0f;
		switch( iType )
		{
		case EGT_ATTACK:
			break;
		case EGT_ATTACK_DEF:
			break;
		case EGT_WOUNDED:
			if( m_bUsedWereWolf )
				fTypeRate = m_fExtraGaugeWoundDec;
			else
				fTypeRate = m_fExtraGaugeWound;
			break;
		case EGT_WOUNDED_DEF:
			if( m_bUsedWereWolf )
				fTypeRate = m_fExtraGaugeWoundDec;
			else
				fTypeRate = m_fExtraGaugeWound;
			break;
		}

		float fCurGauge = fTypeRate * fDamage;
		if( fTypeRate <= 0.0f )
			return;

		if( m_bUsedWereWolf )
		{
			m_fCurChangeDamage -= fCurGauge;
			m_fCurChangeDamage = max( 0.0f, m_fCurChangeDamage );
		}
		else
		{
			m_fCurChangeDamage += fCurGauge;
			m_fCurChangeDamage = min( m_fCurChangeDamage, m_fMaxChangeDamage );
		}
	}
}

bool ioDrSpiderItem::IsRidingState()
{
	if( IsWereWolfState() )
		return true;

	return false;
}

bool ioDrSpiderItem::IsProtectCharByDummyChar()
{
	if( IsWereWolfState() && m_bProtectCharByDummyChar )
		return true;

	return false;
}

bool ioDrSpiderItem::IsEnableActionStopDelay()
{
	if( IsRidingState() )
		return false;

	return true;
}

void ioDrSpiderItem::SetRidingDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
{
	if( !pOwner ) return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
	{
		m_iRidingDummyCharIndex = iCurIndex;
	}
}

bool ioDrSpiderItem::UsingRidingDummyChar()
{
	if( !m_RidingDummyCharName.IsEmpty() )
		return true;

	return false;
}

bool ioDrSpiderItem::CheckReleaseWereWolfState( ioBaseChar *pChar )
{
	if( !m_bUsedWereWolf )
		return false;

	//if( m_fCurChangeDamage < 1.0f )
	//	pChar->SetState( CS_DELAY );

	// bullet check.
	if( GetCurBullet() <= 0 )
	{
		ReleseWereWolfState( pChar );
		return true;
	}

	ioDummyChar *pDummyChar = g_DummyCharMgr.FindDummyCharToName( pChar, m_RidingDummyCharName );
	if( pDummyChar )
	{ 
		if( pDummyChar->GetHP().m_fCurValue < 1.0f || pDummyChar->GetState() == DCS_DIE )
			OnReleased(pChar);
	}
	else
	{
		m_fCurChangeDamage = 0.0f;
		ReleseWereWolfState( pChar );
	}

	if( pChar )
	{
		int iState = pChar->GetState();
		switch( iState )
		{
		case CS_WOUNDED:
		case CS_BLOW_WOUNDED:
		case CS_ETC_STATE:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_BLOW_EXTEND_WOUND:
		case CS_BLOW_DASH_ATTACK:
		case CS_SKIP_STUN:
		case CS_GRAPPLING_WOUNDED:
		case CS_GRAPPLING_WOUNDED_SKILL:
		case CS_GRAPPLING_WOUNDED_SWING:
		case CS_GRAPPLING_WOUNDED_ITEM:
		case CS_GRAPPLING_WOUNDED_BUFF:
		case CS_GRAPPLING_PUSHED_BUFF:
		case CS_WARP_STATE:
		case CS_FROZEN:
		case CS_CLOSEORDER:
		case CS_FLOAT_STATE:
		case CS_ICE_STATE:
		case CS_UROBORUS:
		case CS_PANIC_MOVE:
		case CS_MAGIC_STUN:
		case CS_LOCKUP_BUFF:
		case CS_ADHESIVE_BUFF:
		case CS_WIND_MAGNETIC:
		case CS_GHOST_STEALER_STATE:
		case CS_TITAN_EXTEND_MOVE:
			ReleseWereWolfState( pChar );
			return true;
		default:
			break;
		}

		if( pChar->IsCatchMode() )
			return true;
	}

	return false;
}

void ioDrSpiderItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	if( pOwner && m_iRidingDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//ioDrSpiderSpecialState::ioDrSpiderSpecialState()
//{
//}
//
//ioDrSpiderSpecialState::~ioDrSpiderSpecialState()
//{
//}
//
//void ioDrSpiderSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
//{
//	ioSpecialStateBase::Initialize( pOwner, eCurrState );
//}
//
//void ioDrSpiderSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
//{
//	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
//}
//
//void ioDrSpiderSpecialState::CheckSpecialState( ioBaseChar* pOwner )
//{
//	pOwner->CheckExtraAniJump();
//
//	if( pOwner->GetAnimationEndTime() < FRAMEGETTIME() )
//	{
//		ioBubbleItem *pBubble = ToBubbleItem( pOwner->GetWeapon() );
//		ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
//		if( pBubble && !pBubble->IsDrSpiderState() )
//		{
//			pOwner->SetExtendAttackEndJump( pBubble->GetDrSpiderEndJumpPower(), FLOAT1 );
//			pOwner->ClearAttackFireTimeAndSkill();
//			return;
//		}
//		else if( pChangeOn && pChangeOn->IsDrSpiderState() )
//		{
//			ioChangeOnItem::DrSpiderChangeState eState = pChangeOn->GetWolfChangeState();
//			if( eState == ioChangeOnItem::WSC_GRIFFIN )
//			{
//				pOwner->SetExtendAttackEndJump( pChangeOn->GetDrSpiderEndJumpPower(), FLOAT1 );
//				pOwner->ClearAttackFireTimeAndSkill();
//				return;
//			}
//		}
//
//		pOwner->SetState( CS_DELAY );
//		return;
//	}
//}
//
//void ioDrSpiderSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
//{
//	return;
//}
//
//bool ioDrSpiderSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
//{
//	ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
//	if( pChangeOn && pChangeOn->IsDrSpiderState() )
//		return true;
//
//	return false;
//}