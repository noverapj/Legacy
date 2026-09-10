#include "StdAfx.h"
#include "ioCannonWeaponItem.h"
#include "ioCannonWeaponDummy.h"

#include "ioFloatThrowWeapon.h"
#include "ioZoneEffectWeapon.h"

ioCannonWeaponItem::ioCannonWeaponItem(void)
{
	m_iCurCombo = 0;
	m_fCurExtraGauge =0.0f;
	m_dwControlDummyIndex = -1;
	m_dwStayDummyIndex = -1;
	ClearData();
}

ioCannonWeaponItem::ioCannonWeaponItem( const ioCannonWeaponItem &rhs ):
ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_szDisableChargeBuff( rhs.m_szDisableChargeBuff ),
m_szControlDummyName( rhs.m_szControlDummyName ),
m_szStayDummyName( rhs.m_szStayDummyName ),
m_vControlDummyOffset( rhs.m_vControlDummyOffset ),
m_vStayDummyOffset( rhs.m_vStayDummyOffset ),
m_fCircleMinRange( rhs.m_fCircleMinRange ),
m_fCircleMaxRange( rhs.m_fCircleMaxRange ),
m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
m_ReadyCircle( rhs.m_ReadyCircle ),
m_RedEffect( rhs.m_RedEffect ),
m_BlueEffect( rhs.m_BlueEffect ),
m_szCannonEndAni( rhs.m_szCannonEndAni ),
m_fCannonEndAniRate( rhs.m_fCannonEndAniRate ),
m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
m_fRevisionRate( rhs.m_fRevisionRate ),
m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
m_fUpLimitRange( rhs.m_fUpLimitRange ),
m_CannonAttack( rhs.m_CannonAttack ),
m_CannonFullAttack( rhs.m_CannonFullAttack ),
m_fCannonEscapeEndJumpPower( rhs.m_fCannonEscapeEndJumpPower ),
m_vOwnerBuffList( rhs.m_vOwnerBuffList ),
m_fCannonAttackNeedGauge( rhs.m_fCannonAttackNeedGauge ),
m_szCannonAttackAni( rhs.m_szCannonAttackAni ),
m_fCannonAttackAniRate( rhs.m_fCannonAttackAniRate )
{
	m_iCurCombo = 0;
	m_fCurExtraGauge =0.0f;
	m_dwControlDummyIndex = -1;
	m_dwStayDummyIndex = -1;
	ClearData();
}

ioCannonWeaponItem::~ioCannonWeaponItem(void)
{
}

ioItem* ioCannonWeaponItem::Clone()
{
	return new ioCannonWeaponItem( *this );
}

ioWeaponItem::WeaponSubType ioCannonWeaponItem::GetSubType() const
{
	return ioWeaponItem::WST_CANNON_ITEM;
}

void ioCannonWeaponItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	//Attack Ready
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	if ( iMaxCombo > 0 )
	{
		m_AttackReadyAniList.reserve( iMaxCombo );
		for( int i=0; i < iMaxCombo; ++i )
		{
			wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			m_AttackReadyAniList.push_back( ioHashString(szBuf) );
		}
	}

	//Charge
	rkLoader.LoadString_e( "disable_charge_buff", "", szBuf, MAX_PATH );
	m_szDisableChargeBuff = szBuf;
	LoadWeaponInfo( "cannon_attack", m_CannonAttack, rkLoader );
	LoadWeaponInfo( "cannon_full_attack", m_CannonFullAttack, rkLoader );
	m_fCannonEscapeEndJumpPower = rkLoader.LoadFloat_e( "cannon_escape_end_jump_power", 0.0f );

	//Dummy
	rkLoader.LoadString_e( "control_dummy_name", "", szBuf, MAX_PATH );
	m_szControlDummyName = szBuf;
	rkLoader.LoadString_e( "stay_dummy_name", "", szBuf, MAX_PATH );
	m_szStayDummyName = szBuf;
	m_vControlDummyOffset.x = rkLoader.LoadFloat_e( "control_dummy_offset_x", 0.0f );
	m_vControlDummyOffset.y = rkLoader.LoadFloat_e( "control_dummy_offset_y", 0.0f );
	m_vControlDummyOffset.z = rkLoader.LoadFloat_e( "control_dummy_offset_z", 0.0f );
	m_vStayDummyOffset.x = rkLoader.LoadFloat_e( "stay_dummy_offset_x", 0.0f );
	m_vStayDummyOffset.y = rkLoader.LoadFloat_e( "stay_dummy_offset_y", 0.0f );
	m_vStayDummyOffset.z = rkLoader.LoadFloat_e( "stay_dummy_offset_z", 0.0f );

	//Magic Circle
	m_fCircleMinRange = rkLoader.LoadFloat_e( "magic_circle_min_range", 0.0f );
	m_fCircleMaxRange = rkLoader.LoadFloat_e( "magic_circle_max_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "cannon_end_ani", "", szBuf, MAX_PATH );
	m_szCannonEndAni = szBuf;
	m_fCannonEndAniRate = rkLoader.LoadFloat_e( "cannon_end_ani_rate", 0.0f );

	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "gathering_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "gathering_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "gathering_min_speed", 0.0f );

	m_fRevisionRate = rkLoader.LoadFloat_e( "gathering_revision_rate", 0.0f );

	//
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );

	int nBuffCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_vOwnerBuffList.clear();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		wsprintf_e( szKey, "owner_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vOwnerBuffList.push_back( szBuf );
	}

	m_fCannonAttackNeedGauge = rkLoader.LoadFloat_e( "cannon_attack_need_gauge", 0.0f );

	rkLoader.LoadString_e( "cannon_attack_ani", "", szBuf, MAX_PATH );
	m_szCannonAttackAni = szBuf;
	m_fCannonAttackAniRate = rkLoader.LoadFloat_e( "cannon_attack_ani_rate", 0.0f );
}

void ioCannonWeaponItem::ClearData()
{
	m_ChargeState = CS_NONE;
	m_SpecailState = SS_NONE;
	m_dwMotionStartTime = 0;
	m_dwMotionEndTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwCurMapEffect = -1;
	m_vWeaponDir = ioMath::VEC3_ZERO;
	m_fCurWeaponMoveSpeed = 0.0f;
	m_fCurWeaponFloatPower = 0.0f;
	m_bCreateDummy = false;
	m_bFullGauge = false;
	m_dwCircleEffectID = -1;
	m_bCreateFullChargeEffect = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

void ioCannonWeaponItem::ClearSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecailState = SS_NONE;
	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetAttachEffectAlphaRateDirect( MAX_ALPHA_RATE, true );
	pOwner->SetHideChar( false );
	m_fCurExtraGauge = 0.0f;
	RemoveOwnerBuff( pOwner );
	RemoveCircleMapEffect( pOwner );
	ClearData();
}

int ioCannonWeaponItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioCannonWeaponItem::GetCurBullet()
{
	return m_fCurExtraGauge;
}

void ioCannonWeaponItem::SetCurBullet( int nCurBullet )
{
	if( nCurBullet < 0 )
		return;

	float fCurBullet = nCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioCannonWeaponItem::GetNeedBullet()
{
	return (int)m_fCannonAttackNeedGauge;
}

void ioCannonWeaponItem::DecreaseAllGauge()
{
	m_fCurExtraGauge = 0.0f;
}

void ioCannonWeaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner )
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	ClearCancelInfo();
	ClearData();

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
			pOwner->StopAutoTargetTracking();
	}
}

void ioCannonWeaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ProcessNormalAttack( pOwner );
		break;
	}
}

void ioCannonWeaponItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	ClearCancelInfo();

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
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_NORMAL_ATTACK;
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() && EnableCannonSpecialState( pOwner ) )
	{
		if( m_dwMotionStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			SetCannonSpecialState( pOwner, true );
	}
	else
		ChangeToNormalAttack( pOwner );
}

void ioCannonWeaponItem::ProcessNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );		
		if( ProcessCancel(pOwner) )
			return;
	}

	ioWeaponItem::CheckNormalAttack( pOwner );
}

bool ioCannonWeaponItem::EnableCannonSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( pOwner->HasBuff( pOwner->GetCharName(), m_szDisableChargeBuff ) )
		return false;

	return true;
}

void ioCannonWeaponItem::SetCannonSpecialState( ioBaseChar *pOwner, bool bSendPacket )
{
	if ( !pOwner )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->ClearAttackFireTimeAndSkill();
	ClearCancelInfo();

	AddOwnerBuff( pOwner );

	m_dwControlDummyIndex = -1;
	m_dwStayDummyIndex = -1;

	if ( pOwner->GetState() != CS_CANNON_SPECIAL )
		pOwner->SetState( CS_CANNON_SPECIAL );

	pOwner->SetAlphaRateDirect( 0.0f );
	pOwner->SetAttachEffectAlphaRateDirect( 0, true );
	pOwner->SetHideChar( true );

	m_SpecailState = SS_START;
	if( bSendPacket )
		m_vStartPos = pOwner->GetWorldPosition();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_START;
		kPacket << m_vStartPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ProcessCannonSpecialState( ioBaseChar *pOwner )
{
	if( !CheckSpecialState( pOwner ) )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	switch( m_SpecailState )
	{
	case SS_START:
		if ( pOwner->IsNeedProcess() )
			ChangeToCannonDelayState( pOwner, true );
		break;
	case SS_CANNON_DELAY:
		ProcessCannonDelayState( pOwner );
		break;
	case SS_CANNON_CHARGE:
		ProcessCannonChargeState( pOwner );
		break;
	case SS_CANNON_ATTACK:
		ProcessCannonAttackState( pOwner );
		break;
	case SS_END_MOTION:
		ProcessCannonEndMotionState( pOwner );
		break;
	case SS_CANNON_ATTACK_FAIL:
		ProcessCannonAttackFailState( pOwner );
		break;
	}

	ProcessControlCannonRot( pOwner );
}

bool ioCannonWeaponItem::CheckSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner)
		return false;

	if ( m_SpecailState == SS_NONE )
		return false;

	if ( m_SpecailState != SS_START && m_SpecailState != SS_END_MOTION )
	{
		ioDummyChar *pStayDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwStayDummyIndex );
		if ( !pStayDummy )
			return false;

		ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
		if ( !pControlDummy )
			return false;
	}

	return true;
}

void ioCannonWeaponItem::ProcessControlCannonRot( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_SpecailState != SS_CANNON_DELAY && m_SpecailState != SS_CANNON_CHARGE )
		return;

	ioDummyChar *pStayDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwStayDummyIndex );
	if ( !pStayDummy )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;
	
	D3DXVECTOR3 vPos = pStayDummy->GetWorldPosition();
	vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, pStayDummy, true );
	pStayDummy->SetWorldPosition( vPos );
	vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, pControlDummy, true );
	pControlDummy->SetWorldPosition( vPos );

	bool bChange = false;
	if ( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if ( eDir != m_CurDirKey )
		{
			m_CurDirKey = eDir;
			bChange = true;
		}
	}
	
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXVECTOR3 vPos;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

		vPos = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos *= m_fCircleMoveSpeed * g_FrameTimer.GetSecPerFrame();
		m_vCirclePos += vPos;
	}

	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCurXZ = m_vCirclePos;

	vCharPos.y = 0.0f;
	vCurXZ.y = 0.0f;

	D3DXVECTOR3 vDiff = vCurXZ - vCharPos;
	if( D3DXVec3Length( &vDiff ) > m_fCircleMaxRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= m_fCircleMaxRange;

		float fCurHeight = m_vCirclePos.y;
		m_vCirclePos = vCharPos + vDiff;
		m_vCirclePos.y = fCurHeight;
	}
	else if( D3DXVec3Length( &vDiff ) < m_fCircleMinRange )
	{
		D3DXVec3Normalize( &vDiff, &vDiff );
		vDiff *= m_fCircleMinRange;

		float fCurHeight = m_vCirclePos.y;
		m_vCirclePos = vCharPos + vDiff;
		m_vCirclePos.y = fCurHeight;
	}

	float fMapHeight = pStage->GetMapHeight( m_vCirclePos.x, m_vCirclePos.z, pOwner, false );
	if( fMapHeight > 0.0f )
		m_vCirclePos.y = fMapHeight;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwCircleEffectID );
	if( pEff )
	{
		pEff->SetWorldPosition( m_vCirclePos );
		pEff->SetWorldOrientation( pOwner->GetTargetRot() );
	}

	D3DXVECTOR3 vCircleDir = m_vCirclePos - pOwner->GetWorldPosition();
	vCircleDir.y = 0.0f;
	D3DXVec3Normalize( &vCircleDir, &vCircleDir );

	pOwner->SetTargetRotToDir( vCircleDir, true );
	pControlDummy->SetWorldOrientation( pOwner->GetTargetRot() );

	if ( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_DIR_CANGE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ChangeToCannonDelayState( ioBaseChar *pOwner, bool bStart )
{
	m_SpecailState = SS_CANNON_DELAY;

	if ( !pOwner )
		return;

	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPos;
	if( bStart )
		vPos = m_vStartPos;
	else
		vPos = pOwner->GetWorldPosition();

	if( !m_bCreateDummy )
	{
		D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
		int iIndex = g_DummyCharMgr.GetCurLocalIndex();
		D3DXVECTOR3 vDummyPos = vPos + qtRot * m_vStayDummyOffset;
		ioDummyChar *pStayDummy = pStage->CreateDummyChar( m_szStayDummyName, iIndex, pOwner->GetCharName(), vDummyPos, 0.0f, 0, true );
		if ( !pStayDummy )
			return;

		m_dwStayDummyIndex = pStayDummy->GetDummyCharIdx();

		vDummyPos = vPos + qtRot * m_vControlDummyOffset;
		iIndex = g_DummyCharMgr.GetCurLocalIndex();
		ioDummyChar *pControlDummy = pStage->CreateDummyChar( m_szControlDummyName, iIndex, pOwner->GetCharName(), vDummyPos, 0.0f, 0, true );
		if ( !pControlDummy )
			return;

		m_dwControlDummyIndex = pControlDummy->GetDummyCharIdx();
		m_bCreateDummy = true;

		ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
		if ( !pCannon )
			return;

		pCannon->SetTranslateEnable( false );
	}
	else
	{
		ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
		if ( !pControlDummy )
			return;
		ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
		if ( !pCannon )
			return;
		pCannon->SetCannonDelayState();
		pCannon->SetTranslateEnable( false );
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_vCirclePos == ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vOffSet = pOwner->GetMoveDir() * m_fMagicCircleOffSet;
			m_vCirclePos = vPos + vOffSet;
			m_vCirclePos.y = pStage->GetMapHeight( m_vCirclePos.x, m_vCirclePos.z );
			if( m_vCirclePos.y <= 0.0f )
				m_vCirclePos.y = pOwner->GetBottomHeight();
		}
		
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_ReadyCircle, m_vCirclePos, vScale );
		if ( pMapEffect )
			m_dwCircleEffectID = pMapEffect->GetUniqueID();
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_DELAY;
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ProcessCannonDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	//공격 체크
	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsAttackKey() )
		{
			ChangeToCannonCharge( pOwner );
			return;
		}

		// 종료 체크1
		if( pOwner->IsDefenseKey() )
		{
			ChangeToEndMotionState( pOwner );
			return;
		}

		// 종료 체크2
		if ( pOwner->IsJumpKey() )
		{
			ChangeToCannonEscape( pOwner );
			return;
		}
	}	
}

void ioCannonWeaponItem::ChangeToCannonCharge( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;

	ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
	if ( !pCannon )
		return;

	pCannon->SetCannonChargeState();

	m_SpecailState = SS_CANNON_CHARGE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_CHARGE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ProcessCannonChargeState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		if ( m_fCurExtraGauge < m_fCannonAttackNeedGauge )
		{
			ChangeToCannonAttackFailState( pOwner );
			return;
		}
		else
		{
			ChangeToCannonAttack( pOwner );
			return;
		}
	}
	
	if ( m_fCurExtraGauge == m_fMaxExtraGauge && !m_bCreateFullChargeEffect )
		AddCannonFullChargeEffect( pOwner );
}

void ioCannonWeaponItem::ChangeToCannonAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;

	ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
	if ( !pCannon )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( m_fCurExtraGauge == m_fMaxExtraGauge )
			m_bFullGauge = true;
		else
			m_bFullGauge = false;
	}
	
	m_dwMotionEndTime = 0;
	m_dwFireStartTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	pCannon->SetCannonAttackState( m_bFullGauge, m_dwMotionEndTime, m_dwFireStartTime );
	//if ( m_dwFireStartTime > 0 )
		m_dwFireStartTime += dwCurTime;
	m_dwMotionEndTime += dwCurTime;

	m_SpecailState = SS_CANNON_ATTACK;
	m_bCreateFullChargeEffect = false;
	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	else if( pOwner->GetTeam() == TEAM_BLUE )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pOwner->CreateMapEffectBySkill( m_BlueEffect, m_vCirclePos );
		else
			pMapEffect = pOwner->CreateMapEffectBySkill( m_RedEffect, m_vCirclePos );
	}

	CheckWeaponFloatInfo( pOwner );

	if( pMapEffect )
	{
		if( m_dwCurMapEffect != -1 )
			pOwner->EndMapEffect( m_dwCurMapEffect, false );

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
	}

	// 카메라 흔들림 현상을 위한 더미 애니 추가
	int iAniID = pGrp->GetAnimationIdx( m_szCannonAttackAni );
	if ( iAniID != -1 )
	{
		float fTimeRate = m_fCannonAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->ClearAllLoopAni( FLOAT10, true );
		pGrp->ClearAllActionAni( FLOAT10, true );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}

	//SendNet
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_ATTACK;
		kPacket << m_bFullGauge;
		kPacket << m_vCirclePos;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	//

	DecreaseAllGauge();
	RemoveCircleMapEffect( pOwner );

}

void ioCannonWeaponItem::ProcessCannonAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToCannonDelayState( pOwner, false );
		return;
	}

	if ( m_dwFireStartTime && m_dwFireStartTime < FRAMEGETTIME() )
	{
		m_dwFireStartTime = 0;
		CreateCannonWeapon( pOwner );
	}
}

void ioCannonWeaponItem::CreateCannonWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	WeaponInfo rkInfo;
	if ( m_bFullGauge )
		rkInfo = m_CannonFullAttack;
	else
		rkInfo = m_CannonAttack;

	//무기 생성
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_FIRE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::CheckWeaponFloatInfo( ioBaseChar *pOwner )
{
	// WeaponMove
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = m_vCirclePos - vCharPos;
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	// CharDir
	if( vDir == ioMath::VEC3_ZERO )
	{
		vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );
	}
	else
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		pOwner->SetTargetRotAndMoveDirChange( qtRot, false );
		pOwner->SetWorldOrientation( qtRot );
	}

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( m_fCircleMaxRange > 0.0f )
	{
		fRate = fRange / m_fCircleMaxRange;

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pOwner->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = fRange / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
	}

	m_vWeaponDir = vDir;
	m_fCurWeaponMoveSpeed = fCurWeaponMoveSpeed;
	m_fCurWeaponFloatPower = fCurWeaponFloatPower;
}

float ioCannonWeaponItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioCannonWeaponItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioCannonWeaponItem::ChangeToEndMotionState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->ClearReservedSliding();
	ClearCancelInfo();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szCannonEndAni );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_fCannonEndAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetAttachEffectAlphaRateDirect( MAX_ALPHA_RATE, true );
	pOwner->SetHideChar( false );

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;
	pControlDummy->SetDieState( false, true );

	m_SpecailState = SS_END_MOTION;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwCurTime;

	RemoveCircleMapEffect( pOwner );
	RemoveOwnerBuff( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ProcessCannonEndMotionState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
			return;
		}
	}
	
}

void ioCannonWeaponItem::ChangeToCannonEscape( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;

	ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
	if ( !pCannon )
		return;

	ioDummyChar *pStayDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwStayDummyIndex );
	if ( !pStayDummy )
		return;

	ioCannonWeaponDummy* pStayCannon = ToCannonWeaponDummy( pStayDummy );
	if ( !pStayCannon )
		return;

	pOwner->SetAlphaRateDirect( MAX_ALPHA_RATE );
	pOwner->SetAttachEffectAlphaRateDirect( MAX_ALPHA_RATE, true );
	pOwner->SetHideChar( false );

	DWORD dwMotionEndTime = pCannon->SetCannonDeadLineState();
	pStayCannon->SetCannonDeadLineState( dwMotionEndTime );

	pOwner->SetExtendAttackEndJump( m_fCannonEscapeEndJumpPower, FLOAT1, false, true, true );

	pOwner->AddNewBuff( m_szDisableChargeBuff, pOwner->GetCharName(), GetName(), NULL );

	RemoveCircleMapEffect( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_ESCAPE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::ChangeToCannonAttackFailState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;
	ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
	if ( !pCannon )
		return;
	m_dwMotionEndTime = 0;
	pCannon->SetCannonAttackFailState( m_dwMotionEndTime );

	m_SpecailState = SS_CANNON_ATTACK_FAIL;

	RemoveCircleMapEffect( pOwner );
	m_bCreateFullChargeEffect = false;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_ATTACK_FAIL;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_fCurExtraGauge = 0.0f;
}

void ioCannonWeaponItem::ProcessCannonAttackFailState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		ChangeToCannonDelayState( pOwner, false );
		return;
	}
}

void ioCannonWeaponItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		return;

	if ( pOwner->GetState() == CS_CANNON_SPECIAL && m_SpecailState == SS_CANNON_CHARGE )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );
	}
}

void ioCannonWeaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	float fCurDamageRate, fCurForceRate, fCurBlowRate;
	float fCurWoundedTimeRate, fCurLiveTimeRate;

	fCurDamageRate = fCurForceRate = fCurBlowRate = FLOAT1;
	fCurWoundedTimeRate = fCurLiveTimeRate = FLOAT1;

	if( m_fCurChargeRate > 0.0f && !bJumpAttack )
	{
		float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
		float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
		float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

		float fCurMaxWoundedTime = max( 0.0f, (m_fMaxWoundedTimeRate - FLOAT1) );
		float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

		fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
		fCurForceRate += fCurMaxForce * m_fCurChargeRate;
		fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

		fCurWoundedTimeRate += fCurMaxWoundedTime * m_fCurChargeRate;
		fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

		pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
		pWeapon->SetOwnerChargeForceRate( fCurForceRate );
		pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

		pWeapon->SetOwnerChargeWoundedTimeRate( fCurWoundedTimeRate );
		pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
	}

	ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
	if( !bJumpAttack && pFloatWeapon )
	{
		pFloatWeapon->SetMoveDir( m_vWeaponDir );
		pFloatWeapon->SetMoveSpeed( m_fCurWeaponMoveSpeed );
		pFloatWeapon->SetFloatPower( m_fCurWeaponFloatPower );
		pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
	}

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

void ioCannonWeaponItem::AddOwnerBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_vOwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
		pOwner->AddNewBuff( m_vOwnerBuffList[i], pOwner->GetCharName(), GetName(), NULL );
}

void ioCannonWeaponItem::RemoveOwnerBuff( ioBaseChar *pOwner )
{

	if ( !pOwner )
		return;

	int nBuffCnt = (int)m_vOwnerBuffList.size();
	for ( int i=0; i<nBuffCnt; i++ )
	{
		ioBuff *pRemoveBuff = pOwner->GetBuff( m_vOwnerBuffList[i], pOwner->GetCharName() );
		if ( pRemoveBuff )
			pRemoveBuff->SetReserveEndBuff();
	}
}

void ioCannonWeaponItem::RemoveCircleMapEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_dwCircleEffectID == -1 )
		return;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwCircleEffectID );
	if( pEff )
		pEff->EndEffectForce();
	m_dwCircleEffectID = -1;
}

void ioCannonWeaponItem::AddCannonFullChargeEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_bCreateFullChargeEffect )
		return;

	ioDummyChar *pControlDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_dwControlDummyIndex );
	if ( !pControlDummy )
		return;

	ioCannonWeaponDummy* pCannon = ToCannonWeaponDummy( pControlDummy );
	if ( !pCannon )
		return;

	pCannon->AddFullChargeEffect();

	m_bCreateFullChargeEffect = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ST_CANNON_FULL_CHARGE_EFFECT;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCannonWeaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == ST_NORMAL_ATTACK )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;
		pOwner->SetWorldPosition( vPos );

		rkPacket >> m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
	}
	else if ( iState == ST_CANNON_START )
	{
		rkPacket >> m_vStartPos;		
		SetCannonSpecialState( pOwner, false );
	}
	else if ( iState == ST_CANNON_DELAY )
	{
		rkPacket >> m_vCirclePos;
		ChangeToCannonDelayState( pOwner, true );
	}
	else if ( iState == ST_CANNON_CHARGE )
		ChangeToCannonCharge( pOwner );
	else if ( iState == ST_CANNON_END )
		ChangeToEndMotionState( pOwner );
	else if ( iState == ST_CANNON_ESCAPE )
		ChangeToCannonEscape( pOwner );
	else if ( iState == ST_CANNON_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> m_bFullGauge;
		rkPacket >> m_vCirclePos;
		rkPacket >> qtRot;
		pOwner->SetTargetRotAndMoveDirChange( qtRot );
		ChangeToCannonAttack( pOwner );
	}
	else if ( iState == ST_CANNON_FIRE )
		CreateCannonWeapon( pOwner );
	else if ( iState == ST_CANNON_ATTACK_FAIL )
		ChangeToCannonAttackFailState( pOwner );
	else if ( iState == ST_CANNON_FULL_CHARGE_EFFECT )
		AddCannonFullChargeEffect( pOwner );
	else if ( iState == ST_CANNON_DIR_CANGE )
	{
		int iDir;
		rkPacket >> iDir;
		m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		rkPacket >> m_vCirclePos;
	}
}

//---------------------------------------------------------------
ioCannonSpecialState::ioCannonSpecialState()
{

}

ioCannonSpecialState::~ioCannonSpecialState()
{

}

void ioCannonSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioCannonWeaponItem *pCannonWeaponItem = ToCannonWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCannonWeaponItem )
		pCannonWeaponItem->ClearSpecialState( pOwner );
}

void ioCannonSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioCannonWeaponItem *pCannonWeaponItem = ToCannonWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCannonWeaponItem )
		pCannonWeaponItem->ProcessCannonSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioCannonSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	return true;
}

