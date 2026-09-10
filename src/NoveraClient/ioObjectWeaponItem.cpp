

#include "stdafx.h"

#include "ioObjectWeaponItem.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioObjectWeaponItem::ioObjectWeaponItem()
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

	m_iCurUseCount = 0;

	m_iChargeCombo = 0;
	m_dwStartComboTime = 0;
	m_dwComboAttackTimeEnd = 0;
}

ioObjectWeaponItem::ioObjectWeaponItem( const ioObjectWeaponItem &rhs )
: ioObjectItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_vUseCountAttributeList( rhs.m_vUseCountAttributeList ),
  m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
  m_bEnableObjectDash( rhs.m_bEnableObjectDash ),
  m_dwProtectTime( rhs.m_dwProtectTime ),
  //m_bJumpAttackProtect( rhs.m_bJumpAttackProtect ),
  m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
  m_ChargeComboType( rhs.m_ChargeComboType ),
  bEnableReturnJumpping( rhs.bEnableReturnJumpping )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_iCurUseCount = 0;

	m_iChargeCombo = 0;
	m_dwStartComboTime = 0;
	m_dwComboAttackTimeEnd = 0;
}

ioObjectWeaponItem::~ioObjectWeaponItem()
{
}

void ioObjectWeaponItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	// combo att
	m_ChargeComboType = (ChargeComboType)rkLoader.LoadInt_e( "charge_combo_type", CCT_NORMAL );

	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttList.clear();
	m_vChargeComboAttList.reserve( iComboMax );
	for( int i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}

	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	//
	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );
	m_bEnableObjectDash = rkLoader.LoadBool_e( "enable_object_dash", false );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );
	bEnableReturnJumpping = rkLoader.LoadBool_e( "enable_return_jumpping", false );

	//
	int iUseCount = rkLoader.LoadInt_e( "use_count_attack_cnt", 1 );
	m_vUseCountAttributeList.clear();
	m_vUseCountAttributeList.reserve( iUseCount );
	for( int i=0; i < iUseCount; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "use_count_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vUseCountAttributeList.push_back( kAttribute );
	}
}

ioItem* ioObjectWeaponItem::Clone()
{
	return new ioObjectWeaponItem( *this );
}

void ioObjectWeaponItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	m_dwEquipTime = FRAMEGETTIME();
	//m_bSetJumpAttack = false;
	m_iCurUseCount = 0;
}

ioObjectItem::ObjectSubType ioObjectWeaponItem::GetObjectSubType() const
{
	return OST_CONTACT_WEAPON;
}

void ioObjectWeaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	m_iChargeCombo = 0;
	m_dwComboAttackTimeEnd = 0;

	m_bCharged = false;

	int iExtendCnt = m_AttackReadyAniList.size();
	int iUseAttackCnt = m_vUseCountAttributeList.size();
	if( iExtendCnt == 0 || !COMPARE( iCurCombo, 0, iExtendCnt ) )
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

		DWORD dwTrackingTime = FRAMEGETTIME();

		if( m_ChargeComboType == CS_NONE && 0 < iUseAttackCnt )
		{
			//UseAttackCount do not use
		}
		else
		{
			int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
			if ( iAniID == -1 )
				return;

			float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		}

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	}
}

void ioObjectWeaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
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
	case CS_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				OnComboState( pOwner );
			}

			if( pOwner && pOwner->GetState() == CS_ATTACK )
			{
				ioWeaponItem::CheckNormalAttack( pOwner );
			}
		}
		break;
	}
}

void ioObjectWeaponItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			switch( m_ChargeComboType )
			{
			case CCT_NONE:
				ChangeToAttackFire( pOwner, false );
				break;
			case CCT_NORMAL:
				ChangeToAttackFire( pOwner, true );
				break;
			case CCT_COMBO:
				ChangeToComboState( pOwner );
				break;
			}

			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioObjectWeaponItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		if( m_ChargeComboType == CS_NONE && m_iCurUseCount < (int)m_vUseCountAttributeList.size() )
		{
			const AttackAttribute& rkAttribute = m_vUseCountAttributeList[m_iCurUseCount];
			pOwner->SetNormalAttackByAttribute( rkAttribute );
		}
		else
		{
			const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
			if( pAttribute )
			{
				pOwner->SetNormalAttackByAttribute( *pAttribute );
			}
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;
	//CheckActiveCount( pOwner );

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

void ioObjectWeaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> m_iCurCombo;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_COMBO:
		{
			rkPacket >> m_iChargeCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	}
}

void ioObjectWeaponItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	rkPacket >> m_iChargeCombo;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true );

	SetNextComboState( pOwner );

	pOwner->ApplyAutoTargetInfo( rkPacket );
}

void ioObjectWeaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	ioGraffitiWeapon *pGraffiti = ToGraffitiWeapon( pWeapon );
	ioZoneMoveWeapon *pMove = ToZoneMoveWeapon( pWeapon );

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
	else if( pGraffiti )
	{
		g_WeaponMgr.CheckMaxGraffitiWeaponCnt( pWeapon, GetGraffitiWeaponActionMax() );
	}
	else if( pMove )
	{
		ioBaseChar *pChar = pMove->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3	vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pWeapon->SetMoveDir( vDir );
		}
	}

	if( m_pExtendJump )
	{
		ioDoubleJump2 *pDoubleJump2 = ToDoubleJump2( m_pExtendJump );
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pDoubleJump2 )
		{
			D3DXVECTOR3 vDir = pDoubleJump2->GetFireDir( pWeapon->GetOwner() );
			if( vDir == ioMath::VEC3_ZERO )
				vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			pWeapon->SetMoveDir( vDir );
		}
	}

	//if( m_bSetJumpAttack )
	//	m_bSetJumpAttack = false;
}

bool ioObjectWeaponItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioObjectWeaponItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioObjectWeaponItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioObjectWeaponItem::ProcessCancel( ioBaseChar *pOwner )
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
			
			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
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

		++iter;
	}

	return false;
}

bool ioObjectWeaponItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectWeaponItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectWeaponItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	//m_bSetJumpAttack = true;

	fTimeRate   = pAttr->m_fAttackAniRate;

	SetAirJump( pOwner, iAniID, fTimeRate );

	//CheckActiveCount( pOwner );

	return false;
}

void ioObjectWeaponItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioObjectWeaponItem::IsProtectState()
{
	if( m_dwProtectTime > 0 && m_dwEquipTime + m_dwProtectTime > FRAMEGETTIME() )
	{
		return true;
	}

	//if( m_bJumpAttackProtect && m_bSetJumpAttack )
	//{
	//	return true;
	//}

	return false;
}

bool ioObjectWeaponItem::IsOneTimeWeaponItem() const
{
	if( m_iMaxUseCount > 0 &&  m_iCurUseCount == m_iMaxUseCount )
		return true;

	return false;
}

bool ioObjectWeaponItem::IsEnableDash() const
{
	return m_bEnableObjectDash;
}

bool ioObjectWeaponItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

void ioObjectWeaponItem::CheckAttackCount( ioBaseChar *pOwner )
{
	m_iCurUseCount++;
	m_iCurUseCount = min( m_iCurUseCount, m_iMaxUseCount );

	CheckActiveCount( pOwner );
}

void ioObjectWeaponItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iChargeCombo = 0;
	m_dwStartComboTime = dwCurTime;
	
	SetNextComboState( pOwner );
	pOwner->ApplyNormalAttackResult( m_iChargeCombo, true );

	m_ChargeState = CS_COMBO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send ÈÄ
	m_iChargeCombo++;
}

void ioObjectWeaponItem::SetNextComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iExtendCnt = m_vChargeComboAttList.size();
	if( COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		m_szCurAni = m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation;
		m_fCurAniRate = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vChargeComboAttList[m_iChargeCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation );
		float fTimeRate  = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vChargeComboAttList[m_iChargeCombo].m_dwPreDelay;
		DWORD dwEndDelay = m_vChargeComboAttList[m_iChargeCombo].m_dwEndDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay + dwEndDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
		{
			m_dwKeyReserveTime += dwCurTime;
		}

		//
		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioObjectWeaponItem::OnComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );

		if( m_iChargeCombo == (int)m_vChargeComboAttList.size() )
			return;

		if( m_dwStartComboTime+m_dwMaxComboDuration > dwCurTime )
		{
			if( pOwner->IsAttackKey() )
			{
				SetNextComboState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << m_iChargeCombo;
					kPacket << pOwner->GetTargetRot();
					pOwner->FillAutoTargetInfo( kPacket );
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				m_iChargeCombo++;
			}
			else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
			{
				m_iSkillInput = -1;
				return;
			}
		}
	}
}

