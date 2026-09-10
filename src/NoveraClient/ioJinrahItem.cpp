#include "stdafx.h"

#include "ioJinrahItem.h"
#include "ioDrinkItem.h"
#include "ioChargeComboDash.h"
#include "WeaponDefine.h"

ioJinrahItem::ioJinrahItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iSpecialCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_fCurGauge = 0.0f;
	m_bJump = false;
	m_bReserveAttack = false;
	m_dwReserveAttack = 0;
}

ioJinrahItem::ioJinrahItem( const ioJinrahItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fGatheringAniRate( rhs.m_fGatheringAniRate ),
m_szLandingAni( rhs.m_szLandingAni ),
m_fLandingAniRate( rhs.m_fLandingAniRate ),
m_dwFullChargeTime( rhs.m_dwFullChargeTime ),
m_cFullAttribute( rhs.m_cFullAttribute ),
m_cJumpFullAttribute( rhs.m_cJumpFullAttribute ),
m_ComboAttackAttribute( rhs.m_ComboAttackAttribute ),
m_JumpComboAttackAttribute( rhs.m_JumpComboAttackAttribute ),
m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
m_JumpCancelDashAttribute( rhs.m_JumpCancelDashAttribute ),
m_fCancelDashEndJumpAmt( rhs.m_fCancelDashEndJumpAmt ),
m_fCancelDashRandRate( rhs.m_fCancelDashRandRate ),
m_dwDashFireWeaponIndex( rhs.m_dwDashFireWeaponIndex ),
m_dwDashFireDelayTime( rhs.m_dwDashFireDelayTime ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fJumpHeightGap( rhs.m_fJumpHeightGap ),
m_fComboEndJumpPower( rhs.m_fComboEndJumpPower )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_iSpecialCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwMotionEndTime = 0;
	m_fCurGauge = 0.0f;
	m_bJump = false;
	m_bReserveAttack = false;
	m_dwReserveAttack = 0;

	m_bSpecialAttack = false;
	m_bHalfSpecialAttack = false;
}

ioJinrahItem::~ioJinrahItem()
{
}

void ioJinrahItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH] = "";

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;
	m_fGatheringAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "landing_animation", "", szBuf, MAX_PATH );
	m_szLandingAni = szBuf;
	m_fLandingAniRate = rkLoader.LoadFloat_e( "landing_ani_rate", FLOAT1 );

	int iCnt = rkLoader.LoadInt_e( "combo_attack_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "combo_attack%d", i+1 );
		AttackAttribute ComboAttack;
		LoadAttackAttribute( szBuf, ComboAttack, rkLoader );
		m_ComboAttackAttribute.push_back(ComboAttack);
	}
	iCnt = rkLoader.LoadInt_e( "jump_combo_attack_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szBuf, "jump_combo_attack%d", i+1 );
		AttackAttribute JumpAttack;
		LoadAttackAttribute( szBuf, JumpAttack, rkLoader );
		m_JumpComboAttackAttribute.push_back(JumpAttack);
	}

	LoadAttackAttribute_e( "full_attack", m_cFullAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_full_attack", m_cJumpFullAttribute, rkLoader );
	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	LoadAttackAttribute_e( "jump_cancel_dash", m_JumpCancelDashAttribute, rkLoader );

	m_dwDashFireWeaponIndex = (DWORD)rkLoader.LoadInt_e( "dash_fire_weapon", 0 );
	m_dwDashFireDelayTime = (DWORD)rkLoader.LoadInt_e( "dash_fire_time", 0 );

	m_fCancelDashEndJumpAmt = rkLoader.LoadFloat_e( "cancel_dash_end_jump_amt", 0.0f );
	m_fCancelDashRandRate   = rkLoader.LoadFloat_e( "cancel_dash_rand_rate", FLOAT1 );	

	m_fComboEndJumpPower = rkLoader.LoadFloat_e( "combo_end_jump_power", 500.0f );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_height_gap", 0.0f );

	m_dwFullChargeTime = (DWORD)rkLoader.LoadInt_e( "full_charge_time", 0 );
}

ioItem* ioJinrahItem::Clone()
{
	return new ioJinrahItem( *this );
}

ioWeaponItem::WeaponSubType ioJinrahItem::GetSubType() const
{
	return WST_JINRAH_ITEM;
}

void ioJinrahItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	ClearAttackState( pOwner );

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
	}
}

void ioJinrahItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );
			if( ProcessCancelBySpeicalDash( pOwner ) )
				return;

			CheckCancelReserve( pOwner );
			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioJinrahItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );

		m_pExtendDash->ProcessDash( pOwner );
	}
}

void ioJinrahItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeGatheringState( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioJinrahItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwMotionEndTime <= FRAMEGETTIME() )
			SetAttackState( pOwner, true, false );
	}
	else
		SetHalfAttackState( pOwner, true, false );
}

void ioJinrahItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::ChangeGatheringState( ioBaseChar *pOwner )
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

	m_ChargeState = CS_GATHERING;
	if( m_dwFullChargeTime == 0 )
		m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	else
		m_dwMotionEndTime = dwCurTime + m_dwFullChargeTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;

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
		ChangeToNormalAttack( pOwner );
		break;
	case CS_GATHERING:
		rkPacket >> vPos;
		rkPacket >> qRot;
		pOwner->SetWorldPosition( vPos );
		pOwner->SetTargetRotToRotate( qRot, true );
		ChangeGatheringState( pOwner );
		break;
	}
}

void ioJinrahItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> iState;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iState )
	{
	case JS_FIRE:
		{
			bool bJump;
			bool bHalf;
			rkPacket >> bJump;
			rkPacket >> bHalf;

			if( bHalf )
				SetHalfAttackState( pOwner, false, bJump );
			else
				SetAttackState( pOwner, false, bJump );
		}
		break;
	case JS_COMBO:
		{
			rkPacket >> m_iSpecialCombo;
			SetComboAttack( pOwner, false );
		}
		break;
	case JS_END:
		SetChangeEndState( pOwner, false );
		break;
	case JS_DASH:
		{
			int eCurrDir, eCallerType;
			rkPacket >> eCurrDir;
			rkPacket >> eCallerType;
			SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
		}
		break;
	case JS_DASHFIRE:
		{
			int iCount;
			rkPacket >> iCount;
			m_DashInfo.clear();
			for( int i = 0; i < iCount; i++ )
			{
				D3DXVECTOR3 cVec3;
				rkPacket >> cVec3;
				m_DashInfo.push_back( cVec3 );
			}
			SetDashFire( pOwner );
		}
		break;
	}
}

void ioJinrahItem::SetHalfAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	bool bPrev = m_bSpecialAttack;
	pOwner->SetState( CS_JINRAH_SPECIAL );
	m_JinrahAttackState = JS_COMBO;
	m_bJump = bJump;
	m_iSpecialCombo = 0;
	m_bHalfSpecialAttack = true;
	m_bSpecialAttack = bPrev;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	SetComboAttack( pOwner, false );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)JS_FIRE;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		kPacket << true;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::SetAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	bool bPrev = m_bHalfSpecialAttack;
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->SetState( CS_JINRAH_SPECIAL );
	m_JinrahAttackState = JS_FIRE;
	m_bJump = bJump;
	m_bSpecialAttack = true;
	m_bHalfSpecialAttack = bPrev;

	AttackAttribute cInfo;
	if( bJump )
		cInfo = m_cJumpFullAttribute;
	else
		cInfo = m_cFullAttribute;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = cInfo.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwAttackStartTime = dwCurTime; 
	m_dwFireSTime = m_dwAttackStartTime + ( pGrp->GetAnimationEventTime( iAniID, "fire_s" ) * cInfo.m_fAttackAniRate );
	m_dwMotionEndTime = m_dwAttackStartTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_JinrahAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << bJump;
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::SetComboAttack( ioBaseChar *pOwner, bool bSendNetwork )
{
	if( !pOwner )
		return;

	m_bReserveAttack = false;
	m_JinrahAttackState = JS_COMBO;

	std::vector<AttackAttribute> cInfoVec;
	if( m_bJump )
		cInfoVec = m_JumpComboAttackAttribute;
	else
		cInfoVec = m_ComboAttackAttribute;

	int iCnt = cInfoVec.size();
	if( !COMPARE(m_iSpecialCombo, 0, iCnt) )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllLoopAni( 100.0f, true );
		pGrp->ClearAllActionAni( 100.0f, true );

		AttackAttribute cCurAttack = cInfoVec[m_iSpecialCombo];

		int iAniID = pGrp->GetAnimationIdx(cCurAttack.m_AttackAnimation);
		if( iAniID == -1 )
			return;

		DWORD dwCurTime = FRAMEGETTIME();

		m_szCurAni = cCurAttack.m_AttackAnimation;
		m_dwInputCancelCheckTime = dwCurTime;

		pGrp->SetActionAni( iAniID, 100.0f, 100.0f, 1.0f, 1.0f/cCurAttack.m_fAttackAniRate, 0.0f, cCurAttack.m_dwPreDelay );	
		pOwner->RefreshFireTimeList( iAniID, cCurAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, cCurAttack.m_fAttackAniRate, cCurAttack.m_dwPreDelay );
		pOwner->IncreaseWeaponIndexBase();
		
		DWORD dwTrackingTime = dwCurTime + cCurAttack.m_dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * cCurAttack.m_fAttackAniRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		m_dwMotionEndTime = m_dwReserveAttack = dwCurTime;
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * cCurAttack.m_fAttackAniRate + cCurAttack.m_dwPreDelay;
		m_dwMotionEndTime += pOwner->GetActionStopTotalDelay();
		m_dwReserveAttack += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * cCurAttack.m_fAttackAniRate + cCurAttack.m_dwPreDelay;

		m_iSpecialCombo++;
	}

	if( pOwner->IsNeedSendNetwork() && bSendNetwork )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_JinrahAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_iSpecialCombo - 1;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	switch( m_JinrahAttackState )
	{
	case JS_FIRE:
		ProcessFireAttack( pOwner );
		break;
	case JS_COMBO:
		ProcessComboAttack( pOwner );
		break;
	case JS_DASH:
		ProcessCancelDashState( pOwner );
		break;
	case JS_DASHFIRE:
		ProcessDashFireState( pOwner );
		break;
	case JS_LAND:
		if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			SetChangeEndState( pOwner, true );
		}
		break;
	}
}

void ioJinrahItem::ProcessCancelDashState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetJumpPower( 0 );
	pOwner->SetAirJumpPower( 0 );
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
		else
			ChangeReserveKeyInputState( pOwner );

		m_iCurCombo = 0;
		m_DashInfo.clear();
		ClearKeyReserve();
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( !pOwner->IsNeedProcess() )
		return;

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	CheckReserve( pOwner );
	if( ProcessSpeicalFire( pOwner ) )
		return;
}

bool ioJinrahItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialTime )
	{
		dwInputSpecialTime *= m_fCurAniRate;
		dwInputSpecialTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioJinrahItem::ProcessSpeicalFire( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialTime )
	{
		dwInputSpecialTime *= m_fCurAniRate;
		dwInputSpecialTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
	{
		if( m_bEnableDefenseState )
		{
			ChangeToDashFire( pOwner );
			return true;
		}
	}
	return false;
}

void ioJinrahItem::ProcessComboAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_bJump )
	{
		pOwner->InitForcePower();
		pOwner->SetJumpPower( 0 );
		pOwner->SetAirJumpPower( 0 );
		pOwner->SetCurMoveSpeed( 0 );

		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
		pOwner->ClearReservedSliding();
		pOwner->SetForcePowerAmt( 0.0f );
	}
	if( !pOwner->IsNeedProcess() )
		return;

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		if( m_bReserveAttack )
		{
			SetComboAttack( pOwner, true );
		}
		else
		{
			pOwner->SetSKillEndJumpState( m_fComboEndJumpPower );
		}
	}
	else if( COMPARE(FRAMEGETTIME(), m_dwReserveAttack, m_dwMotionEndTime) )
	{
		if( m_bReserveAttack )
		{
			SetComboAttack( pOwner, true );
		}
		else if( pOwner->IsAttackKey() )
		{
			m_bReserveAttack = true;
		}
	}
}

void ioJinrahItem::ProcessFireAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_bJump )
	{
		pOwner->SetJumpPower( 0 );
		pOwner->SetAirJumpPower( 0 );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	if( m_bJump )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( fHeightGap < FLOAT1 )
			SetChangeLandState( pOwner );
	}
	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwMotionEndTime )
		SetChangeEndState( pOwner, true );
}

void ioJinrahItem::SetChangeLandState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_iCurCombo = 0;
	m_DashInfo.clear();
	ClearKeyReserve();
	m_JinrahAttackState = JS_LAND;
	m_bJump = false;
	m_bSpecialAttack = false;
	m_bHalfSpecialAttack = false;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pOwner->ClearAttackFireTimeAndSkill();

	int iAniID	     = pGrp->GetAnimationIdx( m_szLandingAni );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fLandingAniRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fLandingAniRate );
}

void ioJinrahItem::ProcessDashFireState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_bJump )
	{
		pOwner->SetJumpPower( 0 );
		pOwner->SetAirJumpPower( 0 );
		pOwner->SetCurJumpPowerAmt( 0.0f );
		pOwner->SetGravityAmt( 0.0f );
	}
	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwDashFireStartTime == 0 )
	{
		SetChangeEndState( pOwner, true );
		return;
	}
	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime > m_dwDashFireStartTime + m_dwDashFireDelayTime )
	{
		SetDashFire( pOwner );

		m_iCurCombo = 0;
		m_DashInfo.clear();
		ClearKeyReserve();
	}
}

void ioJinrahItem::SetDashFire( ioBaseChar *pOwner )
{
	int iCount = m_DashInfo.size();
	if( iCount >= 2)
	{
		int WeaponIndex = pOwner->IncreaseWeaponIndexBase();
		D3DXVECTOR3 vDir = m_DashInfo[1] - m_DashInfo[0];
		D3DXVec3Normalize( &vDir, &vDir );

		FireTime kFireTime;
		kFireTime.dwStart				= FRAMEGETTIME();
		kFireTime.iAttributeIdx			= m_dwDashFireWeaponIndex;
		kFireTime.iResistanceIdx		= 0;
		kFireTime.szWoundedAni			= "";
		kFireTime.dwWoundedAniDuration	= 0;
		kFireTime.bLoopWoundedAni		= false;
		kFireTime.dwWeaponIdx			= WeaponIndex;
		kFireTime.eFireTimeType			= FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, m_DashInfo[0], vDir, GetName() );
	}
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_JinrahAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();

		int iCount = m_DashInfo.size();
		kPacket << iCount;
		for( int i = 0; i < iCount; i++ )
			kPacket << m_DashInfo[i];
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	if ( fHeightGap >= FLOAT1 )
		pOwner->SetExtendAttackEndJump( m_fCancelDashEndJumpAmt, m_fCancelDashRandRate, false, true, false, true );
	else
		ChangeReserveKeyInputState( pOwner );
}

void ioJinrahItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_JinrahAttackState = JS_NONE;

	m_dwAttackStartTime = 0;
	m_dwMotionEndTime = 0;
	m_dwFireSTime = 0;
	if( !m_bJump )
	{
		m_bSpecialAttack = false;
		m_bHalfSpecialAttack = false;
	}
}

void ioJinrahItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioJinrahItem::OnReleased( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );
	
	ioAttackableItem::OnReleased( pOwner );

	m_fCurGauge = 0.0f;
}

void ioJinrahItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	m_iCurCombo = 0;
	m_DashInfo.clear();
	ClearKeyReserve();
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << JS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioJinrahItem::ChangeToDashFire( ioBaseChar *pOwner )
{
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();
	ClearKeyReserve();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwDashFireStartTime = FRAMEGETTIME();
	m_JinrahAttackState = JS_DASHFIRE;
	m_DashInfo.push_back( pOwner->GetMidPositionByRate() );
}

void ioJinrahItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	if( pOwner->GetState() != CS_JINRAH_SPECIAL )
		pOwner->SetState( CS_JINRAH_SPECIAL );
	if( ioAttackableItem::CCSD_JUMP == eCallerType )
		m_bJump = true;

	AttackAttribute cCurInfo;
	if( m_bJump )
		cCurInfo = m_JumpCancelDashAttribute;
	else
		cCurInfo = m_CancelDashAttribute;

	int iAniID = pGrp->GetAnimationIdx( cCurInfo.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = cCurInfo.m_dwPreDelay;
	DWORD dwEndDelay = cCurInfo.m_dwEndDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	m_bSpecialAttack = false;
	m_bHalfSpecialAttack = false;
	m_JinrahAttackState = JS_DASH;
	m_szCurAni = cCurInfo.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;
	m_DashInfo.push_back( pOwner->GetMidPositionByRate() );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = cCurInfo.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( cCurInfo.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		cCurInfo.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DecreaseGauge();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_JinrahAttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioJinrahItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioJinrahItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioJinrahItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioJinrahItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioJinrahItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioJinrahItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioJinrahItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioJinrahItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioJinrahItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioJinrahItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioJinrahItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioJinrahItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

bool ioJinrahItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	return IsEnableGauge();
}

bool ioJinrahItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if ( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_NORMAL_ATTACK:
			{
				if( !bCheck )
					return false;
				return true;
			}
		}
	}
	else if( pOwner->GetState() == CS_JINRAH_SPECIAL )
	{
		switch( m_JinrahAttackState )
		{
		case JS_COMBO:
		case JS_FIRE:
		case JS_DASH:
			{
				DWORD dwCurTime = FRAMEGETTIME();
				int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
				DWORD dwInputSpecialTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
				if( 0 < dwInputSpecialTime )
				{
					dwInputSpecialTime *= m_fCurAniRate;
					dwInputSpecialTime += m_dwInputCancelCheckTime;		
				}

				if( 0 < dwInputSpecialTime && dwCurTime > dwInputSpecialTime )
					return true;
			}
			break;
		}
	}
	return false;
}

void ioJinrahItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
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

		if( ToDrinkItem( pItem ) )
		{
			pOwner->SetDrinkState();
		}
		else
		{
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioJinrahItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_JINRAH )
		{
			fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_JINRAH )
		{
			fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		{
			fTypeRate = m_fExtraGaugeWound;
		}
		break;
	case EGT_WOUNDED_DEF:
		{
			fTypeRate = m_fExtraGaugeWoundDef;
		}
		break;
	}
	m_fCurGauge += fTypeRate;
	m_fCurGauge = max( 0.0f, m_fCurGauge );
	m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
}

void ioJinrahItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pOwner = pWeapon->GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMissileWeapon6 *pMissile6 = ToMissileWeapon6( pWeapon );
	if( pMissile6 )
		pMissile6->SetMoveInfo( m_DashInfo );

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

void ioJinrahItem::ClearJumpSpecial()
{
	m_bJump = false;
	m_bSpecialAttack = false;
	m_bHalfSpecialAttack = false;
}
//////////////////////////////////////////////////////////////////////////
ioJinrahSpecialState::ioJinrahSpecialState()
{
}

ioJinrahSpecialState::~ioJinrahSpecialState()
{
}

void ioJinrahSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioJinrahSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioJinrahItem *pJinrah = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJinrah )
		pJinrah->ClearAttackState( pOwner );
}

void ioJinrahSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioJinrahItem *pJinrah = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJinrah )
		pJinrah->ProcessSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioJinrahSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

bool ioJinrahSpecialState::IsEnableDashState( ioBaseChar* pOwner )
{
	ioJinrahItem *pJinrah = ToJinrahItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pJinrah && pJinrah->IsEnableAttackDashCheck( pOwner, true ) )
		return true;

	return false;
}