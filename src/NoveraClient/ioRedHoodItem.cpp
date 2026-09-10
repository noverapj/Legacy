
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioRedHoodItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioRedHoodItem::ioRedHoodItem()
{
	m_ChargeState = ES_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_fCurGauge = 0.0f;
	m_SpecialMoveState = SMS_NONE;
}

ioRedHoodItem::ioRedHoodItem( const ioRedHoodItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
m_NormalBlinkInfo( rhs.m_NormalBlinkInfo ),
m_ChargeAttackList( rhs.m_ChargeAttackList ),
m_stSD_Blow_Motion( rhs.m_stSD_Blow_Motion ),
m_fSD_BlowMotionRate( rhs.m_fSD_BlowMotionRate ),
m_SpecialMoveInfo( rhs.m_SpecialMoveInfo ),
m_CounterAttack( rhs.m_CounterAttack ),
m_CounterAttackList( rhs.m_CounterAttackList )
{
	m_ChargeState = ES_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_SpecialMoveState = SMS_NONE;

	m_fCurGauge = 0.0f;

	m_bThirdChargeAttack = false;

	m_bSDSkillUsed = false;

	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;
}

ioRedHoodItem::~ioRedHoodItem()
{
}

void ioRedHoodItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadInfo( rkLoader );
	

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT1 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", .0f );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );
}

void ioRedHoodItem::LoadInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "special_dash_animation", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_stDashAni = szBuf;
	m_NormalBlinkInfo.m_fDashAniRate = rkLoader.LoadFloat_e( "special_dash_ani_rate", FLOAT1 );
	m_NormalBlinkInfo.m_fDashSpeed = rkLoader.LoadFloat_e( "special_dash_speed", 0.f );
	m_NormalBlinkInfo.m_fDashLength = rkLoader.LoadFloat_e( "special_dash_range", 0.f );

	rkLoader.LoadString_e( "special_dash_end_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_stDashEndAni = szBuf;
	m_NormalBlinkInfo.m_fDashEndAniRate = rkLoader.LoadFloat_e( "special_dash_end_ani_rate", FLOAT1 );

	m_NormalBlinkInfo.m_fMaxBlinkLength = rkLoader.LoadFloat_e( "max_blink_length", 0.f );
	m_NormalBlinkInfo.m_fMapHeightGap = rkLoader.LoadFloat_e( "max_height_gap", 0.f );

	rkLoader.LoadString_e( "blink_start_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_StartBlinkMotion = szBuf;
	m_NormalBlinkInfo.m_fStartBlinkMotionRate = rkLoader.LoadFloat_e( "blink_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "blink_end_ani", "", szBuf, MAX_PATH );
	m_NormalBlinkInfo.m_EndBlinkMotion = szBuf;
	m_NormalBlinkInfo.m_fEndBlinkMotionRate = rkLoader.LoadFloat_e( "blink_end_ani_rate", FLOAT1 );

	m_NormalBlinkInfo.m_fJumpAmt = rkLoader.LoadFloat_e( "blink_end_jump_power", 0.f );

	//////////////////////////////////////////////////////////////////////////
	int iExtendCnt = rkLoader.LoadInt_e( "max_extend_attack_cnt", 0 );
	m_NormalBlinkInfo.m_SpecialAttributeList.clear();
	m_NormalBlinkInfo.m_SpecialAttributeList.reserve( iExtendCnt );
	for( int i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_NormalBlinkInfo.m_SpecialAttributeList.push_back( kAttribute );
	}

	m_NormalBlinkInfo.m_fSpecilAfterJumpPower = rkLoader.LoadFloat_e( "afeter_special_jump_amt", 0.f );

	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "red_hood_special_fly_ani", "", szBuf, MAX_PATH );
	m_SpecialMoveInfo.m_StartAni = szBuf;
	m_SpecialMoveInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "red_hood_special_fly_ani_rate", FLOAT1 );

	m_SpecialMoveInfo.m_fStartAngle = rkLoader.LoadFloat_e( "red_hood_special_fly_angle", 0.0f );
	m_SpecialMoveInfo.m_fStartMaxRange = rkLoader.LoadFloat_e( "red_hood_special_fly_max_range", 0.0f );
	m_SpecialMoveInfo.m_fStartSpeed = rkLoader.LoadFloat_e( "red_hood_special_fly_speed", 0.0f );	
	m_SpecialMoveInfo.m_fEndJumpAmt = rkLoader.LoadFloat_e( "red_hood_special_fly_end_jump_amt", 0.f );
}

ioItem* ioRedHoodItem::Clone()
{
	return new ioRedHoodItem( *this );
}

ioWeaponItem::WeaponSubType ioRedHoodItem::GetSubType() const
{
	return WST_RED_HOOD_ITEM;
}

void ioRedHoodItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;
	m_bEndAttackJump = false;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_SpecialMoveState = SMS_NONE;

	ClearSpecialState( pOwner );

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = ES_NONE;
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

		m_ChargeState = ES_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		//////////////////////////////////////////////////////////////////////////
		/*DWORD dwSpecialMoveMacroTime = pGrp->GetAnimationEventTime_e( iAniID, "red_hood" ) * fTimeRate;
		if( dwSpecialMoveMacroTime )
		{
			m_dwSpecialMoveMacroTime = FRAMEGETTIME() + dwSpecialMoveMacroTime;
		}*/
		//////////////////////////////////////////////////////////////////////////

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


void ioRedHoodItem::SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime )
{
	m_dwSpecialMoveMacroTime_S = 0;
	m_dwSpecialMoveMacroTime_E = 0;
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwSpecialMoveMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_s" ) * fTimeRate;
	DWORD dwSpecialMoveMacroTime_E = pGrp->GetAnimationEventTime_e( iAniID, "red_hood_e" ) * fTimeRate;
	if( dwSpecialMoveMacroTime_S )
	{
		m_dwSpecialMoveMacroTime_S = dwCurTime + dwSpecialMoveMacroTime_S;
		m_dwSpecialMoveMacroTime_E = dwCurTime + dwSpecialMoveMacroTime_E + dwEndTime;
	}
}


void ioRedHoodItem::SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}
	
	pOwner->SetState( CS_RED_HOOD_MOVE );
	ClearSpecialState( pOwner );
	DecreaseGauge();
	pOwner->ClearReserveKeyInput();

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SpecialMoveInfo.m_StartAni );
	float fTimeRate  = m_SpecialMoveInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	m_dwSpecialMoveStartTime = FRAMEGETTIME();
	m_dwSpecialMoveEndTime = 0;

	m_SpecialMoveState = SMS_MOVE;
	m_fFlyCurRange = 0.f;

	//pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	if(pOwner->IsNeedProcess() && bSendNet )
	{
		//Send
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << ES_SPECIAL_MOVE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );

	}
}


void ioRedHoodItem::ProcessSpecialMoveState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SpecialMoveState )
	{
	case SMS_NONE:
		break;
	case SMS_MOVE:
		pOwner->SetCurMoveSpeed( 0.f );
		if( !ProcessFlyStart(pOwner) )
		{
			ChangeSpecialMoveEndJump( pOwner );
		}
		break;
	case SMS_END:
		break;
	}
}


bool ioRedHoodItem::ProcessFlyStart( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_SpecialMoveInfo.m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_SpecialMoveInfo.m_fStartSpeed == 0.f )
		return false;

	float fCurSpeed = m_SpecialMoveInfo.m_fStartSpeed;	//m_CurFly.m_fStartMaxRange / (float)m_CurFly.m_dwStartDuration * FLOAT1000;
	//fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_SpecialMoveInfo.m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_SpecialMoveInfo.m_fStartMaxRange - m_fFlyCurRange;
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


void ioRedHoodItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && m_dwSpecialMoveMacroTime_S && m_dwSpecialMoveMacroTime_S < dwCurTime && m_dwSpecialMoveMacroTime_E > dwCurTime && pOwner->IsDefenseKey() )
	{
		SetSpecailMoveState( pOwner, true );
		return;
	}

	switch( m_ChargeState )
	{
	case ES_CHARGING:
		OnCharging( pOwner );
		break;
	case ES_SPECIAL_ATTACK:
		CheckAniRotate( pOwner );

		if( m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, false );
			return;
		}
		break;
	
	case ES_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioRedHoodItem::OnCharging( ioBaseChar *pOwner )
{
	m_bKeyReserved = false;
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;

	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( (pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown()) || (pOwner->IsAttackKeyDown() && pOwner->HasBuff( BT_CHECK_CHAIN )) )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToChargeAttack( pOwner, m_iCurCombo );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioRedHoodItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//
	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

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

		//
		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );

		int iAniID = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		SetMacroTime( pGrp, iAniID, pAttribute->m_fAttackAniRate, pAttribute->m_dwEndDelay );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = ES_NORMAL_ATTACK;

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

void ioRedHoodItem::ChangeToChargeAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( iCurCombo >= (int)m_NormalBlinkInfo.m_SpecialAttributeList.size())
		return;

	pOwner->SetAttackMoveEnable( false );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_ChargeState = ES_SPECIAL_ATTACK;

	int iAniID	     = pGrp->GetAnimationIdx( m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_AttackAnimation );
	float fTimeRate  = m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->SetReservedSliding( m_NormalBlinkInfo.m_SpecialAttributeList[iCurCombo].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( iCurCombo == 0 )
		m_bEndAttackJump = true;

	SetMacroTime( pGrp, iAniID, fTimeRate, 0 );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	SetAniRotate( pOwner, iAniID, fTimeRate );

	//Send
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRedHoodItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	
	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	int iComboCnt;

	switch( iState )
	{
	case ES_NONE:
		rkPacket >> m_iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = ES_NONE;
		break;

	case ES_NORMAL_ATTACK:
		rkPacket >> m_iCurCombo;

		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;

	case ES_SPECIAL_ATTACK:
		rkPacket >> qtRot;
		rkPacket >> vPos;
		rkPacket >> iComboCnt;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		ChangeToChargeAttack(pOwner, iComboCnt);
		break;

	case ES_SPECIAL_MOVE:
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRotToRotate( qtRot, true );
		pOwner->SetWorldPosition( vPos );

		SetSpecailMoveState( pOwner, false);
		break;
	}
}

void ioRedHoodItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	//int iSyncState;
	//rkPacket >> iSyncState;

	//D3DXQUATERNION qtRot;
	//D3DXVECTOR3 vPos;
	//float fGauge;

	//rkPacket >> qtRot;
	//rkPacket >> vPos;
	//rkPacket >> fGauge;

	//pOwner->SetTargetRotToRotate( qtRot, true );
	//pOwner->SetWorldPosition( vPos );

	//m_fCurGauge = fGauge;

	//float fJumpPower;

	//switch( iSyncState )
	//{
	//case BS_NONE:
	//	SetEvyBlinkState( pOwner, false );
	//	break;
	///*case BS_BOMB_MOTION:
	//	ChangeToSpecialBlow( pOwner );
	//	break;*/
	//case BS_DASH:
	//	break;
	//case BS_DASH_ENDING:
	//	break;
	//case BS_DASH_END_JUMP:
	//	ChangeDashEndJump( pOwner );
	//	break;
	//case BS_STARTMOTION:
	//	ChangeToBlinkStartMotion( pOwner );
	//	break;
	//case BS_ENDMOTION:
	//	ChangeToBlinkEndMotion( pOwner );
	//	break;
	//case BS_END_JUMP:
	//	rkPacket >> fJumpPower;
	//	pOwner->SetSKillEndJumpState( fJumpPower, false, false, false, true );
	//	break;
	//case BS_END:
	//	ChangeToEnd( pOwner );
	//	break;
	//}
}

void ioRedHoodItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
			pZone->SetZoneValue( true, false );
		}
	}
}

bool ioRedHoodItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case ES_CHARGING:
			return true;
		}
	}

	return false;
}

void ioRedHoodItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = ES_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioRedHoodItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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


int ioRedHoodItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioRedHoodItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioRedHoodItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioRedHoodItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioRedHoodItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioRedHoodItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioRedHoodItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioRedHoodItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioRedHoodItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioRedHoodItem::DecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioRedHoodItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioRedHoodItem::UpdateExtraData( ioBaseChar *pOwner )
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
	case CS_DASH:
	case CS_RUN:
		if( m_fCurGauge < m_fMaxGauge )
		{
			float fGauge = m_fIncreaseGauge * fTimePerSec;

			m_fCurGauge += fGauge;
			m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
		}
		return;
	}
}

void ioRedHoodItem::ClearSpecialState( ioBaseChar *pOwner )
{
	m_SpecialMoveState = SMS_NONE;

	// 순서중요
	//m_CurBlankInfo.Init();

	//m_vFlyMoveDir = ioMath::VEC3_ZERO;
	//m_fFlyMoveSpeed = 0.0f;
}

void ioRedHoodItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioRedHoodItem::OnReleased( ioBaseChar *pOwner )
{
	// 순서 주의
	ClearSpecialState( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

bool ioRedHoodItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioRedHoodItem::KeyReserve( ioBaseChar *pOwner )
{
	if( pOwner->IsAttackKey() )
		m_bAttackKeyReserved = true;

	if( pOwner->IsDefenseKey() )
		m_bDefenseKeyReserved = true;

	if( pOwner->IsJumpKey() )
		m_bJumpKeyReserved = true;
}


void ioRedHoodItem::ChangeDefenseState( ioBaseChar *pOwner )
{
	pOwner->SetDefenseState();
}

void ioRedHoodItem::ChangeSpecialMoveEndJump( ioBaseChar *pOwner )
{
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	pOwner->SetSKillEndJumpState( m_SpecialMoveInfo.m_fEndJumpAmt, false, false, false, true );
	pOwner->ClearDirDoubleClick();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SMS_JUMP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_fCurGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRedHoodItem::ClearReserveKey()
{
	m_bAttackKeyReserved = false;
	m_bDefenseKeyReserved = false;
	m_bJumpKeyReserved = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioRedHoodItemMoveSpecialState::ioRedHoodItemMoveSpecialState()
{
}

ioRedHoodItemMoveSpecialState::~ioRedHoodItemMoveSpecialState()
{
}

void ioRedHoodItemMoveSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioRedHoodItemMoveSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioRedHoodItem *pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pRedHoodItem )
		pRedHoodItem->ClearSpecialState( pOwner );
}

void ioRedHoodItemMoveSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioRedHoodItem *pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pRedHoodItem )
		pRedHoodItem->ProcessSpecialMoveState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

bool ioRedHoodItemMoveSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	ioRedHoodItem *pRedHoodItem = ToRedHoodItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pRedHoodItem )
		return true;

	return true;
}

void ioRedHoodItemMoveSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}


bool ioRedHoodItemMoveSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense,
	TeamType eOwnerTeam,
	TeamType eOtherTeam ) const
{
	return true;
}
