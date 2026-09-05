

#include "stdafx.h"

#include "ioYuMiRaItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioYuMiRaItem::ioYuMiRaItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedGPState = false;
	m_fCurExtraGauge = 0.0f;
	m_dwGPStateEffect = -1;
}

ioYuMiRaItem::ioYuMiRaItem( const ioYuMiRaItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_fNeedGPGauge( rhs.m_fNeedGPGauge ),
  m_ChangeGPStateAttack( rhs.m_ChangeGPStateAttack ),
  m_ReleaseGPStateAttack( rhs.m_ReleaseGPStateAttack ),
  m_vGPStateBuffList( rhs.m_vGPStateBuffList ),
  m_vReleaseGPStateBuffList( rhs.m_vReleaseGPStateBuffList ),
  m_dwGPStateObjectItem( rhs.m_dwGPStateObjectItem ),
  m_GPStateEffect( rhs.m_GPStateEffect )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedGPState = false;
	m_fCurExtraGauge = 0.0f;
	m_dwGPStateEffect = -1;
}

ioYuMiRaItem::~ioYuMiRaItem()
{
}

void ioYuMiRaItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

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

	m_AttackReadyAniList.clear();
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	m_fNeedGPGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );

	//
	wsprintf_e( szBuf, "change_gp_attack" );
	LoadAttackAttribute( szBuf, m_ChangeGPStateAttack, rkLoader );

	wsprintf_e( szBuf, "release_gp_attack" );
	LoadAttackAttribute( szBuf, m_ReleaseGPStateAttack, rkLoader );

	m_vGPStateBuffList.clear();
	m_vReleaseGPStateBuffList.clear();

	int iBuffCnt = rkLoader.LoadInt_e( "gp_buff_cnt", 0 );
	for( int k=0; k < iBuffCnt; ++k )
	{
		wsprintf_e( szKey, "gp_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vGPStateBuffList.push_back( szBuff );
	}

	int iReleaseBuffCnt = rkLoader.LoadInt_e( "release_gp_buff_cnt", 0 );
	for( int l=0; l < iReleaseBuffCnt; ++l )
	{
		wsprintf_e( szKey, "release_gp_buff%d", l+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vReleaseGPStateBuffList.push_back( szBuff );
	}

	m_dwGPStateObjectItem = (DWORD)rkLoader.LoadInt_e( "gp_object_item", 0 );

	rkLoader.LoadString_e( "enable_gp_effect", "", szBuf, MAX_PATH );
	m_GPStateEffect = szBuf;
}

ioItem* ioYuMiRaItem::Clone()
{
	return new ioYuMiRaItem( *this );
}

ioWeaponItem::WeaponSubType ioYuMiRaItem::GetSubType() const
{
	return WST_YUMIRA_ITEM;
}

void ioYuMiRaItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseGPState( pOwner );
	ReleaseEnableGPStateEffect( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioYuMiRaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
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

void ioYuMiRaItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	}
}

void ioYuMiRaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( IsEnableGPState( pOwner ) )
			{
				SetUseGPState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_GP_STATE );
					kPacket << pOwner->GetCharName();
					kPacket << GP_STATE_SET;
					kPacket << m_fCurExtraGauge;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
			else
			{
				ChangeToAttackFire( pOwner, true );
				g_TutorialMgr.ActionChargeAttack();
				return;
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioYuMiRaItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioYuMiRaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToAttackFire( pOwner, bCharged );
}

void ioYuMiRaItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iSubType;
	rkPacket >> iSubType;

	switch( iSubType )
	{
	case ST_SET_GP_EFFECT:
		{
			SetEnableGPStateEffect( pOwner );
		}
		break;
	}
}

void ioYuMiRaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
}

bool ioYuMiRaItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioYuMiRaItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioYuMiRaItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioYuMiRaItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioYuMiRaItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioYuMiRaItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioYuMiRaItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioYuMiRaItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioYuMiRaItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( pOwner->GetHP().m_fCurValue <= 0.0f )
	{
		m_fCurExtraGauge = 0.0f;
		return;
	}
	
	bool bGPState = IsGPState();
	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_YUMIRA )
		{
			if( bGPState )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_YUMIRA )
		{
			if( bGPState )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		if( bGPState )
			fTypeRate = m_fExtraGaugeWoundDec;
		else
			fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		if( bGPState )
			fTypeRate = m_fExtraGaugeWoundDefDec;
		else
			fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	if( bGPState )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

bool ioYuMiRaItem::IsGPState()
{
	return m_bUsedGPState;
}

void ioYuMiRaItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	bool bGPState = IsGPState();

	// 이펙트 체크
	if( !bGPState && m_fCurExtraGauge >= m_fNeedGPGauge &&
		pOwner->IsNeedProcess() )
	{
		SetEnableGPStateEffect( pOwner );
	}

	if( pOwner->GetHP().m_fCurValue <= 0.0f )
	{
		m_fCurExtraGauge = 0.0f;
		ReleaseEnableGPStateEffect( pOwner );
		return;
	}

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( bGPState )
	{
		if( m_fExtraGaugeBaseDec <= 0.0f )
			return;
	}
	else
	{
		if( m_fExtraGaugeBaseInc <= 0.0f )
			return;
	}

	if( pOwner->IsSystemState() )
		return;

	if( bGPState )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	}
	else
	{
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

int ioYuMiRaItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioYuMiRaItem::GetNeedBullet()
{
	return (int)m_fNeedGPGauge;
}

int ioYuMiRaItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioYuMiRaItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

bool ioYuMiRaItem::IsEnableGPState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( m_bUsedGPState )
		return false;

	if( !pOwner->IsEquipEnableObject() )
		return false;

	if( m_fCurExtraGauge < m_fNeedGPGauge )
		return false;

	return true;
}

void ioYuMiRaItem::SetUseGPState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// GP 사용가능 이펙트 제거 필요. 
	ReleaseEnableGPStateEffect( pOwner );

	// change state & motion
	pOwner->SetWereWolfState( m_ChangeGPStateAttack.m_AttackAnimation, m_ChangeGPStateAttack.m_fAttackAniRate );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeGPStateAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeGPStateAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_ChangeGPStateAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChangeGPStateAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	// create werewolf objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwGPStateObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_dwGPStateObjectItem;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwGPStateObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();
				
				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pOwner->EquipItem( pItem );
			}
		}
	}

	m_bUsedGPState = true;
}

void ioYuMiRaItem::ApplyUseGPState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// GP 사용가능 이펙트 제거 필요. 
	ReleaseEnableGPStateEffect( pOwner );

	// change state & motion
	pOwner->SetWereWolfState( m_ChangeGPStateAttack.m_AttackAnimation, m_ChangeGPStateAttack.m_fAttackAniRate );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeGPStateAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeGPStateAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_ChangeGPStateAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetReservedSliding( m_ChangeGPStateAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}

	m_bUsedGPState = true;
}

void ioYuMiRaItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	ReleaseGPState( pOwner, true );
}

void ioYuMiRaItem::ReleaseGPState( ioBaseChar *pOwner, bool bObjReleased )
{
	if( !pOwner ) return;

	// remove buff
	int iBuffCnt = m_vGPStateBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vGPStateBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}

	m_bUsedGPState = false;

	if( bObjReleased )
		return;

	if( pOwner && m_dwGPStateObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwGPStateObjectItem )
		{
			pOwner->ReleaseObjectItem( __FUNCTION__ );
		}
	}
}

void ioYuMiRaItem::SetEnableGPStateEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( pChar && m_dwGPStateEffect == -1 && !m_GPStateEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_GPStateEffect );
		if( pEffect )
		{
			m_dwGPStateEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << ST_SET_GP_EFFECT;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioYuMiRaItem::ReleaseEnableGPStateEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwGPStateEffect != -1 )
	{
		pChar->EndEffect( m_dwGPStateEffect, false );
	}

	m_dwGPStateEffect = -1;
}

bool ioYuMiRaItem::CheckReleaseGPState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;
	
	if( !m_bUsedGPState )
		return false;

	if( GetCurBullet() > 0 && pOwner->GetHP().m_fCurValue > 0.0f )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
		return false;

	m_fCurExtraGauge = 0.0f;
	ReleaseGPState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GP_STATE );
		kPacket << pOwner->GetCharName();
		kPacket << GP_STATE_RELEASE;
		kPacket << m_fCurExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioYuMiRaItem::SetUseExtraGauge( float fAmt )
{
	if( !IsGPState() )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

