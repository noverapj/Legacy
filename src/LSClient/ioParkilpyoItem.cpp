
#include "stdafx.h"

#include "ioParkilpyoItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioParkilpyoItem::ioParkilpyoItem()
{
	Init();
}

ioParkilpyoItem::ioParkilpyoItem( const ioParkilpyoItem &rhs )
	: ioWeaponItem( rhs ),
	  m_fNeedGauge( rhs.m_fNeedGauge ),  
	  m_dwObjectItem( rhs.m_dwObjectItem ),
	  m_szPreGatherAniList( rhs.m_szPreGatherAniList ),
	  m_szGatherAni( rhs.m_szGatherAni ),
	  m_fGatherAniRate( rhs.m_fGatherAniRate ),
	  m_EquipAttribute( rhs.m_EquipAttribute ),
	  m_JumpEquipAttribute( rhs.m_JumpEquipAttribute ),
	  m_DashEquipAttribute( rhs.m_DashEquipAttribute ),
	  m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	  m_ChargeComboList( rhs.m_ChargeComboList ),
	  m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
	  m_dwInputGapTime( rhs.m_dwInputGapTime ),
	  m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
	  m_szEquipCmd( rhs.m_szEquipCmd )

{
	Init();
}

ioParkilpyoItem::~ioParkilpyoItem()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	m_dwObjectItem = static_cast<DWORD>( rkLoader.LoadInt_e( "object_item", 0 ) );
	
	int nPreCount = rkLoader.LoadInt_e( "pre_gather_animation_cnt", 0 );
	for( int i = 0; i<nPreCount; i++ )
	{
		wsprintf_e( szKey, "pre_gather_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szPreGatherAniList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_szGatherAni = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gather_animation_rate", FLOAT1 );

	LoadAttackAttribute( "equip", m_EquipAttribute, rkLoader );
	LoadAttackAttribute( "jump_equip", m_JumpEquipAttribute, rkLoader );
	LoadAttackAttribute( "dash_equip", m_DashEquipAttribute, rkLoader );
	LoadAttackAttribute( "charge_attack", m_ChargeAttackAttribute, rkLoader );
	
	m_ChargeComboList.clear();
	int nMaxChargeComboCnt = rkLoader.LoadInt_e( "charge_combo_cnt", 0 );
	for ( int i=0; i<nMaxChargeComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "charge_combo%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_ChargeComboList.push_back( sAttackAttribute );
	}

	m_dwInputMaxTime = rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputGapTime = rkLoader.LoadInt_e( "input_gap_time", 0 );
	m_dwInputStartGapTime = rkLoader.LoadInt_e( "input_start_gap_time", 0 );

	rkLoader.LoadString_e( "equip_cmd", "", szBuf, MAX_PATH );
	m_szEquipCmd = szBuf;
}

ioItem* ioParkilpyoItem::Clone()
{
	return new ioParkilpyoItem( *this );
}

ioWeaponItem::WeaponSubType ioParkilpyoItem::GetSubType() const
{
	return WST_PARKILPYO_ITEM;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ioParkilpyoItem::GetAttackStateText() const
{
	switch( m_ChargeState )
	{
	case CS_NONE:
		return "CS_NONE";
	case CS_CHARGING:
		return "CS_CHARGING";
	case CS_GATHERING:
		return "CS_GATHERING";
	case CS_ATTACK_FIRE:
		return "CS_ATTACK_FIRE";
	case CS_CHARGE_ATTACK:
		return "CS_CHARGE_ATTACK";
	case CS_CHARGE_COMBO:
		return "CS_CHARGE_COMBO";
	case CS_EQUIP_OBJECT:
		return "CS_EQUIP_OBJECT";
	case CS_END:
		return "CS_END";
	}

	return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();	
	ClearCmdInfo();
	
	m_ChargeState		= CS_CHARGING;
	m_dwAttackStartTime	= FRAMEGETTIME();
	m_nCurCombo			= iCurCombo;

	pOwner->SetState( CS_ATTACK );
	pOwner->SetChargingState( true );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iPreGatherCnt = m_szPreGatherAniList.size();

	ioHashString szPreAni;
	if( 0 < iPreGatherCnt && COMPARE( iCurCombo, 0, iPreGatherCnt ) )
	{
		szPreAni = m_szPreGatherAniList[iCurCombo];
		
		int iAniID = pGrp->GetAnimationIdx( szPreAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
}

void ioParkilpyoItem::CheckNormalAttack( ioBaseChar *pOwner )
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
	case CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;
				
				if( CheckCmdInputState( pOwner ) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_CHARGE_ATTACK:
	case CS_CHARGE_COMBO:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );
				if( ProcessCancel(pOwner) )
					return;

				if( CheckCmdInputState( pOwner ) )
					return;
			}

			if( m_dwMotionEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, false );
				return;
			}

			if ( CheckChargeComboInput( pOwner) )
			{
				ChangeToChargeComboAttack( pOwner );
				return;
			}
		}
		break;
	}
}

bool ioParkilpyoItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "%s - AttackAttribute Not Exist", __FUNCTION__ );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - JumpAttack Animation Not Exist", __FUNCTION__ );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioParkilpyoItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioParkilpyoItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	bool bGPState = m_bEquipObjectItem;
	
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioParkilpyoItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioParkilpyoItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;

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

void ioParkilpyoItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseObject( pOwner );

	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioParkilpyoItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	m_bEquipObjectItem = false;
	m_ChargeState = CS_NONE;
}

void ioParkilpyoItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	byte eState;
	rkPacket >> eState;

	switch( eState )
	{
	case ST_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurCombo;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToAttackFire( pOwner );
		}
		break;
	case ST_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case ST_OBJECT_EQUIP:
		{
			byte eObjectState;
			rkPacket >> eObjectState;
			ChangeEquipObjectState( pOwner, static_cast<EquipObjectState>( eObjectState ) );
		}
		break;
	case ST_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRot( qtRot );
			ChangeToChargeAttack( pOwner );
		}
		break;
	case ST_CHARGE_COMBO:
		{
			int nCurKey;
			rkPacket >> nCurKey;
			rkPacket >> m_nCurChargeCombo;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			ChangeToChargeComboAttack( pOwner );
		}
		break;
	}	
}

void ioParkilpyoItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	if( m_ChargeState == CS_EQUIP_OBJECT )
		ReleaseObject( pOwner );
	else
		m_ChargeState = CS_NONE;
}

void ioParkilpyoItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	bool bGPState = m_bEquipObjectItem;
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
	if ( bGPState )
		fCurGauge = -fCurGauge;

	m_fCurExtraGauge += fCurGauge;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioParkilpyoItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioParkilpyoItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioParkilpyoItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioParkilpyoItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::Init()
{
	m_ChargeState				= CS_NONE;

	m_dwAttackStartTime			= 0;
	m_nCurCombo					= 0;
	m_iCurAirJump				= 0;

	m_fAirJumpPowerRate			= 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_bEquipObjectItem = false;
	m_dwMotionEndTime = 0;
	m_dwEnableKeyTime = 0;
	m_nCurChargeCombo = 0;

	m_nCurCmdIndex = 0;

	ClearCmdInfo();
}

void ioParkilpyoItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
		ChangeToAttackFire( pOwner );
}

void ioParkilpyoItem::OnGathering( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	DWORD dwCurTime	= FRAMEGETTIME();

	if( ( m_fCurExtraGauge >= m_fNeedGauge && pOwner->IsAttackKeyDown() ) && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime < dwCurTime )
			ChangeEquipObjectState( pOwner );
	}
	else
	{
		ChangeToChargeAttack( pOwner );
	}
}

void ioParkilpyoItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_nCurCombo );
	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_ATTACK );
		kPacket << m_nCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioParkilpyoItem::ChangeToChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( m_ChargeAttackAttribute.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = m_ChargeAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_ChargeAttackAttribute.m_dwPreDelay;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( m_ChargeAttackAttribute.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	m_dwEnableKeyTime = dwCurTime;
	m_dwEnableKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;

	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_nCurChargeCombo = 0;

	m_ChargeState = CS_CHARGE_ATTACK;

	SetCurAttackAniForInputJump( m_ChargeAttackAttribute.m_AttackAnimation, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_CHARGE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioParkilpyoItem::ChangeToChargeComboAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	if ( !COMPARE( m_nCurChargeCombo, 0, (int)m_ChargeComboList.size() ) )
		return;

	pOwner->InitActionStopDelay( true );

	const AttackAttribute &sChargeCombo = m_ChargeComboList[m_nCurChargeCombo];

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pGrp->GetAnimationIdx( sChargeCombo.m_AttackAnimation );
	if ( nAniID == -1 )
		return;

	float fTimeRate  = sChargeCombo.m_fAttackAniRate;
	DWORD dwPreDelay = sChargeCombo.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->ClearReservedSliding();
	pOwner->SetReservedSliding( sChargeCombo.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, sChargeCombo.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	m_dwEnableKeyTime = dwCurTime;
	m_dwEnableKeyTime += pGrp->GetAnimationEventTime( nAniID, "key_reserve" ) * fTimeRate;
	
	m_ChargeState = CS_CHARGE_COMBO;

	SetCurAttackAniForInputJump( sChargeCombo.m_AttackAnimation, fTimeRate );

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << static_cast<byte>( ST_CHARGE_COMBO );
			kPacket << (int)eNewDirKey;
			kPacket << m_nCurChargeCombo;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_nCurChargeCombo++;
}

void ioParkilpyoItem::ChangeToGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szGatherAni );
	float fTimeRate = m_fGatherAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_ChargeState = CS_GATHERING;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( ST_GATHERING );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioParkilpyoItem::ChangeToEquipObject( ioBaseChar* pOwner, EquipObjectState eState )
{
	DWORD dwCurTime = FRAMEGETTIME();

	AttackAttribute* pAttribute = NULL;
	switch( eState )	
	{
	case EOS_NORMAL:
		{
			pAttribute = &m_EquipAttribute;	
		}
		break;
	case EOS_JUMP:
		{
			pAttribute = &m_JumpEquipAttribute;

			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );
		}
		break;
	case EOS_DASH:
		{	
			pAttribute = &m_DashEquipAttribute;
		}
		break;
	}

	if( !pAttribute )
		return false;

	m_dwInputCancelCheckTime	= dwCurTime;
	m_dwCancelCheckTime			= dwCurTime;

	pOwner->SetObjectEquipState( pAttribute );

	return true;
}

bool ioParkilpyoItem::CheckChargeComboInput( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwEnableKeyTime > 0 && m_dwEnableKeyTime < dwCurTime && pOwner->IsAttackKey() )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::ChangeEquipObjectState( ioBaseChar *pOwner, EquipObjectState eState )
{
	if( !pOwner )
		return;

	if( !ChangeToEquipObject( pOwner, eState ) )
	{
		ChangeToChargeAttack( pOwner );
		return;
	}

	m_ChargeState = CS_EQUIP_OBJECT;
	m_bEquipObjectItem = true;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();

	if( pStage && m_dwObjectItem > 0 )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			pOwner->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kUdpPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kUdpPacket );
			kUdpPacket << static_cast<byte>( ST_OBJECT_EQUIP );
			kUdpPacket << static_cast<byte>( eState );
			P2PNetwork::SendToAllPlayingUser( kUdpPacket );

			//아래 패킷으로 ioAttackableItem::EquipObjectItemByAttakable()가 호출됨
			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_dwObjectItem;
			kPacket << GetName();
			kPacket << pOwner->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );			
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
			ioObjectItem *pObject = ToObjectItem( pItem );
			if( pObject )
			{
				pObject->ReLoadProperty();
				EquipObjectItemByAttakable( pOwner, pObject );
								
				pObject->SetObjectCreateItem( GetName() );
				pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				pOwner->EquipItem( pObject );
			}
		}
	}
}

bool ioParkilpyoItem::IsEnableEquipObject()
{
	if ( m_ChargeState == CS_EQUIP_OBJECT )
		return false;

	if ( m_bEquipObjectItem )
		return false;

	if ( m_fCurExtraGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioParkilpyoItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
			return true;
	}

	return false;
}

void ioParkilpyoItem::ReleaseObject( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	if( pOwner && m_dwObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pOwner->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwObjectItem )
		{
			//애니메이션 초기화
			if( pOwner->GetState() == CS_DELAY )
			{
				pOwner->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			}
			else if( pOwner->GetState() == CS_RUN )
			{
				pOwner->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			}
			else if( pOwner->GetState() == CS_DASH )
			{
				pOwner->ReleaseObjectItem( __FUNCTION__, CS_DASH );
			}
			else
			{
				pOwner->ReleaseObjectItem( __FUNCTION__ );
			}

			m_bEquipObjectItem = false;
			m_ChargeState = CS_NONE;			
		}
	}
}

void ioParkilpyoItem::EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem )
{	
	ioObjectItem *pObject = ToObjectItem( pObjectItem );
	if( !pObject )
		return;

	if( m_dwObjectItem != pObject->GetItemCode() )
		return;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::FillParkilpyoInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	rkPacket << static_cast<BYTE>( m_ChargeState );
	rkPacket << m_fCurExtraGauge;
}

void ioParkilpyoItem::SetParkilpyoInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	BYTE eState = 0;
	float fGauge = 0.0f;

	rkPacket >> eState;
	rkPacket >> fGauge;

	m_ChargeState = static_cast<ChargeState>( eState );
	SetCurExtraGauge( fGauge );

	ChangeEquipObjectState( pOwner, EOS_NONE );

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && eState == CS_EQUIP_OBJECT )
	{
		ioItem *pItem = pStage->CreateItem( m_dwObjectItem );
		ioObjectItem *pObject = ToObjectItem( pItem );
		if( pObject )
		{
			pObject->ReLoadProperty();
			EquipObjectItemByAttakable( pOwner, pObject );
			
			pObject->SetObjectCreateItem( GetName() );
			pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
			pOwner->EquipItem( pObject );
		}
	}
}

bool ioParkilpyoItem::CheckReleaseObjectItem( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( !pOwner->IsNeedProcess() )
		return false;
	
	if ( !m_bEquipObjectItem )
		return false;

	if( GetCurBullet() > 0 )
		return false;

	switch ( pOwner->GetState() )
	{
	case CS_ATTACK:
	case CS_JUMP:
	case CS_EXTEND_DASH:
	case CS_OBJECT_EQUIP:
			return false;
		return false;
	}

	ReleaseObject( pOwner );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioParkilpyoItem::ClearCmdInfo()
{
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_nCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );
	
	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;	
}

void ioParkilpyoItem::ClearCmdTime()
{
	m_dwInputStartTime = 0;
}

void ioParkilpyoItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsChatModeState( false ) )
		return;
	
	ClearCmdInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwInputStartTime = dwCurTime;
	m_dwInputCheckTime = dwCurTime+m_dwInputStartGapTime;
}

void ioParkilpyoItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdEquip( pOwner );
}

bool ioParkilpyoItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if ( pOwner->IsHasCrown() )
		return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;

	if( CheckCmdInputTime() )
		return ChangeToCmdEquip( pOwner );

	CheckCmdInput( pOwner );

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioParkilpyoItem::ChangeToCmdEquip( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();
	
	if( strcmp( m_szEquipCmd.c_str(), m_CurCmdInfo ) == 0 )
	{			
		ClearCmdInfo();

		if( IsEnableEquipObject() )
		{
			CharState eState = pOwner->GetState();
			EquipObjectState eEquipState = EOS_NORMAL;
			switch ( eState )
			{
			case CS_DASH:
			case CS_EXTEND_DASH:
				eEquipState = EOS_DASH;
				break;
			case CS_JUMP:
				eEquipState = EOS_JUMP;
			}

			ChangeEquipObjectState( pOwner, eEquipState );
			return true;
		}
	}

	ClearCmdInfo();

	return false;
}

bool ioParkilpyoItem::CheckCmdInputTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwInputStartTime+m_dwInputMaxTime < dwCurTime )
		return true;

	if( m_dwInputCheckTime < dwCurTime )
		return true;

	return false;
}

bool ioParkilpyoItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	int iCurMaxCmd = MAX_COMMAND_CNT - 1;
	iCurMaxCmd = max( 0, iCurMaxCmd );
	DWORD dwCurTime = FRAMEGETTIME();

	ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
	if( m_PreDirKey == eDirKey )
		return true;

	char szChar = '\0';

	switch( eDirKey )
	{
	case ioUserKeyInput::DKI_NONE:
		m_PreDirKey = eDirKey;
		return true;
	case ioUserKeyInput::DKI_RIGHTUP:
	case ioUserKeyInput::DKI_RIGHTDOWN:
	case ioUserKeyInput::DKI_LEFTDOWN:
	case ioUserKeyInput::DKI_LEFTUP:
		return true;
	case ioUserKeyInput::DKI_UP:
		szChar = '1';
		break;
	case ioUserKeyInput::DKI_RIGHT:
		szChar = '2';
		break;
	case ioUserKeyInput::DKI_DOWN:
		szChar = '3';
		break;
	case ioUserKeyInput::DKI_LEFT:
		szChar = '4';
		break;
	}

	if( !COMPARE( m_nCurCmdIndex, 0, iCurMaxCmd ) )
		return false;

	m_CurCmdInfo[m_nCurCmdIndex] = szChar;

	m_PreDirKey = eDirKey;
	m_nCurCmdIndex++;
	m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

	return true;
}