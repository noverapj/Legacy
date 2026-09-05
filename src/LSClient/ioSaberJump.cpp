#include "StdAfx.h"

#include "ioSaberJump.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioSaberJump::ioSaberJump(void)
{
	ClearData();
}

ioSaberJump::ioSaberJump( const ioSaberJump &rhs ) :
ioExtendJump( rhs ),
m_dwFullJumpChargeTime( rhs.m_dwFullJumpChargeTime ),
 m_ComboAttackList( rhs.m_ComboAttackList ),
m_fDoubleJumpPower( rhs.m_fDoubleJumpPower ),
m_fDoubleJumpForce( rhs.m_fDoubleJumpForce ),
m_iMaxJumpCnt( rhs.m_iMaxJumpCnt ),
m_fDoubleJumpEnableHeight( rhs.m_fDoubleJumpEnableHeight ),
m_dwDoubleJumpEnableTime( rhs.m_dwDoubleJumpEnableTime ),
m_vDoubleJumpAniList( rhs.m_vDoubleJumpAniList ),
m_fDoubleJumpAniRate( rhs.m_fDoubleJumpAniRate ),
m_dwAttackKeyChargingTime( rhs.m_dwAttackKeyChargingTime ),
m_dwAttackKeyGatheringTime( rhs.m_dwAttackKeyGatheringTime ),
m_AttackKeyChargingAniList( rhs.m_AttackKeyChargingAniList ),
m_dwJumpKeyChargingTime( rhs.m_dwJumpKeyChargingTime ),
m_fJumpSpeedRateFR( rhs.m_fJumpSpeedRateFR ),
m_fJumpSpeedRateSI( rhs.m_fJumpSpeedRateSI ),
m_fJumpSpeedRateBG( rhs.m_fJumpSpeedRateBG ),
m_fDoubleJumpSpeedRateFR( rhs.m_fDoubleJumpSpeedRateFR ),
m_fDoubleJumpSpeedRateSI( rhs.m_fDoubleJumpSpeedRateSI ),
m_fDoubleJumpSpeedRateBG( rhs.m_fDoubleJumpSpeedRateBG ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
m_szSlowDownAni( rhs.m_szSlowDownAni ),
m_fSlowDownAniRate( rhs.m_fSlowDownAniRate ),
m_szSlowDownLoopAni( rhs.m_szSlowDownLoopAni ),
m_fSlowDownLoopAniRate( rhs.m_fSlowDownLoopAniRate ),
m_fSlowDownEnableHeight( rhs.m_fSlowDownEnableHeight ),
m_fSlowDownGravityAmt( rhs.m_fSlowDownGravityAmt )
{
	ClearData();
}

ioSaberJump::~ioSaberJump(void)
{
}

void ioSaberJump::ClearData()
{
	m_SaberJumpState = SJS_NONE;
	m_bReserveJumpAttack = false;

	m_iCurJumpCnt = 1;
	m_bUsedDoubleJump = false;
	m_dwChargingStartTime = 0;

	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;
}

ioExtendJump* ioSaberJump::Clone()
{
	return new ioSaberJump( *this );
}

JumpType ioSaberJump::GetType() const
{
	return JT_SABER_JUMP;
}

void ioSaberJump::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	ioExtendJump::LoadProperty( rkLoader );
	m_dwFullJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	//double jump
	m_fDoubleJumpPower = rkLoader.LoadFloat_e( "double_jump_power", 0.0f );
	m_fDoubleJumpForce = rkLoader.LoadFloat_e( "double_jump_force", 0.0f );
	m_fDoubleJumpEnableHeight = rkLoader.LoadFloat_e( "double_jump_enable_height", 0.0f );
	m_dwDoubleJumpEnableTime = rkLoader.LoadInt_e( "double_jump_enable_time", 0 );
	m_vDoubleJumpAniList.clear();
	m_vDoubleJumpAniList.reserve( 4 );
	rkLoader.LoadString_e( "double_jump_animation_front", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_right", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_back", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	rkLoader.LoadString_e( "double_jump_animation_left", "", szBuf, MAX_PATH );
	m_vDoubleJumpAniList.push_back( szBuf );
	m_fDoubleJumpAniRate = rkLoader.LoadFloat_e( "double_jump_ani_rate", FLOAT1 );

	m_iMaxJumpCnt = max( 1, rkLoader.LoadInt_e( "max_jump_cnt", 1 ) );

	//AttackKey charge
	m_dwAttackKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "charging_time", 0 );
	m_dwAttackKeyGatheringTime = (DWORD)rkLoader.LoadInt_e( "jump_gathering_time", 0 );
	int iMaxChargeAni = rkLoader.LoadInt_e( "charging_jump_ani_cnt", 0 );
	for( int i=0; i < iMaxChargeAni; ++i )
	{
		wsprintf_e( szKey, "charging_jump_ani%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackKeyChargingAniList.push_back( szBuf );
	}

	//JumpKey Charge
	m_dwJumpKeyChargingTime = (DWORD)rkLoader.LoadInt_e( "jump_key_charging_time", 0 );

	//jump attack
	int iMaxCombo = rkLoader.LoadInt_e( "jump_max_combo", 0 );
	m_ComboAttackList.clear();
	for( int i=0 ; i<iMaxCombo ; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "jump_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_ComboAttackList.push_back( kAttribute );
	}

	m_fDoubleJumpSpeedRateFR = rkLoader.LoadFloat_e( "double_jump_speed_rate_front", FLOAT1 );
	m_fDoubleJumpSpeedRateSI = rkLoader.LoadFloat_e( "double_jump_speed_rate_side", FLOAT1 );
	m_fDoubleJumpSpeedRateBG = rkLoader.LoadFloat_e( "double_jump_speed_rate_back", FLOAT1 );

	// combo
	m_fJumpSpeedRateFR = rkLoader.LoadFloat_e( "extend_jump_speed_rate_front", FLOAT1 );
	m_fJumpSpeedRateSI = rkLoader.LoadFloat_e( "extend_jump_speed_rate_side", FLOAT1 );
	m_fJumpSpeedRateBG = rkLoader.LoadFloat_e( "extend_jump_speed_rate_back", FLOAT1 );

	rkLoader.LoadString_e( "jump_gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "jump_gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_slowdown_ani", "", szBuf, MAX_PATH );
	m_szSlowDownAni = szBuf;
	m_fSlowDownAniRate = rkLoader.LoadFloat_e( "jump_slowdown_ani_rate", FLOAT1 );
	rkLoader.LoadString_e( "jump_slowdown_loop_ani", "", szBuf, MAX_PATH );
	m_szSlowDownLoopAni = szBuf;
	m_fSlowDownLoopAniRate = rkLoader.LoadFloat_e( "jump_slowdown_loop_ani_rate", FLOAT1 );
	m_fSlowDownEnableHeight = rkLoader.LoadFloat_e( "jump_slowdown_enable_height", 0.0f );
	m_fSlowDownGravityAmt = rkLoader.LoadFloat_e( "jump_slowdown_gravity_amt", 0.0f );
}

DWORD ioSaberJump::GetChargingTime( ioBaseChar *pChar )
{
	return m_dwFullJumpChargeTime;
}

void ioSaberJump::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_SaberJumpState = SJS_NONE;
	pOwner->SetUsedFlashJump( false );

	m_bUseExtendJump = false;
	m_bUsedDoubleJump = false;

	// combo
	m_iCurComboCnt = 0;
	m_dwEnableComboTime_S = 0;
	m_dwEnableComboTime_E = 0;

	m_bReserveJumpAttack = false;

	// charge
	m_dwChargingStartTime = 0;

	m_dwReserveEnableTime = 0;
	m_iCurJumpCnt = 1;
}

void ioSaberJump::CheckComboJumpAttack( ioBaseChar *pOwner )
{
	switch( m_SaberJumpState )
	{
	case SJS_NONE:
	case SJS_NORMAL:
	case SJS_DOUBLE_JUMP:
	case SJS_A_CHARGE:
		break;
	default:
		return;
	}

	if( m_bReserveJumpAttack )
		return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = 0;

	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_iCurComboCnt > 0 && !COMPARE( dwCurTime, m_dwEnableComboTime_S+dwLooseTime, m_dwEnableComboTime_E+dwLooseTime ) )
		return;

	if( m_iCurComboCnt > 0 && pOwner->IsHasCrown() )
		return;

	m_bReserveJumpAttack = true;
}

void ioSaberJump::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge )
{
	if( !pOwner )
		return;

	if ( m_dwChargingStartTime > 0 && CheckJumpCharging( pOwner ) )
		return;
	
	// combo
	if( m_bReserveJumpAttack )
	{
		SetAttackChargingState( pOwner );
		return;
	}

	// Double
	if( IsCanJumpCharge( pOwner, fHeightGap ) )
	{
		SetJumpChargingState( pOwner );
		return;
	}
	if( IsCanDefenseKeyState( pOwner, fHeightGap ) )
		return;
}

void ioSaberJump::SetAttackChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;
	
	if( m_AttackKeyChargingAniList.empty() )
		return;

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	ioHashString szCurChargingAni;
	int iChargingCnt = m_AttackKeyChargingAniList.size();
	if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		szCurChargingAni = m_AttackKeyChargingAniList[m_iCurComboCnt];
	else
		szCurChargingAni = m_AttackKeyChargingAniList[0];

	int iAniID = pGrp->GetAnimationIdx( szCurChargingAni );
	float fTimeRate = (float)m_dwAttackKeyChargingTime / pGrp->GetAnimationFullTime( iAniID );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();

	m_SaberJumpState = SJS_D_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberJump::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	switch( m_SaberJumpState )
	{
	case SJS_NORMAL:
		ProcessComboJumpAttack( pOwner );
		break;
	case SJS_D_CHARGE:
		ProcessAttackCharging( pOwner );
		break;
	case SJS_A_CHARGE:
		CheckJumpCharging( pOwner );
		break;
	case SJS_D_GATHERING:
		ProcessAttackGathering( pOwner );
		break;
	case SJS_SLOWDOWN:
		ProcessSlowDown( pOwner );
		break;
	case SJS_SLOWDOWN_LOOP:
		ProcessSlowDownLoop( pOwner );
		break;
	}
}

void ioSaberJump::ProcessAttackCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	bool bEnableChargeAttack = false;
	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetWeapon() );
	if( pSaberItem && pSaberItem->IsEnableChargingAttackGauge() )
		bEnableChargeAttack = true;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetWeapon() );
	if( pNeoItem )
		bEnableChargeAttack = true;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && bEnableChargeAttack )
	{
		if( m_dwChargingStartTime + m_dwAttackKeyChargingTime < dwCurTime )
		{
			if( pNeoItem )
				ChangeGatheringState( pOwner );
			else
				ChangeToSaberChargeAttack( pOwner );
		}
	}
	else
		SetComboJumpAttack( pOwner );
}

void ioSaberJump::ProcessAttackGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetWeapon() );
	if( !pNeoItem )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && pNeoItem->IsEnableChargeState( false ) )
	{
		if( m_dwChargingStartTime + m_dwAttackKeyGatheringTime < dwCurTime )
		{
			pNeoItem->SetAttackState( pOwner, true, ioNeoItem::OBS_JUMP );
		}
		pOwner->SetJumpPower( 0 );
		pOwner->SetAirJumpPower( 0 );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}
	else if( pNeoItem->IsEnableChargeState( true )  )
		pNeoItem->SetHalfAttackState( pOwner, true, ioNeoItem::OBS_JUMP );
	else
		SetComboJumpAttack( pOwner );
}

void ioSaberJump::ProcessSlowDown( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwEnableComboTime_E < FRAMEGETTIME() )
		{
			ChangeToSlowDownLoop( pOwner );
			return;
		}
	}
	CheckAirJump( pOwner );
}

void ioSaberJump::ProcessSlowDownLoop( ioBaseChar *pOwner )
{
	if ( !pOwner |!pOwner->IsNeedProcess() )
		return;

	if ( !pOwner->IsDefenseKeyDown() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if ( !pGrp )
			return;
		int iAniID = pOwner->GetJumppingAnimationIdx();
		if( iAniID != -1 )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetActionAni( iAniID,	0.0f, 0.0f, FLOAT1, FLOAT1,	0.0f, 0.0f,	true );
		}

		m_SaberJumpState = SJS_NONE;
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_JUMP );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << SJS_SLOWDOWN_END;
			pOwner->FillAutoTargetInfo( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}		
	}
}

bool ioSaberJump::IsJumpSlowDownState()
{
	switch( m_SaberJumpState )
	{
	case SJS_SLOWDOWN:
	case SJS_SLOWDOWN_LOOP:
		return true;
	}

	return false;
}

float ioSaberJump::GetCurGravityAmt()
{
	switch( m_SaberJumpState )
	{
	case SJS_SLOWDOWN:
	case SJS_SLOWDOWN_LOOP:
		return m_fSlowDownGravityAmt;
	}
	return 0.0f;
}

void ioSaberJump::ProcessComboJumpAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	CheckAirJump( pOwner );

	DWORD dwLooseTime = 0;
	if( pOwner )
		dwLooseTime = pOwner->GetActionStopTotalDelay();

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( dwCurTime >= m_dwEnableComboTime_S + dwLooseTime )
	{
		if ( m_bReserveJumpAttack )
		{	
			SetAttackChargingState( pOwner );
			return;
		}
		int iChargingCnt = m_AttackKeyChargingAniList.size();
		if( COMPARE( m_iCurComboCnt, 0, iChargingCnt ) )
		{
			float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
			if( IsCanJumpCharge( pOwner, fHeightGap ) )
			{
				SetJumpChargingState( pOwner );
				return;
			}
		}

		if( IsCanDefenseKeyState( pOwner, fHeightGap ) )
			return;
	}	
}

void ioSaberJump::SetComboJumpAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	int iMaxCnt = m_ComboAttackList.size();
	if( !COMPARE( m_iCurComboCnt, 0, iMaxCnt ) )
		return;

	pOwner->ClearSylphid();

	DWORD dwCurTime = FRAMEGETTIME();
	m_SaberJumpState = SJS_NORMAL;

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner && !pOwner->IsActionStop() )
		pOwner->InitActionStopDelay( true );
	else
		pOwner->InitActionStopDelay( false );

	int iAniID = pGrp->GetAnimationIdx( m_ComboAttackList[m_iCurComboCnt].m_AttackAnimation );
	float fTimeRate = m_ComboAttackList[m_iCurComboCnt].m_fAttackAniRate;
	DWORD dwPreDelay = m_ComboAttackList[m_iCurComboCnt].m_dwPreDelay;

	DWORD dwStartTime = dwCurTime + dwPreDelay;
	m_dwEnableComboTime_E = dwStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );		
	pOwner->RefreshFireTimeList( iAniID, m_ComboAttackList[m_iCurComboCnt].m_vWeaponInfoList,
		FTT_JUMP_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	m_dwEnableComboTime_S = dwStartTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );	

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		kPacket << m_iCurComboCnt;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetMoveDir();
		kPacket << pOwner->GetInputDir();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	m_iCurComboCnt++;
	pOwner->SetJumpAttackState();
}

void ioSaberJump::ChangeToSaberChargeAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if( pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}

	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetWeapon() );
	if ( pSaberItem )
		pSaberItem->SetSaberChargeAttackState( pOwner, true );

	m_dwChargingStartTime = 0;
}

void ioSaberJump::ChangeGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_szGatheringAni );
	float fTimeRate  = m_fGatheringAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_bUseExtendJump = true;

	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
	
	m_SaberJumpState = SJS_D_GATHERING;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSaberJump::IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( !pOwner->IsJumpKeyPress() ||
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	switch( m_SaberJumpState )
	{
	case SJS_NONE:
	case SJS_DOUBLE_JUMP:
	case SJS_NORMAL:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

bool ioSaberJump::IsCanDefenseKeyState( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( !pOwner->IsDefenseKey() )
		return false;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetWeapon() );
	if( pNeoItem && fHeightGap > m_fSlowDownEnableHeight )
	{
		ChangeToSlowDownState( pOwner );
		return true;
	}

	return false;
}

void ioSaberJump::ChangeToSlowDownState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SaberJumpState = SJS_SLOWDOWN;

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	if( pOwner->IsNeedProcess() && pOwner->IsCanRotateJumpAttack() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, false, false );
			pOwner->SetInputDirection( pOwner->GetMoveDir() );
		}
	}
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szSlowDownAni );
	if ( iAniID == -1 )
		return;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fSlowDownAniRate, 0.0f, 0.f, true );		
	m_dwEnableComboTime_E = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fSlowDownAniRate;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, m_fSlowDownAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bReserveJumpAttack = false;
	m_bUseExtendJump = true;
	pOwner->SetEnableHide( false );
	pOwner->SetNoWoundState( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );
	pOwner->SetJumpAttackState();
}

void ioSaberJump::ChangeToSlowDownLoop( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SaberJumpState = SJS_SLOWDOWN_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_szSlowDownLoopAni );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSlowDownLoopAniRate );

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, m_fSlowDownLoopAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberJump::SetDoubleJump( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	pOwner->ClearSylphid();

	m_SaberJumpState = SJS_DOUBLE_JUMP;

	m_bUsedDoubleJump = true;
	m_bUseExtendJump = true;
	m_iCurJumpCnt++;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetJumpPower( m_fDoubleJumpPower );

	D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
	if( m_fDoubleJumpForce > 0.0f )
		pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
	else
		pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
	pOwner->SetChangeJumpMotion( false, "", "", "", 0.0f );

	g_TutorialMgr.ActionDoubleJump();

	SetDoubleJumpAni( pOwner );

	m_dwChargingStartTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DOUBLE_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << vJumpDir;
		kPacket << m_SaberJumpState;
		kPacket << m_fDoubleJumpPower;
		kPacket << m_fDoubleJumpForce;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSaberJump::SetDoubleJumpAni( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	D3DXVECTOR3 vSightDir = pOwner->GetMoveDir();

	D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDoubleJumpAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && !m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			if( fRate <= FLOAT05 )
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, (1.0f - fRate), FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
			else
				pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, fRate, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex].IsEmpty() && m_vDoubleJumpAniList[iIndex2].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else if( !m_vDoubleJumpAniList[iIndex2].IsEmpty() && m_vDoubleJumpAniList[iIndex].IsEmpty() )
		{
			pGrp->SetActionAni( m_vDoubleJumpAniList[iIndex2], FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
		else
		{
			int iAniID = pOwner->GetJumppingAnimationIdx();
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
		}
	}
	else
	{
		int iAniID = pOwner->GetJumppingAnimationIdx();
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDoubleJumpAniRate, 0.0f, 0, true );
	}
}

void ioSaberJump::SetJumpChargingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;


	DWORD dwCurTime = FRAMEGETTIME();

	m_bUseExtendJump = true;

	m_SaberJumpState = SJS_A_CHARGE;
	m_dwChargingStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_JUMP );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_SaberJumpState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioSaberJump::CheckJumpCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bEnableFly = false;

	ioSaberItem *pSaberItem = ToSaberItem( pOwner->GetWeapon() );
	if( pSaberItem && pSaberItem->IsEnableSaberFlyGauge() )
		bEnableFly = true;

	ioNeoItem *pNeoItem = ToNeoItem( pOwner->GetWeapon() );
	if( pNeoItem && pNeoItem->IsEnableGauge() )
		bEnableFly = true;

	if ( pOwner->IsJumpKeyDown() && !pOwner->IsHasCrown() && bEnableFly )
	{
		if( m_dwChargingStartTime + m_dwJumpKeyChargingTime < dwCurTime )
		{
			m_dwChargingStartTime = 0;
			if( pSaberItem )
				pSaberItem->SetSaberFlyState( pOwner, true );
			else if( pNeoItem )
				pNeoItem->SetFlyReadyState( pOwner, true );

			return true;
		}
	}
	else
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( IsCanDoubleJump( pOwner, fHeightGap ) && m_fDoubleJumpPower > 0.0f )
		{
			SetDoubleJump( pOwner );
			return true;
		}
		else
		{
			m_SaberJumpState = SJS_NONE;
			m_dwChargingStartTime = 0;
		}
	}

	return false;
}

bool ioSaberJump::IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return false;

	if( //!pOwner->IsJumpKeyPress() || // 이미 점프키를 누른 차징 상태에서의 전환이기 때문에 주석처리
		pOwner->IsHasCrown() ||
		pOwner->IsCatchMode() ||
		pOwner->IsPrisonerMode() ||
		(pOwner->GetUsedBuffJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffMultiFlyJump() != BJUT_NONE) ||
		(pOwner->GetUsedBuffFlyJump() != BJUT_NONE) )
		return false;

	if( pOwner->HasBuff( BT_BUFF_FLY_JUMP ) || pOwner->HasBuff( BT_BUFF_FLY_JUMP2 ) )
		return false;

	if ( pOwner->HasBuff( BT_BUFF_AIR_WALK ) )
		return false;

	if( pOwner->IsBuffLimitExtendJump() )
		return false;

	if( m_iCurJumpCnt >= m_iMaxJumpCnt )
		return false;
	
	switch( m_SaberJumpState )
	{
	case SJS_NONE:
	case SJS_DOUBLE_JUMP:
	case SJS_A_CHARGE:
		break;
	default:
		return false;
	}

	DWORD dwMoveStartTime = pOwner->GetJumpMoveStartTime();
	if( dwMoveStartTime == 0 ) 
		return false;

	if( FRAMEGETTIME() - dwMoveStartTime < m_dwDoubleJumpEnableTime )
		return false;

	if( fHeightGap < m_fDoubleJumpEnableHeight )
		return false;

	return true;
}

void ioSaberJump::ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SJS_NORMAL:
		{
			rkPacket >> m_iCurComboCnt;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			D3DXVECTOR3 vMoveDir, vInputDir;
			rkPacket >> vMoveDir >> vInputDir;

			pOwner->SetJumpAttckTargetRot( qtRot, vMoveDir, vInputDir );

			SetComboJumpAttack( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case SJS_D_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetAttackChargingState( pOwner );
		}
		break;
	case SJS_D_GATHERING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			ChangeGatheringState( pOwner );
		}
		break;
	case SJS_A_CHARGE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			SetJumpChargingState( pOwner );
		}
		break;
	case SJS_DOUBLE_JUMP:
		{
			float fJumpPower, fJumpForce;
			D3DXQUATERNION qtRotate;

			rkPacket >> fJumpPower >> fJumpForce;
			rkPacket >> qtRotate;

			if( fJumpPower >= 0.0f )
			{
				pOwner->ClearSylphid();
				m_SaberJumpState = SJS_DOUBLE_JUMP;

				m_bUsedDoubleJump = true;
				m_bUseExtendJump = true;

				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );

				pOwner->SetTargetRot( qtRotate );
				pOwner->SetJumpPower( fJumpPower );

				D3DXVECTOR3 vJumpDir = pOwner->GetMoveDir();
				if( m_fDoubleJumpForce > 0.0f )
					pOwner->SetForcePower( vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );
				else
					pOwner->SetForcePower( -vJumpDir, fabs(m_fDoubleJumpForce), FLOAT1, true );

				SetDoubleJumpAni( pOwner );
			}
		}
		break;
	}
}

float ioSaberJump::GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const
{
	D3DXVECTOR3 vCurDir = vDir;

	D3DXVECTOR3 vXZDir( vCurDir.x, 0.0f, vCurDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;

	int iTest = (int)fAngleGap % 360;
	iTest /= 45;

	if( m_SaberJumpState == SJS_DOUBLE_JUMP )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fDoubleJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fDoubleJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fDoubleJumpSpeedRateBG;
		}
	}
	else if( m_bUseExtendJump )
	{
		switch( iTest )
		{
		case 0:
		case 7:
			return m_fJumpSpeedRateFR;
		case 1:
		case 2:
		case 5:
		case 6:
			return m_fJumpSpeedRateSI;
		case 3:
		case 4:
			return m_fJumpSpeedRateBG;
		}
	}

	return m_fJumpSpeedRate;
}