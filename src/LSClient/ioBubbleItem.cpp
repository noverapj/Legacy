
#include "stdafx.h"

#include "ioBubbleItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioBubbleItem::ioBubbleItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurSpecialAttCnt = 0;
	m_fCurChangeDamage = 0.f;
	m_iRidingDummyCharIndex = 0;
}

ioBubbleItem::ioBubbleItem( const ioBubbleItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_RidingDummyCharName( rhs.m_RidingDummyCharName ),
  m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
  m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),
  m_fMaxChangeDamage( rhs.m_fMaxChangeDamage ),
  m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
  m_fDownDecreaseChangeDamage( rhs.m_fDownDecreaseChangeDamage ),
  m_fUseGaugeDecreaseChangeDamage( rhs.m_fUseGaugeDecreaseChangeDamage ),
  m_fEndJumpPower( rhs.m_fEndJumpPower ),
  m_szReleaseWereWolfAni( rhs.m_szReleaseWereWolfAni ),
  m_fReleaseWereWolfAniRate( rhs.m_fReleaseWereWolfAniRate )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurSpecialAttCnt = 0;
	m_fCurChangeDamage = 0.f;
	m_iRidingDummyCharIndex = 0;
}

ioBubbleItem::~ioBubbleItem()
{
}

void ioBubbleItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// dummy char
	rkLoader.LoadString_e( "riding_dummy_char_name", "", szBuf, MAX_PATH );
	m_RidingDummyCharName = szBuf;

	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );
	
	m_fMaxChangeDamage = rkLoader.LoadFloat_e( "max_change_damage", 0.0f );
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );
	m_fDownDecreaseChangeDamage = rkLoader.LoadFloat_e( "down_decrease_change_damage", 0.0f );
	m_fUseGaugeDecreaseChangeDamage = rkLoader.LoadFloat_e( "use_gauge_decrease_change_damage", 0.0f );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "change_end_jump_power", 1000.0f );

	rkLoader.LoadString_e( "release_werewolf_ani", "", szBuf, MAX_PATH );
	m_szReleaseWereWolfAni = szBuf;
	m_fReleaseWereWolfAniRate = rkLoader.LoadFloat_e( "release_werewolf_ani_rate", 1.f );
}

ioItem* ioBubbleItem::Clone()
{
	return new ioBubbleItem( *this );
}

ioWeaponItem::WeaponSubType ioBubbleItem::GetSubType() const
{
	return WST_BUBBLE_ITEM;
}

void ioBubbleItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

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

void ioBubbleItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioBubbleItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToRiding( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioBubbleItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

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

void ioBubbleItem::ChangeToRiding( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	if( pOwner->CheckChangeWereWolfState( ECT_BUBBLE ) && CheckChangeWereWolfState( pOwner ) )
	{
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		if( !CreateDummyChar( pOwner, iCurIndex ) )
			iCurIndex = 0;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEREWOLF_STATE );
			kPacket << pOwner->GetCharName();
			kPacket << BUBBLE_RIDING_SET;
			kPacket << m_fMaxChangeDamage;
			kPacket << iCurIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		return;
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioBubbleItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner );
		}
		break;
	}
}

void ioBubbleItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioBubbleItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

void ioBubbleItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioBubbleItem::ProcessCancel( ioBaseChar *pOwner )
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
			
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioBubbleItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioBubbleItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioBubbleItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioBubbleItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioBubbleItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( m_bUsedWereWolf )
		return;

	int iState = pOwner->GetState();
	if( iState != CS_DELAY && iState != CS_RUN && iState != CS_DASH )
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		m_fCurExtraGauge = 0.f;
	}
}

void ioBubbleItem::DecreaseChangeDamage( ioBaseChar *pOwner )
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

	if( m_dwChangeWereWolfTime > 0 )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fCurDecrease = 0.0f;

		ioObjectWereWolfItem *pWereWolfObjItem = ToObjectWereWolfItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pWereWolfObjItem && pWereWolfObjItem->IsUseGaugeMode() )
			fCurDecrease = m_fUseGaugeDecreaseChangeDamage * fTimePerSec;
		else if( CheckOwnerDownState(pOwner) && m_fDownDecreaseChangeDamage > 0.0f )
			fCurDecrease = m_fDownDecreaseChangeDamage * fTimePerSec;
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

void ioBubbleItem::ReleseWereWolfState( ioBaseChar *pChar )
{
	ioAttackableItem::ReleseWereWolfState( pChar );

	if( pChar && m_dwWereWolfObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pChar->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
		{
			//애니메이션 초기화
			if( pChar->GetState() == CS_DELAY )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			}
			else if( pChar->GetState() == CS_RUN )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			}
			else if( pChar->GetState() == CS_DASH )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DASH );
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
}

void ioBubbleItem::ReleseWereWolfStateByOwner( ioBaseChar *pChar )
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
	pChar->SetBaseAttackExtraAniJump( iAniID, fTimeRate, 0 );
	pChar->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, 0 );
}

void ioBubbleItem::ReleseWereWolfStateByGauge( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );
	
	if( m_szReleaseWereWolfAni.IsEmpty() )
		return;

	// change state & motion
	pChar->SetWereWolfState( m_szReleaseWereWolfAni, m_fReleaseWereWolfAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szReleaseWereWolfAni );
	float fTimeRate = m_fReleaseWereWolfAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->SetBaseAttackExtraAniJump( iAniID, fTimeRate, 0 );
	pChar->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, 0 );
}

bool ioBubbleItem::CheckChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;
	if( !m_bEnableWereWolf )
		return false;
	if( m_bUsedWereWolf )
		return false;

	if( m_iCurSpecialAttCnt < m_iNeedSpecialAttCnt )
		return false;

	SetChangeWereWolfState( pChar );
	return true;
}

bool ioBubbleItem::IsEnableGauge()
{
	if( m_iCurSpecialAttCnt < m_iNeedSpecialAttCnt )
		return false;

	return true;
}

void ioBubbleItem::SetChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

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

	// create werewolf objectitem
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
	DecreaseExtraNeedGauge();
}

bool ioBubbleItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetHP().m_fCurValue == 0.0f )
		return true;

	return false;
}

void ioBubbleItem::OnReleased( ioBaseChar *pOwner )
{
	ReleseWereWolfState( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioBubbleItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioBubbleItem::ApplyChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

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

	// create werewolf objectitem
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

bool ioBubbleItem::CreateDummyChar( ioBaseChar *pOwner, int iIndex )
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
		m_iRidingDummyCharIndex = iIndex;
		return true;
	}

	return false;
}

void ioBubbleItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bUsedWereWolf )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		break;
	case EGT_ATTACK_DEF:
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWoundDec;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDefDec;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;
}

bool ioBubbleItem::IsRidingState()
{
	if( IsWereWolfState() )
		return true;

	return false;
}

bool ioBubbleItem::IsEnableActionStopDelay()
{
	if( IsRidingState() )
		return false;

	return true;
}

void ioBubbleItem::SetRidingDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
{
	if( !pOwner ) return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
	{
		m_iRidingDummyCharIndex = iCurIndex;
	}
}

bool ioBubbleItem::UsingRidingDummyChar()
{
	if( !m_RidingDummyCharName.IsEmpty() )
		return true;

	return false;
}

bool ioBubbleItem::CheckReleaseWereWolfState( ioBaseChar *pChar )
{
	if( !m_bUsedWereWolf )
	{
		if( pChar && m_dwWereWolfObjectItem > 0 )
		{
			ioObjectItem *pObjectItem = pChar->GetObject();
			if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
				ReleseWereWolfState( pChar );
		}
		return false;
	}
	if( GetCurChangeDamage() <= 0 )
	{
		ReleseWereWolfStateByGauge( pChar );
		return true;
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

void ioBubbleItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	if( pOwner && m_iRidingDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iRidingDummyCharIndex = 0;
	}
}

int ioBubbleItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

int ioBubbleItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

int ioBubbleItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

void ioBubbleItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	iCurBullet = max( 0, min( iCurBullet, m_iMaxSpecialAttCnt ) );
	m_iCurSpecialAttCnt = iCurBullet;
}

void ioBubbleItem::DecreaseExtraNeedGauge()
{
	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		m_fCurExtraGauge = 0.0f;

	m_fCurChangeDamage = m_fMaxChangeDamage;
	m_iCurSpecialAttCnt -= m_iNeedSpecialAttCnt;
	m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );
}