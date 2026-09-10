
#include "stdafx.h"

#include "ioGhostItem.h"
#include "WeaponDefine.h"

ioGhostItem::ioGhostItem()
{
	m_ChargeState = CS_NONE;
	m_GhostFlyState = PFS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fGhostFlyStartRange = 0.0f;

	// target
	m_GhostTarget.Clear();

	// key
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	// Ghost gauge
	m_fCurGhostGauge = 0.0f;

	// etc
	m_bUseGhostFly = false;
	m_dwEnableGhostEffect = 0;
	m_dwGhostFlyEnableTime = 0;

	m_dwMotionEndTime = 0;
}

ioGhostItem::ioGhostItem( const ioGhostItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_GhostInfo( rhs.m_GhostInfo ),
m_EnableGhostEffect( rhs.m_EnableGhostEffect ),
m_fMaxGhostGauge( rhs.m_fMaxGhostGauge ),
m_fNeedGhostGauge( rhs.m_fNeedGhostGauge ),
m_fIncreaseGhostGauge( rhs.m_fIncreaseGhostGauge ),
m_fDecreaseGhostGauge( rhs.m_fDecreaseGhostGauge ),
m_fFirstDecreaseGhostGauge( rhs.m_fFirstDecreaseGhostGauge ),
m_fDefenseDashDecreaseGhostGauge( rhs.m_fDefenseDashDecreaseGhostGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseGhostGauge( rhs.m_fHighDecreaseGhostGauge ),
m_fGhostEndJumpHeightGap( rhs.m_fGhostEndJumpHeightGap ),
m_GhostMoveAttack( rhs.m_GhostMoveAttack )
{
	m_ChargeState = CS_NONE;
	m_GhostFlyState = PFS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fGhostFlyStartRange = 0.0f;

	// target
	m_GhostTarget.Clear();

	// key
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	// Ghost gauge
	m_fCurGhostGauge = 0.0f;

	// etc
	m_bUseGhostFly = false;
	m_dwEnableGhostEffect = 0;
	m_dwGhostFlyEnableTime = 0;

	m_dwMotionEndTime = 0;
}

ioGhostItem::~ioGhostItem()
{
}

void ioGhostItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadGhostFlyInfo( rkLoader );

	m_fMaxGhostGauge = rkLoader.LoadFloat_e( "max_ghost_gauge", FLOAT100 );
	m_fNeedGhostGauge = rkLoader.LoadFloat_e( "need_ghost_gauge", FLOAT100 );
	m_fIncreaseGhostGauge = rkLoader.LoadFloat_e( "increase_ghost_gauge", FLOAT1 );
	m_fDecreaseGhostGauge = rkLoader.LoadFloat_e( "decrease_ghost_gauge", 2.0f );
	m_fFirstDecreaseGhostGauge = rkLoader.LoadFloat_e( "first_decrease_ghost_gauge", 2.0f );
	m_fDefenseDashDecreaseGhostGauge = rkLoader.LoadFloat_e( "defense_dash_decrease_ghost_gauge", 2.0f );
	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGhostGauge = rkLoader.LoadFloat_e( "high_decrease_ghost_gauge", 3.0f );
	m_fGhostEndJumpHeightGap = rkLoader.LoadFloat_e( "ghost_end_jump_height_gap", FLOAT10 );

	rkLoader.LoadString_e( "enable_ghost_effect", "", szBuf, MAX_PATH );
	m_EnableGhostEffect = szBuf;
	
	LoadAttackAttribute( "ghost_move_attack", m_GhostMoveAttack, rkLoader );
}

void ioGhostItem::LoadGhostFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_GhostInfo.Init();

	rkLoader.LoadString_e( "ghost_wait_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_WaitAni = szBuf;
	m_GhostInfo.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "ghost_wait_duration", 0 );

	rkLoader.LoadString_e( "ghost_start_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_StartAni = szBuf;
	m_GhostInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "ghost_start_ani_rate", FLOAT1 );
	m_GhostInfo.m_fAlphaRate = rkLoader.LoadFloat_e( "ghost_alpha_rate", FLOAT1 );
	m_GhostInfo.m_fStartAngle = rkLoader.LoadFloat_e( "ghost_start_angle", 0.0f );
	m_GhostInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "ghost_start_max_range", 0.0f );
	m_GhostInfo.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "ghost_start_duration", 0 );

	rkLoader.LoadString_e( "ghost_delay_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_DelayAni = szBuf;
	m_GhostInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "ghost_delay_ani_rate", FLOAT1 );

	m_GhostInfo.m_dwPushDelayTime = rkLoader.LoadInt_e( "ghost_push_delay_time", 0 );	
	m_GhostInfo.m_dwPushOffset = rkLoader.LoadInt_e( "ghost_push_offset", 0 );
	rkLoader.LoadString_e( "ghost_push_effect", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szPushEffect = szBuf;
	rkLoader.LoadString_e( "ghost_push_attack_buff", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szPushAttackBuff = szBuf;
	rkLoader.LoadString_e( "ghost_push_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_PushAni = szBuf;
	m_GhostInfo.m_fPushAniRate = rkLoader.LoadFloat_e( "ghost_push_ani_rate", FLOAT1 );
	m_GhostInfo.m_fGhostPushEndJumpRate = rkLoader.LoadFloat_e( "ghost_push_end_jump_rate", FLOAT1 );

	m_GhostInfo.m_dwDashDelayTime = rkLoader.LoadInt_e( "ghost_dash_delay_time", 0 );
	m_GhostInfo.m_dwDashOffset = rkLoader.LoadInt_e( "ghost_dash_offset", 0 );
	rkLoader.LoadString_e( "ghost_dash_effect", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szDashEffect = szBuf;
	rkLoader.LoadString_e( "ghost_dash_attack_buff", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szDashAttackBuff = szBuf;
	rkLoader.LoadString_e( "ghost_dash_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_DashAni = szBuf;
	m_GhostInfo.m_fDashAniRate = rkLoader.LoadFloat_e( "ghost_dash_ani_rate", FLOAT1 );
	m_GhostInfo.m_fGhostDashEndJumpRate = rkLoader.LoadFloat_e( "ghost_dash_end_jump_rate", FLOAT1 );

	m_GhostInfo.m_dwJumpDelayTime = rkLoader.LoadInt_e( "ghost_jump_delay_time", 0 );	
	m_GhostInfo.m_dwJumpOffset = rkLoader.LoadInt_e( "ghost_jump_offset", 0 );
	rkLoader.LoadString_e( "ghost_jump_effect", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szJumpEffect = szBuf;
	rkLoader.LoadString_e( "ghost_jump_attack_buff", "", szBuf, MAX_PATH );
	m_GhostInfo.m_szJumpAttackBuff = szBuf;
	rkLoader.LoadString_e( "ghost_jump_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_JumpAni = szBuf;
	m_GhostInfo.m_fJumpAniRate = rkLoader.LoadFloat_e( "ghost_jump_ani_rate", FLOAT1 );
	m_GhostInfo.m_fGhostJumpEndJumpRate = rkLoader.LoadFloat_e( "ghost_jump_end_jump_rate", FLOAT1 );

	_ENCSTR("ghost_push_attack", szPushWeapon);
	_ENCSTR("ghost_dash_attack", szDashWeapon);
	_ENCSTR("ghost_jump_attack", szJumpWeapon);
	LoadGhostWeaponInfo( szPushWeapon, m_GhostInfo.m_GhostPushWeaponList, rkLoader );
	LoadGhostWeaponInfo( szDashWeapon, m_GhostInfo.m_GhostDashWeaponList, rkLoader );
	LoadGhostWeaponInfo( szJumpWeapon, m_GhostInfo.m_GhostJumpWeaponList, rkLoader );

	rkLoader.LoadString_e( "ghost_move_ani", "", szBuf, MAX_PATH );
	m_GhostInfo.m_MoveAni = szBuf;
	m_GhostInfo.m_fMoveAniRate = rkLoader.LoadFloat_e( "ghost_move_ani_rate", FLOAT1 );
	m_GhostInfo.m_fMoveSpeed = rkLoader.LoadFloat_e( "ghost_move_speed", FLOAT100 );
	m_GhostInfo.m_fUpDownSpeed = rkLoader.LoadFloat_e( "ghost_updown_speed", FLOAT100 );

	rkLoader.LoadString_e( "ghost_possession", "", szBuf, MAX_PATH );
	m_GhostInfo.m_PossessionAni = szBuf;
	m_GhostInfo.m_fPossessionRate = rkLoader.LoadFloat_e( "ghost_possession_rate", FLOAT1 );
	m_GhostInfo.m_dwPossessionObjectItem = rkLoader.LoadInt_e( "ghost_possession_object", 0 );
	m_GhostInfo.m_dwPossessionDuration = rkLoader.LoadInt_e( "ghost_possession_duration", 0 );
	m_GhostInfo.m_dwPossessionInputTime = rkLoader.LoadInt_e( "ghost_possession_input_time", 100 );

	int iBuffCnt = rkLoader.LoadInt_e( "ghost_possession_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "ghost_possession_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_GhostInfo.m_vPossessionBuff.push_back(szBuff);
	}
	iBuffCnt = rkLoader.LoadInt_e( "ghost_possession_end_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "ghost_possession_end_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_GhostInfo.m_vPossessionEndBuff.push_back(szBuff);
	}
	iBuffCnt = rkLoader.LoadInt_e( "ghost_possession_owner_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "ghost_possession_owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_GhostInfo.m_vPossessionOwnerBuff.push_back(szBuff);
	}

	iBuffCnt = rkLoader.LoadInt_e( "ghost_possession_remove_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "ghost_possession_remove_buff%d", i+1 );
		int iBuffType = rkLoader.LoadInt( szKey, 0 );

		m_GhostInfo.m_vPossessionRemoveBuff.push_back( iBuffType );
	}

	rkLoader.LoadString_e( "ghost_defense_dash", "", szBuf, MAX_PATH );
	m_GhostInfo.m_DefenseDashAni = szBuf;
	m_GhostInfo.m_fDefenseDashAniRate = rkLoader.LoadFloat_e( "ghost_defense_dash_rate", FLOAT1 );
	iBuffCnt = rkLoader.LoadInt_e( "ghost_defense_dash_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "ghost_defense_dash_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_GhostInfo.m_vDefenseDashBuff.push_back(szBuff);
	}

	m_GhostInfo.m_fGhostFlyJumpAmt = rkLoader.LoadFloat_e( "ghost_jump_amt", 0.0f );

	rkLoader.LoadString_e( "ghost_effect", "", szBuf, MAX_PATH );
	m_GhostInfo.m_GhostFlyEffect = szBuf;
	m_GhostInfo.m_dwGhostFlyEnableTime = (DWORD)rkLoader.LoadInt_e( "ghost_enable_time", 0 );
}

void ioGhostItem::LoadGhostWeaponInfo( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_type_count", szAttack );
	int iCnt = rkLoader.LoadInt( szKey, 0 );
	WeaponInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "%s_type%d", szAttack, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szAttack, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szAttack, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_loop_ani", szAttack, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack, i+1 );
		kInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

		WeaponInfoList.push_back( kInfo );
	}
}

ioItem* ioGhostItem::Clone()
{
	return new ioGhostItem( *this );
}

ioWeaponItem::WeaponSubType ioGhostItem::GetSubType() const
{
	return WST_GHOST_ITEM;
}

void ioGhostItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearGhostState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
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

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
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

void ioGhostItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner )
			pOwner->CheckExtraAniJump();

		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->CheckExtendAttackEndJump() )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

bool ioGhostItem::CheckGhostFlyEnableTime()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGhostFlyEnableTime > 0 && dwCurTime < m_dwGhostFlyEnableTime )
		return false;

	return true;
}

void ioGhostItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckGhostFlyEnableTime() )
	{
		ChangeToNormalAttack( pOwner );
		return;
	}

	if( IsEnableGhostGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			SetGhostFlyState( pOwner, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioGhostItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->InitExtendAttackTagInfo();

	m_fCurChargeRate = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
			float fTimeRate = pAttribute->m_fAttackAniRate;
			DWORD dwPreDelay = pAttribute->m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
		}
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioGhostItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnableGhostEffect( pOwner );
			SetCurGhostGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		CheckGhostFlyState( pOwner, rkPacket );
		break;
	case SST_MOVE_KEY:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;
			rkPacket >> bSetUpKey >> bSetDownKey;			

			if( bChangeDir )
			{
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
			CheckGhostFlyMoveAni( pOwner );
		}
		break;
	}
}

void ioGhostItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioGhostItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioGhostItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioGhostItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioGhostItem::GetAutoTargetValue( ioBaseChar *pOwner,
										  float &fRange, float &fMinAngle, float &fMaxAngle,
										  AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}


		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;

		return;
	}
}

void ioGhostItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;

		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( bChangeDir || bChangeUpDown )
	{
		CheckGhostFlyMoveAni( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MOVE_KEY;
			kPacket << bChangeDir;
			kPacket << bChangeUpDown;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGhostItem::SetGhostFlyStateAttack( ioBaseChar *pOwner )
{
	SetGhostFlyState( pOwner, false );
}

void ioGhostItem::SetPushState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_PUSH;
	m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwPushDelayTime;

	if( pOwner->IsNeedProcess() && bSendPacket )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}	

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget )
		return;

	CreateEffect( pTarget, m_GhostInfo.m_szPushEffect );
	pTarget->SetReturnJumpping( false );
	RelesePossessionObjectItem( pTarget );

	int iCnt = m_GhostInfo.m_vPossessionBuff.size();
	for( int i=0; i<iCnt; ++i )
		pTarget->RemoveBuff( m_GhostInfo.m_vPossessionBuff[i] );

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_GhostTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::SetDashState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_DASH;
	m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwDashDelayTime;
	if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
		m_GhostInfo.m_eDashDirKey = pOwner->CheckCurDirKey();

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget )
		return;

	CreateEffect( pTarget, m_GhostInfo.m_szDashEffect );
	pTarget->SetReturnJumpping( false );
	RelesePossessionObjectItem( pTarget );

	int iCnt = m_GhostInfo.m_vPossessionBuff.size();
	for( int i=0; i<iCnt; ++i )
		pTarget->RemoveBuff( m_GhostInfo.m_vPossessionBuff[i] );

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_GhostTarget;
		kPacket << (int)m_GhostInfo.m_eDashDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::SetJumpState( ioBaseChar *pOwner, bool bSendPacket )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_JUMP;
	m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwJumpDelayTime;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget )
		return;

	CreateEffect( pTarget, m_GhostInfo.m_szJumpEffect );
	pTarget->SetReturnJumpping( false );
	RelesePossessionObjectItem( pTarget );

	int iCnt = m_GhostInfo.m_vPossessionBuff.size();
	for( int i=0; i<iCnt; ++i )
		pTarget->RemoveBuff( m_GhostInfo.m_vPossessionBuff[i] );

	if( bSendPacket && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_GhostTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::CreateEffect( ioBaseChar *pChar, ioHashString szEffect )
{
	if( szEffect.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( pGrp )
		pGrp->AttachEffect( szEffect, NULL, ioMath::QUAT_IDENTITY );
}

void ioGhostItem::SetGhostFlyState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_GHOST_FLY );
	ReleaseEnableGhostEffect( pOwner );
	DecreaseGhostGauge( m_fFirstDecreaseGhostGauge );

	if( m_GhostInfo.m_WaitAni.IsEmpty() )
	{
		ChangeGhostFlyStart( pOwner );
		m_fGhostFlyStartRange = 0.0f;
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_WaitAni );
		pGrp->SetLoopAni( iAniID, FLOAT10 );

		m_GhostFlyState = PFS_WAIT;
		m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwWaitDuration;
		m_fGhostFlyStartRange = 0.0f;
	}

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGhostGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::ChangeGhostFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GhostInfo.m_StartAni );
	float fTimeRate  = m_GhostInfo.m_fStartAniRate;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_GhostFlyState = PFS_START;
	m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwStartDuration; 

	m_bUseGhostFly = true;
}

void ioGhostItem::ChangeGhostFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GhostInfo.m_DelayAni );
	float fTimeRate  = m_GhostInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_GhostFlyState = PFS_MOVE;
	m_CurMoveAni = m_GhostInfo.m_DelayAni;
	m_dwMotionEndTime = 0;
}

void ioGhostItem::ChangeGhostFlyMoveAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GhostMoveAttack.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GhostMoveAttack.m_fAttackAniRate );
	if( !m_GhostMoveAttack.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_GhostMoveAttack.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_GhostMoveAttack.m_fAttackAniRate, m_GhostMoveAttack.m_dwPreDelay );

	m_GhostFlyState = PFS_MOVE_ATTACK;
	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_GhostMoveAttack.m_fAttackAniRate);
	
	pOwner->CheckCharColSkipTime( iAniID, m_GhostMoveAttack.m_fAttackAniRate, m_GhostMoveAttack.m_dwPreDelay );
	pOwner->SetReservedSliding( m_GhostMoveAttack.m_vForceInfoList, iAniID, m_GhostMoveAttack.m_fAttackAniRate, m_GhostMoveAttack.m_dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::ChangeGhostFlyPossession( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( m_GhostFlyState != PFS_MOVE_ATTACK )
		return;

	if( !pOwner ) return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	m_GhostTarget = szTargetName;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget ) return;

	pTarget->ClearReservedSliding();
	pTarget->SetForcePowerAmt( 0.0f );
	pTarget->SetReturnJumpping( true );
	EquipPossessionObjectItem( pTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_PossessionAni );
	float fTimeRate = m_GhostInfo.m_fPossessionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	pOwner->SetWorldPosition( pTarget->GetWorldPosition() );
	pOwner->SetTargetRotToDir( -vDir, false );

	m_dwMotionEndTime = FRAMEGETTIME() + m_GhostInfo.m_dwPossessionDuration;
	m_dwInputTime = FRAMEGETTIME() + m_GhostInfo.m_dwPossessionInputTime;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );

	m_GhostFlyState = PFS_POSSESSION;

	int iCnt = m_GhostInfo.m_vPossessionBuff.size();
	for( int i=0; i<iCnt; ++i )
		pTarget->AddNewBuff( m_GhostInfo.m_vPossessionBuff[i], pOwner->GetCharName(), "", NULL );

	iCnt = m_GhostInfo.m_vPossessionRemoveBuff.size();
	for( int i=0; i<iCnt; ++i )
		pTarget->RemoveBuffType( m_GhostInfo.m_vPossessionRemoveBuff[i] );

	pTarget->SetPossessionChar( pOwner->GetCharName() );

	iCnt = m_GhostInfo.m_vPossessionOwnerBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->AddNewBuff( m_GhostInfo.m_vPossessionOwnerBuff[i], pOwner->GetCharName(), "", NULL );
	
	int iAlphaRate = MAX_ALPHA_RATE * m_GhostInfo.m_fAlphaRate;
	iAlphaRate = max( 1, min(iAlphaRate, MAX_ALPHA_RATE) );
	pOwner->SetAlphaRateDirect( iAlphaRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGhostItem::ProcessGhostFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_GhostFlyState )
	{
	case PFS_WAIT:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostFlyStart( pOwner );
			return;
		}
		break;
	case PFS_START:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostFlyMove( pOwner );
			return;
		}
		else if( !ProcessGhostFlyStart( pOwner ) )
		{
			ChangeGhostFlyMove( pOwner );
			return;
		}
		break;
	case PFS_MOVE:
		if( m_fCurGhostGauge <= 0.0f )
		{
			ChangeGhostFlyEnd( pOwner );
			return;
		}
		if( !ProcessGhostFlyMove( pOwner ) )
		{
			ChangeGhostFlyEnd( pOwner );
			return;
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKey() )
				{
					ChangeGhostFlyMoveAttack( pOwner );
					return;
				}
			}
			CheckKeyInput( pOwner );
		}
		break;
	case PFS_MOVE_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostFlyEnd( pOwner );
			return;
		}
		ProcessGhostFlyMoveAttack( pOwner );
		break;
	case PFS_POSSESSION:
		if( m_dwMotionEndTime < dwCurTime || !ProcessGhostFlyPossession( pOwner ) )
		{
			ChangeGhostFlyEnd( pOwner );
			return;
		}
		if( m_dwInputTime > dwCurTime )
			return;
		if( pOwner->IsAttackKey() )
		{
			SetPushState( pOwner, true );
			return;
		}
		else if( pOwner->IsDirKeyDoubleClick() )
		{
			SetDashState( pOwner, true );
			return;
		}
		else if( pOwner->IsJumpKey() )
		{
			SetJumpState( pOwner, true );
			return;
		}
		break;
	case PFS_PUSH:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionEndTime = 0;
			SetPushAttackState( pOwner );
		}
		break;
	case PFS_PUSH_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostAttackEnd( pOwner, m_GhostInfo.m_fGhostPushEndJumpRate );
			return;
		}
		break;
	case PFS_DASH:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionEndTime = 0;
			SetDashAttackState( pOwner );
		}
		break;
	case PFS_DASH_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostAttackEnd( pOwner, m_GhostInfo.m_fGhostDashEndJumpRate );
			return;
		}
		break;
	case PFS_JUMP:
		if( m_dwMotionEndTime > 0 && m_dwMotionEndTime < dwCurTime )
		{
			m_dwMotionEndTime = 0;
			SetJumpAttackState( pOwner );
		}
		break;
	case PFS_JUMP_ATTACK:
		if( m_dwMotionEndTime < dwCurTime )
		{
			ChangeGhostAttackEnd( pOwner, m_GhostInfo.m_fGhostJumpEndJumpRate );
			return;
		}
		break;
	case PFS_END:
	case PFS_ATTACK_END:
		break;
	case PFS_DEFENSE_DASH:
		if( m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
			pOwner->SetFallState( true );
			return;
		}
		break;
	}
}

void ioGhostItem::ProcessGhostFlyMoveAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;	

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
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
	float fPreHeight = pOwner->GetBottomHeight();
	if( vPos.y < fPreHeight )
		vPos.y = fPreHeight;

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}
}

void ioGhostItem::SetPushAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_GhostTarget.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( pTarget )
		pTarget->AddNewBuff( m_GhostInfo.m_szPushAttackBuff, pOwner->GetCharName(), "", NULL );

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_PushAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GhostInfo.m_fPushAniRate );

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_PUSH_ATTACK;
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_GhostInfo.m_fPushAniRate);

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vMoveDir = pOwner->GetMoveDir();
	D3DXVECTOR3 vNewPos = vCharPos - vMoveDir * m_GhostInfo.m_dwPushOffset;
	pOwner->SetWorldPosition( vNewPos );

	if( !m_GhostInfo.m_GhostPushWeaponList.empty() )
		pOwner->WeaponByWeaponExplicitList( m_GhostInfo.m_GhostPushWeaponList, FTT_NORMAL_ATTACK, vCharPos, vMoveDir );

	int iCnt = m_GhostInfo.m_vPossessionOwnerBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->RemoveBuff( m_GhostInfo.m_vPossessionOwnerBuff[i] );
}

void ioGhostItem::SetDashAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_GhostTarget.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( pTarget )
		pTarget->AddNewBuff( m_GhostInfo.m_szDashAttackBuff, pOwner->GetCharName(), "", NULL );

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_DashAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GhostInfo.m_fDashAniRate );

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_DASH_ATTACK;
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_GhostInfo.m_fDashAniRate);

	D3DXVECTOR3 vDashDir = D3DXVECTOR3( 0.f, 0.f, 0.f );
	if( m_GhostInfo.m_eDashDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_GhostInfo.m_eDashDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		vDashDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDashDir, &vDashDir );
	}
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vNewPos = vCharPos + vDashDir * m_GhostInfo.m_dwDashOffset;
	pOwner->SetWorldPosition( vNewPos );
	pOwner->SetTargetRotToTargetPos( vCharPos, true );

	D3DXVECTOR3 vFirePos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vFireDir = pOwner->GetMoveDir();
	if( !m_GhostInfo.m_GhostDashWeaponList.empty() )
		pOwner->WeaponByWeaponExplicitList( m_GhostInfo.m_GhostDashWeaponList, FTT_NORMAL_ATTACK, vFirePos, vFireDir );

	int iCnt = m_GhostInfo.m_vPossessionOwnerBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->RemoveBuff( m_GhostInfo.m_vPossessionOwnerBuff[i] );
}

void ioGhostItem::SetJumpAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || m_GhostTarget.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( pTarget )
		pTarget->AddNewBuff( m_GhostInfo.m_szJumpAttackBuff, pOwner->GetCharName(), "", NULL );

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_JumpAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GhostInfo.m_fJumpAniRate );

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	m_GhostFlyState = PFS_JUMP_ATTACK;
	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_GhostInfo.m_fJumpAniRate);

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vNewPos = vCharPos + D3DXVECTOR3( 0.f, 1.f, 0.f ) * m_GhostInfo.m_dwJumpOffset;
	pOwner->SetWorldPosition( vNewPos );
	pOwner->SetTargetRotToTargetPos( vCharPos, true );

	D3DXVECTOR3 vFirePos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vFireDir = pOwner->GetMoveDir();
	if( !m_GhostInfo.m_GhostJumpWeaponList.empty() )
		pOwner->WeaponByWeaponExplicitList( m_GhostInfo.m_GhostJumpWeaponList, FTT_NORMAL_ATTACK, vFirePos, vFireDir );

	int iCnt = m_GhostInfo.m_vPossessionOwnerBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->RemoveBuff( m_GhostInfo.m_vPossessionOwnerBuff[i] );
}

bool ioGhostItem::ProcessGhostFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_GhostInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_GhostInfo.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_GhostInfo.m_fStartMaxRange / (float)m_GhostInfo.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fGhostFlyStartRange+fDistance < m_GhostInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fGhostFlyStartRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_GhostInfo.m_fStartMaxRange - m_fGhostFlyStartRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}

bool ioGhostItem::ProcessGhostFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = m_GhostInfo.m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );

	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_GhostInfo.m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}

	vCurPos.y = max( vCurPos.y, fMapHeight );

	pStage->CheckFlyMapLimit( &vCurPos );
	pOwner->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return true;
}

bool ioGhostItem::ProcessGhostFlyPossession( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget )
		return false;

	if( !CheckPossessionTargetValidate( pOwner ) )
		return false;

	D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToDir( -vDir, true );
	return true;
}

void ioGhostItem::CheckGhostFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iGhostFlyState;
	rkPacket >> iGhostFlyState;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );
	pOwner->SetMoveDirByRotate( qtRot );

	switch( iGhostFlyState )
	{
	case PFS_WAIT:
	case PFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurGhostGauge( fCurGauge );
			SetGhostFlyState( pOwner, false );
			return;
		}
		break;
	case PFS_MOVE_ATTACK:
		ChangeGhostFlyMoveAttack( pOwner );
		break;
	case PFS_POSSESSION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;
			ChangeGhostFlyPossession( pOwner, szTargetName );
		}
		break;
	case PFS_PUSH:
		{
			rkPacket >> m_GhostTarget;
			SetPushState( pOwner, false );
		}
		break;
	case PFS_DASH:
		{
			int iDir;
			rkPacket >> m_GhostTarget;
			rkPacket >> iDir;
			m_GhostInfo.m_eDashDirKey = ioUserKeyInput::DirKeyInput( iDir );
			SetDashState( pOwner, false );
		}
		break;
	case PFS_JUMP:
		{
			rkPacket >> m_GhostTarget;
			SetJumpState( pOwner, false );
		}
		break;
	case PFS_END:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurGhostGauge( fCurGauge );
			ChangeGhostFlyEnd( pOwner );
		}
		break;
	case PFS_ATTACK_END:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurGhostGauge( fCurGauge );

			float fJumpRate;
			rkPacket >> fJumpRate;
			ChangeGhostAttackEnd( pOwner, fJumpRate );
		}
		break;
	case PFS_DEFENSE_DASH:
		ChangeToDefenseDash( pOwner );
		break;
	}
}

int ioGhostItem::GetMaxBullet()
{
	return (int)m_fMaxGhostGauge;
}

int ioGhostItem::GetNeedBullet()
{
	if( !CheckGhostFlyEnableTime() )
		return (int)m_fMaxGhostGauge + 1;

	return (int)m_fNeedGhostGauge;
}

int ioGhostItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGhostGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGhostGauge ) );

	return (int)fCurCoolTime;
}

void ioGhostItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGhostGauge ) );

	m_fCurGhostGauge = fCurBullet;
}

void ioGhostItem::InitGhostGauge()
{
	m_fCurGhostGauge = 0.0f;
}

void ioGhostItem::MaxGhostGauge()
{
	m_fCurGhostGauge = m_fMaxGhostGauge;
}

float ioGhostItem::GetCurGhostGauge()
{
	return m_fCurGhostGauge;
}

void ioGhostItem::SetCurGhostGauge( float fGauge )
{
	m_fCurGhostGauge = fGauge;
	m_fCurGhostGauge = (float)max( 0, min( m_fCurGhostGauge, m_fMaxGhostGauge ) );
}

void ioGhostItem::DecreaseGhostGauge( float fGauge )
{
	m_fCurGhostGauge -= fGauge;
	m_fCurGhostGauge = (float)max( 0, min( m_fCurGhostGauge, m_fMaxGhostGauge ) );
}

void ioGhostItem::SetEnableGhostEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableGhostEffect == 0 && !m_EnableGhostEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableGhostEffect );
		if( pEffect )
		{
			m_dwEnableGhostEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurGhostGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGhostItem::ReleaseEnableGhostEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableGhostEffect != 0 )
		pChar->EndEffect( m_dwEnableGhostEffect, false );

	m_dwEnableGhostEffect = 0;
}

void ioGhostItem::CheckEnableGhostGauge( ioBaseChar *pChar )
{
	if( m_fCurGhostGauge >= m_fNeedGhostGauge )
		SetEnableGhostEffect( pChar );
}

bool ioGhostItem::IsEnableGhostGauge()
{
	if( m_fCurGhostGauge >= m_fNeedGhostGauge )
		return true;

	return false;
}

bool ioGhostItem::IsDefenseDashEnable()
{
	if( m_fCurGhostGauge >= m_fDefenseDashDecreaseGhostGauge )
		return true;

	return false;
}

bool ioGhostItem::IsGhostState()
{
	switch( m_GhostFlyState )
	{
	case PFS_START:
	case PFS_MOVE:
	case PFS_MOVE_ATTACK:
	case PFS_POSSESSION:
	case PFS_PUSH:
	case PFS_PUSH_ATTACK:
	case PFS_DASH:
	case PFS_DASH_ATTACK:
	case PFS_JUMP:
	case PFS_JUMP_ATTACK:
		return true;
	}

	return false;
}

void ioGhostItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_GHOST_FLY:
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			float fGauge = m_fDecreaseGhostGauge * fTimePerSec;

			if( m_GhostFlyState != PFS_WAIT && m_GhostFlyState != PFS_START && 
				m_GhostFlyState != PFS_MOVE && m_GhostFlyState != PFS_MOVE_ATTACK )
			{
				return;
			}
			else if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseGhostGauge * fTimePerSec;
			}

			m_fCurGhostGauge -= fGauge;
			m_fCurGhostGauge = max( 0.0f, m_fCurGhostGauge );
		}
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGhostGauge < m_fMaxGhostGauge )
		{
			float fGauge = m_fIncreaseGhostGauge * fTimePerSec;

			m_fCurGhostGauge += fGauge;
			m_fCurGhostGauge = min( m_fCurGhostGauge, m_fMaxGhostGauge );
		}
		return;
	}
}

void ioGhostItem::ClearGhostState( ioBaseChar *pOwner )
{
	if( pOwner && !m_GhostInfo.m_GhostFlyEffect.IsEmpty() )
		pOwner->EndEffect( m_GhostInfo.m_GhostFlyEffect, false );

	ioBaseChar *pTarget = NULL;
	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( pTarget )
	{
		pTarget->ClearPossessionChar();

		int iCnt = m_GhostInfo.m_vPossessionBuff.size();
		for( int i=0; i<iCnt; ++i )
			pTarget->RemoveBuff( m_GhostInfo.m_vPossessionBuff[i] );

		pTarget->SetReturnJumpping( false );
		RelesePossessionObjectItem( pTarget );
	}

	int iCnt = m_GhostInfo.m_vPossessionOwnerBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->RemoveBuff( m_GhostInfo.m_vPossessionOwnerBuff[i] );

	m_dwMotionEndTime = 0;
	m_GhostTarget.Clear();

	m_bUseGhostFly = false;
	m_fGhostFlyStartRange = 0.0f;

	m_GhostFlyState = PFS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner )
		pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
}

bool ioGhostItem::IsGhostFlyCharCollisionSkipState()
{
	switch( m_GhostFlyState )
	{
	case PFS_START:
	case PFS_MOVE:
	case PFS_MOVE_ATTACK:
	case PFS_POSSESSION:
	case PFS_PUSH:
	case PFS_PUSH_ATTACK:
	case PFS_DASH:
	case PFS_DASH_ATTACK:
	case PFS_JUMP:
	case PFS_JUMP_ATTACK:
		return true;
	}

	return false;
}

bool ioGhostItem::IsGhostFlyWeaponCollisionSkipState()
{
	return false;
}

void ioGhostItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableGhostEffect( pOwner );

	ClearGhostState( pOwner );
	m_fCurGhostGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioGhostItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	ReleaseEnableGhostEffect( pOwner );

	ClearGhostState( pOwner );
	m_fCurGhostGauge = 0.0f;
}

void ioGhostItem::ChangeGhostFlyEnd( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = NULL;
	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_GhostTarget );

	if( m_GhostFlyState == PFS_POSSESSION )
	{
		m_dwGhostFlyEnableTime = FRAMEGETTIME() + m_GhostInfo.m_dwGhostFlyEnableTime;

		if( pTarget )
			pTarget->ClearPossessionChar();
	}

	bool bPossession = false;
	if( m_GhostFlyState == PFS_POSSESSION )
		bPossession = true;

	m_GhostFlyState = PFS_END;

	g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );

	if( bPossession )
	{
		if( pTarget )
		{
			int iCnt = m_GhostInfo.m_vPossessionEndBuff.size();
			for( int i=0; i<iCnt; ++i )
				pTarget->AddNewBuff( m_GhostInfo.m_vPossessionEndBuff[i], pOwner->GetCharName(), "", NULL );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_FLY_STATE;
			kPacket << m_GhostFlyState;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << m_fCurGhostGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	
	if( pOwner && pOwner->IsNeedProcess() )
		pOwner->SetExtendAttackEndJump( m_GhostInfo.m_fGhostFlyJumpAmt, m_fExtendLandingRate );
}

void ioGhostItem::ChangeGhostAttackEnd( ioBaseChar *pOwner, float fJumpRate )
{
	ioBaseChar *pTarget = NULL;
	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_GhostTarget );

	m_dwGhostFlyEnableTime = FRAMEGETTIME() + m_GhostInfo.m_dwGhostFlyEnableTime;

	if( pTarget )
		pTarget->ClearPossessionChar();

	m_GhostFlyState = PFS_ATTACK_END;

	g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGhostGauge;
		kPacket << fJumpRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	
	if( pOwner && pOwner->IsNeedProcess() )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( fHeightGap >= m_fGhostEndJumpHeightGap )
			pOwner->SetChangeJumppingState( fJumpRate, false, 0 );
		else
			pOwner->SetState( CS_DELAY );
	}
}

bool ioGhostItem::IsEnableTargetPossessionState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();
	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_ATTACK:
	case CS_DEFENSE:
	case CS_JUMP:
		break;
	case CS_FALL:
	case CS_WOUNDED:
	case CS_BLOW_WOUNDED:
	case CS_BOUND_BLOW_WOUNDED:
		if( pTarget->HasEnablePossessionBuff() )
			break;
		return false;
	default:
		return false;
	}

	if( pTarget->IsChatModeState( false ) )
		return false;

	if( pTarget->HasHideBuff() )
		return false;

	return true;
}

bool ioGhostItem::CheckPossessionTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GhostTarget );
	if( !pTarget )
		return false;

	if( !IsEnableTargetPossessionState( pTarget ) )
		return false;

	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

	return true;
}

const ioHashString& ioGhostItem::GetGhostTarget()
{
	return m_GhostTarget;
}

bool ioGhostItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioGhostItem::CheckGhostFlyMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_GhostInfo.m_DelayAni );
	float fTimeRate  = m_GhostInfo.m_fDelayAniRate;

	if( m_CurMoveAni == m_GhostInfo.m_DelayAni )
	{
		if( m_bSetUpKey || m_bSetDownKey || (m_CurDirKey != ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_GhostInfo.m_MoveAni );
			fTimeRate = m_GhostInfo.m_fMoveAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_GhostInfo.m_MoveAni;
		}
	}
	else if( m_CurMoveAni == m_GhostInfo.m_MoveAni )
	{
		if( !m_bSetUpKey && !m_bSetDownKey && (m_CurDirKey == ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_GhostInfo.m_DelayAni;
		}
	}
}

bool ioGhostItem::IsEnablePossessionState()
{
	if( m_GhostFlyState == PFS_MOVE_ATTACK )
		return true;

	return false;
}

ioUserKeyInput::DirKeyInput ioGhostItem::GetCurDirKey()
{
	return m_CurDirKey;
}

void ioGhostItem::RelesePossessionObjectItem( ioBaseChar *pChar )
{
	if( pChar && m_GhostInfo.m_dwPossessionObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pChar->GetObject();
		if( !pObjectItem )
			return;

		if( pObjectItem && pObjectItem->GetItemCode() == m_GhostInfo.m_dwPossessionObjectItem )
		{
			//애니메이션 초기화
			if( pChar->GetState() == CS_DELAY )
			{				
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
				pChar->OnReleaseObjectItem( CS_DELAY );
			}
			else if( pChar->GetState() == CS_RUN )
			{				
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
				pChar->OnReleaseObjectItem( CS_RUN );
			}
			else if( pChar->GetState() == CS_DASH )
			{				
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DASH );
				pChar->OnReleaseObjectItem( CS_DASH );
			}
			else
			{				
				pChar->ReleaseObjectItem( __FUNCTION__ );
				pChar->OnReleaseObjectItem();
			}
		}
	}
}

void ioGhostItem::EquipPossessionObjectItem( ioBaseChar *pChar )
{
	if( pChar->GetControlType() != CONTROL_USER )
		return;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_GhostInfo.m_dwPossessionObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_GhostInfo.m_dwPossessionObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_EQUIP_SKILL );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_GhostInfo.m_dwPossessionObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();			

				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_EQUIP_BUFF1 );
				}
				pChar->EquipItem( pItem );
			}
		}
	}
}

bool ioGhostItem::CheckDefenseSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ChangeToDefenseDash( pOwner );
	return true;
}

void ioGhostItem::ChangeToDefenseDash( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->GetState() != CS_GHOST_FLY )
		pOwner->SetState( CS_GHOST_FLY );

	DecreaseGhostGauge( m_fDefenseDashDecreaseGhostGauge );

	pOwner->ClearAttackFireTimeAndSkill();
	pOwner->ClearDirDoubleClick();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_GhostInfo.m_DefenseDashAni );
	float fTimeRate  = m_GhostInfo.m_fDefenseDashAniRate;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0, true );

	m_GhostFlyState = PFS_DEFENSE_DASH;
	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * fTimeRate );

	int iCnt = m_GhostInfo.m_vDefenseDashBuff.size();
	for( int i=0; i<iCnt; ++i )
		pOwner->AddNewBuff( m_GhostInfo.m_vDefenseDashBuff[i], pOwner->GetCharName(), "", NULL );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_GhostFlyState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}
//---------------------------------------------------------------------------------------------------------
ioGhostFlySpecialState::ioGhostFlySpecialState()
{
}

ioGhostFlySpecialState::~ioGhostFlySpecialState()
{
}

void ioGhostFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGhostFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	//ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhostItem )
		pGhostItem->ClearGhostState( pOwner );

	pOwner->ClearAttackFireTimeAndSkill();
}

bool ioGhostFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhostItem && pGhostItem->IsGhostFlyCharCollisionSkipState() )
		return true;

	return false;
}

void ioGhostFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGhostItem *pGhost = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhost )
		pGhost->ProcessGhostFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioGhostFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioGhostFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

bool ioGhostFlySpecialState::IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon )
{
	ioGhostItem *pGhostItem = ToGhostItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGhostItem && pGhostItem->IsGhostFlyWeaponCollisionSkipState() )
		return false;

	return true;
}