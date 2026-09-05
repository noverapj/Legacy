
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioPhantomItem.h"

#include "WeaponDefine.h"

ioPhantomItem::ioPhantomItem()
{
	m_ChargeState = CS_NONE;
	m_PhantomFlyState = PFS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	// phantomfly
	m_CurPhantomFly.Init();

	// wait
	m_dwPhantomFlyWaitTime = 0;

	// start
	m_dwPhantomFlyStartTime = 0;
	m_fPhantomFlyStartRange = 0.0f;

	// target
	m_PhantomTarget.Clear();
	m_PhantomTargetSkill.Clear();

	// trace
	m_fPhantomFlyTraceRange = 0.0f;
	m_vPhantomFlyTraceDir = ioMath::VEC3_ZERO;

	// key
	m_bTargetRot = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	// phantom gauge
	m_fCurPhantomGauge = 0.0f;

	// etc
	m_bUsePhantomFly = false;

	m_dwEnablePhantomEffect = 0;
	m_dwFlyMoveEffect = 0;
	m_dwPhantomFlyEnableTime = 0;
}

ioPhantomItem::ioPhantomItem( const ioPhantomItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackPhantomFly( rhs.m_AttackPhantomFly ),
m_EnablePhantomEffect( rhs.m_EnablePhantomEffect ),
m_fMaxPhantomGauge( rhs.m_fMaxPhantomGauge ),
m_fNeedPhantomGauge( rhs.m_fNeedPhantomGauge ),
m_fIncreasePhantomGauge( rhs.m_fIncreasePhantomGauge ),
m_fDecreasePhantomGauge( rhs.m_fDecreasePhantomGauge ),
m_fFirstDecreasePhantomGauge( rhs.m_fFirstDecreasePhantomGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreasePhantomGauge( rhs.m_fHighDecreasePhantomGauge ),
m_fPossessionDecreaseGauge( rhs.m_fPossessionDecreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_PhantomFlyState = PFS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	// phantomfly
	m_CurPhantomFly.Init();

	// wait
	m_dwPhantomFlyWaitTime = 0;

	// start
	m_dwPhantomFlyStartTime = 0;
	m_fPhantomFlyStartRange = 0.0f;

	// target
	m_PhantomTarget.Clear();
	m_PhantomTargetSkill.Clear();

	// trace
	m_fPhantomFlyTraceRange = 0.0f;
	m_vPhantomFlyTraceDir = ioMath::VEC3_ZERO;

	// key
	m_bTargetRot = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	// phantom gauge
	m_fCurPhantomGauge = 0.0f;

	// etc
	m_bUsePhantomFly = false;

	m_dwEnablePhantomEffect = 0;
	m_dwFlyMoveEffect = 0;
	m_dwPhantomFlyEnableTime = 0;
}

ioPhantomItem::~ioPhantomItem()
{
}

void ioPhantomItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadPhantomFlyInfo( rkLoader );

	m_fMaxPhantomGauge = rkLoader.LoadFloat_e( "max_phantom_gauge", FLOAT100 );
	m_fNeedPhantomGauge = rkLoader.LoadFloat_e( "need_phantom_gauge", FLOAT100 );

	m_fIncreasePhantomGauge = rkLoader.LoadFloat_e( "increase_phantom_gauge", FLOAT1 );
	m_fDecreasePhantomGauge = rkLoader.LoadFloat_e( "decrease_phantom_gauge", 2.0f );
	m_fFirstDecreasePhantomGauge = rkLoader.LoadFloat_e( "first_decrease_phantom_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreasePhantomGauge = rkLoader.LoadFloat_e( "high_decrease_phantom_gauge", 3.0f );
	m_fPossessionDecreaseGauge = rkLoader.LoadFloat_e( "possession_decrease_gauge", 5.0f );

	rkLoader.LoadString_e( "enable_phantom_effect", "", szBuf, MAX_PATH );
	m_EnablePhantomEffect = szBuf;
}

void ioPhantomItem::LoadPhantomFlyInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_AttackPhantomFly.Init();

	// mesh list
	int iMeshCnt = rkLoader.LoadInt_e( "phantom_mesh_cnt", 0 );
	for( int i=0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "phantom_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szMesh = szBuf;
		m_AttackPhantomFly.m_PhantomMeshList.push_back(szMesh);
	}

	int iMeshWCnt = rkLoader.LoadInt_e( "phantom_female_mesh_cnt", 0 );
	for( int j=0; j < iMeshWCnt; ++j )
	{
		wsprintf_e( szKey, "phantom_female_mesh%d", j+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szMesh = szBuf;
		m_AttackPhantomFly.m_PhantomFemaleMeshList.push_back(szMesh);
	}

	// wait
	rkLoader.LoadString_e( "phantom_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_WaitAni = szBuf;
	m_AttackPhantomFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "phantom_fly_wait_duration", 0 );

	// start
	rkLoader.LoadString_e( "phantom_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_StartAni = szBuf;
	m_AttackPhantomFly.m_fStartAniRate = rkLoader.LoadFloat_e( "phantom_fly_start_ani_rate", FLOAT1 );

	m_AttackPhantomFly.m_fAlphaRate = rkLoader.LoadFloat_e( "phantom_fly_alpha_rate", FLOAT1 );

	m_AttackPhantomFly.m_fStartAngle = rkLoader.LoadFloat_e( "phantom_fly_start_angle", 0.0f );
	m_AttackPhantomFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "phantom_fly_start_max_range", 0.0f );
	m_AttackPhantomFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "phantom_fly_start_duration", 0 );

	// delay
	rkLoader.LoadString_e( "phantom_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_DelayAni = szBuf;
	m_AttackPhantomFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "phantom_fly_delay_ani_rate", FLOAT1 );

	// move
	rkLoader.LoadString_e( "phantom_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_MoveAni = szBuf;
	m_AttackPhantomFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "phantom_fly_move_ani_rate", FLOAT1 );

	m_AttackPhantomFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "phantom_fly_move_speed", FLOAT100 );
	m_AttackPhantomFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "phantom_fly_updown_speed", FLOAT100 );

	m_AttackPhantomFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "phantom_fly_rotate_speed", 0 );

	// target
	m_AttackPhantomFly.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "fly_target_wound_type", TWT_ALL );
	m_AttackPhantomFly.m_fTargetAngle = rkLoader.LoadFloat_e( "fly_target_angle", 45.0f );
	m_AttackPhantomFly.m_fTargetRange = rkLoader.LoadFloat_e( "fly_target_range", 45.0f );
	m_AttackPhantomFly.m_fTargetUpHeight = rkLoader.LoadFloat_e( "fly_target_up_height", 45.0f );
	m_AttackPhantomFly.m_fTargetDownHeight = rkLoader.LoadFloat_e( "fly_target_down_height", 45.0f );

	m_AttackPhantomFly.m_fTargetMarkerRange = rkLoader.LoadFloat_e( "fly_target_marker_range", 45.0f );

	// trace
	rkLoader.LoadString_e( "phantom_fly_trace_move", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_TraceMoveAni = szBuf;
	m_AttackPhantomFly.m_fTraceMoveRate = rkLoader.LoadFloat_e( "phantom_fly_trace_move_rate", FLOAT1 );

	m_AttackPhantomFly.m_fTraceMoveSpeed = rkLoader.LoadFloat_e( "phantom_fly_trace_move_speed", FLOAT1 );
	m_AttackPhantomFly.m_fTraceMoveRange = rkLoader.LoadFloat_e( "phantom_fly_trace_move_range", FLOAT1 );
	m_AttackPhantomFly.m_fTraceMoveGapRange = rkLoader.LoadFloat_e( "phantom_fly_trace_move_gap_range", FLOAT1 );
	m_AttackPhantomFly.m_dwTraceRotateTime = (DWORD)rkLoader.LoadInt_e( "phantom_fly_trace_rotate_speed", 1000 );

	// attack
	wsprintf_e( szKey, "phantom_fly_attack_type" );
	m_AttackPhantomFly.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "phantom_fly_attack_type_resist" );
	m_AttackPhantomFly.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "phantom_fly_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_WeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "phantom_fly_attack_type_wounded_time" );
	m_AttackPhantomFly.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "phantom_fly_attack_type_wounded_loop_ani" );
	m_AttackPhantomFly.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	// possession
	rkLoader.LoadString_e( "phantom_fly_possession", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_PossessionAni = szBuf;
	m_AttackPhantomFly.m_fPossessionRate = rkLoader.LoadFloat_e( "phantom_fly_possession_rate", FLOAT1 );

	int iBuffCnt = rkLoader.LoadInt_e( "phantom_fly_possession_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "phantom_fly_possession_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_AttackPhantomFly.m_vPossessionBuff.push_back(szBuff);
	}

	iBuffCnt = rkLoader.LoadInt_e( "phantom_fly_possession_end_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "phantom_fly_possession_end_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_AttackPhantomFly.m_vPossessionEndBuff.push_back(szBuff);
	}

	m_AttackPhantomFly.m_vPossessionOffset.x = rkLoader.LoadFloat_e( "phantom_fly_possession_x", 0.0f );
	m_AttackPhantomFly.m_vPossessionOffset.y = rkLoader.LoadFloat_e( "phantom_fly_possession_y", 0.0f );
	m_AttackPhantomFly.m_vPossessionOffset.z = rkLoader.LoadFloat_e( "phantom_fly_possession_z", 0.0f );

	// end
	m_AttackPhantomFly.m_fPhantomFlyJumpAmt = rkLoader.LoadFloat_e( "phantom_fly_jump_amt", 0.0f );

	// etc
	rkLoader.LoadString_e( "phantom_fly_effect", "", szBuf, MAX_PATH );
	m_AttackPhantomFly.m_PhantomFlyEffect = szBuf;

	m_AttackPhantomFly.m_dwPhantomFlyEnableTime = (DWORD)rkLoader.LoadInt_e( "phantom_fly_enable_time", 0 );
}

ioItem* ioPhantomItem::Clone()
{
	return new ioPhantomItem( *this );
}

ioWeaponItem::WeaponSubType ioPhantomItem::GetSubType() const
{
	return WST_PHANTOM;
}

void ioPhantomItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearPhantomState( pOwner );

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

void ioPhantomItem::CheckNormalAttack( ioBaseChar *pOwner )
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

bool ioPhantomItem::CheckPhantomFlyEnableTime()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwPhantomFlyEnableTime > 0 && dwCurTime < m_dwPhantomFlyEnableTime )
		return false;

	return true;
}

void ioPhantomItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !CheckPhantomFlyEnableTime() )
	{
		ChangeToNormalAttack( pOwner );
		return;
	}

	if( IsEnablePhantomGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			SetPhantomFlyState( pOwner, m_AttackPhantomFly, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioPhantomItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->InitExtendAttackTagInfo();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

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

void ioPhantomItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioPhantomItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			SetEnablePhantomEffect( pOwner );
			SetCurPhantomGauge( fGauge );
		}
		break;
	case SST_FLY_STATE:
		CheckPhantomFlyState( pOwner, rkPacket );
		break;
	case SST_MARKER:
		{
			rkPacket >> m_PhantomTarget;
			rkPacket >> m_bAimedBall;

			bool bChangeReady;
			rkPacket >> bChangeReady;

			switch( m_PhantomFlyState )
			{
			case PFS_TRACE_MOVE:
			case PFS_POSSESSION:
			case PFS_END:
				return;
			}

			if( bChangeReady )
			{
				pOwner->SetTargetMarkerTarget( m_PhantomTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			}
			else if( !m_PhantomTarget.IsEmpty() || m_bAimedBall )
			{
				pOwner->SetTargetMarkerTarget( m_PhantomTarget, m_bAimedBall );
				pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
			}
		}
		break;
	case SST_MOVE_KEY:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;
			rkPacket >> bSetUpKey >> bSetDownKey;

			pOwner->SetTargetRot( qtRot );

			if( bChangeDir )
			{
				m_bTargetRot = false;
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}

			CheckPhantomFlyMoveAni( pOwner );
		}
		break;
	case SST_POSSESSION_KEY:
		{
			int iDir;
			rkPacket >> iDir;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	}
}

void ioPhantomItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	bool bPhantom = false;

	switch( m_PhantomFlyState )
	{
	case PFS_WAIT:
	case PFS_START:
	case PFS_MOVE:
	case PFS_TRACE_MOVE:
		bPhantom = true;
		break;
	}

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( bPhantom, false );
		}
	}
}

bool ioPhantomItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioPhantomItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioPhantomItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioPhantomItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioPhantomItem::CheckKeyInput( ioBaseChar *pOwner )
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
		m_bTargetRot = false;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
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
		CheckPhantomFlyMoveAni( pOwner );

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

void ioPhantomItem::CheckPossessionKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
	{
		ChangePhantomFlyEnd( pOwner, true );
		return;
	}

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;
		m_bTargetRot = false;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_POSSESSION_KEY;
		kPacket << (int)m_CurDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPhantomItem::SetPhantomFlyStateAttack( ioBaseChar *pOwner )
{
	SetPhantomFlyState( pOwner, m_AttackPhantomFly, false );
}

void ioPhantomItem::SetPhantomFlyState( ioBaseChar *pOwner, const PhantomFlyInfo &rkFlyInfo, bool bSendNet )
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

	pOwner->SetState( CS_PHANTOM_FLY );
	ReleaseEnablePhantomEffect( pOwner );
	ReleasePhantomFlyMoveEffect( pOwner );
	DecreasePhantomGauge( m_fFirstDecreasePhantomGauge );

	m_CurPhantomFly = rkFlyInfo;

	if( m_CurPhantomFly.m_WaitAni.IsEmpty() )
	{
		ChangePhantomFlyStart( pOwner );
		m_fPhantomFlyStartRange = 0.0f;
		m_fPhantomFlyTraceRange = 0.0f;
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_CurPhantomFly.m_WaitAni );
		pGrp->SetLoopAni( iAniID, FLOAT10 );

		m_PhantomFlyState = PFS_WAIT;
		m_dwPhantomFlyWaitTime = FRAMEGETTIME();
		m_dwPhantomFlyStartTime = 0;

		m_fPhantomFlyStartRange = 0.0f;
		m_fPhantomFlyTraceRange = 0.0f;
	}

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_PhantomFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurPhantomGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPhantomItem::ChangePhantomFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurPhantomFly.m_StartAni );
	float fTimeRate  = m_CurPhantomFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_PhantomFlyState = PFS_START;

	m_dwPhantomFlyStartTime = FRAMEGETTIME(); 

	pOwner->SetTargetMarkerRange( m_CurPhantomFly.m_fTargetMarkerRange );

	EquipPhantomMesh( pOwner );
	m_bUsePhantomFly = true;
}

void ioPhantomItem::ChangePhantomFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurPhantomFly.m_DelayAni );
	float fTimeRate  = m_CurPhantomFly.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_PhantomFlyState = PFS_MOVE;
	m_CurMoveAni = m_CurPhantomFly.m_DelayAni;

	m_dwPhantomFlyWaitTime = 0;
	m_dwPhantomFlyStartTime = 0;
}

void ioPhantomItem::ChangePhantomFlyTrace( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_CurPhantomFly.m_fTraceMoveSpeed <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if( !m_CurPhantomFly.m_TraceMoveAni.IsEmpty() )
	{
		int iAniID = pGrp->GetAnimationIdx( m_CurPhantomFly.m_TraceMoveAni );
		float fTimeRate = m_CurPhantomFly.m_fTraceMoveRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		pOwner->CheckCharColSkipTime( iAniID, fTimeRate );
	}

	pOwner->NormalAttackFireExplicit( m_CurPhantomFly.m_WeaponInfo );

	m_PhantomFlyState = PFS_TRACE_MOVE;

	m_dwPhantomFlyWaitTime = 0;
	m_dwPhantomFlyStartTime = 0;
	
	if( pOwner->IsNeedProcess() )
	{
		m_vPhantomFlyTraceDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
		if( pTarget )
			m_vPhantomFlyTraceDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();

		D3DXVec3Normalize( &m_vPhantomFlyTraceDir, &m_vPhantomFlyTraceDir );

		pOwner->SetTargetRotToDir( m_vPhantomFlyTraceDir, false );
	}

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	ReleasePhantomFlyMoveEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_PhantomFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vPhantomFlyTraceDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPhantomItem::ChangePhantomFlyPossession( ioBaseChar *pOwner, const ioHashString &szTargetName )
{
	if( m_PhantomFlyState != PFS_TRACE_MOVE )
		return;

	if( !pOwner ) return;

	m_PhantomTarget = szTargetName;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
	if( !pTarget ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( !CheckTraceTargetValidate( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_CurPhantomFly.m_PossessionAni );
	float fTimeRate = m_CurPhantomFly.m_fPossessionRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->SetWorldPosition( pTarget->GetWorldPosition() );
	pOwner->SetTargetRotToRotate( pTarget->GetTargetRot(), false );

	m_dwPhantomFlyWaitTime = 0;
	m_dwPhantomFlyStartTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;
	g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );

	m_PhantomFlyState = PFS_POSSESSION;

	if( pTarget )
	{
		int iCnt = m_CurPhantomFly.m_vPossessionBuff.size();
		for( int i=0; i<iCnt; ++i )
		{
			pTarget->AddNewBuff( m_CurPhantomFly.m_vPossessionBuff[i], pOwner->GetCharName(), "", NULL );
		}

		pTarget->SetPossessionChar( pOwner->GetCharName() );
	}

	int iAlphaRate = MAX_ALPHA_RATE * m_CurPhantomFly.m_fAlphaRate;
	iAlphaRate = max( 1, min(iAlphaRate, MAX_ALPHA_RATE) );
	pOwner->SetAlphaRateDirect( iAlphaRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_PhantomFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPhantomItem::ProcessPhantomFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_PhantomFlyState )
	{
	case PFS_WAIT:
		if( m_dwPhantomFlyWaitTime+m_CurPhantomFly.m_dwWaitDuration < dwCurTime )
		{
			ChangePhantomFlyStart( pOwner );
			return;
		}
		break;
	case PFS_START:
		if( m_dwPhantomFlyStartTime+m_CurPhantomFly.m_dwStartDuration < dwCurTime )
		{
			ChangePhantomFlyMove( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		else if( !ProcessPhantomFlyStart( pOwner ) )
		{
			ChangePhantomFlyMove( pOwner );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
			return;
		}
		break;
	case PFS_MOVE:
		if( m_fCurPhantomGauge <= 0.0f )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}

		CheckFlyAttTarget( pOwner );
		UpdatePhantomFlyMoveEffect( pOwner );

		if( !ProcessPhantomFlyMove( pOwner ) )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKey() )
				{
					ChangePhantomFlyTrace( pOwner );
					return;
				}
			}

			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case PFS_TRACE_MOVE:
		if( m_fCurPhantomGauge <= 0.0f )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}

		if( !ProcessPhantomFlyTrace(pOwner ) )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}

		if( !ProcessPhantomFlyTraceMove( pOwner ) )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}
		break;
	case PFS_POSSESSION:
		if( m_fCurPhantomGauge <= 0.0f )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}

		if( !ProcessPhantomFlyPossession( pOwner ) )
		{
			ChangePhantomFlyEnd( pOwner, true );
			return;
		}

		CheckPossessionKeyInput( pOwner );
		break;
	case PFS_END:
		break;
	}
}

bool ioPhantomItem::ProcessPhantomFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_CurPhantomFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurPhantomFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurPhantomFly.m_fStartMaxRange / (float)m_CurPhantomFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fPhantomFlyStartRange+fDistance < m_CurPhantomFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fPhantomFlyStartRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurPhantomFly.m_fStartMaxRange - m_fPhantomFlyStartRange;
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

bool ioPhantomItem::ProcessPhantomFlyMove( ioBaseChar *pOwner )
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
		float fCurMoveSpeed = m_CurPhantomFly.m_fMoveSpeed * fTimePerSec;
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
		float fCurUpDownSpeed = m_CurPhantomFly.m_fUpDownSpeed * fTimePerSec;

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

bool ioPhantomItem::ProcessPhantomFlyTrace( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( m_vPhantomFlyTraceDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurPhantomFly.m_dwTraceRotateTime == 0 )
		return false;

	if( !CheckTraceTargetValidate( pOwner ) )
		return true;

	return true;
	/*
	ioBaseChar *pTarget = pStage->GetBaseChar( m_PhantomTarget );
	if( !pTarget )
		return false;
	
	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurPhantomFly.m_dwTraceRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

	D3DXVECTOR3 vTargetMoveDir = vTargetPos - vPos;
	D3DXVec3Normalize( &vTargetMoveDir, &vTargetMoveDir );

	float fDotValue = (float)D3DXVec3Dot( &m_vPhantomFlyTraceDir, &vTargetMoveDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
		return true;

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		D3DXVECTOR3 vTmpAxis;

		if( m_vPhantomFlyTraceDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_Y, &m_vPhantomFlyTraceDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}
		else
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_X, &m_vPhantomFlyTraceDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}

		D3DXVec3Cross( &vAxis, &vTmpAxis, &m_vPhantomFlyTraceDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}
	else
	{
		D3DXVec3Cross( &vAxis, &m_vPhantomFlyTraceDir, &vTargetMoveDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}

	float fCurRotateAngle = fRotateAngle;
	if( fGapAngle < fRotateAngle )
		fCurRotateAngle = fGapAngle;

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * m_vPhantomFlyTraceDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vPhantomFlyTraceDir = vNewDir;

	pOwner->Rotate( vAxis, fCurRotateAngle );

	return true;
	*/
}

bool ioPhantomItem::ProcessPhantomFlyTraceMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = m_CurPhantomFly.m_fTraceMoveSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = m_vPhantomFlyTraceDir * fCurMoveSpeed;

	bool bRangeLimit = false;
	if( m_fPhantomFlyTraceRange+fCurMoveSpeed < m_CurPhantomFly.m_fTraceMoveRange )
	{
		bRangeLimit = true;
		vMoveAmt = m_vPhantomFlyTraceDir * fCurMoveSpeed;
		m_fPhantomFlyTraceRange += fCurMoveSpeed;
	}
	else
	{
		bRangeLimit = false;
		float fGapDistance = m_CurPhantomFly.m_fTraceMoveRange - m_fPhantomFlyTraceRange;
		fGapDistance = max( 0.0f, fGapDistance );
		fGapDistance *= fTimePerSec;

		vMoveAmt = m_vPhantomFlyTraceDir * fGapDistance;
		m_fPhantomFlyTraceRange += fGapDistance;
	}

	bool bCol = false;
	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();
	if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		pOwner->Translate( vMoveAmt );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

	float fMapHeight1 = pStage->GetMapHeight( vCurPos.x, vCurPos.z, NULL, true, vPrePos.y );
	float fMapHeight2 = pStage->GetMapHeight( vCurPos.x, vCurPos.z, NULL, true, vCurPos.y );
	if( fMapHeight1 > 0.0f && fMapHeight2 <= 0.0f )
	{
		vCurPos.y = fMapHeight1 + 5.0f;
	}

	pStage->CheckFlyMapLimit( &vCurPos );
	pOwner->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	return bRangeLimit;
}

bool ioPhantomItem::ProcessPhantomFlyPossession( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
	if( !pTarget )
		return false;

	if( !CheckPossessionTargetValidate( pOwner ) )
		return false;

	D3DXQUATERNION qtRot = pTarget->GetTargetRot();
	D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
	D3DXVECTOR3 vOffset =  qtRot * m_CurPhantomFly.m_vPossessionOffset;

	vPos += vOffset;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, false );

	return true;
}

void ioPhantomItem::CheckPhantomFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iMechanicsFlyState;
	rkPacket >> iMechanicsFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iMechanicsFlyState )
	{
	case PFS_WAIT:
	case PFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurPhantomGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetPhantomFlyState( pOwner, m_AttackPhantomFly, false );
			return;
		}
		break;
	case PFS_TRACE_MOVE:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangePhantomFlyTrace( pOwner );

			m_vPhantomFlyTraceDir = vDir;
		}
		break;
	case PFS_POSSESSION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangePhantomFlyPossession( pOwner, szTargetName );
		}
		break;
	case PFS_END:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			SetCurPhantomGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			bool bEndBuff;
			rkPacket >> bEndBuff;
			ChangePhantomFlyEnd( pOwner, bEndBuff );
		}
		break;
	}
}

int ioPhantomItem::GetMaxBullet()
{
	return (int)m_fMaxPhantomGauge;
}

int ioPhantomItem::GetNeedBullet()
{
	if( !CheckPhantomFlyEnableTime() )
		return (int)m_fMaxPhantomGauge + 1;

	return (int)m_fNeedPhantomGauge;
}

int ioPhantomItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurPhantomGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxPhantomGauge ) );

	return (int)fCurCoolTime;
}

void ioPhantomItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxPhantomGauge ) );

	m_fCurPhantomGauge = fCurBullet;
}

void ioPhantomItem::InitPhantomGauge()
{
	m_fCurPhantomGauge = 0.0f;
}

void ioPhantomItem::MaxPhantomGauge()
{
	m_fCurPhantomGauge = m_fMaxPhantomGauge;
}

float ioPhantomItem::GetCurPhantomGauge()
{
	return m_fCurPhantomGauge;
}

void ioPhantomItem::SetCurPhantomGauge( float fGauge )
{
	m_fCurPhantomGauge = fGauge;
	m_fCurPhantomGauge = (float)max( 0, min( m_fCurPhantomGauge, m_fMaxPhantomGauge ) );
}

void ioPhantomItem::DecreasePhantomGauge( float fGauge )
{
	m_fCurPhantomGauge -= fGauge;
	m_fCurPhantomGauge = (float)max( 0, min( m_fCurPhantomGauge, m_fMaxPhantomGauge ) );
}

void ioPhantomItem::SetEnablePhantomEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnablePhantomEffect == 0 && !m_EnablePhantomEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnablePhantomEffect );
		if( pEffect )
		{
			m_dwEnablePhantomEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pChar, kPacket );
			kPacket << SST_EFFECT;
			kPacket << m_fCurPhantomGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPhantomItem::ReleaseEnablePhantomEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnablePhantomEffect != 0 )
		pChar->EndEffect( m_dwEnablePhantomEffect, false );

	m_dwEnablePhantomEffect = 0;
}

void ioPhantomItem::CheckEnablePhantomGauge( ioBaseChar *pChar )
{
	if( m_fCurPhantomGauge >= m_fNeedPhantomGauge )
		SetEnablePhantomEffect( pChar );
}

bool ioPhantomItem::IsEnablePhantomGauge()
{
	if( m_fCurPhantomGauge >= m_fNeedPhantomGauge )
		return true;

	return false;
}

bool ioPhantomItem::IsPhantomState()
{
	switch( m_PhantomFlyState )
	{
	case PFS_START:
	case PFS_MOVE:
	case PFS_TRACE_MOVE:
	case PFS_POSSESSION:
		return true;
	}

	return false;
}

void ioPhantomItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_PHANTOM_FLY:
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			float fGauge = m_fDecreasePhantomGauge * fTimePerSec;

			if( m_PhantomFlyState == PFS_POSSESSION )
			{
				fGauge = m_fPossessionDecreaseGauge * fTimePerSec;
			}
			else if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreasePhantomGauge * fTimePerSec;
			}

			m_fCurPhantomGauge -= fGauge;
			m_fCurPhantomGauge = max( 0.0f, m_fCurPhantomGauge );
		}
		return;
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurPhantomGauge < m_fMaxPhantomGauge )
		{
			float fGauge = m_fIncreasePhantomGauge * fTimePerSec;

			m_fCurPhantomGauge += fGauge;
			m_fCurPhantomGauge = min( m_fCurPhantomGauge, m_fMaxPhantomGauge );
		}
		return;
	}
}

void ioPhantomItem::ClearPhantomState( ioBaseChar *pOwner )
{
	// 순서중요 m_CurPhantomFly 초기화 이전에 처리해야할 것들.
	if( pOwner && !m_CurPhantomFly.m_PhantomFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurPhantomFly.m_PhantomFlyEffect, false );
	}

	if( pOwner && m_PhantomFlyState == PFS_POSSESSION )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );

		if( pTarget )
		{
			pTarget->ClearPossessionChar();
		}
	}

	ReleasePhantomFlyMoveEffect( pOwner );
	ReleasePhantomMesh( pOwner );

	ioBaseChar *pTarget = NULL;
	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_PhantomTarget );
	if( pTarget )
	{
		int iCnt = m_CurPhantomFly.m_vPossessionBuff.size();
		for( int i=0; i<iCnt; ++i )
		{
			pTarget->RemoveBuff( m_CurPhantomFly.m_vPossessionBuff[i] );
		}
	}

	m_CurPhantomFly.Init();

	m_dwPhantomFlyWaitTime = 0;
	m_dwPhantomFlyStartTime = 0;

	m_PhantomTarget.Clear();
	m_PhantomTargetSkill.Clear();

	m_bUsePhantomFly = false;
	m_fPhantomFlyStartRange = 0.0f;
	m_fPhantomFlyTraceRange = 0.0f;

	m_PhantomFlyState = PFS_NONE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	if( pOwner )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	}
}

bool ioPhantomItem::IsPhantomFlyCharCollisionSkipState()
{
	switch( m_PhantomFlyState )
	{
	case PFS_START:
	case PFS_MOVE:
	case PFS_TRACE_MOVE:
	case PFS_POSSESSION:
		return true;
	}

	return false;
}

bool ioPhantomItem::IsPhantomFlyWeaponCollisionSkipState()
{
	/*
	switch( m_PhantomFlyState )
	{
	case PFS_POSSESSION:
		return true;
	}
	*/

	return false;
}

void ioPhantomItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	if( !pOwner )
		return;

	pOwner->SetTargetMarker( ioTargetMarker::MT_OTHER_MARKER,
							 m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker,"" );

	pOwner->SetTargetMarkerRange( m_AttackPhantomFly.m_fTargetMarkerRange );
}

void ioPhantomItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnablePhantomEffect( pOwner );
	ReleasePhantomFlyMoveEffect( pOwner );

	ClearPhantomState( pOwner );
	m_fCurPhantomGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "","" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioPhantomItem::ChangePhantomFlyEnd( ioBaseChar *pOwner, bool bEndBuff )
{
	ioBaseChar *pTarget = NULL;
	if( pOwner )
		pTarget = pOwner->GetBaseChar( m_PhantomTarget );

	if( m_PhantomFlyState == PFS_POSSESSION )
	{
		m_dwPhantomFlyEnableTime = FRAMEGETTIME() + m_CurPhantomFly.m_dwPhantomFlyEnableTime;

		if( pTarget )
		{
			pTarget->ClearPossessionChar();
		}
	}

	bool bPossession = false;
	if( m_PhantomFlyState == PFS_POSSESSION )
		bPossession = true;

	m_PhantomFlyState = PFS_END;
	ReleasePhantomFlyMoveEffect( pOwner );

	g_WeaponMgr.DestroyReserveZoneWeapon( pOwner->GetCharName() );

	if( bEndBuff && bPossession )
	{
		if( pTarget )
		{
			int iCnt = m_CurPhantomFly.m_vPossessionEndBuff.size();
			for( int i=0; i<iCnt; ++i )
			{
				pTarget->AddNewBuff( m_CurPhantomFly.m_vPossessionEndBuff[i], pOwner->GetCharName(), "", NULL );
			}
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_FLY_STATE;
			kPacket << m_PhantomFlyState;
			kPacket << pOwner->GetTargetRot();
			kPacket << m_fCurPhantomGauge;
			kPacket << bEndBuff;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurPhantomFly.m_fPhantomFlyJumpAmt, m_fExtendLandingRate );
	}
}

void ioPhantomItem::CheckFlyAttTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	bool bChange = false;
	if( m_PhantomTarget.IsEmpty() && !m_bAimedBall )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_PhantomTarget.Clear();
			m_bAimedBall = true;
		}

		if( !m_PhantomTarget.IsEmpty() || m_bAimedBall )
		{
			bChange = true;
			pOwner->SetTargetMarkerTarget( m_PhantomTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}

		if( bChange && pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_PhantomTarget;
			kPacket << m_bAimedBall;
			kPacket << false;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( !CheckTargetValidate( pOwner ) )
	{
		FindFlyAttTarget( pOwner );

		if( CompareTarget( pOwner ) )
		{
			m_PhantomTarget.Clear();
			m_bAimedBall = true;
		}

		bool bChangeReady = false;
		if( !m_PhantomTarget.IsEmpty() || m_bAimedBall )
		{
			pOwner->SetTargetMarkerTarget( m_PhantomTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
		}
		else
		{
			bChangeReady = true;
			pOwner->SetTargetMarkerTarget( m_PhantomTarget, m_bAimedBall );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << SST_MARKER;
			kPacket << m_PhantomTarget;
			kPacket << m_bAimedBall;
			kPacket << bChangeReady;				// change MS_READY
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioPhantomItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurPhantomFly.m_TargetWoundType ) )
		return false;

	return true;
}

bool ioPhantomItem::IsEnableTargetTraceState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	return true;
}

bool ioPhantomItem::IsEnableTargetPossessionState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();
	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
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

void ioPhantomItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_PhantomTarget.Clear();

	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurPhantomFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurPhantomFly.m_fTargetRange * m_CurPhantomFly.m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurPhantomFly.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurPhantomFly.m_fTargetDownHeight )
			continue;

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_PhantomTarget = szTarget;
		}
	}
}

bool ioPhantomItem::CheckTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurPhantomFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurPhantomFly.m_fTargetRange * m_CurPhantomFly.m_fTargetRange;

	if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurPhantomFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurPhantomFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
		if( !pTarget )
			return false;

		if( !IsEnableTargetState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				return false;
		}

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurPhantomFly.m_fTargetUpHeight )
			return false;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurPhantomFly.m_fTargetDownHeight )
			return false;

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioPhantomItem::CheckTraceTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fTargetRangeSq = m_CurPhantomFly.m_fTraceMoveGapRange * m_CurPhantomFly.m_fTraceMoveGapRange;

	if( m_bAimedBall )
	{
		ioBall *pBall = pOwner->FindBall();
		if( !pBall ) return false;

		D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
		if( !pTarget )
			return false;

		if( !IsEnableTargetTraceState( pTarget ) )
			return false;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			return false;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 거리 체크
		if( fDiffRangeSq >= fTargetRangeSq )
			return false;
	}

	return true;
}

bool ioPhantomItem::CheckPossessionTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_PhantomTarget );
	if( !pTarget )
		return false;

	if( !IsEnableTargetPossessionState( pTarget ) )
		return false;

	if( pTarget->GetExperienceMode() != EMS_NONE )
		return false;

	return true;
}

bool ioPhantomItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurPhantomFly.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurPhantomFly.m_fTargetRange * m_CurPhantomFly.m_fTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

	// 각도 체크
	if( fCurAngle != 0.0f )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXVECTOR3 vDiff2 = vDiff;
		vDiff2.y = 0.0f;
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
		if( fCurValue < fHalfValue )
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_CurPhantomFly.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurPhantomFly.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

const ioHashString& ioPhantomItem::GetPhantomTarget()
{
	return m_PhantomTarget;
}

const ioHashString& ioPhantomItem::GetPhantomTargetSkill()
{
	return m_PhantomTargetSkill;
}

void ioPhantomItem::ClearPhantomTargetSkill()
{
	m_PhantomTargetSkill.Clear();
}

bool ioPhantomItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioPhantomItem::UpdatePhantomFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect == 0 )
		return;

	pChar->UpdateAttachMapEffect( m_dwFlyMoveEffect, true );
}

void ioPhantomItem::ReleasePhantomFlyMoveEffect( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwFlyMoveEffect != 0 )
	{
		pChar->DestroyMapEffect( m_dwFlyMoveEffect );
		m_dwFlyMoveEffect = 0;
	}
}

void ioPhantomItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurPhantomFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurPhantomFly.m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioPhantomItem::CheckPhantomFlyMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurPhantomFly.m_DelayAni );
	float fTimeRate  = m_CurPhantomFly.m_fDelayAniRate;

	if( m_CurMoveAni == m_CurPhantomFly.m_DelayAni )
	{
		if( m_bSetUpKey || m_bSetDownKey || (m_CurDirKey != ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_CurPhantomFly.m_MoveAni );
			fTimeRate = m_CurPhantomFly.m_fMoveAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurPhantomFly.m_MoveAni;
		}
	}
	else if( m_CurMoveAni == m_CurPhantomFly.m_MoveAni )
	{
		if( !m_bSetUpKey && !m_bSetDownKey && (m_CurDirKey == ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurPhantomFly.m_DelayAni;
		}
	}
}

void ioPhantomItem::EquipPhantomMesh( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_bUsePhantomFly ) return;

	pOwner->SetInvisibleState( true, true, true );

	if( pOwner->IsMale() )
	{
		int iMeshCnt = m_CurPhantomFly.m_PhantomMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_CurPhantomFly.m_PhantomMeshList[i];
			pOwner->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_CurPhantomFly.m_PhantomFemaleMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_CurPhantomFly.m_PhantomFemaleMeshList[i];
			pOwner->AddEntity( szMeshName );
		}
	}

	if( !m_CurPhantomFly.m_PhantomFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurPhantomFly.m_PhantomFlyEffect, false );
		pOwner->AttachEffect( m_CurPhantomFly.m_PhantomFlyEffect );
	}

}

void ioPhantomItem::ReleasePhantomMesh( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !m_bUsePhantomFly ) return;

	m_bUsePhantomFly = false;

	if( pOwner->IsMale() )
	{
		int iMeshCnt = m_CurPhantomFly.m_PhantomMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_CurPhantomFly.m_PhantomMeshList[i];
			pOwner->DestroyEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_CurPhantomFly.m_PhantomFemaleMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_CurPhantomFly.m_PhantomFemaleMeshList[i];
			pOwner->DestroyEntity( szMeshName );
		}
	}

	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}

bool ioPhantomItem::IsEnablePossessionState()
{
	if( m_PhantomFlyState == PFS_TRACE_MOVE )
		return true;

	return false;
}

ioUserKeyInput::DirKeyInput ioPhantomItem::GetCurDirKey()
{
	return m_CurDirKey;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioPhantomFlySpecialState::ioPhantomFlySpecialState()
{
}

ioPhantomFlySpecialState::~ioPhantomFlySpecialState()
{
}

void ioPhantomFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioPhantomFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	//ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioPhantomItem *pPhantomItem = ToPhantomItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPhantomItem )
		pPhantomItem->ClearPhantomState( pOwner );

	pOwner->ClearAttackFireTimeAndSkill();
}

bool ioPhantomFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioPhantomItem *pPhantomItem = ToPhantomItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPhantomItem && pPhantomItem->IsPhantomFlyCharCollisionSkipState() )
		return true;

	return false;
}

void ioPhantomFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioPhantomItem *pPhantom = ToPhantomItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pPhantom )
		pPhantom->ProcessPhantomFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioPhantomFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioPhantomFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}