
#include "stdafx.h"

#include "ioSasinmuItem.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

#include "ioIljimaeJump.h"

ioSasinmuItem::ioSasinmuItem()
{	
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	Init();
}

ioSasinmuItem::ioSasinmuItem( const ioSasinmuItem &rhs )
	: ioWeaponItem( rhs ),
	m_fNeedGauge( rhs.m_fNeedGauge ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_szWaitPreDelayAni( rhs.m_szWaitPreDelayAni ),
	m_fWaitPreDelayAniRate( rhs.m_fWaitPreDelayAniRate ),
	m_szWaitLoopAni( rhs.m_szWaitLoopAni ),
	m_szWaitMoveAni( rhs.m_szWaitMoveAni ),
	m_fWaitMoveAniRate( rhs.m_fWaitMoveAniRate ),
	m_nMaxSpecialComboCnt( rhs.m_nMaxSpecialComboCnt ),
	m_SpecialChargeAttack( rhs.m_SpecialChargeAttack ),
	m_SpecialComboAttackList( rhs.m_SpecialComboAttackList ),
	m_SpecialCmdInfo( rhs.m_SpecialCmdInfo ),
	m_SpecialAttackInfo( rhs.m_SpecialAttackInfo ),
	m_SpecialDefenseInfo( rhs.m_SpecialDefenseInfo ),
	m_SpecialJumpInfo( rhs.m_SpecialJumpInfo ),
	m_SpecialDashInfo( rhs.m_SpecialDashInfo ),
	m_SpecialChargeDashInfo( rhs.m_SpecialChargeDashInfo ),
	m_fSpecialCmdGauge( rhs.m_fSpecialCmdGauge ),
	m_fSpecialAttackGauge( rhs.m_fSpecialAttackGauge ),
	m_fSpecialDefenseGauge( rhs.m_fSpecialDefenseGauge ),
	m_fSpecialJumpGauge( rhs.m_fSpecialJumpGauge ),
	m_fSpecialDashGauge( rhs.m_fSpecialDashGauge ),
	m_fSpecialChargeDashGauge( rhs.m_fSpecialChargeDashGauge ),
	m_PowerUpSpecialChargeAttack( rhs.m_PowerUpSpecialChargeAttack ),
	m_PowerUpSpecialComboAttackList( rhs.m_PowerUpSpecialComboAttackList ),
	m_PowerUpSpecialCmdInfo( rhs.m_PowerUpSpecialCmdInfo ),
	m_PowerUpSpecialAttackInfo( rhs.m_PowerUpSpecialAttackInfo ),
	m_PowerUpSpecialDefenseInfo( rhs.m_PowerUpSpecialDefenseInfo ),
	m_PowerUpSpecialJumpInfo( rhs.m_PowerUpSpecialJumpInfo ),
	m_PowerUpSpecialDashInfo( rhs.m_PowerUpSpecialDashInfo ),
	m_PowerUpSpecialChargeDashInfo( rhs.m_PowerUpSpecialChargeDashInfo ),
	m_dwInputGapTime( rhs.m_dwInputGapTime ),
	m_szSpecialCmd( rhs.m_szSpecialCmd ),
	m_bUseDefenceCounterAttack( rhs.m_bUseDefenceCounterAttack ),
	m_fSpecialJumpMoveAngle( rhs.m_fSpecialJumpMoveAngle ),
	m_fSpecialJumpMoveSpeed( rhs.m_fSpecialJumpMoveSpeed ),
	m_fSpecialJumpEndJumpPower( rhs.m_fSpecialJumpEndJumpPower ),
	m_fPowerUpSpecialJumpMoveAngle( rhs.m_fPowerUpSpecialJumpMoveAngle ),
	m_fPowerUpSpecialJumpMoveSpeed( rhs.m_fPowerUpSpecialJumpMoveSpeed ),
	m_fPowerUpSpecialJumpEndJumpPower( rhs.m_fPowerUpSpecialJumpEndJumpPower ),
	m_fSpecialChargeDashMoveAngle( rhs.m_fSpecialChargeDashMoveAngle ),
	m_fSpecialChargeDashMoveSpeed( rhs.m_fSpecialChargeDashMoveSpeed ),
	m_fSpecialChargeDashEndJumpPower( rhs.m_fSpecialChargeDashEndJumpPower ),
	m_fPowerUpSpecialChargeDashMoveAngle( rhs.m_fPowerUpSpecialChargeDashMoveAngle ),
	m_fPowerUpSpecialChargeDashMoveSpeed( rhs.m_fPowerUpSpecialChargeDashMoveSpeed ),
	m_fPowerUpSpecialChargeDashEndJumpPower( rhs.m_fPowerUpSpecialChargeDashEndJumpPower ),
	m_SpecialDefenseBuff( rhs.m_SpecialDefenseBuff ),
	m_PowerUpSpecialDefenseBuff( rhs.m_PowerUpSpecialDefenseBuff ),
	m_PowerUpBuff( rhs.m_PowerUpBuff ),
	m_PowerDownBuff( rhs.m_PowerDownBuff )
{
	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_fCurExtraGauge = 0.0f;
	m_bPowerUpMode = false;

	Init();
}

ioSasinmuItem::~ioSasinmuItem()
{
}

void ioSasinmuItem::LoadProperty( ioINILoader &rkLoader )
{
	//기본 파일 읽기
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_extra_gauge", FLOAT100 );

	//공격 준비 애니메이션
	m_AttackReadyAniList.clear();
	int nMaxCombo = GetMaxCombo();
	for( int i=0; i<nMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}
	
	rkLoader.LoadString_e( "wait_pre_delay_ani", "", szBuf, MAX_PATH );
	m_szWaitPreDelayAni = szBuf;
	m_fWaitPreDelayAniRate = rkLoader.LoadFloat_e( "wait_pre_delay_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "wait_loop_ani", "", szBuf, MAX_PATH );
	m_szWaitLoopAni = szBuf;
	rkLoader.LoadString_e( "wait_move_ani", "", szBuf, MAX_PATH );
	m_szWaitMoveAni = szBuf;
	m_fWaitMoveAniRate = rkLoader.LoadFloat_e( "wait_move_ani_rate", FLOAT1 );

	LoadAttackAttribute( "special_charge_attack", m_SpecialChargeAttack, rkLoader );

	m_nMaxSpecialComboCnt = rkLoader.LoadInt_e( "max_special_combo_attack", 0 );
	m_SpecialComboAttackList.clear();
	for ( int i=0; i<m_nMaxSpecialComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "special_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_SpecialComboAttackList.push_back( sAttackAttribute );
	}

	LoadAttackAttribute( "special_cmd_info", m_SpecialCmdInfo, rkLoader );
	LoadAttackAttribute( "special_attack_info", m_SpecialAttackInfo, rkLoader );
	LoadAttackAttribute( "special_defense_info", m_SpecialDefenseInfo, rkLoader );
	LoadAttackAttribute( "special_jump_info", m_SpecialJumpInfo, rkLoader );
	LoadAttackAttribute( "special_dash_info", m_SpecialDashInfo, rkLoader );
	LoadAttackAttribute( "special_charge_dash_info", m_SpecialChargeDashInfo, rkLoader );

	m_fSpecialCmdGauge = rkLoader.LoadFloat_e( "special_cmd_gauge", 0.0f );
	m_fSpecialAttackGauge = rkLoader.LoadFloat_e( "special_attack_gauge", 0.0f );
	m_fSpecialDefenseGauge = rkLoader.LoadFloat_e( "special_defense_gauge", 0.0f );
	m_fSpecialJumpGauge = rkLoader.LoadFloat_e( "special_jump_gauge", 0.0f );
	m_fSpecialDashGauge = rkLoader.LoadFloat_e( "special_dash_gauge", 0.0f );
	m_fSpecialChargeDashGauge = rkLoader.LoadFloat_e( "special_charge_dash_gauge", 0.0f );

	LoadAttackAttribute( "power_up_special_charge_attack", m_PowerUpSpecialChargeAttack, rkLoader );

	m_PowerUpSpecialComboAttackList.clear();
	for ( int i=0; i<m_nMaxSpecialComboCnt; i++ )
	{
		AttackAttribute sAttackAttribute;
		wsprintf_e( szKey, "power_up_special_combo_attack%d", i+1 );
		LoadAttackAttribute( szKey, sAttackAttribute, rkLoader );
		m_PowerUpSpecialComboAttackList.push_back( sAttackAttribute );
	}

	LoadAttackAttribute( "power_up_special_cmd_info", m_PowerUpSpecialCmdInfo, rkLoader );
	LoadAttackAttribute( "power_up_special_attack_info", m_PowerUpSpecialAttackInfo, rkLoader );
	LoadAttackAttribute( "power_up_special_defense_info", m_PowerUpSpecialDefenseInfo, rkLoader );
	LoadAttackAttribute( "power_up_special_jump_info", m_PowerUpSpecialJumpInfo, rkLoader );
	LoadAttackAttribute( "power_up_special_dash_info", m_PowerUpSpecialDashInfo, rkLoader );
	LoadAttackAttribute( "power_up_special_charge_dash_info", m_PowerUpSpecialChargeDashInfo, rkLoader );

	m_dwInputGapTime = rkLoader.LoadInt_e( "input_gap_time", 0 );

	rkLoader.LoadString_e( "special_cmd", "", szBuf, MAX_PATH );
	m_szSpecialCmd = szBuf;

	m_bUseDefenceCounterAttack = rkLoader.LoadBool_e( "enable_defence_counter_attack", false );

	m_fSpecialJumpMoveAngle = rkLoader.LoadFloat_e( "special_jump_move_angle", 0.0f );
	m_fSpecialJumpMoveSpeed = rkLoader.LoadFloat_e( "special_jump_move_speed", 0.0f );
	m_fSpecialJumpEndJumpPower = rkLoader.LoadFloat_e( "special_jump_end_jump_power", 0.0f );

	m_fPowerUpSpecialJumpMoveAngle = rkLoader.LoadFloat_e( "power_up_special_jump_move_angle", 0.0f );
	m_fPowerUpSpecialJumpMoveSpeed = rkLoader.LoadFloat_e( "power_up_special_jump_move_speed", 0.0f );
	m_fPowerUpSpecialJumpEndJumpPower = rkLoader.LoadFloat_e( "power_up_special_jump_end_jump_power", 0.0f );

	m_fSpecialChargeDashMoveAngle = rkLoader.LoadFloat_e( "special_charge_dash_move_angle", 0.0f );
	m_fSpecialChargeDashMoveSpeed = rkLoader.LoadFloat_e( "special_charge_dash_move_speed", 0.0f );
	m_fSpecialChargeDashEndJumpPower = rkLoader.LoadFloat_e( "special_charge_dash_end_jump_power", 0.0f );

	m_fPowerUpSpecialChargeDashMoveAngle = rkLoader.LoadFloat_e( "power_up_special_charge_dash_move_angle", 0.0f );
	m_fPowerUpSpecialChargeDashMoveSpeed = rkLoader.LoadFloat_e( "power_up_special_charge_dash_move_speed", 0.0f );
	m_fPowerUpSpecialChargeDashEndJumpPower = rkLoader.LoadFloat_e( "power_up_special_charge_dash_end_jump_power", 0.0f );

	LoadBuff( rkLoader, "special_defense", m_SpecialDefenseBuff );
	LoadBuff( rkLoader, "power_up_special_defense", m_PowerUpSpecialDefenseBuff );
	LoadBuff( rkLoader, "power_up", m_PowerUpBuff );
	LoadBuff( rkLoader, "power_down", m_PowerDownBuff );
}

ioItem* ioSasinmuItem::Clone()
{
	return new ioSasinmuItem( *this );
}

ioWeaponItem::WeaponSubType ioSasinmuItem::GetSubType() const
{
	return WST_SASINMU;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetNormalAttack( ioBaseChar *pOwner, int nCurCombo )
{
	ClearCancelInfo();

	Init();

	DWORD dwCurTime = FRAMEGETTIME();
	int nComboCnt = GetMaxCombo();
	if( !COMPARE( nCurCombo, 0, nComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, nCurCombo );

		m_nNormalCombo = 0;
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_WeaponItemState = WS_NONE;
	}
	else
	{
		//공격 준비 애니메이션 설정
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[nCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_dwAttackStartTime = dwCurTime;
		m_nNormalCombo = nCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_WeaponItemState = WS_CHECK_CHARGING;
	}
}

void ioSasinmuItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_WeaponItemState )
	{
	case WS_CHECK_CHARGING:
		CheckCharging( pOwner );
		break;
	case WS_NORMAL_ATTACK:
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_WeaponItemState == WS_CHECK_CHARGING )
			return true;
	}

	return false;
}

bool ioSasinmuItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	switch( m_SpecialState )
	{
	case SS_COMBO_ATTACK:
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case WS_NORMAL_ATTACK:
		{
			rkPacket >> m_nNormalCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true );
			SetNormalAttackState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_INIT:
		{
			Init();

			pOwner->SetState( CS_SASINMU_SPECIAL );

			bool bPreDelayState;
			rkPacket >> bPreDelayState;

			if ( bPreDelayState )
				SetPreDelayState( pOwner );
		}
		break;
	case WS_SPECIAL_STATE_PROCESS:
		RecvSpecialState( pOwner, rkPacket );
		break;
	case WS_DEFENCE_COUNTER_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			DefenceAttack( pOwner );
		}
		break;
	case WS_EXTEND_DEFENSE_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			DefenseEnd( pOwner );
		}
		break;
	case WS_POWER_MODE:
		{
			rkPacket >> m_bPowerUpMode;

			if ( m_bPowerUpMode )
			{
				AddBuff( pOwner, m_PowerUpBuff );
			}
			else
			{
				AddBuff( pOwner, m_PowerDownBuff );
				m_fCurExtraGauge = 0.0f;
			}
		}
		break;
	}
}

void ioSasinmuItem::RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	int nSpecialState;
	rkPacket >> nSpecialState;

	switch( nSpecialState )
	{
	case SS_CHARGE_ATTACK:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialChargeAttack, SS_CHARGE_ATTACK );
			else
				SetComboSpecialState( pOwner, m_SpecialChargeAttack, SS_CHARGE_ATTACK );
		}
		break;
	case SS_COMBO_ATTACK:
		{
			int nCurKey;
			rkPacket >> nCurKey;
			rkPacket >> m_nCurSpecialComboCnt;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboAttackState( pOwner, m_PowerUpSpecialComboAttackList );
			else
				SetComboAttackState( pOwner, m_SpecialComboAttackList );
		}
		break;
	case SS_SPECIAL_CMD:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialCmdInfo, SS_SPECIAL_CMD, false );
			else
				SetComboSpecialState( pOwner, m_SpecialCmdInfo, SS_SPECIAL_CMD, false );
		};
		break;
	case SS_SPECIAL_ATTACK:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialAttackInfo, SS_SPECIAL_ATTACK );
			else
				SetComboSpecialState( pOwner, m_SpecialAttackInfo, SS_SPECIAL_ATTACK );
		};
		break;
	case SS_SPECIAL_DEFENSE:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialDefenseInfo, SS_SPECIAL_DEFENSE );
			else
				SetComboSpecialState( pOwner, m_SpecialDefenseInfo, SS_SPECIAL_DEFENSE );
		};
		break;
	case SS_SPECIAL_JUMP:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialJumpInfo, SS_SPECIAL_JUMP );
			else
				SetComboSpecialState( pOwner, m_SpecialJumpInfo, SS_SPECIAL_JUMP );
		};
		break;
	case SS_SPECIAL_DASH:
		{
			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialDashInfo, SS_SPECIAL_DASH );
			else
				SetComboSpecialState( pOwner, m_SpecialDashInfo, SS_SPECIAL_DASH );
		};
		break;
	case SS_SPECIAL_CHARGE_DASH:
		{
			Init();

			int nCurKey;
			rkPacket >> nCurKey;

			if ( (ioUserKeyInput::DirKeyInput)nCurKey != ioUserKeyInput::DKI_NONE )
				pOwner->SetTargetRotToDirKey( (ioUserKeyInput::DirKeyInput)nCurKey );

			pOwner->ApplyAutoTargetInfo( rkPacket );

			if ( m_bPowerUpMode )
				SetComboSpecialState( pOwner, m_PowerUpSpecialChargeDashInfo, SS_SPECIAL_CHARGE_DASH );
			else
				SetComboSpecialState( pOwner, m_SpecialChargeDashInfo, SS_SPECIAL_CHARGE_DASH );
		};
		break;
	case SS_END:
		{
			m_SpecialState = SS_END;			
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner || HasPowerUpBuff(pOwner) || HasPowerDownBuff(pOwner) )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_SASINMU )
			fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_SASINMU )
			fTypeRate = m_fExtraGaugeAttackDef;
		break;
	case EGT_WOUNDED:
		fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	m_fCurExtraGauge += fTypeRate;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

void ioSasinmuItem::SetUseExtraGauge( float fAmt )
{
	m_fCurExtraGauge += fAmt;
	m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
}

void ioSasinmuItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner || pOwner->IsSystemState() ) 
		return;
	
	bool bSendNetwork = false;
	if ( !m_bPowerUpMode && m_fCurExtraGauge >= m_fNeedGauge )
	{
		AddBuff( pOwner, m_PowerUpBuff );
		m_fCurExtraGauge = 0.0f;
		m_bPowerUpMode = true;
		bSendNetwork = true;
	}

	if ( m_bPowerUpMode && !HasPowerUpBuff( pOwner ) )
	{
		AddBuff( pOwner, m_PowerDownBuff );
		m_fCurExtraGauge = 0.0f;
		m_bPowerUpMode = false;
		bSendNetwork = true;
	}

	if ( bSendNetwork && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_POWER_MODE;
		kPacket << m_bPowerUpMode;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioSasinmuItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioSasinmuItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSasinmuItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioSasinmuItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::OnReleased( ioBaseChar *pOwner )
{
	if ( HasPowerUpBuff(pOwner) )
		AddBuff( pOwner, m_PowerDownBuff );

	RemoveBuff( pOwner, m_PowerUpBuff );

	ioWeaponItem::OnReleased( pOwner );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::Init()
{	
	m_WeaponItemState = WS_NONE;
	m_SpecialState = SS_NONE;
	m_DefenceCounterState = DCS_None;

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwEnableReserveKeyTime = 0;
	m_nCurSpecialComboCnt = 0;
	m_bSetMove = false;
	m_dwFireStartTime = 0;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	ClearCmdInfo();

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = 0;
	m_dwDefenseEndTime = 0;

	m_vSpecialJumpMoveDir = ioMath::VEC3_ZERO;
}

void ioSasinmuItem::CheckCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess()  )	
		return;

	if( !HasPowerDownBuff(pOwner) && pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetSasinmuSpecialState( pOwner );
	}
	else
		SetNormalAttackState( pOwner );
}

void ioSasinmuItem::SetNormalAttackState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_nNormalCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_nNormalCombo );
	m_WeaponItemState = WS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_WeaponItemState;
		kPacket << m_nNormalCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSasinmuItem::SetJumpLandingExtraState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	ioIljimaeJump *pIljimaeJump = ToIljimaeJump( GetCurExtendJump() );
	if ( !pIljimaeJump || !pIljimaeJump->IsChargeAttacked() )
		return false;

	if ( !HasPowerDownBuff( pOwner ) )
	{
		Init();

		pOwner->SetState( CS_SASINMU_SPECIAL );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_INIT;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}

		if ( m_bPowerUpMode )
			SetComboAttackState( pOwner, m_PowerUpSpecialComboAttackList );
		else
			SetComboAttackState( pOwner, m_SpecialComboAttackList );
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetSasinmuSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetState( CS_SASINMU_SPECIAL );

	SetPreDelayState( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_INIT;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSasinmuItem::ProcessSasinmuSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialState )
	{
	case SS_PRE_DELAY:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				SetLoopState( pOwner );
		}
		break;
	case SS_LOOP:
		{
			CheckLoopMoveAni( pOwner );
			CheckInputKey( pOwner );
		}
		break;
	case SS_COMBO_ATTACK:
		{
			if ( pOwner->IsNeedProcess() && !CheckComboInputKey( pOwner ) )
			{
				if ( m_dwMotionEndTime < dwCurTime )
				{
					if ( !HasPowerDownBuff(pOwner) && m_nCurSpecialComboCnt < m_nMaxSpecialComboCnt )
					{
						if ( m_bPowerUpMode )
							SetComboAttackState( pOwner, m_PowerUpSpecialComboAttackList );
						else
							SetComboAttackState( pOwner, m_SpecialComboAttackList );
					}
					else
						CheckReserveState( pOwner );
				}
			}
		}
		break;
	case SS_CHARGE_ATTACK:
	case SS_SPECIAL_CMD:
	case SS_SPECIAL_ATTACK:
	case SS_SPECIAL_DEFENSE:
	case SS_SPECIAL_DASH:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( !HasPowerDownBuff(pOwner) )
				{
					if ( m_bPowerUpMode )
						SetComboAttackState( pOwner, m_PowerUpSpecialComboAttackList );
					else
						SetComboAttackState( pOwner, m_SpecialComboAttackList );
				}
				else
					CheckReserveState( pOwner );
			}
		}
		break;
	case SS_SPECIAL_JUMP:
	case SS_SPECIAL_CHARGE_DASH:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				pOwner->SetSKillEndJumpState( m_fCurSpecialJumpEndJumpPower );
			else if (m_dwJumpMoveStartTime < dwCurTime )
				ProcessSpecialJump( pOwner );
		}
		break;
	case SS_END:
		{
			pOwner->SetState( CS_DELAY );
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::CheckInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_bChargeCharJump && pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		pOwner->SetJumpState();
	else if ( !pOwner->IsAttackKeyDown() )
	{
		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialChargeAttack, SS_CHARGE_ATTACK );
		else
			SetComboSpecialState( pOwner, m_SpecialChargeAttack, SS_CHARGE_ATTACK );
	}
}

bool ioSasinmuItem::CheckComboInputKey( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( CheckCmdInputState( pOwner ) )
	{
		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialCmdInfo, SS_SPECIAL_CMD, false );
		else
			SetComboSpecialState( pOwner, m_SpecialCmdInfo, SS_SPECIAL_CMD, false );
		return true;
	}
	else if ( pOwner->IsAttackKey() )
	{
		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialAttackInfo, SS_SPECIAL_ATTACK );
		else
			SetComboSpecialState( pOwner, m_SpecialAttackInfo, SS_SPECIAL_ATTACK );
		return true;
	}
	else if ( pOwner->IsDefenseKey() )
	{
		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialDefenseInfo, SS_SPECIAL_DEFENSE );
		else
			SetComboSpecialState( pOwner, m_SpecialDefenseInfo, SS_SPECIAL_DEFENSE );
		return true;
	}
	else if ( pOwner->IsJumpKey() )
	{
		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialJumpInfo, SS_SPECIAL_JUMP );
		else
			SetComboSpecialState( pOwner, m_SpecialJumpInfo, SS_SPECIAL_JUMP );
		return true;
	}
	else if ( pOwner->IsDirKeyDoubleClick() )
	{
		pOwner->ClearDirDoubleClick();

		if ( m_bPowerUpMode )
			SetComboSpecialState( pOwner, m_PowerUpSpecialDashInfo, SS_SPECIAL_DASH );	
		else
			SetComboSpecialState( pOwner, m_SpecialDashInfo, SS_SPECIAL_DASH );
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetPreDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( m_szWaitPreDelayAni );
	if( nAniID == -1 )	
		return;

	float fTimeRate = m_fWaitPreDelayAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;

	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0 );

	m_SpecialState = SS_PRE_DELAY;
}

void ioSasinmuItem::SetLoopState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsSettedDirection() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );
		m_bSetMove = true;
	}
	else
	{		
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );
		m_bSetMove = false;
	}

	m_SpecialState = SS_LOOP;

	if( pOwner->IsNeedProcess() )
		pOwner->ChangeDirectionByInputDir( false );
}

void ioSasinmuItem::SetComboAttackState( ioBaseChar *pOwner, const AttributeList& ComboList )
{
	if ( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	if ( !COMPARE( m_nCurSpecialComboCnt, 0, (int)ComboList.size() ) )
		return;

	const AttackAttribute& ComboAttack = ComboList[m_nCurSpecialComboCnt];

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( ComboAttack.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = ComboAttack.m_fAttackAniRate;
	DWORD dwPreDelay = ComboAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( ComboAttack.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, ComboAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_SpecialState = SS_COMBO_ATTACK;

	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if (m_nCurSpecialComboCnt == 0)
		ClearCmdInfo();

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		pOwner->SetAutoTarget( ATT_NORMAL );

		DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << SS_COMBO_ATTACK;
			kPacket << (int)eNewDirKey;
			kPacket << m_nCurSpecialComboCnt;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_nCurSpecialComboCnt++;
}

void ioSasinmuItem::SetComboSpecialState( ioBaseChar *pOwner, const AttackAttribute& Special, SpecialState eSpecialState, bool bRotate )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	int nAniID = pGrp->GetAnimationIdx( Special.m_AttackAnimation );
	if( nAniID == -1 )	
		return;

	float fTimeRate  = Special.m_fAttackAniRate;
	DWORD dwPreDelay = Special.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( nAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->CheckCharColSkipTime( nAniID, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( Special.m_vForceInfoList, nAniID, fTimeRate, dwPreDelay );

	pOwner->RefreshFireTimeList( nAniID, Special.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( nAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay;

	m_dwJumpMoveStartTime = dwCurTime;
	m_dwJumpMoveStartTime += pGrp->GetAnimationEventTime_e( nAniID, "jump_move" ) * fTimeRate;
	m_dwJumpMoveStartTime += dwPreDelay;

	m_SpecialState = eSpecialState;

	m_nCurSpecialComboCnt = 0;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
	m_vSpecialJumpMoveDir = ioMath::VEC3_ZERO;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
		if ( bRotate && pOwner->IsSettedDirection() )
		{
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}
		
		pOwner->SetAutoTarget( ATT_NORMAL );

		DWORD dwTrackingTime = dwCurTime + dwPreDelay + pGrp->GetAnimationEventTime_e( nAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		if ( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << WS_SPECIAL_STATE_PROCESS;
			kPacket << m_SpecialState;
			kPacket << (int)eNewDirKey;
			if (m_SpecialState != SS_SPECIAL_DASH)
				pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	float fAddGauge = 0.0f;
	switch( eSpecialState )
	{
	case SS_SPECIAL_CMD:
		fAddGauge = m_fSpecialCmdGauge;
		break;
	case SS_SPECIAL_ATTACK:
		fAddGauge = m_fSpecialAttackGauge;
		break;
	case SS_SPECIAL_DEFENSE:
		{
			fAddGauge = m_fSpecialDefenseGauge;

			AddBuff( pOwner, m_SpecialDefenseBuff );
		}
		break;
	case SS_SPECIAL_JUMP:
		{
			fAddGauge = m_fSpecialJumpGauge;
			
			if ( m_bPowerUpMode )
			{
				m_vSpecialJumpMoveDir = GetSpecialJumpMoveDir( pOwner, m_fPowerUpSpecialJumpMoveAngle );
				m_fCurSpecialJumpMoveSpeed = m_fPowerUpSpecialJumpMoveSpeed;
				m_fCurSpecialJumpEndJumpPower = m_fPowerUpSpecialJumpEndJumpPower;
			}
			else
			{
				m_vSpecialJumpMoveDir = GetSpecialJumpMoveDir( pOwner, m_fSpecialJumpMoveAngle );
				m_fCurSpecialJumpMoveSpeed = m_fSpecialJumpMoveSpeed;
				m_fCurSpecialJumpEndJumpPower = m_fSpecialJumpEndJumpPower;
			}
		}
		break;
	case SS_SPECIAL_DASH:
		fAddGauge = m_fSpecialDashGauge;
		break;
	case SS_SPECIAL_CHARGE_DASH:
		{
			fAddGauge = m_fSpecialChargeDashGauge;

			if ( m_bPowerUpMode )
			{
				m_vSpecialJumpMoveDir = GetSpecialJumpMoveDir( pOwner, m_fPowerUpSpecialChargeDashMoveAngle );
				m_fCurSpecialJumpMoveSpeed = m_fPowerUpSpecialChargeDashMoveSpeed;
				m_fCurSpecialJumpEndJumpPower = m_fPowerUpSpecialChargeDashEndJumpPower;
			}
			else
			{
				m_vSpecialJumpMoveDir = GetSpecialJumpMoveDir( pOwner, m_fSpecialChargeDashMoveAngle );
				m_fCurSpecialJumpMoveSpeed = m_fSpecialChargeDashMoveSpeed;
				m_fCurSpecialJumpEndJumpPower = m_fSpecialChargeDashEndJumpPower;
			}
		}
		break;
	}

	if ( !HasPowerUpBuff(pOwner) && !HasPowerDownBuff(pOwner) && fAddGauge > 0.0f )
		SetUseExtraGauge( fAddGauge );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::IsEnableMoveState()
{
	switch ( m_SpecialState )
	{
	case SS_LOOP:
		return true;
	}

	return false;
}

ioSasinmuItem::SpecialState ioSasinmuItem::GetCurSpecialState()
{
	return m_SpecialState;
}

void ioSasinmuItem::CheckLoopMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );

			m_bSetMove = true;
		}
	}
	else
	{		
		if( m_bSetMove )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );

			m_bSetMove = false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::ReserveInputKey(ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}

void ioSasinmuItem::CheckReserveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_bAttackKeyReserved )
	{
		SetReserveAttack( pOwner );
		return;
	}
	else if( m_bDefenseKeyReserved )
	{
		SetReserveDefence( pOwner );
		return;
	}
	else if ( m_bJumpKeyReserved )
	{
		SetReserveJump( pOwner );
		return;
	}

	m_SpecialState = SS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_SPECIAL_STATE_PROCESS;
		kPacket << SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}	
}

void ioSasinmuItem::SetReserveAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetNormalAttack(0);

	m_bAttackKeyReserved = false;
}

void ioSasinmuItem::SetReserveDefence( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetDefenseState();
	m_bDefenseKeyReserved = false;
}

void ioSasinmuItem::SetReserveJump( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	pOwner->SetJumpState();
	m_bJumpKeyReserved = false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::ClearCmdInfo()
{
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );
	m_nCurCmdIndex = 0;
	m_PreActionKey = ioUserKeyInput::AKI_NONE;
	m_dwInputCheckTime = 0;
}

bool ioSasinmuItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;
	
	CheckCmdInput( pOwner );

	if( m_dwInputCheckTime > 0 && m_dwInputCheckTime < FRAMEGETTIME() )
		return CheckSpecialCmd( pOwner );

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::CheckSpecialCmd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( strcmp( m_szSpecialCmd.c_str(), m_CurCmdInfo ) == 0 )
	{			
		ClearCmdInfo();
		return true;
	}

	ClearCmdInfo();
	return false;
}

void ioSasinmuItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	if( !COMPARE( m_nCurCmdIndex, 0, MAX_COMMAND_CNT - 1 ) )
		return;
	/*
	ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
	if( m_PreDirKey == eDirKey )
		return;

	char szChar = '\0';

	switch( eDirKey )
	{
	case ioUserKeyInput::DKI_NONE:
		m_PreDirKey = eDirKey;
		return;
	case ioUserKeyInput::DKI_RIGHTUP:
	case ioUserKeyInput::DKI_RIGHTDOWN:
	case ioUserKeyInput::DKI_LEFTDOWN:
	case ioUserKeyInput::DKI_LEFTUP:
		return;
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
	}*/
		
	ioUserKeyInput::ActionKeyInput eActionKey = pOwner->GetCurActionKey();
	if( m_PreActionKey == eActionKey )
		return;

	char szChar = '\0';

	switch( eActionKey )
	{
	case ioUserKeyInput::DKI_NONE:
		m_PreActionKey = eActionKey;
		return;
	case ioUserKeyInput::AKI_ZX:
		szChar = '1';
		break;
	case ioUserKeyInput::AKI_ZXC:
		szChar = '2';
		break;
	case ioUserKeyInput::AKI_ZC:
		szChar = '3';
		break;
	case ioUserKeyInput::AKI_XC:
		szChar = '4';
		break;
	}

	m_CurCmdInfo[m_nCurCmdIndex] = szChar;

	m_PreActionKey = eActionKey;
	m_nCurCmdIndex++;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwInputCheckTime = dwCurTime + m_dwInputGapTime;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::IsUseDefenceCounterAttack()
{
	return m_bUseDefenceCounterAttack;
}

void ioSasinmuItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_stDefenceCounterDelayAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fDefenceCounterDelayAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDefenceDelayEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Delay;
}

void ioSasinmuItem::DefenceAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DefenceCounterAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_DefenceCounterAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_DefenceCounterAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	pOwner->RefreshFireTimeList( iAniID, m_DefenceCounterAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );

	pOwner->SetReservedSliding( m_DefenceCounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDefenceAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_DefenceCounterState = DCS_Attack;

	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_DEFENCE_COUNTER_ATTACK;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSasinmuItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch ( m_DefenceCounterState )
	{
	case DCS_Delay:
		{
			if( dwCurTime > m_dwDefenceDelayEndTime )
			{
				pOwner->SetState( CS_DELAY, true );
				return;
			}

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKey() )
					DefenceAttack( pOwner );
			}
		}
		break;
	case DCS_Attack:
		{
			if( m_dwDefenceAttackEndTime < dwCurTime )
			{
				pOwner->SetState( CS_DELAY, true );
				return;
			}
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if ( !pOwner || pOwner->IsPrisonerMode() || pOwner->IsCatchMode() )
		return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( false, true );

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	return true;
}

void ioSasinmuItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
					m_iSkillInput = -1;
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
					pOwner->SetDefenseState();
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
					pOwner->SetJumpState();
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
						pOwner->SetNormalAttack( 0 );
				}
				else
					pOwner->SetState( CS_DELAY );
			}
			else
				pOwner->SetState( CS_DELAY );
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}
		return;
	}

	if( !pOwner->IsNeedProcess() ) 
		return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	if( m_dwExtendDefenseWoundedTime > 0 )
	{
		if( m_dwExtendDefenseWoundedTime > dwCurTime )
			return;
		else
			m_dwExtendDefenseWoundedTime = 0;
	}

	if( !pOwner->IsDefenseKeyDown() )
		DefenseEnd( pOwner );
	else if( pOwner->IsAttackKeyDown() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() )
		{
			if( pOwner->SetUseSkill(0, SUT_NORMAL) )
				return;

			DefenseEnd( pOwner );
		}
	}
}

void ioSasinmuItem::DefenseEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pOwner->GetDelayAnimationIdx();
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllLoopAniExceptOne( nAniID, FLOAT100 );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( nAniID, (float)m_dwDefenseEndDuration );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << WS_EXTEND_DEFENSE_END;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ioSasinmuItem::GetSpecialJumpMoveDir( ioBaseChar *pOwner, float fAngle )
{
	if ( !pOwner )
		return ioMath::VEC3_ZERO;

	float fPitchR = DEGtoRAD( fAngle );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, 0.0f, fPitchR, 0.0f );

	D3DXQUATERNION qtCharRotate = pOwner->GetTargetRot();
	D3DXQuaternionMultiply( &qtRotate, &qtRotate, &qtCharRotate );

	D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	return vMoveDir;
}

void ioSasinmuItem::ProcessSpecialJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_fCurSpecialJumpMoveSpeed <= 0.0f || m_vSpecialJumpMoveDir == ioMath::VEC3_ZERO )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = g_FrameTimer.GetSecPerFrame() * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fCurSpecialJumpMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )
		{
			vMoveAmt = m_vSpecialJumpMoveDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}

		if( pOwner->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pOwner,
						false,
						pOwner->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::LoadBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList )
{
	char szBuff[MAX_PATH], szKey[MAX_PATH];

	BuffList.clear();

	wsprintf_e( szKey, "%s_buff_cnt", szName );
	int nMaxCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0; i<nMaxCnt; i++ )
	{
		wsprintf_e( szKey, "%s_buff%d", szName, i+1 );
		rkLoader.LoadString( szKey, "", szBuff, MAX_PATH );
		BuffList.push_back( szBuff );
	}
}

void ioSasinmuItem::AddBuff( ioBaseChar *pOwner, ioHashStringVec &BuffList )
{
	if( !pOwner )	
		return;

	if( BuffList.empty() ) 
		return;

	int nMaxCnt = BuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
		pOwner->AddNewBuff( BuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioSasinmuItem::RemoveBuff( ioBaseChar *pOwner, ioHashStringVec &BuffList )
{
	if( !pOwner )	
		return;

	if( BuffList.empty() ) 
		return;

	int nMaxCnt = BuffList.size();
	for( int i=0; i<nMaxCnt; i++ )
	{
		ioBuff* pBuff = pOwner->GetBuff( BuffList[i] );
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioSasinmuItem::HasPowerUpBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	bool bHasPowerUpBuff = false;
	if ( !m_PowerUpBuff.empty() && pOwner->HasBuff( m_PowerUpBuff[0] ) )
		bHasPowerUpBuff = true;

	return bHasPowerUpBuff;
}

bool ioSasinmuItem::HasPowerDownBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	bool bHasPowerDownBuff = false;
	if ( !m_PowerDownBuff.empty() && pOwner->HasBuff( m_PowerDownBuff[0] ) )
		bHasPowerDownBuff = true;

	return bHasPowerDownBuff;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioSasinmuItem::SetSpecialChargeDashState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	Init();

	if ( m_bPowerUpMode )
		SetComboSpecialState( pOwner, m_PowerUpSpecialChargeDashInfo, SS_SPECIAL_CHARGE_DASH );
	else
		SetComboSpecialState( pOwner, m_SpecialChargeDashInfo, SS_SPECIAL_CHARGE_DASH );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSasinmuSpecialState::ioSasinmuSpecialState()
{
}

ioSasinmuSpecialState::~ioSasinmuSpecialState()
{
}

void ioSasinmuSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSasinmuSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioSasinmuSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSasinmuItem *pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSasinmuItem )
		pSasinmuItem->ProcessSasinmuSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioSasinmuSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioSasinmuItem *pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSasinmuItem )
		return false;

	if ( pSasinmuItem->GetCurSpecialState() == ioSasinmuItem::SS_SPECIAL_JUMP ||
		 pSasinmuItem->GetCurSpecialState() == ioSasinmuItem::SS_SPECIAL_CHARGE_DASH )
		return true;

	return false;
}

void ioSasinmuSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioSasinmuItem *pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSasinmuItem && pSasinmuItem->IsEnableMoveState() )
		pOwner->ChangeDirectionByInputDir( false );
}

bool ioSasinmuSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioSasinmuItem *pSasinmu = ToSasinmuItem( pOwner->GetWeapon() );
	if( pSasinmu && pSasinmu->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}

void ioSasinmuSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSasinmuSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSasinmuItem *pSasinmuItem = ToSasinmuItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSasinmuItem )
		return false;

	if ( pSasinmuItem->GetCurSpecialState() == ioSasinmuItem::SS_SPECIAL_JUMP ||
		pSasinmuItem->GetCurSpecialState() == ioSasinmuItem::SS_SPECIAL_CHARGE_DASH ||
		pSasinmuItem->GetCurSpecialState() == ioSasinmuItem::SS_SPECIAL_DASH )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}