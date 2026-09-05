

#include "stdafx.h"

#include "ioSurferItem.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ioSurferItem::ioSurferItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioSurferItem::ioSurferItem( const ioSurferItem &rhs )
: ioWeaponItem( rhs ),
  m_dwRotateTime( rhs.m_dwRotateTime ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_SurfingEndAttack( rhs.m_SurfingEndAttack ),
  m_SurfingEffect( rhs.m_SurfingEffect ),
  m_SurfingAnimation( rhs.m_SurfingAnimation ),
  m_fSurfingAniRate( rhs.m_fSurfingAniRate ),
  m_dwSurfingDuration( rhs.m_dwSurfingDuration ),
  m_fSurfingSpeed( rhs.m_fSurfingSpeed ),
  m_fSurfingUpSpeed( rhs.m_fSurfingUpSpeed ),
  m_fSurfingEndJumpPower( rhs.m_fSurfingEndJumpPower ),
  m_AttachWeapon( rhs.m_AttachWeapon ),
  m_fMaxGauge( rhs.m_fMaxGauge ),
  m_fNeedGauge( rhs.m_fNeedGauge ),
  m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_fCurGauge = 0.0f;
}

ioSurferItem::~ioSurferItem()
{
}

void ioSurferItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	LoadAttackAttribute( "surfing_end_attack", m_SurfingEndAttack, rkLoader );

	m_AttackReadyAniList.clear();
	for( int i=0 ; i<GetMaxCombo() ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	rkLoader.LoadString_e( "surfing_animation", "", szBuf, MAX_PATH );
	m_SurfingAnimation = szBuf;
	
	rkLoader.LoadString_e( "surfing_effect", "", szBuf, MAX_PATH );
	m_SurfingEffect = szBuf;

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "surfing_rotate_speed", 0 );
	m_fSurfingAniRate = rkLoader.LoadFloat_e( "surfing_ani_rate", FLOAT1 );
	m_dwSurfingDuration = rkLoader.LoadInt_e( "surfing_duration", 0 );
	m_fSurfingSpeed = rkLoader.LoadFloat_e( "surfing_speed", 0.0f );
	m_fSurfingUpSpeed = rkLoader.LoadFloat_e( "surfing_up_speed", 0.0f );
	m_fSurfingEndJumpPower = rkLoader.LoadFloat_e( "surfing_end_jump_power", 0.0f );

	m_AttachWeapon.m_iWeaponIdx = rkLoader.LoadInt( "attach_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_AttachWeapon.m_iResistanceIdx = rkLoader.LoadInt( "attach_weapon_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString( "attach_weapon_type_wounded", "", szBuf, MAX_PATH );
	m_AttachWeapon.m_WoundedAnimation = szBuf;
	m_AttachWeapon.m_dwWoundedDuration = rkLoader.LoadInt( "attach_weapon_type_wounded_time", 0 );
	m_AttachWeapon.m_bWoundedLoopAni = rkLoader.LoadBool( "attach_weapon_type_wounded_loop_ani", false );

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
}

ioItem* ioSurferItem::Clone()
{
	return new ioSurferItem( *this );
}

void ioSurferItem::ClearInfo( ioBaseChar *pOwner )
{
	if( pOwner && !m_SurfingEffect.IsEmpty() )
		pOwner->EndEffect( m_SurfingEffect, false );
}

ioWeaponItem::WeaponSubType ioSurferItem::GetSubType() const
{
	return WST_SURFER;
}

void ioSurferItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	ClearCancelInfo();

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	if( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
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
		m_CurDirKey = ioUserKeyInput::DKI_NONE;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioSurferItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
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

void ioSurferItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() && IsEnableGauge() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetSurferState( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioSurferItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSurferItem::ChangeToSurfing( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fCurRate = FLOAT1;
	if( m_fSurfingAniRate > 0.0f )
		fCurRate = m_fSurfingAniRate;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->SetLoopAni( m_SurfingAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	}

	m_ChargeState = CS_SURFING;
	m_dwSurfingStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_AttachWeapon.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_AttachWeapon.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_AttachWeapon.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_AttachWeapon.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_AttachWeapon.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
	
	if( !m_SurfingEffect.IsEmpty() )
		pOwner->AttachEffect( m_SurfingEffect );
}

void ioSurferItem::ProcessSurfing( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( m_dwSurfingStartTime + m_dwSurfingDuration < FRAMEGETTIME() )
		{
			ChangeToEndAttackFire( pOwner );
			return;
		}
		else if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToEndAttackFire( pOwner );
			return;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(0.0f) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return;

	float fDistance = m_fSurfingSpeed * fTimePerSec;
	float fUpDistance = m_fSurfingUpSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fDistance;


	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	vMoveAmt = ioMath::UNIT_Y;
	vMoveAmt.y += fUpDistance;
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );
}

void ioSurferItem::ChangeToEndAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_SurfingEffect.IsEmpty() )
		pOwner->EndEffect( m_SurfingEffect, false );

	m_ChargeState = CS_END_ATTACK;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_SurfingEndAttack.m_AttackAnimation );
	float fTimeRate  = m_SurfingEndAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_SurfingEndAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_SurfingEndAttack.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	//
	pOwner->RefreshFireTimeList( iAniID, m_SurfingEndAttack.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, dwPreDelay );
	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedProcess() && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_END_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSurferItem::ProcessEndAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
		pOwner->SetSKillEndJumpState( m_fSurfingEndJumpPower, false, false, true, true );
}

void ioSurferItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

	if( eNewDirKey != ioUserKeyInput::DKI_NONE )
	{	
		float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
		D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

		m_bTargetRot = false;
		m_bLeftRot = false;
		if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
		{
			float fCurYaw, fTargetYaw;
			fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

			fCurYaw = ioMath::ArrangeHead( fCurYaw );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

			float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
			if( fRotAngle < 0.0f )
				m_bLeftRot = true;
		}

		if( m_CurDirKey != eNewDirKey )
		{
			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)SS_SURFING_MOVE;
				kPacket << m_iCurCombo;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		m_bLeftRot = false;
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)SS_SURFING_MOVE;
				kPacket << m_iCurCombo;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				kPacket << pOwner->GetWorldPosition();
				kPacket << pOwner->GetWorldOrientation();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioSurferItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioSurferItem::SetSurferState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	CheckDecreaseGauge();
	pOwner->SetState( CS_SURFER_STATE );

	ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_JUMP );
	if( pItem )
	{
		ioExtendJump *pJump = pItem->GetCurExtendJump();
		if( pJump )
			pJump->InitJump(); // 지상 특별 사용시 점프 특별 사용횟수 초기화.
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)SS_SURFING;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSurferItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case SS_SURFING:
		{
			SetSurferState( pOwner );
		}
		break;
	case SS_SURFING_MOVE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetWorldOrientation( qtRot );

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
			{
				m_bTargetRot = false;
			}
		}
		break;
	case SS_NORMAL_ATTACK:
		{
			ChangeToAttackFire( pOwner );
		}
		break;
	case SS_END_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetWorldOrientation( qtRot );

			ChangeToEndAttackFire( pOwner );
		}
		break;
	}
}

void ioSurferItem::OnReleased( ioBaseChar *pOwner )
{
	m_fCurGauge = 0.0f;

	ioWeaponItem::OnReleased( pOwner );

}
void ioSurferItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	if( !pWeapon )
		return;

	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_ZONE_EFFECT:
		{
			ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
			if( pZone )
			{
				ioBaseChar *pChar = pZone->GetOwner();
				if( pChar )
				{
					D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					pZone->SetCollisionDir( vDir );
					pZone->SetZoneValue( false, false );
					pZone->SetLiveTime( m_dwSurfingDuration );
				}
			}
		}
		break;
	case ioWeapon::WT_BOMB_MISSILE3:
		{
			ioBombMissileWeapon3* pBombMissileWeapon3 = ToBombMissileWeapon3( pWeapon );
			if( pBombMissileWeapon3 )
			{
				ioBaseChar *pChar = pBombMissileWeapon3->GetOwner();
				if( pChar )
				{
					pBombMissileWeapon3->SetOrientation( pChar->GetTargetRot() );
				}
				pBombMissileWeapon3->SetDsiableChangeDir( false );
			}
		}
		break;
	case ioWeapon::WT_ZONE_MOVE_WEAPON:
		{
			ioZoneMoveWeapon *pZoneMoveWeapon = ToZoneMoveWeapon( pWeapon );
			if( pZoneMoveWeapon )
			{
				ioBaseChar *pChar = pZoneMoveWeapon->GetOwner();
				if( pChar )
				{
					D3DXVECTOR3	vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					pZoneMoveWeapon->SetMoveDir( vDir );
				}
			}
		}
		break;
	}
}

bool ioSurferItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner && pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_SURFING:
			return true;
		}
	}

	return false;
}

void ioSurferItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	if( !pOwner )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

bool ioSurferItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

bool ioSurferItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

void ioSurferItem::GetAutoTargetValue( ioBaseChar *pOwner,
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
		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;
	}
}

int ioSurferItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioSurferItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioSurferItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioSurferItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioSurferItem::IsEnableGauge()
{
	if( m_fCurGauge >= m_fNeedGauge )
		return true;

	return false;
}

void ioSurferItem::CheckDecreaseGauge()
{
	m_fCurGauge -= m_fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

void ioSurferItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

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
//////////////////////////////////////////////////////////////////////////////////
ioSurferSpecialState::ioSurferSpecialState()
{
}

ioSurferSpecialState::~ioSurferSpecialState()
{
}

void ioSurferSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	ioSurferItem *pSurfingItem = ToSurferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSurfingItem )
	{
		pSurfingItem->ChangeToSurfing( pOwner );
	}
}

void ioSurferSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioSurferItem *pSurfingItem = ToSurferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pSurfingItem )
	{
		pSurfingItem->ClearInfo( pOwner );
	}
}

bool ioSurferSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	if( Help::CheckCharColState( dwStartTime, dwEndTime, dwSkipType, bTeamOnly, bDefense, eOwnerTeam, eOtherTeam ) )
		return true;

	return false;
}

void ioSurferSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioSurferItem *pSurfingItem = ToSurferItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pSurfingItem )
		return;

	switch( pSurfingItem->GetChargingState() )
	{
	case ioSurferItem::CS_SURFING:
		{
			pSurfingItem->ProcessSurfing( pOwner );
		}
		break;
	case ioSurferItem::CS_END_ATTACK:
		{
			pSurfingItem->ProcessEndAttack( pOwner );
		}
		break;
	}
}

void ioSurferSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioSurferSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	return true;
}

bool ioSurferSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	return true;
}
