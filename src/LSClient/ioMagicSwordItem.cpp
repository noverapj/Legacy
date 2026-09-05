#include "StdAfx.h"
#include "ioMagicSwordItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"
#include "FindPredicateImpl.h"
ioMagicSwordItem::ioMagicSwordItem(void)
{
	ClearData();
	m_fCurGauge = 0.0f;
}

ioMagicSwordItem::ioMagicSwordItem( const ioMagicSwordItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackKeyAttribute( rhs.m_AttackKeyAttribute ),
m_DefenseKeyAttribute( rhs.m_DefenseKeyAttribute ),
m_JumpKeyAttribute( rhs.m_JumpKeyAttribute ),
m_fChargeAutoTragetRange( rhs.m_fChargeAutoTragetRange ),
m_fChargeAutoTragetMinAngle( rhs.m_fChargeAutoTragetMinAngle ),
m_fChargeAutoTragetMaxAngle( rhs.m_fChargeAutoTragetMaxAngle ),
m_NormalMagicSwordStateInfo( rhs.m_NormalMagicSwordStateInfo ),
m_DashMagicSwordStateInfo( rhs.m_DashMagicSwordStateInfo ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	ClearData();
	m_fCurGauge = 0.0f;
}

ioMagicSwordItem::~ioMagicSwordItem(void)
{
}

void ioMagicSwordItem::ClearData()
{
	m_ChargeState		= CGS_NONE;
	m_iCurCombo			= 0;
	m_dwMotionStartTime	= 0;
	m_dwMotionEndTime	= 0;
	ClearMagicSwordData();

	m_iCurMagicSwordElementCount = 0;
	m_CurMagicSwordStateInfo.Init();
}

void ioMagicSwordItem::ClearMagicSwordData()
{
	m_fCurMagicSwordAttackMoveSpeed = 0.0f;
	m_vCurMagicSwordAttackMoveDir	= ioMath::VEC3_ZERO;
	m_fCurMaxRange					= 0.0f;
	m_fCurRange						= 0.0f;
	m_bHitCharByMoveAttack = false;
}

ioWeaponItem::WeaponSubType ioMagicSwordItem::GetSubType() const
{
	return WST_MAGIC_SWORD_ITEM;
}

ioItem* ioMagicSwordItem::Clone()
{
	return new ioMagicSwordItem( *this );
}

void ioMagicSwordItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//Elemental Attack
	LoadAttackAttribute_e( "attack_key_attack", m_AttackKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "defense_key_attack", m_DefenseKeyAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_key_attack", m_JumpKeyAttribute, rkLoader );

	//Magic Sword State
	LoadMagicSwordStateInfoProperty( rkLoader );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", 0.0f );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", 0.0f );
}

void ioMagicSwordItem::LoadMagicSwordStateInfoProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_fChargeAutoTragetRange = rkLoader.LoadFloat_e( "charge_auto_target_range", 0.0f );
	m_fChargeAutoTragetMinAngle = rkLoader.LoadFloat_e( "charge_auto_target_min_angle", 0.0f );
	m_fChargeAutoTragetMaxAngle = rkLoader.LoadFloat_e( "charge_auto_target_max_angle", 0.0f );

	//Normal
	m_NormalMagicSwordStateInfo.Init();

	rkLoader.LoadString_e( "magic_sword_move_attack_ani", "", szBuf, MAX_PATH );
	m_NormalMagicSwordStateInfo.m_szMoveAttackAni = szBuf;
	m_NormalMagicSwordStateInfo.m_fMoveAttackRate = rkLoader.LoadFloat_e( "magic_sword_move_attack_rate", 0.0f );
	m_NormalMagicSwordStateInfo.m_iMoveAttackWeaponIdx  = rkLoader.LoadInt_e( "magic_sword_move_attack_weapon_index", 0.0f );
	m_NormalMagicSwordStateInfo.m_dwMoveAttackLoopDuration  = (DWORD)rkLoader.LoadInt_e( "magic_sword_move_attack_loop_duration", 0.0f );
	m_NormalMagicSwordStateInfo.m_fMoveAttackSpeed = rkLoader.LoadFloat_e( "move_attack_speed", 0.0f );
	m_NormalMagicSwordStateInfo.m_fMaxMoveRange = rkLoader.LoadFloat_e( "max_move_range", 0.0f );

	LoadAttackAttribute_e( "magic_sword_bash_attack", m_NormalMagicSwordStateInfo.m_BashAttack, rkLoader );

	m_NormalMagicSwordStateInfo.m_ElementComboAttack.clear();
	for( int i=0 ; i<100 ; ++i )
	{
		AttackAttribute AttackAttri;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "magic_sword_element_attack%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		LoadAttackAttribute( szKey, AttackAttri, rkLoader );
		if ( AttackAttri.m_AttackAnimation.IsEmpty() )
			break;
		m_NormalMagicSwordStateInfo.m_ElementComboAttack.push_back( AttackAttri );
	}

	for( int i=0 ; i<3 ; ++i )
	{
		ioHashString szAttack;
		//Attack Key
		if ( i == 0 )
			szAttack = "magic_sword_element_attack_key";
		else if ( i == 1 )
			szAttack = "magic_sword_element_defense_key";
		else
			szAttack = "magic_sword_element_jump_key";

		wsprintf_e( szKey, "%s_type_count", szAttack.c_str() );
		int iCount = rkLoader.LoadInt( szKey, 0 );

		for ( int j=0 ; j<iCount ; ++j )
		{
			WeaponInfo kInfo;

			ZeroMemory( szKey, MAX_PATH );
			ZeroMemory( szBuf, MAX_PATH );
			wsprintf_e( szKey, "%s_type%d", szAttack.c_str(), j+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "%s_type%d_resist", szAttack.c_str(), j+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "%s_type%d_wounded", szAttack.c_str(), j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack.c_str(), j+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szAttack.c_str(), j+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			if ( i == 0 )
				m_NormalMagicSwordStateInfo.m_vAttackKeyWeaponInfoList.push_back( kInfo );
			else if ( i == 1 )
				m_NormalMagicSwordStateInfo.m_vDefenseKeyWeaponInfoList.push_back( kInfo );
			else
				m_NormalMagicSwordStateInfo.m_vJumpKeyWeaponInfoList.push_back( kInfo );
		}
	}

	//Dash

	m_DashMagicSwordStateInfo.Init();
	rkLoader.LoadString_e( "dash_magic_sword_move_attack_ani", "", szBuf, MAX_PATH );
	m_DashMagicSwordStateInfo.m_szMoveAttackAni = szBuf;
	m_DashMagicSwordStateInfo.m_fMoveAttackRate = rkLoader.LoadFloat_e( "dash_magic_sword_move_attack_rate", 0.0f );
	m_DashMagicSwordStateInfo.m_iMoveAttackWeaponIdx  = rkLoader.LoadInt_e( "dash_magic_sword_move_attack_weapon_index", 0.0f );
	m_DashMagicSwordStateInfo.m_dwMoveAttackLoopDuration  = (DWORD)rkLoader.LoadInt_e( "dash_magic_sword_move_attack_loop_duration", 0.0f );
	m_DashMagicSwordStateInfo.m_fMoveAttackSpeed = rkLoader.LoadFloat_e( "dash_move_attack_speed", 0.0f );
	m_DashMagicSwordStateInfo.m_fMaxMoveRange = rkLoader.LoadFloat_e( "dash_max_move_range", 0.0f );

	LoadAttackAttribute_e( "dash_magic_sword_bash_attack", m_DashMagicSwordStateInfo.m_BashAttack, rkLoader );
	m_DashMagicSwordStateInfo.m_ElementComboAttack.clear();
	for( int i=0 ; i<100 ; ++i )
	{
		AttackAttribute AttackAttri;
		ZeroMemory( szKey, MAX_PATH );
		ZeroMemory( szBuf, MAX_PATH );
		wsprintf_e( szKey, "dash_magic_sword_element_attack%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		LoadAttackAttribute( szKey, AttackAttri, rkLoader );
		if ( AttackAttri.m_AttackAnimation.IsEmpty() )
			break;
		m_DashMagicSwordStateInfo.m_ElementComboAttack.push_back( AttackAttri );
	}

	for( int i=0 ; i<3 ; ++i )
	{
		ioHashString szAttack;
		//Attack Key
		if ( i == 0 )
			szAttack = "dash_magic_sword_element_attack_key";
		else if ( i == 1 )
			szAttack = "dash_magic_sword_element_defense_key";
		else
			szAttack = "dash_magic_sword_element_jump_key";

		wsprintf_e( szKey, "%s_type_count", szAttack.c_str() );
		int iCount = rkLoader.LoadInt( szKey, 0 );

		for ( int j=0 ; j<iCount ; ++j )
		{
			ZeroMemory( szKey, MAX_PATH );
			ZeroMemory( szBuf, MAX_PATH );

			WeaponInfo kInfo;

			wsprintf_e( szKey, "%s_type%d", szAttack.c_str(), j+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "%s_type%d_resist", szAttack.c_str(), j+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szKey, "%s_type%d_wounded", szAttack.c_str(), j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack.c_str(), j+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szAttack.c_str(), j+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			if ( i == 0 )
				m_DashMagicSwordStateInfo.m_vAttackKeyWeaponInfoList.push_back( kInfo );
			else if ( i == 1 )
				m_DashMagicSwordStateInfo.m_vDefenseKeyWeaponInfoList.push_back( kInfo );
			else
				m_DashMagicSwordStateInfo.m_vJumpKeyWeaponInfoList.push_back( kInfo );
		}
	}
}

void ioMagicSwordItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_fCurChargeRate = 0.0f;

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CGS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CGS_CHARGING;
		m_dwMotionStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioMagicSwordItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ChargeState )
	{
	case CGS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CGS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	case CGS_ELEMENT_ATTACK:
		ProcessElementalAttack( pOwner );
		break;
	}
}

void ioMagicSwordItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	
		return;

	if( IsEnableMagicSwordGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetMagicSwordAttackState( pOwner, ECS_NORMAL );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioMagicSwordItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	//
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();
	m_dwKeyReserveTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
			pOwner->SetAutoTarget( ATT_NORMAL );

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		float fTimeRate = pAttribute->m_fAttackAniRate;
		DWORD dwPreDelay = pAttribute->m_dwPreDelay;
		float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserve > 0.0f )
			m_dwKeyReserveTime = FRAMEGETTIME() + fReserve + dwPreDelay;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CGS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_NORMAL_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicSwordItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		bool bCheckCombo = false;
		if( m_iCurCombo == GetMaxCombo()-1 )
			bCheckCombo = true;

		if( bCheckCombo && m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
			if ( m_bEnableDefenseState || m_bEnableAttackState || m_bEnableJumpState )
			{
				ChangeToNormalElementAttack( pOwner );
				return;
			}
		}
	}
	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioMagicSwordItem::ChangeToNormalElementAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	bool bAttackKey = m_bEnableAttackState;
	bool bDefenseKey = m_bEnableDefenseState;
	bool bJumpKey = m_bEnableJumpState;

	int iAniID = -1;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	AttackAttribute Attr;
	Attr.Init();

	if ( m_bEnableAttackState )
		Attr = m_AttackKeyAttribute;
	else if ( m_bEnableDefenseState )
		Attr = m_DefenseKeyAttribute;
	else if ( m_bEnableJumpState )
		Attr = m_JumpKeyAttribute;
	
	ClearCancelInfo();

	pOwner->SetNormalAttackByAttribute( Attr );
	iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}

	fTimeRate = Attr.m_fAttackAniRate;
	dwPreDelay = Attr.m_dwPreDelay;
	dwEndDelay = Attr.m_dwEndDelay;
		
	m_ChargeState = CGS_ELEMENT_ATTACK;
	

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwMotionEndTime += dwEndDelay;

	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fReserve + dwPreDelay;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = FRAMEGETTIME();
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	SetCurAttackAniForInputJump( Attr.m_AttackAnimation, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_ELEMENT_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << bAttackKey;
		kPacket << bDefenseKey;
		kPacket << bJumpKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicSwordItem::ProcessElementalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	CheckCancelReserve( pOwner );

	if( ProcessCancel(pOwner) )
		return;

	if( m_dwMotionEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioMagicSwordItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

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

void ioMagicSwordItem::SetMagicSwordAttackState( ioBaseChar *pOwner, ExtendChargeState ExtendState )
{
	if( !pOwner ) 
		return;

	pOwner->SetState( CS_MAGIC_SWORD_ATTACK );
	
	ClearMagicSwordData();
	m_iCurMagicSwordElementCount = 0;
	m_CurMagicSwordStateInfo.Init();

	if ( ExtendState == ECS_NORMAL )
		m_CurMagicSwordStateInfo = m_NormalMagicSwordStateInfo;
	else if ( ExtendState == ECS_DASH )
		m_CurMagicSwordStateInfo = m_DashMagicSwordStateInfo;

	m_fCurMaxRange = m_CurMagicSwordStateInfo.m_fMaxMoveRange;
	m_fCurMagicSwordAttackMoveSpeed = m_CurMagicSwordStateInfo.m_fMoveAttackSpeed;
	

	ChangeToMoveAttack( pOwner );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_MOVE_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket <<static_cast<byte>( ExtendState );
		kPacket << m_vCurMagicSwordAttackMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	DecreaseGauge();
}

void ioMagicSwordItem::ChangeToMoveAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;


	DWORD dwCurTime = FRAMEGETTIME();

	ClearCancelInfo();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;

	m_ChargeState = CGS_MOVE_ATTACK;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurMagicSwordStateInfo.m_szMoveAttackAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_CurMagicSwordStateInfo.m_fMoveAttackRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	m_dwMotionStartTime = dwCurTime;
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	
	if( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		const ioBaseChar* pTraget = TargetSearch( pOwner, m_fChargeAutoTragetMinAngle, m_fChargeAutoTragetMaxAngle, m_fChargeAutoTragetRange );
		if ( pTraget )
		{
			D3DXVECTOR3 vAimedDir = pTraget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vAimedDir, &vAimedDir );

			vMoveDir = D3DXVECTOR3( vAimedDir.x, 0.0f, vAimedDir.z );
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );

			pOwner->SetTargetRotToDir( vMoveDir, true );
		}
		
		m_vCurMagicSwordAttackMoveDir = vMoveDir;
	}

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx = m_CurMagicSwordStateInfo.m_iMoveAttackWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
}

const ioBaseChar* ioMagicSwordItem::TargetSearch( ioBaseChar *pOwner, float fTargetMinAngle, float fTargetMaxAngle, float fTargetRange )
{
	if ( !pOwner )
		return NULL;
	ioPlayStage* pState = pOwner->GetCreator();
	if ( !pState )
		return NULL;

	FD_AutoTarget kAutoPred( pOwner,
		fTargetMinAngle,
		fTargetMaxAngle,
		fTargetRange );

	const ioBaseChar *pTarget = pState->FindUser( kAutoPred );
	if ( !pTarget )
		return NULL;

	return pTarget;
}

void ioMagicSwordItem::ProcessMagicSwordAttackState( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CGS_MOVE_ATTACK:
		ProcessMoveAttack( pOwner );
		break;
	case CGS_BASH_ATTACK:
		ProcessBashAttack( pOwner );
		break;
	case CGS_MAGIC_SWORD_ELEMENT_ATTACK:
		ProcessMagicSwordElementAttack( pOwner );
		break;
	}

	ProcessMagicSwordMove( pOwner );
}

void ioMagicSwordItem::ProcessMoveAttack( ioBaseChar *pOwner )
{
	if ( m_bHitCharByMoveAttack )
	{
		ChangeToBashAttack( pOwner );
		return;
	}
	//RangeCheck
	if ( m_fCurMaxRange < m_fCurRange )
	{
		ChangeToBashAttack( pOwner );
		return;
	}

	if ( m_dwMotionStartTime + m_CurMagicSwordStateInfo.m_dwMoveAttackLoopDuration < FRAMEGETTIME() )
	{
		ChangeToBashAttack( pOwner );
		return;
	}
}

void ioMagicSwordItem::ProcessMagicSwordMove( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );


	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fCurMagicSwordAttackMoveSpeed > 0.0f && m_vCurMagicSwordAttackMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fCurMagicSwordAttackMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vCurMagicSwordAttackMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
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

			if ( m_fCurMaxRange > 0.0f )
				m_fCurRange += D3DXVec3Length( &vTotalMove );

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

void ioMagicSwordItem::ChangeToBashAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	//pOwner->SetCurJumpPowerAmt( 0.0f );
	//pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	ClearMagicSwordData();
	ClearCancelInfo();
	m_iCurMagicSwordElementCount = 0;

	const AttackAttribute& Attr = m_CurMagicSwordStateInfo.m_BashAttack;
	int iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate = FLOAT1;
	if ( Attr.m_fAttackAniRate > 0.0f )
		fTimeRate = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, Attr.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	
	if( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );;
		const ioBaseChar* pTraget = TargetSearch( pOwner, m_fChargeAutoTragetMinAngle, m_fChargeAutoTragetMaxAngle, m_fChargeAutoTragetRange );
		if ( pTraget )
		{
			D3DXVECTOR3 vAimedDir = pTraget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vAimedDir, &vAimedDir );

			vMoveDir = D3DXVECTOR3( vAimedDir.x, 0.0f, vAimedDir.z );
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );
			pOwner->SetTargetRotToDir( vMoveDir, true );
		}
	}

	

 	m_ChargeState = CGS_BASH_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwMotionEndTime += dwEndDelay;

	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fReserve + dwPreDelay;

	SetCurAttackAniForInputJump( Attr.m_AttackAnimation, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_BASH_ATTACK );
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicSwordItem::ProcessBashAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
			if ( ( m_bEnableDefenseState || m_bEnableAttackState || m_bEnableJumpState ) &&
				!m_CurMagicSwordStateInfo.m_ElementComboAttack.empty() )
			{
				ChangeToMagicSwordElementAttack( pOwner );
				return;
			}
		}
	}
}

void ioMagicSwordItem::ChangeToMagicSwordElementAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if ( !COMPARE( m_iCurMagicSwordElementCount, 0, (int)m_CurMagicSwordStateInfo.m_ElementComboAttack.size() ) )
		return;

	WeaponInfoList weaponInfo;
	if ( m_bEnableAttackState )
		weaponInfo = m_CurMagicSwordStateInfo.m_vAttackKeyWeaponInfoList;
	else if ( m_bEnableDefenseState )
		weaponInfo = m_CurMagicSwordStateInfo.m_vDefenseKeyWeaponInfoList;
	else if ( m_bEnableJumpState )
		weaponInfo = m_CurMagicSwordStateInfo.m_vJumpKeyWeaponInfoList;
	
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	//pOwner->SetCurJumpPowerAmt( 0.0f );
	//pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	ClearMagicSwordData();

	AttackAttribute Attr = m_CurMagicSwordStateInfo.m_ElementComboAttack[m_iCurMagicSwordElementCount];

	int iAniID = pGrp->GetAnimationIdx( Attr.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Animation is Not Exist", __FUNCTION__, Attr.m_AttackAnimation.c_str() );
		return;
	}

	m_ChargeState = CGS_MAGIC_SWORD_ELEMENT_ATTACK;

	float fTimeRate = FLOAT1;
	if ( Attr.m_fAttackAniRate > 0.0f )
		fTimeRate = Attr.m_fAttackAniRate;
	DWORD dwPreDelay = Attr.m_dwPreDelay;
	DWORD dwEndDelay = Attr.m_dwEndDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID, weaponInfo, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	pOwner->SetReservedSliding( Attr.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwMotionEndTime += dwEndDelay;

	m_dwKeyReserveTime = 0;
	float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fReserve > 0.0f )
		m_dwKeyReserveTime = dwCurTime + fReserve + dwPreDelay;

	SetCurAttackAniForInputJump( Attr.m_AttackAnimation, fTimeRate );


	
	if( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eNewDirKey = ioUserKeyInput::DKI_NONE;
			eNewDirKey = pOwner->CheckCurDirKey();
			pOwner->SetTargetRotToDirKey( eNewDirKey );
		}

		D3DXVECTOR3 vMoveDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );;
		const ioBaseChar* pTraget = TargetSearch( pOwner, m_fChargeAutoTragetMinAngle, m_fChargeAutoTragetMaxAngle, m_fChargeAutoTragetRange );
		if ( pTraget )
		{
			D3DXVECTOR3 vAimedDir = pTraget->GetWorldPosition() - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vAimedDir, &vAimedDir );

			vMoveDir = D3DXVECTOR3( vAimedDir.x, 0.0f, vAimedDir.z );
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );

			pOwner->SetTargetRotToDir( vMoveDir, true );
		}
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << static_cast<byte>( SS_MAGIC_SWORD_ELEMENT_ATTACK );
		kPacket << pOwner->GetTargetRot();
		kPacket << m_bEnableAttackState;
		kPacket << m_bEnableDefenseState;
		kPacket << m_bEnableJumpState;
		kPacket << m_iCurMagicSwordElementCount;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ClearCancelInfo();
	m_iCurMagicSwordElementCount++;
}

void ioMagicSwordItem::ProcessMagicSwordElementAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;

		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
			if ( ( m_bEnableDefenseState || m_bEnableAttackState || m_bEnableJumpState ) 
				&& m_iCurMagicSwordElementCount < (int)m_CurMagicSwordStateInfo.m_ElementComboAttack.size() )
			{
				ChangeToMagicSwordElementAttack( pOwner );
				return;
			}
		}
	}
}

void ioMagicSwordItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		{
			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			if( m_fCurGauge < m_fMaxGauge )
			{
				float fGauge = m_fIncreaseGauge * fTimePerSec;

				m_fCurGauge += fGauge;
				m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
			}
		}
		return;
	}
}

int ioMagicSwordItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioMagicSwordItem::GetCurBullet()
{
	return (int)m_fCurGauge;
}

int ioMagicSwordItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

bool ioMagicSwordItem::IsEnableMagicSwordGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioMagicSwordItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioMagicSwordItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKey() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_bEnableDashState = false;
		m_ConditionalSpecialDashKey = ioUserKeyInput::DKI_NONE;
		m_iSkillInput = -1;
	}
}

void ioMagicSwordItem::ProcessReserveKeyInput( ioBaseChar *pOwner )
{
	// 필살기 예약되었으면 필살기 발동
	if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
	{
		m_iSkillInput = -1;
		return;
	}

	// 방어
	if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
	{
		pOwner->SetDefenseState();
		return;
	}

	// 점프
	if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// 공격
	if( m_bEnableAttackState )
	{
		ioObjectItem *pItem = pOwner->GetObject();

		if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
		{
			pOwner->SetNormalAttack( 0 );
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioMagicSwordItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	byte eState;
	rkPacket >> eState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	switch( eState )
	{
	case SS_NORMAL_ATTACK:
		{	
			rkPacket >> m_iCurCombo;
			ChangeToNormalAttack( pOwner );
		}
		break;
	case SS_ELEMENT_ATTACK:
		{
			rkPacket >> m_bEnableAttackState;
			rkPacket >> m_bEnableDefenseState;
			rkPacket >> m_bEnableJumpState;
			ChangeToNormalElementAttack( pOwner );
		}
		break;;
	case SS_MOVE_ATTACK:
		{
			ClearMagicSwordData();
			m_iCurMagicSwordElementCount = 0;
			m_CurMagicSwordStateInfo.Init();

			byte ExtendState;
			rkPacket >> ExtendState;
			rkPacket >> m_vCurMagicSwordAttackMoveDir;
			pOwner->SetState( CS_MAGIC_SWORD_ATTACK );

			if ( ExtendState == ECS_NORMAL )
				m_CurMagicSwordStateInfo = m_NormalMagicSwordStateInfo;
			else if ( ExtendState == ECS_DASH )
				m_CurMagicSwordStateInfo = m_DashMagicSwordStateInfo;

			m_fCurMaxRange = m_CurMagicSwordStateInfo.m_fMaxMoveRange;
			m_fCurMagicSwordAttackMoveSpeed = m_CurMagicSwordStateInfo.m_fMoveAttackSpeed;

			ChangeToMoveAttack( pOwner );
		}
		break;
	case SS_BASH_ATTACK:
		ChangeToBashAttack( pOwner );
		break;
	case SS_MAGIC_SWORD_ELEMENT_ATTACK:
		{
			rkPacket >> m_bEnableAttackState;
			rkPacket >> m_bEnableDefenseState;
			rkPacket >> m_bEnableJumpState;
			rkPacket >> m_iCurMagicSwordElementCount;
			ChangeToMagicSwordElementAttack( pOwner );
		}
		break;
	case SS_HIT_CHAR_BY_MOVE_ATTACK:
		m_bHitCharByMoveAttack = true;
		break;
	}
}

void ioMagicSwordItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( pWoundChar->IsNeedProcess() && m_ChargeState == CGS_MOVE_ATTACK )
	{
		if ( m_bHitCharByMoveAttack )
			return;

		if ( pWoundChar && pAttacker )
		{
			m_bHitCharByMoveAttack = true;
			if ( pWoundChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pAttacker, kPacket );
				kPacket << static_cast<byte>( SS_HIT_CHAR_BY_MOVE_ATTACK );
				kPacket << pAttacker->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

//----------------------------------------------------------------------------------------------
ioMagicSwordSpecialState::ioMagicSwordSpecialState()
{

}

ioMagicSwordSpecialState::~ioMagicSwordSpecialState()
{

}

void ioMagicSwordSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioMagicSwordItem* pMagicSwordItem = ToMagicSwordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pMagicSwordItem )
	{
		pMagicSwordItem->ProcessMagicSwordAttackState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioMagicSwordSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return false;
}
