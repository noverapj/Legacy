
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioSuccubusItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioSuccubusItem::ioSuccubusItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_SuccubusAttackState = MFS_NONE;
	m_fGoomihoFlyCurRange = 0.0f;
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
//	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;
	m_dwFireMotionTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_Target.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGauge = 0.0f;
	m_bGrapplingDirMove = false;
	m_dwWeaponIndex = 0;
}

ioSuccubusItem::ioSuccubusItem( const ioSuccubusItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackInfo( rhs.m_AttackInfo ),
m_bInitGaugeDown( rhs.m_bInitGaugeDown ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge ),
m_fDecreaseGauge( rhs.m_fDecreaseGauge ),
m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
m_fHighDecreaseGauge( rhs.m_fHighDecreaseGauge )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_SuccubusAttackState = MFS_NONE;
	m_fGoomihoFlyCurRange = 0.0f;
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
//	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_Target.Clear();
//	m_GoomihoTargetSkill.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_fCurGauge = 0.0f;
	m_bGrapplingDirMove = false;

	m_dwMotionEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwWeaponIndex = 0;
	m_dwHitedTime = 0;
}

ioSuccubusItem::~ioSuccubusItem()
{
}

void ioSuccubusItem::LoadProperty( ioINILoader &rkLoader )
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

	LoadSuccubusInfo( rkLoader );

	//
	m_bInitGaugeDown = rkLoader.LoadBool_e( "init_goomiho_gauge_down", false );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_goomiho_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_goomiho_gauge", FLOAT100 );

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_goomiho_gauge", FLOAT1 );
	m_fDecreaseGauge = rkLoader.LoadFloat_e( "decrease_goomiho_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseGauge = rkLoader.LoadFloat_e( "high_decrease_goomiho_gauge", 3.0f );
}

void ioSuccubusItem::LoadSuccubusInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_AttackInfo.Init();

	// start
	rkLoader.LoadString_e( "first_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "first_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniDown = szBuf;


	//m_AttackInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "first_attack_actionAni_Rate", FLOAT1 );
	//m_AttackInfo.m_nFirstWeaponIdx = rkLoader.LoadInt_e( "first_attack", 0 );

	//시작
	wsprintf_e( szKey, "first_attack" );
	LoadAttackAttribute( szKey, m_AttackInfo.m_FirstAttack, rkLoader );


	// 타겟 대상 검색용
	m_AttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_AttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_AttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_AttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "target_up_height", 0.0f );
	m_AttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "target_down_height", 0.0f );

	rkLoader.LoadString_e( "delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniDown = szBuf;
	m_AttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "delay_attack_actionAni_Rate", FLOAT1 );

	m_AttackInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "delay_attack_max_wait_time", 0 );

	rkLoader.LoadString_e( "add_attack_actionAni", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AddAttackAni = szBuf;

	// attack move
	wsprintf_e( szKey, "add_attack" );
	LoadAttackAttribute( szKey, m_AttackInfo.m_AddAttack, rkLoader );

	/*m_AttackInfo.m_fAddAttackRate = rkLoader.LoadFloat_e( "add_attack_actionAni_Rate", FLOAT1 );
	m_AttackInfo.m_nAddWeaponIdx = rkLoader.LoadInt_e( "add_attack", 0 );*/
}

ioItem* ioSuccubusItem::Clone()
{
	return new ioSuccubusItem( *this );
}

ioWeaponItem::WeaponSubType ioSuccubusItem::GetSubType() const
{
	return WST_SUCCUBUS;
}

void ioSuccubusItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fGoomihoFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearAttackState( pOwner );
//	m_GoomihoTargetSkill.Clear();

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

void ioSuccubusItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
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

void ioSuccubusItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetSuccubusAttackState( pOwner, m_AttackInfo, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioSuccubusItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

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

void ioSuccubusItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioSuccubusItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_FLY_STATE:
		CheckSuccubusState( pOwner, rkPacket );
		break;
	}
}

void ioSuccubusItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon4 *pUroborus4 = ToUroborusWeapon4( pWeapon );
	if( pUroborus4 )
	{
		m_dwWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	

			pUroborus4->SetMoveDir( vDir );
		}
	}

	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	if( pMissile )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
					vPos.y = pTargetChar->GetMidHeightByRate();
					pMissile->SetPosition( vPos );
				}
			}	
		}
	}

	ioContactWeapon *pContact = ToContactWeapon( pWeapon );
	if( pContact && m_SuccubusAttackState == MFS_ATTACK_ADD )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar && !m_Target.IsEmpty() )
		{
			ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
			if( pTargetChar )
			{
				D3DXVECTOR3 vPos = pTargetChar->GetWorldPosition();
				vPos.y = pTargetChar->GetMidHeightByRate();
				pContact->SetPosition( vPos );
			}
		}
	}

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

	ioOwnerAttackWeapon* pOwnerWepon = ToOwnerAttackWeapon(pWeapon);
	if( pOwnerWepon && !m_Target.IsEmpty() )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			ioPlayStage* pStage = pChar->GetCreator();
			if( pStage )
				pOwnerWepon->SetTargetName( m_Target, pStage );
		}
		
	}
}

bool ioSuccubusItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioSuccubusItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioSuccubusItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioSuccubusItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioSuccubusItem::SetSuccubusAttackState( ioBaseChar *pOwner, const SuccubusAttackInfo &rkFlyInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_nAttackCnt = 0;
	pOwner->SetState( CS_SUCCUBUS_ATTACK );

	m_CurAttackInfo = rkFlyInfo;

	if( pOwner->IsNeedProcess() )
	{
		FindFlyAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}


	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		//m_dwGoomihoFlyAttackMoveTime = dwCurTime;

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			//////////////////////////////////////////////////////////////////////////
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			//////////////////////////////////////////////////////////////////////////
			m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			m_vFlyMoveDir = vDir;
		}
	}

	ChangeAttackStart( pOwner );
	m_fGoomihoFlyCurRange = 0.0f;
	m_dwHitedTime = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_SuccubusAttackState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	CheckDecreaseGauge();
}

void ioSuccubusItem::SetSuccubusStateByDash( ioBaseChar *pOwner, const SuccubusAttackInfo &rkFlyInfo )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetState( CS_SUCCUBUS_ATTACK );

	m_CurAttackInfo = rkFlyInfo;
	m_nAttackCnt = 0;

	//ChangeAttackStart( pOwner );
	m_fGoomihoFlyCurRange = 0.0f;

	//CheckDecreaseGauge();
	SetSuccubusAttackState( pOwner, rkFlyInfo, false);
}

void ioSuccubusItem::ChangeAttackStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//////////////////////////////////////////////////////////////////////////
	m_SuccubusAttackState = MFS_START;
	//////////////////////////////////////////////////////////////////////////

	m_bGrapplingDirMove = false;
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	//트랙킹
	ChangeTrackingUD_Fire( pOwner );

	//상중하 다 시간은 같다.
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAniCenter );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_AddAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_FirstAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate,
		dwPreDelay );

	/*int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAni );
	float fTimeRate  = m_CurAttackInfo.m_fStartAniRate;
	DWORD dwPreDelay = 0;
	DWORD dwEndDelay = 0;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );*/
	
	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwMotionEndTime = m_dwAttackStartTime;
	m_dwFireMotionTime = m_dwAttackStartTime;
	//m_dwFireMotionTime += (pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_CurAttackInfo.m_fStartAniRate);
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate );

//	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;
}

/*void ioSuccubusItem::ChangeAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwMoveDuration = 0;

	m_SuccubusAttackState = MFS_ATTACK_WAIT;


	m_dwAttackStartTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx = m_CurAttackInfo.m_nFirstWeaponIdx;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	ioWeapon* pWeapon =	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
	m_dwWeaponIndex = pWeapon->GetWeaponIndex();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_SuccubusAttackState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_fFlyMoveSpeed;
		kPacket << dwMoveDuration;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}*/


void ioSuccubusItem::ChangeAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	m_SuccubusAttackState = MFS_ATTACK_HITED;

	ChangeTrackingUD_Fire( pOwner );
}

void ioSuccubusItem::ChangeAttackAdd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AddAttack.m_AttackAnimation );
	float fTimeRate  = m_CurAttackInfo.m_AddAttack.m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_SuccubusAttackState = MFS_ATTACK_ADD;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_AddAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_AddAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionTime = dwCurTime;
	//m_dwFireMotionTime += (pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * m_CurAttackInfo.m_fStartAniRate);
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_AddAttack.m_fAttackAniRate );

	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_FLY_STATE;
		kPacket << m_SuccubusAttackState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurGauge;
		kPacket << (int)m_CurDirKey;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	RemoveWeapon( pOwner );
}

void ioSuccubusItem::ProcessSuccubusState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SuccubusAttackState )
	{
	case MFS_START:
		
		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwMotionEndTime) )
			SetChangeEndState( pOwner );
		break;

	case MFS_ATTACK_HITED:
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
			ChangeAttackAdd(pOwner);

		if( pOwner->IsNeedProcess() && m_dwHitedTime && (m_dwHitedTime+m_CurAttackInfo.m_dwWaitMaxDelayTime) < dwCurTime )
			ChangeAttackAdd(pOwner);
	
		break;
	case MFS_ATTACK_ADD:
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwMotionEndTime )
			SetChangeEndState( pOwner );

		break;
	
	case MFS_END:
		break;
	}


	if( m_SuccubusAttackState == MFS_ATTACK_HITED || m_SuccubusAttackState == MFS_ATTACK_ADD )
		CheckAttackHited( pOwner );

	if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		ProcessWeaponLine( pOwner );
}

void ioSuccubusItem::ProcessWeaponLine( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_Target.IsEmpty() || m_dwWeaponIndex == 0 )
		return;


	ioBaseChar* pTargetDest = pChar->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIndex );
	ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
	if( !pUroborus4 )
		return;

	if( pTargetDest && IsSuccubusLockState(pTargetDest) )
	{
		D3DXVECTOR3 vPosDest = pTargetDest->GetMidPositionByRate();
		pUroborus4->SetPosition( vPosDest );
	}

}

void ioSuccubusItem::CheckSuccubusState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iState )
	{
	case MFS_START:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;
			rkPacket >> m_Target;

			SetCurExtraGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			SetSuccubusAttackState( pOwner, m_AttackInfo, false );
			return;
		}
		break;
	case MFS_ATTACK_ADD:
		{
			float fCurGauge;
			rkPacket >> fCurGauge;

			SetCurExtraGauge( fCurGauge );

			pOwner->SetTargetRotToRotate( qtRot, true );

			int nCurKey;
			rkPacket >> nCurKey;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)nCurKey;

			ChangeAttackAdd( pOwner );
			return;
		}
	}
}

int ioSuccubusItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSuccubusItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSuccubusItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSuccubusItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

void ioSuccubusItem::InitExtraGauge()
{
	m_fCurGauge = 0.0f;
}

void ioSuccubusItem::MaxExtraGauge()
{
	m_fCurGauge = m_fMaxGauge;
}

float ioSuccubusItem::GetMaxExtraGauge()
{
	return m_fMaxGauge;
}

float ioSuccubusItem::GetCurExtraGauge()
{
	return m_fCurGauge;
}

void ioSuccubusItem::SetCurExtraGauge( float fGauge )
{
	m_fCurGauge = fGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioSuccubusItem::CheckEnableGauge( ioBaseChar *pChar )
{
}

void ioSuccubusItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

bool ioSuccubusItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

bool ioSuccubusItem::IsSuccubusState()
{
	switch( m_SuccubusAttackState )
	{
	case MFS_START:
	case MFS_ATTACK_HITED:
	case MFS_ATTACK_ADD:
		return true;
	}

	return false;
}

void ioSuccubusItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_SUCCUBUS_ATTACK:
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
		break;
	}
}

void ioSuccubusItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_SuccubusAttackState = MFS_NONE;

	// 순서중요
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
//	m_dwGoomihoFlyAttackMoveTime = 0;
	m_dwGoomihoFlyGrapplingLoopTime = 0;
	m_dwGoomihoFlyGrapplingMoveTime = 0;
	m_dwGoomihoFlyEndAttackTime = 0;
	m_dwGoomihoFlyEndTime = 0;

	m_dwGoomihoFlyGrapplingEnableMoveTime = 0;

	m_vFlyMoveDir = ioMath::VEC3_ZERO;
	m_fFlyMoveSpeed = 0.0f;

	m_Target.Clear();

	m_bGrapplingDirMove = false;
	m_dwMotionEndTime = 0;

	RemoveWeapon(pOwner);
}

bool ioSuccubusItem::IsGoomihoFlyCharCollisionSkipState()
{
	switch( m_SuccubusAttackState )
	{
	case MFS_START:
	//case MFS_GRAPPLING_MOVE:
		return true;
	}

	return false;
}

void ioSuccubusItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioSuccubusItem::OnReleased( ioBaseChar *pOwner )
{
	// 순서 주의!
//	ReleaseAttackMoveEffect( pOwner );
	//ReleaseeGrapplingMoveEffect( pOwner );

	ClearAttackState( pOwner );
	
//	m_GoomihoTargetSkill.Clear();
	m_fCurGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

bool ioSuccubusItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurAttackInfo.m_TargetWoundType ) )
		return false;

	return true;
}

void ioSuccubusItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_Target.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

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
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
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
			m_Target = szTarget;
		}
	}
}

bool ioSuccubusItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

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
	if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioSuccubusItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioSuccubusItem::IncreaseChangeDamage( float fDamage, int iType, bool bBuff )
{
	if( iType == SSGT_FOX || iType == SSGT_NONE )
	{
		m_fCurGauge += fDamage;
		m_fCurGauge = max( 0.0f, m_fCurGauge );
		m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
	}
}

bool ioSuccubusItem::SetTarget( const ioHashString &szTargetName, ioBaseChar *pOwner )
{
	//명중 
	m_Target = szTargetName;
	m_dwHitedTime = FRAMEGETTIME();
	ChangeAttackHited( pOwner );
	return true;
}

void ioSuccubusItem::RemoveWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );

	if( pWeapon )
	{
		ioUroborusWeapon4 *pUroborus4 =	ToUroborusWeapon4( pWeapon );
		if( pUroborus4 )
			pUroborus4->SetEndState( false, false, false );
	}
}

void ioSuccubusItem::CheckAttackHited( ioBaseChar *pOwner )
{
	ioBaseChar* pTarget = pOwner->GetBaseChar(m_Target);
	if( !pTarget )
	{
		SetChangeEndState( pOwner );
		return;
	}

	if( !IsSuccubusLockState(pTarget) && m_SuccubusAttackState != MFS_ATTACK_ADD )
	{
		SetChangeEndState( pOwner );
		return;
	}

	if( !IsSuccubusLockState(pTarget) )
	{
		RemoveWeapon(pOwner);
	}
}

void ioSuccubusItem::SetChangeEndState( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );
	pOwner->SetState( CS_DELAY );
}

//////////////////////////////////////////////////////////////////////////

float ioSuccubusItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

void ioSuccubusItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = FLOAT1;//pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = 0;//pAttr->m_dwPreDelay;

	if( m_SuccubusAttackState == MFS_START )
		fTimeRate = m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate;
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		fTimeRate = m_CurAttackInfo.m_fAttackLoopAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}

void ioSuccubusItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;

	if( m_SuccubusAttackState == MFS_START )
	{
		szFireUp = m_CurAttackInfo.m_StartAniUp;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
	{
		szFireUp = m_CurAttackInfo.m_AttackLoopAniUp;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_SuccubusAttackState == MFS_START )
			pGrp->SetActionAni( szFireUp,FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate,	0.0f, dwPreDelay,	bHold, bAniFx1 );
		else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireUp, FLOAT100, fWeight, FLOAT1/fTimeRate );

	}

	if( m_SuccubusAttackState == MFS_START )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
}

void ioSuccubusItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;

	if( m_SuccubusAttackState == MFS_START )
	{
		szFireDown = m_CurAttackInfo.m_StartAniDown;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
	{
		szFireDown = m_CurAttackInfo.m_AttackLoopAniDown;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_SuccubusAttackState == MFS_START )
			pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
		else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
			pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
	}

	if( m_SuccubusAttackState == MFS_START )
		pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_SuccubusAttackState == MFS_ATTACK_HITED )
		pGrp->SetLoopAni( szFireDown, FLOAT100, fWeight, FLOAT1/fTimeRate );
}

ioUserKeyInput::DirKeyInput ioSuccubusItem::GetCurKey()
{
	return m_CurDirKey;
}

bool ioSuccubusItem::IsSuccubusLockState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	return pOwner->HasBuff( BT_HIT_COUNT_HOLD );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioSuccubusSpecialState::ioSuccubusSpecialState()
{
}

ioSuccubusSpecialState::~ioSuccubusSpecialState()
{
}

void ioSuccubusSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioSuccubusSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSuccubusItem *pSuccubus = ToSuccubusItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuccubus )
		pSuccubus->ClearAttackState( pOwner );
}
bool ioSuccubusSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioSuccubusItem *pSuccubus = ToSuccubusItem( pOwner->GetPriorityItem(SPT_ATTACK) );
	if( pSuccubus && pSuccubus->IsGoomihoFlyCharCollisionSkipState() )
		return true;

	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSuccubusSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioSuccubusItem *pSuccubus = ToSuccubusItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuccubus )
		pSuccubus->ProcessSuccubusState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioSuccubusSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSuccubusSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioSuccubusItem *pSuccubusItem = ToSuccubusItem( pOwner->GetWeapon() );
	if( pSuccubusItem && 
		( pSuccubusItem->GetGoomihoFlyState() == ioGoomihoItem::MFS_AUTO_ATTACK ||
		pSuccubusItem->GetGoomihoFlyState() == ioGoomihoItem::MFS_AUTO_PLUS_ATTACK ) )
		return false;

	return true;
}

void ioSuccubusSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioSuccubusItem *pSuccubus = ToSuccubusItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSuccubus )
		fCurGravity = 0.0f;
}