

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioBuffAddItem.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioBuffAddItem::ioBuffAddItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_bBlitzAttackState = false;
	m_bBlitzBack = false;
	m_iCurBuffAddCombo = 0;

	m_fCurChangeGauge = 0.0f;
}

ioBuffAddItem::ioBuffAddItem( const ioBuffAddItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_BuffAddAttribute( rhs.m_BuffAddAttribute ),
  m_BlitzAttributeFr( rhs.m_BlitzAttributeFr ),
  m_BlitzAttributeBk( rhs.m_BlitzAttributeBk ),
  m_vBuffAddList( rhs.m_vBuffAddList ),
  m_fBlitzRange( rhs.m_fBlitzRange ),
  m_fBlitzMaxAngle( rhs.m_fBlitzMaxAngle ),
  m_fBlitzMinAngle( rhs.m_fBlitzMinAngle ),
  m_fMaxChangeGauge( rhs.m_fMaxChangeGauge ),
  m_fIncreaseChangeGauge( rhs.m_fIncreaseChangeGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_bBlitzAttackState = false;
	m_bBlitzBack = false;
	m_iCurBuffAddCombo = 0;

	m_fCurChangeGauge = 0.0f;
}

ioBuffAddItem::~ioBuffAddItem()
{
	m_vExtendAttributeList.clear();
	m_vBuffAddList.clear();

	m_BlitzAttributeFr.clear();
	m_BlitzAttributeBk.clear();
}

void ioBuffAddItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

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

	//
	LoadAttackAttribute_e( "buff_add_attack", m_BuffAddAttribute, rkLoader );

	m_vBuffAddList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "buff_add_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "buff_add%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuffName = szBuf;
		m_vBuffAddList.push_back( szBuffName );
	}

	// fr
	int iBlitzFrCnt = rkLoader.LoadInt_e( "blitz_attack_max_fr", 0 );
	m_BlitzAttributeFr.clear();
	m_BlitzAttributeFr.reserve( iBlitzFrCnt );
	for( i=0; i < iBlitzFrCnt; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "blitz_fr_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_BlitzAttributeFr.push_back( kAttribute );
	}

	// bk
	int iBlitzBkCnt = rkLoader.LoadInt_e( "blitz_attack_max_bk", 0 );
	m_BlitzAttributeBk.clear();
	m_BlitzAttributeBk.reserve( iBlitzBkCnt );
	for( i=0; i < iBlitzBkCnt; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szKey, "blitz_bk_attack%d", i+1 );
		LoadAttackAttribute( szKey, kAttribute, rkLoader );
		m_BlitzAttributeBk.push_back( kAttribute );
	}

	m_fBlitzRange = rkLoader.LoadFloat_e( "blitz_target_range", 0.0f );
	m_fBlitzMaxAngle = rkLoader.LoadFloat_e( "blitz_target_max_angle", 0.0f );
	m_fBlitzMinAngle = rkLoader.LoadFloat_e( "blitz_target_min_angle", 0.0f );

	m_fMaxChangeGauge = rkLoader.LoadFloat_e( "max_change_gauge", 0.0f );
	m_fIncreaseChangeGauge = rkLoader.LoadFloat_e( "increase_change_gauge", 0.0f );
}

ioItem* ioBuffAddItem::Clone()
{
	return new ioBuffAddItem( *this );
}

ioWeaponItem::WeaponSubType ioBuffAddItem::GetSubType() const
{
	return WST_BUFF_ADD;
}

void ioBuffAddItem::OnReleased( ioBaseChar *pOwner )
{
	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_vBuffAddList[i] );
	}

	ioAttackableItem::OnReleased( pOwner );
}

void ioBuffAddItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();

	m_bCharged = false;

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );
		
		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else
	{
		if( m_bBlitzAttackState )
		{
			ChangeToBlitzAttack( pOwner, false );
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
			m_dwAttackStartTime = dwCurTime;
			m_iCurCombo = iCurCombo;
			pOwner->SetState( CS_ATTACK );

			DWORD dwTrackingTime = dwCurTime;
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}
}

void ioBuffAddItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
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
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_ADD_BUFF:
		break;
	case CS_BLITZ_ATTACK:
		OnBlitzAttack( pOwner );
		break;
	}
}

void ioBuffAddItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	// 은신중 공격
	bool bEnableBlitz = CheckBuffAddState( pOwner );
	if( bEnableBlitz )
	{
		m_bBlitzAttackState = true;
		m_bBlitzBack = CheckBlitzBackTarget( pOwner );
	
		ChangeToBlitzAttack( pOwner, true );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( m_fCurChangeGauge < m_fMaxChangeGauge )
				ChangeToAttackFire( pOwner );
			else
				SetBuffAddState( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioBuffAddItem::OnBlitzAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			return;

		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioBuffAddItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = false;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

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

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBuffAddItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

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
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	else
		pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBuffAddItem::SetBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// change state & motion
	pOwner->SetWereWolfState( m_BuffAddAttribute.m_AttackAnimation, m_BuffAddAttribute.m_fAttackAniRate );

	m_fCurChangeGauge = 0.0f;
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_BuffAddAttribute.m_AttackAnimation );
	float fTimeRate = m_BuffAddAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_BuffAddAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_BuffAddAttribute.m_dwPreDelay );
	pOwner->SetReservedSliding( m_BuffAddAttribute.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vBuffAddList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vBuffAddList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	m_ChargeState = CS_NONE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_ADD_BUFF;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBuffAddItem::ChangeToBlitzAttack( ioBaseChar *pOwner, bool bFirst )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = true;
	pOwner->InitExtendAttackTagInfo();

	if( !m_bBlitzBack )
		m_iMaxBuffAddCombo = m_BlitzAttributeFr.size();
	else
		m_iMaxBuffAddCombo = m_BlitzAttributeBk.size();

	ClearCancelInfo();

	if( pOwner->IsNeedProcess() && bFirst )
	{
		m_iCurBuffAddCombo = 0;
	}

	const AttackAttribute *pAttr = NULL;
	if( COMPARE( m_iCurBuffAddCombo, 0, m_iMaxBuffAddCombo ) )
	{
		if( !m_bBlitzBack )
			pAttr = &m_BlitzAttributeFr[m_iCurBuffAddCombo];
		else
			pAttr = &m_BlitzAttributeBk[m_iCurBuffAddCombo];
	}

	if( pAttr )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		int iAniID = -1;
		float fTimeRate = FLOAT1;
		DWORD dwPreDelay = 0;
		ioEntityGroup *pGrp = pOwner->GetGroup();

		pOwner->SetNormalAttackByAttribute( *pAttr );

		iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
		fTimeRate = pAttr->m_fAttackAniRate;
		dwPreDelay = pAttr->m_dwPreDelay;

		m_szCurAni = pAttr->m_AttackAnimation;
		m_fCurAniRate = pAttr->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		if( m_iMaxBuffAddCombo <= m_iCurBuffAddCombo + 1 )
			pOwner->ApplyNormalAttackResult( 0, true );
		else
			pOwner->ApplyNormalAttackResult( 0 );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( 0, true );
	}

	m_ChargeState = CS_BLITZ_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << bFirst;
		kPacket << m_iCurBuffAddCombo;
		kPacket << m_bBlitzBack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후...
	m_iCurBuffAddCombo++;
}

void ioBuffAddItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( iState )
	{
	case CS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ChangeToAttackFire( pOwner );
		break;
	case CS_ADD_BUFF:
		SetBuffAddState( pOwner );
		break;
	case CS_BLITZ_ATTACK:
		{
			bool bFirst;
			rkPacket >> bFirst;
			if( bFirst )
				m_bBlitzAttackState = true;

			rkPacket >> m_iCurBuffAddCombo;
			rkPacket >> m_bBlitzBack;

			ChangeToBlitzAttack( pOwner, false );
		}
		break;
	}
}

void ioBuffAddItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pOwner = pWeapon->GetOwner();

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		if( pOwner )
		{
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}

	bool bInputLiveTime = false;
	DWORD dwLiveTime = 0;

	ioChargeComboJump2 *pCharComboJump2 = ToChargeComboJump2( GetCurExtendJump() );
	if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboJump2 &&
				pCharComboJump2->GetCurChargeComboState() == ioChargeComboJump2::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboJump2->GetFlightTime() * 10;
			}
		}
	}

	ioChargeComboJump4 *pCharComboJump4 = ToChargeComboJump4( GetCurExtendJump() );
	if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboJump4 &&
				pCharComboJump4->GetCurChargeComboState() == ioChargeComboJump4::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboJump4->GetFlightTime() * 10;
			}
		}
	}

	ioChargeComboDefenseJump *pCharComboDefense = ToChargeComboDefenseJump( GetCurExtendJump() );
	if( pOwner && pOwner->GetState() == CS_JUMP )
	{
		if( pOwner->GetJumpState() == JS_JUMP_ATTACK )
		{
			if( pCharComboDefense &&
				pCharComboDefense->GetCurChargeComboState() == ioChargeComboDefenseJump::CCS_EXTEND_ATTACK )
			{
				bInputLiveTime = true;
				dwLiveTime = pCharComboDefense->GetFlightTime() * 10;
			}
		}
	}

	if( bInputLiveTime )
	{
		pWeapon->SetLiveTime( dwLiveTime );
	}
}

bool ioBuffAddItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioBuffAddItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioBuffAddItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioBuffAddItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioBuffAddItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioBuffAddItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBuffAddItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioBuffAddItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump(  pOwner );
}

bool ioBuffAddItem::CheckBuffAddState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	bool bExist = false;
	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		if( pOwner->HasBuff( m_vBuffAddList[i] ) )
			bExist = true;
	}

	return bExist;
}

void ioBuffAddItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_NONE;

	m_bBlitzAttackState = false;
	m_bBlitzBack = false;
	m_iCurBuffAddCombo = 0;
}

bool ioBuffAddItem::CheckBlitzBackTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return false;

	FD_GrapplingAutoTarget kAutoPred( pOwner, m_fBlitzMinAngle, m_fBlitzMaxAngle, m_fBlitzRange );
	ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
	if( pTarget )
	{
		D3DXQUATERNION qtRot = pTarget->GetTargetRot();
		D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vAttackDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vAttackDir, &vAttackDir );

		float fDotValue = D3DXVec3Dot( &vTargetDir, &vAttackDir );
		if( fDotValue >= 0.0f )	
			return true;
	}

	return false;
}

void ioBuffAddItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( CheckBuffAddState( pOwner ) )
		return;
	
	float fExtraRate = FLOAT1;
	ioBuff* pBuff = pOwner->GetRefudBuff();
	if( pBuff && ToRefudBuff(pBuff) )
	{
		fExtraRate = ToRefudBuff(pBuff)->GetExtraRate();
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurIncrease = m_fIncreaseChangeGauge * fExtraRate * fTimePerSec;

	m_fCurChangeGauge += fCurIncrease;
	m_fCurChangeGauge = (float)max( 0, min( m_fCurChangeGauge, m_fMaxChangeGauge ) );
}

void ioBuffAddItem::ClearBuffAddList( ioBaseChar *pOwner, const ioHashString &szBuffName, bool bAll )
{
	bool bExist = false;
	int iBuffCnt = m_vBuffAddList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		if( szBuffName == m_vBuffAddList[i] )
		{
			bExist = true;
			break;
		}
	}

	if( !bExist )
		return;

	for( int i=0; i < iBuffCnt; ++i )
	{
		if( !bAll && szBuffName == m_vBuffAddList[i] )
			continue;

		pOwner->RemoveBuff( m_vBuffAddList[i] );
	}
}

int ioBuffAddItem::GetMaxBullet()
{
	return (int)m_fMaxChangeGauge;
}

int ioBuffAddItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurChangeGauge;
	fCurCoolTime = max( 0.0f, min( fCurCoolTime, m_fMaxChangeGauge ) );

	return (int)fCurCoolTime;
}

void ioBuffAddItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0.0f, min( fCurBullet, m_fMaxChangeGauge ) );

	m_fCurChangeGauge = fCurBullet;
}
