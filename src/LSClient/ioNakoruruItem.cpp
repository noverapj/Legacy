

#include "stdafx.h"

#include "ioNakoruruItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "DummyCharDefine.h"

ioNakoruruItem::ioNakoruruItem()
{
	m_fWitchFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurWitchFly.Init();
	m_fCurWitchGauge = 0.0f;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	m_WitchFlyState = WFS_NONE;
	m_dwEnableWitchEffect = -1;

	m_iCurWitchCombo = 0;
	m_dwWitchFlyKeyReserveTime = 0;
	m_dwWitchFlyCancelEndTime = 0;

	m_iEquipDummyCharIndex = -1;
}

ioNakoruruItem::ioNakoruruItem( const ioNakoruruItem &rhs )
: ioChargeCommandItem( rhs ),
m_AttackWitchFly( rhs.m_AttackWitchFly ),
m_fMaxWitchGauge( rhs.m_fMaxWitchGauge ),
m_fNeedWitchGauge( rhs.m_fNeedWitchGauge ),
m_fIncreaseWitchGauge( rhs.m_fIncreaseWitchGauge ),
m_fDecreaseWitchGauge( rhs.m_fDecreaseWitchGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseWitchGauge( rhs.m_fHighDecreaseWitchGauge ),
m_EnableWitchEffect( rhs.m_EnableWitchEffect ),
m_WitchFlyCancelAttack( rhs.m_WitchFlyCancelAttack ),
m_fWitchFlyCancelGravityRate( rhs.m_fWitchFlyCancelGravityRate ),
m_EquipDummyCharName( rhs.m_EquipDummyCharName ),
m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate )
{
	m_fWitchFlyCurRange = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	m_CurWitchFly.Init();
	m_fCurWitchGauge = 0.0f;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	m_WitchFlyState = WFS_NONE;
	m_dwEnableWitchEffect = -1;

	m_iCurWitchCombo = 0;
	m_dwWitchFlyKeyReserveTime = 0;
	m_dwWitchFlyCancelEndTime = 0;

	m_iEquipDummyCharIndex = -1;
	m_dwMotionEndTime = 0;
}

ioNakoruruItem::~ioNakoruruItem()
{
}

void ioNakoruruItem::LoadProperty( ioINILoader &rkLoader )
{
	ioChargeCommandItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_AttackWitchFly.Init();

	rkLoader.LoadString_e( "witch_fly_effect", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WitchFlyEffect = szBuf;

	rkLoader.LoadString_e( "witch_fly_wait_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_WaitAni = szBuf;
	m_AttackWitchFly.m_dwWaitDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_wait_duration", 0 );

	m_AttackWitchFly.m_dwChargeTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_charge_time", 0 );
	rkLoader.LoadString_e( "witch_fly_charge_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_ChargeAni = szBuf;

	wsprintf_e( szBuf, "witch_fly_attack" );
	LoadAttackAttribute( szBuf, m_AttackWitchFly.m_WitchAttackAttribute, rkLoader );

	int iWitchComboCnt = rkLoader.LoadInt_e( "witch_fly_combo_max", 0 );
	if( iWitchComboCnt > 0 )
	{
		for( int i=0; i < iWitchComboCnt; ++i )
		{
			AttackAttribute kAttack;
			wsprintf_e( szKey, "witch_fly_combo%d_attack", i+1 );
			LoadAttackAttribute( szKey, kAttack, rkLoader );

			m_AttackWitchFly.m_WitchComboList.push_back( kAttack );
		}
	}

	wsprintf_e( szBuf, "witch_fly_cancel_attack" );
	LoadAttackAttribute( szBuf, m_WitchFlyCancelAttack, rkLoader );

	m_fWitchFlyCancelGravityRate = rkLoader.LoadFloat_e( "witch_fly_cancel_gravity_rate", FLOAT1 );

	rkLoader.LoadString_e( "witch_fly_start_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_StartAni = szBuf;
	m_AttackWitchFly.m_fStartAniRate = rkLoader.LoadFloat_e( "witch_fly_start_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fStartAngle = rkLoader.LoadFloat_e( "witch_fly_start_angle", 0.0f );
	m_AttackWitchFly.m_fStartMaxRange = rkLoader.LoadFloat_e( "witch_fly_start_max_range", 0.0f );
	m_AttackWitchFly.m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "witch_fly_start_duration", 0 );

	rkLoader.LoadString_e( "witch_fly_delay_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_DelayAni = szBuf;
	m_AttackWitchFly.m_fDelayAniRate = rkLoader.LoadFloat_e( "witch_fly_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "witch_fly_move_ani", "", szBuf, MAX_PATH );
	m_AttackWitchFly.m_MoveAni = szBuf;
	m_AttackWitchFly.m_fMoveAniRate = rkLoader.LoadFloat_e( "witch_fly_move_ani_rate", FLOAT1 );

	m_AttackWitchFly.m_fMoveSpeed = rkLoader.LoadFloat_e( "witch_fly_move_speed", FLOAT100 );
	m_AttackWitchFly.m_fUpDownSpeed = rkLoader.LoadFloat_e( "witch_fly_updown_speed", FLOAT100 );

	m_AttackWitchFly.m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "witch_fly_rotate_speed", 0 );

	m_AttackWitchFly.m_fWitchFlyJumpAmt = rkLoader.LoadFloat_e( "witch_fly_jump_amt", 0.0f );

	m_fMaxWitchGauge = rkLoader.LoadFloat_e( "max_witch_gauge", FLOAT100 );
	m_fNeedWitchGauge = rkLoader.LoadFloat_e( "need_witch_gauge", FLOAT100 );

	m_fIncreaseWitchGauge = rkLoader.LoadFloat_e( "increase_witch_gauge", FLOAT1 );
	m_fDecreaseWitchGauge = rkLoader.LoadFloat_e( "decrease_witch_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseWitchGauge = rkLoader.LoadFloat_e( "high_decrease_witch_gauge", 3.0f );

	rkLoader.LoadString_e( "enable_witch_effect", "", szBuf, MAX_PATH );
	m_EnableWitchEffect = szBuf;

	//
	rkLoader.LoadString_e( "equip_dummy_char_name", "", szBuf, MAX_PATH );
	m_EquipDummyCharName = szBuf;

	//jumpAttack
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );
}

ioItem* ioNakoruruItem::Clone()
{
	return new ioNakoruruItem( *this );
}

ioWeaponItem::WeaponSubType ioNakoruruItem::GetSubType() const
{
	return WST_NAKORURU;
}

void ioNakoruruItem::OnEquiped( ioBaseChar *pOwner )
{
	ioChargeCommandItem::OnEquiped( pOwner );

	// equip dummychar create
	CreateEquipDummyChar( pOwner );
}

void ioNakoruruItem::OnReleased( ioBaseChar *pOwner )
{
	ReleaseEnableWitchEffect( pOwner );
	ClearWitchState( pOwner );
	m_fCurWitchGauge = 0.0f;

	// equip dummychar destroy
	DestroyEquipDummyChar( pOwner );

	ioChargeCommandItem::OnReleased( pOwner );
}

void ioNakoruruItem::CreateEquipDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	if ( pOwner->IsSystemState() )
		return;

	DestroyEquipDummyChar( pOwner );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_EquipDummyCharName, iCurIndex,
												   pOwner->GetCharName(),
												   vTargetPos,
												   0.0f,
												   0, true );

	if( pDummy )
	{
		m_iEquipDummyCharIndex = iCurIndex;
	}
}

void ioNakoruruItem::DestroyEquipDummyChar( ioBaseChar *pOwner )
{
	g_DummyCharMgr.DestroyMamaHahaDummy( pOwner );
	m_iEquipDummyCharIndex = -1;
}

void ioNakoruruItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsEnableWitchGauge() && CheckEnableOwnerAttach( pOwner ) && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			SetWitchFlyState( pOwner, m_AttackWitchFly, false );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioNakoruruItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( !pOwner )
		return;

	ioChargeCommandItem::OnEndExtendAttack( pOwner, bReleaseItem );
}

void ioNakoruruItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case ST_EXTRA_ADD:
		m_bSetExtraGauge = true;
		m_fCurExtraGauge = m_fMaxExtraGauge;
		SetExtraGaugeBuff( pOwner );
		break;
	case ST_EXTRA_REMOVE:
		m_bSetExtraGauge = false;
		m_fCurExtraGauge = 0.0f;
		ReleaseExtraGaugeBuff( pOwner, false );
		break;
	case ST_FLY_STATE:
		{
			CheckWitchFlyState( pOwner, rkPacket );
		}
		break;
	case ST_ROTATE:
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

			CheckWitchFlyMoveAni( pOwner );
		}
		break;
	case ST_JUMP_GRAP:
		{
			D3DXQUATERNION qtRot;

			rkPacket >> qtRot;
			rkPacket >> m_GrapplingTargetName;

			pOwner->SetTargetRot( qtRot );

			SetItemGrapplingState( pOwner, m_GrapplingTargetName );

			ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
			if( !pTarget ) 
				return ;
		}
		break;
	}
}

void ioNakoruruItem::CheckKeyInput( ioBaseChar *pOwner )
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
		CheckWitchFlyMoveAni( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ST_ROTATE;
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

void ioNakoruruItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_CurWitchFly.m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_CurWitchFly.m_dwRotateTime;
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

void ioNakoruruItem::CheckWitchFlyMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_CurWitchFly.m_DelayAni.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_DelayAni );
	float fTimeRate  = m_CurWitchFly.m_fDelayAniRate;

	if( m_CurMoveAni == m_CurWitchFly.m_DelayAni )
	{
		if( m_bSetUpKey || m_bSetDownKey || (m_CurDirKey != ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_MoveAni );
			fTimeRate = m_CurWitchFly.m_fMoveAniRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurWitchFly.m_MoveAni;
		}
	}
	else if( m_CurMoveAni == m_CurWitchFly.m_MoveAni )
	{
		if( !m_bSetUpKey && !m_bSetDownKey && (m_CurDirKey == ioUserKeyInput::DKI_NONE) )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

			m_CurMoveAni = m_CurWitchFly.m_DelayAni;
		}
	}
}

void ioNakoruruItem::CheckWitchFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iWitchFlyState;
	rkPacket >> iWitchFlyState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iWitchFlyState )
	{
	case WFS_WAIT:
	case WFS_MOVE:
		{
			bool bUseSkill;
			rkPacket >> bUseSkill;

			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurWitchGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetWitchFlyState( pOwner, m_AttackWitchFly, bUseSkill );
			return;
		}
		break;
	case WFS_CHARGE:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeWitchFlyCharge( pOwner );
			return;
		}
		break;
	case WFS_ATTACK:
		{
			rkPacket >> m_iCurWitchCombo;

			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeWitchFlyAttack( pOwner );
			return;
		}
		break;
	case WFS_MOVE2:
		{
			ChangeWitchFlyMove( pOwner, false );
		}
		break;
	case WFS_CANCEL:
		{
			pOwner->SetTargetRotToRotate( qtRot, true );
			pOwner->ApplyAutoTargetInfo( rkPacket );

			ChangeWitchFlyCancel( pOwner );
			return;
		}
		break;
	}
}

void ioNakoruruItem::SetWitchFlyState( ioBaseChar *pOwner, const WitchFlyInfo &rkWitchFlyInfo, bool bUseSkill )
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

	pOwner->SetState( CS_NAKORURU_FLY );
	ReleaseEnableWitchEffect( pOwner );

	m_iCurWitchCombo = 0;
	m_dwWitchFlyKeyReserveTime = 0;
	m_dwWitchFlyCancelEndTime = 0;

	m_CurWitchFly = rkWitchFlyInfo;

	if( !m_CurWitchFly.m_WitchFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurWitchFly.m_WitchFlyEffect, false );
		pOwner->AttachEffect( m_CurWitchFly.m_WitchFlyEffect );
	}

	if( m_CurWitchFly.m_WaitAni.IsEmpty() )
	{
		ChangeWitchFlyStart( pOwner );

		m_fWitchFlyCurRange = 0.0f;
	}
	else
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_WaitAni );
		pGrp->SetLoopAni( iAniID, FLOAT10 );

		m_WitchFlyState = WFS_WAIT;
		m_dwWitchFlyWaitTime = FRAMEGETTIME();
		m_dwWitchFlyStartTime = 0;
		m_dwWitchFlyAttackEndTime = 0;

		m_fWitchFlyCurRange = 0.0f;
	}

	//
	ioMamaHahaDummy *pMamaHaha = ToMamaHahaDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_iEquipDummyCharIndex ) );
	if( pMamaHaha )
	{
		pMamaHaha->SetMamaHahaOwnerAttach();
	}

	if( !bUseSkill && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << false;
		kPacket << m_fCurWitchGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNakoruruItem::ChangeWitchFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if( m_CurWitchFly.m_StartAni.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_StartAni );
	float fTimeRate  = m_CurWitchFly.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_WitchFlyState = WFS_START;
	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = FRAMEGETTIME(); 
	m_dwWitchFlyAttackEndTime = 0;
}

void ioNakoruruItem::ChangeWitchFlyMove( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	ClearKeyReserve();

	m_iCurWitchCombo = 0;
	m_dwWitchFlyKeyReserveTime = 0;
	m_dwWitchFlyCancelEndTime = 0;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_CurWitchFly.m_DelayAni.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_DelayAni );
	float fTimeRate  = m_CurWitchFly.m_fDelayAniRate;
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );		

	m_CurMoveAni = m_CurWitchFly.m_DelayAni;

	m_WitchFlyState = WFS_MOVE;	

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_FLY_STATE;
		kPacket << WFS_MOVE2;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNakoruruItem::ChangeWitchFlyCharge( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if( m_CurWitchFly.m_ChargeAni.IsEmpty() )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CurWitchFly.m_ChargeAni );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_WitchFlyState = WFS_CHARGE;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyChargeTime = dwCurTime;
	m_dwWitchFlyAttackEndTime = 0;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNakoruruItem::ChangeWitchFlyAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	
	ClearKeyReserve();
	m_dwWitchFlyKeyReserveTime = 0;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iMaxCombo = m_CurWitchFly.m_WitchComboList.size();
	if( iMaxCombo == 0 )
	{
		m_iCurWitchCombo = 0;
		return;
	}

	if( !COMPARE( m_iCurWitchCombo, 0, iMaxCombo ) )
		m_iCurWitchCombo = 0;

	if( m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_AttackAnimation.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_AttackAnimation );
	float fTimeRate  = m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_fAttackAniRate;
	DWORD dwPreDelay = m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_dwPreDelay;
	DWORD dwEndDelay = m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurWitchFly.m_WitchComboList[m_iCurWitchCombo].m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( fKeyReserve > 0.0f )
		m_dwWitchFlyKeyReserveTime = dwCurTime + fKeyReserve + dwPreDelay;

	m_WitchFlyState = WFS_ATTACK;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iCurWitchCombo;
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 이후...
	m_iCurWitchCombo++;
}

void ioNakoruruItem::ChangeWitchFlyCancel( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();
	ClearKeyReserve();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	if( m_WitchFlyCancelAttack.m_AttackAnimation.IsEmpty() )
		return;

	int iAniID	     = pGrp->GetAnimationIdx( m_WitchFlyCancelAttack.m_AttackAnimation );
	float fTimeRate  = m_WitchFlyCancelAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_WitchFlyCancelAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_WitchFlyCancelAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	pOwner->RefreshFireTimeList( iAniID, m_WitchFlyCancelAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->SetReservedSliding( m_WitchFlyCancelAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	m_WitchFlyState = WFS_CANCEL;

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;
	m_dwWitchFlyCancelEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_FLY_STATE;
		kPacket << m_WitchFlyState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioNakoruruItem::ChangeWitchFlyEnd( ioBaseChar *pOwner )
{
	m_WitchFlyState = WFS_END;

	if( pOwner && pOwner->IsNeedProcess() )
	{
		pOwner->SetExtendAttackEndJump( m_CurWitchFly.m_fWitchFlyJumpAmt, m_fExtendLandingRate );
	}
}

void ioNakoruruItem::ProcessWitchFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_fCurWitchGauge <= 0.0f )
	{
		ChangeWitchFlyEnd( pOwner );
		return;
	}

	switch( m_WitchFlyState )
	{
	case WFS_WAIT:
		if( m_dwWitchFlyWaitTime+m_CurWitchFly.m_dwWaitDuration < dwCurTime )
		{
			ChangeWitchFlyStart( pOwner );
			return;
		}
		break;
	case WFS_START:
		if( m_dwWitchFlyStartTime+m_CurWitchFly.m_dwStartDuration < dwCurTime )
		{
			ChangeWitchFlyMove( pOwner, false );
			return;
		}
		else if( !ProcessWitchFlyStart( pOwner ) )
		{
			ChangeWitchFlyMove( pOwner, false );
			return;
		}
		break;
	case WFS_MOVE:
		if( !ProcessWitchFlyMove( pOwner ) )
		{
			ChangeWitchFlyEnd( pOwner );
			return;
		}
		else
		{
			if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
			{
				if( m_CurWitchFly.m_dwChargeTime > 0 )
					ChangeWitchFlyCharge( pOwner );
				else
					ChangeWitchFlyAttack( pOwner );
				return;
			}

			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case WFS_CHARGE:
		if( pOwner->IsNeedProcess() )
		{
			if( pOwner->IsAttackKeyDown() )
			{
				if( m_dwWitchFlyChargeTime+m_CurWitchFly.m_dwChargeTime < dwCurTime )
				{
					ChangeWitchFlyCancel( pOwner );
					return;
				}
			}
			else
			{
				ChangeWitchFlyAttack( pOwner );
				return;
			}
		}
		break;
	case WFS_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			if( m_dwWitchFlyAttackEndTime < dwCurTime )
			{
				if( m_bEnableAttackState )
					ChangeWitchFlyCharge( pOwner );
				else
					ChangeWitchFlyMove( pOwner, true );
				return;
			}
			else if( m_dwWitchFlyKeyReserveTime > 0 && m_dwWitchFlyKeyReserveTime < dwCurTime )
			{
				if( !m_bEnableAttackState && pOwner->IsAttackKey() )
				{
					m_bEnableAttackState = true;
				}
			}
		}
		break;
	case WFS_CANCEL:
		if( !ProcessWitchFlyCancel( pOwner ) )
		{
			ChangeWitchFlyEnd( pOwner );
			return;
		}
		break;
	case WFS_END:
		break;
	}
}

bool ioNakoruruItem::ProcessWitchFlyLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime+dwLooseTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return false;
	}
	else if( m_dwWitchFlyKeyReserveTime > 0 && m_dwWitchFlyKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}

	return true;
}

void ioNakoruruItem::ProcessWitchJumpGrapMove( ioBaseChar *pOwner )
{
	if( m_WitchFlyState  == WFS_JUMP_GRAP )
	{
		if( CheckLanding( pOwner ) )
			StartLandAttack( pOwner );
	}
	else if( m_WitchFlyState == WFS_LAND_ATTACK )
	{
		;
	}
	else 
		assert( false );
}


bool ioNakoruruItem::ProcessWitchFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_CurWitchFly.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_CurWitchFly.m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_CurWitchFly.m_fStartMaxRange / (float)m_CurWitchFly.m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	
	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fWitchFlyCurRange+fDistance < m_CurWitchFly.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fWitchFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_CurWitchFly.m_fStartMaxRange - m_fWitchFlyCurRange;
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

bool ioNakoruruItem::ProcessWitchFlyMove( ioBaseChar *pOwner )
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
		float fCurMoveSpeed = m_CurWitchFly.m_fMoveSpeed * fTimePerSec;
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
		float fCurUpDownSpeed = m_CurWitchFly.m_fUpDownSpeed * fTimePerSec;

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

bool ioNakoruruItem::ProcessWitchFlyCancel( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwWitchFlyCancelEndTime > 0 && m_dwWitchFlyCancelEndTime < dwCurTime )
		return false;

	ProcessWitchFlyMoving( pOwner );

	return true;
}

void ioNakoruruItem::ProcessWitchFlyMoving( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	bool bCurMove = false;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	DWORD dwCurTime = FRAMEGETTIME();
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
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( 1.8f ) );

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}
	}

	// 높이값은 무시하여 바로 리턴될수 있으므로 여기서 한 번 해줘야함.
	if( pStage && pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vCurPos );
		pOwner->SetWorldPosition( vCurPos );
	}

	// 이곳부터 높이 값에 대한 처리
	float fCurGravity = pStage->GetRoomGravity();
	bool bDropZone = false;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	if( pOwner->GetCurJumpPowerAmt() > 0.0f )
	{
		vPos.y += pOwner->GetCurJumpPowerAmt() * fTimePerSec;
	}

	bool bPreFrameIsHigh = false;
	float fMapHeight = pOwner->GetBottomHeight();

	float fGravityAmt = pOwner->GetGravityAmt();
	float fCurJumpPowerAmt = pOwner->GetCurJumpPowerAmt();

	if( vPos.y > fMapHeight )
	{
		fGravityAmt += (fCurGravity*m_fWitchFlyCancelGravityRate) * fTimePerSec;

		vPos.y -= fGravityAmt * fTimePerSec;
		bPreFrameIsHigh = true;
	}

	bool bCurFrameDrop = false;
	float fCurGravityGapAmt = fGravityAmt - fCurJumpPowerAmt;
	// 현재 하강중이고, 지면에 착지 or 더 낮게 내려간 경우
	if( fCurJumpPowerAmt <= fGravityAmt && vPos.y <= fMapHeight )
	{
		vPos.y = fMapHeight;

		fCurJumpPowerAmt = 0.0f;
		fGravityAmt = 0.0f;
		bCurFrameDrop = bPreFrameIsHigh;
	}

	// 맵을 머리로 뚫는 현상 방지용
	if( fCurJumpPowerAmt > fGravityAmt )
	{
		D3DXVECTOR3 vDiff = vPos - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDiff, &vDiff );

		ioRay kRay( pOwner->GetMidPositionByRate(0.75), vDiff );
		if( pStage->CheckMapCollision( pOwner, kRay, vPos ) )
		{
			vPos = pOwner->GetWorldPosition();
		}
	}

	pOwner->SetGravityAmt( fGravityAmt );
	pOwner->SetCurJumpPowerAmt( fCurJumpPowerAmt );
	pOwner->SetCurFrameDrop( bCurFrameDrop );

	if( pStage && pOwner->IsFlyMapLimit() )
	{
		pStage->CheckFlyMapLimit( &vPos );
	}

	if( pStage && pOwner->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
	}

	pOwner->SetWorldPosition( vPos );
}

bool ioNakoruruItem::IsWitchFlyCharCollisionSkipState()
{
	switch( m_WitchFlyState )
	{
	case WFS_START:
		return true;
	case WFS_MOVE:
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE || m_bSetUpKey || m_bSetDownKey )
			return true;
		break;
	}

	return false;
}

int ioNakoruruItem::GetMaxBullet()
{
	return (int)m_fMaxWitchGauge;
}

int ioNakoruruItem::GetNeedBullet()
{
	return (int)m_fNeedWitchGauge;
}

int ioNakoruruItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurWitchGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxWitchGauge ) );

	return (int)fCurCoolTime;
}

void ioNakoruruItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxWitchGauge ) );

	m_fCurWitchGauge = fCurBullet;
}

void ioNakoruruItem::InitWitchGauge()
{
	m_fCurWitchGauge = 0.0f;
}

void ioNakoruruItem::MaxWitchGauge()
{
	m_fCurWitchGauge = m_fMaxWitchGauge;
}

float ioNakoruruItem::GetCurWitchGauge()
{
	return m_fCurWitchGauge;
}

void ioNakoruruItem::SetCurWitchGauge( float fGauge )
{
	m_fCurWitchGauge = fGauge;
	m_fCurWitchGauge = (float)max( 0, min( m_fCurWitchGauge, m_fMaxWitchGauge ) );
}

void ioNakoruruItem::SetEnableWitchEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableWitchEffect == -1 && !m_EnableWitchEffect.IsEmpty() )
	{
		ioEffect *pEffect = pChar->AttachEffect( m_EnableWitchEffect );
		if( pEffect )
		{
			m_dwEnableWitchEffect = pEffect->GetUniqueID();
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_NAKORURU_STATE );
			kPacket << pChar->GetCharName();
			kPacket << WITCH_ENABLE_GAUGE;
			kPacket << m_fCurWitchGauge;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioNakoruruItem::ReleaseEnableWitchEffect( ioBaseChar *pChar )
{
	if( pChar && m_dwEnableWitchEffect != -1 )
		pChar->EndEffect( m_dwEnableWitchEffect, false );

	m_dwEnableWitchEffect = -1;
}

void ioNakoruruItem::CheckEnableWitchGauge( ioBaseChar *pChar )
{
	if( m_fCurWitchGauge >= m_fNeedWitchGauge )
		SetEnableWitchEffect( pChar );
}

bool ioNakoruruItem::IsEnableWitchGauge()
{
	if( m_fCurWitchGauge >= m_fNeedWitchGauge )
		return true;

	return false;
}

bool ioNakoruruItem::IsWitchState()
{
	switch( m_WitchFlyState )
	{
	case WFS_WAIT:
	case WFS_START:
	case WFS_MOVE:
	case WFS_ATTACK:
	case WFS_CANCEL:
		return true;
	}

	return false;
}

void ioNakoruruItem::ClearWitchState( ioBaseChar *pOwner )
{
	m_WitchFlyState = WFS_NONE;

	// 순서중요
	if( pOwner && !m_CurWitchFly.m_WitchFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurWitchFly.m_WitchFlyEffect, false );
	}

	m_CurWitchFly.Init();
	m_CurMoveAni.Clear();

	m_dwWitchFlyWaitTime = 0;
	m_dwWitchFlyStartTime = 0;
	m_dwWitchFlyAttackEndTime = 0;

	//
	ioMamaHahaDummy *pMamaHaha = ToMamaHahaDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_iEquipDummyCharIndex ) );
	if( pMamaHaha )
	{
		pMamaHaha->ReleaseMamaHahaOwnerAttach();
	}
}

void ioNakoruruItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_NAKORURU_FLY:
		{
			float fGauge = m_fDecreaseWitchGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseWitchGauge * fTimePerSec;
			}

			m_fCurWitchGauge -= fGauge;
			m_fCurWitchGauge = max( 0.0f, m_fCurWitchGauge );
		}
		break;
	default:
		if( m_fCurWitchGauge < m_fMaxWitchGauge )
		{
			float fGauge = m_fIncreaseWitchGauge * fTimePerSec;

			m_fCurWitchGauge += fGauge;
			m_fCurWitchGauge = min( m_fCurWitchGauge, m_fMaxWitchGauge );
		}
		break;
	}
}

bool ioNakoruruItem::CheckEnableMamaHahaCmd( ioBaseChar *pOwner, CmdUseType eUseType )
{
	ioMamaHahaDummy *pMamaHaha = ToMamaHahaDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_iEquipDummyCharIndex ) );
	if( pMamaHaha )
	{
		return pMamaHaha->CheckEnableUseCmd( eUseType );
	}

	return false;
}


//jump attack
void ioNakoruruItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
	m_dwMotionEndTime = 0;
}


bool ioNakoruruItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	//-->
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_CurJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	pChar->SetJumpPower( m_CurJumpAttackInfo.m_fJumpPower );
	m_WitchFlyState = WFS_JUMP_GRAP;

	pChar->SetState( CS_NAKORURU_JUMP_ATTACK );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << ST_JUMP_GRAP;
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurJumpAttackInfo.m_GrapplingInfo );
	return true;
}


float ioNakoruruItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}


bool ioNakoruruItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}


void ioNakoruruItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ClearKeyReserve();
	m_dwWitchFlyKeyReserveTime = 0;

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_WitchFlyState = WFS_LAND_ATTACK;

	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fKeyReserve > 0.0f )
		m_dwWitchFlyKeyReserveTime = dwCurTime + fKeyReserve;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
										 iAniID,
										 m_CurJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList,
										 FTT_NORMAL_ATTACK,
										 fAniRate,
										 0 );
}

bool ioNakoruruItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime == 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}


D3DXVECTOR3 ioNakoruruItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
			vPos += m_fGrpplingCharToCharGap * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

bool ioNakoruruItem::CheckEnableOwnerAttach( ioBaseChar *pOwner )
{
	ioMamaHahaDummy *pMamaHaha = ToMamaHahaDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_iEquipDummyCharIndex ) );
	if( pMamaHaha )
	{
		return pMamaHaha->CheckEnableOwnerAttach();
	}

	return false;
}

void ioNakoruruItem::ProcessReserveDummyCreate( ioBaseChar *pOwner, const ioPlayMode* const pMode )
{
	if ( m_iEquipDummyCharIndex != -1 )
		return;

	if( pMode && !pMode->GetDummyTypeCreateValue() )
		return;

	if ( !m_EquipDummyCharName.IsEmpty() )
		CreateEquipDummyChar( pOwner );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNakoruruFlySpecialState::ioNakoruruFlySpecialState()
{
}

ioNakoruruFlySpecialState::~ioNakoruruFlySpecialState()
{
}

void ioNakoruruFlySpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNakoruruFlySpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	//ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru )
		pNakoruru->ClearWitchState( pOwner );

	pOwner->ClearAttackFireTimeAndSkill();
}

bool ioNakoruruFlySpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru )
	{
		if( pNakoruru->IsWitchFlyCharCollisionSkipState() )
			return true;
		else if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
			return true;
	}

	return false;
}

void ioNakoruruFlySpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru )
		pNakoruru->ProcessWitchFlyState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNakoruruFlySpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioNakoruruFlySpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioNakoruruJumpAttackSpecialState::ioNakoruruJumpAttackSpecialState()
{
}

ioNakoruruJumpAttackSpecialState::~ioNakoruruJumpAttackSpecialState()
{
}

void ioNakoruruJumpAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioNakoruruJumpAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioNakoruruJumpAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru )
	{
		/*float fHeightGap = GetWorldPosition().y - GetBottomHeight();

		if( m_bUseGhostState )
		{
			if( m_JumpState == JS_JUMP_ATTACK && GetBottomHeight() > 0.0f )
				m_fPreHeightForGhostState = GetBottomHeight();

			fHeightGap = GetWorldPosition().y - m_fPreHeightForGhostState;
		}*/

		pNakoruru->ProcessWitchJumpGrapMove( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioNakoruruJumpAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioNakoruruJumpAttackSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pNakoruru )
		return false;

	if( !pNakoruru->ProcessWitchFlyLandAttack( pOwner ) )
		return true;

	/*
	if( pNakoruru->GetWitchFlyState() == ioNakoruruItem::WFS_LAND_ATTACK && pNakoruru->IsLandAttackMotionEnd( m_dwActionStopTotalDelay) )
	{
		SetState( CS_DELAY );
		return;
	}
	*/

	if( pNakoruru->GetWitchFlyState() == ioNakoruruItem::WFS_JUMP_GRAP )
		return false;

	return true;
}

void ioNakoruruJumpAttackSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioNakoruruItem *pNakoruru = ToNakoruruItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pNakoruru && pNakoruru->GetWitchFlyState() == ioNakoruruItem::WFS_JUMP_GRAP )
		fCurGravity = pNakoruru->GetGrapGravity();
}