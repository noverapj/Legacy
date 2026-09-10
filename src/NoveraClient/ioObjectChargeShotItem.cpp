

#include "stdafx.h"

#include "ioObjectChargeShotItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioObjectChargeShotItem::ioObjectChargeShotItem()
{
	m_AttackState = AS_NONE;

	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_dwMotionEndTime = 0;
	m_dwWeaponFireCheckTime = 0;
	m_dwAttackStartTime = 0;

	m_dwMarkEffectID = -1;
	m_vMarkPosition = ioMath::VEC3_ZERO;
	m_fMarkCurRange = 0.0f;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioObjectChargeShotItem::ioObjectChargeShotItem( const ioObjectChargeShotItem &rhs )
: ioObjectItem( rhs ),
  m_bEnableObjectDefense( rhs.m_bEnableObjectDefense ),
  m_PreDelayAnimation( rhs.m_PreDelayAnimation ),
  m_fPreDelayAniRate( rhs.m_fPreDelayAniRate ),
  m_AttackAnimation( rhs.m_AttackAnimation ),
  m_fAttackAniRate( rhs.m_fAttackAniRate ),
  m_AttackEndAnimation( rhs.m_AttackEndAnimation ),
  m_fAttackEndAniRate( rhs.m_fAttackEndAniRate ),
  m_dwWeaponFireTime( rhs.m_dwWeaponFireTime ),
  m_dwWeaponFireAngle( rhs.m_dwWeaponFireAngle ),
  m_WeaponInfo( rhs.m_WeaponInfo ),
  m_MarkEffect( rhs.m_MarkEffect ),
  m_MarkAfterEffect( rhs.m_MarkAfterEffect ),
  m_fMarkFrontOffset( rhs.m_fMarkFrontOffset ),
  m_fMarkHeightOffset( rhs.m_fMarkHeightOffset ),
  m_fMarkSpeed( rhs.m_fMarkSpeed ),
  m_fMarkRange( rhs.m_fMarkRange ),
  m_fRevisionRate( rhs.m_fRevisionRate ),
  m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
  m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
  m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
  m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
  m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
  m_fUpLimitRange( rhs.m_fUpLimitRange ),
  m_dwRotateTime( rhs.m_dwRotateTime ),
  m_dwProtectTime( rhs.m_dwProtectTime )
{
	m_AttackState = AS_NONE;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_dwMotionEndTime = 0;
	m_dwWeaponFireCheckTime = 0;
	m_dwAttackStartTime = 0;

	m_dwMarkEffectID = -1;
	m_vMarkPosition = ioMath::VEC3_ZERO;
	m_fMarkCurRange = 0.0f;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}

ioObjectChargeShotItem::~ioObjectChargeShotItem()
{
}

void ioObjectChargeShotItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bEnableObjectDefense = rkLoader.LoadBool_e( "enable_object_defense", false );

	rkLoader.LoadString_e( "predelay_animation", "", szBuf, MAX_PATH );
	m_PreDelayAnimation = szBuf;
	m_fPreDelayAniRate = rkLoader.LoadFloat_e( "predelay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "attack_animation", "", szBuf, MAX_PATH );
	m_AttackAnimation = szBuf;
	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "attack_end_animation", "", szBuf, MAX_PATH );
	m_AttackEndAnimation = szBuf;
	m_fAttackEndAniRate = rkLoader.LoadFloat_e( "attack_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "mark_effect", "", szBuf, MAX_PATH );
	m_MarkEffect = szBuf;
	rkLoader.LoadString_e( "mark_after_effect", "", szBuf, MAX_PATH );
	m_MarkAfterEffect = szBuf;
	m_fMarkFrontOffset  = rkLoader.LoadFloat_e( "mark_front_offset", 0.0f );
	m_fMarkHeightOffset = rkLoader.LoadFloat_e( "mark_height_offset", 0.0f );
	m_fMarkSpeed        = rkLoader.LoadFloat_e( "mark_speed", 0.0f );
	m_fMarkRange        = rkLoader.LoadFloat_e( "mark_range", 0.0f );


	m_dwWeaponFireTime = (DWORD)rkLoader.LoadInt_e( "weapon_fire_time", FLOAT1 );
	m_dwWeaponFireAngle = (DWORD)rkLoader.LoadInt_e( "weapon_fire_angle", FLOAT1 );

	rkLoader.LoadString_e( "weapon_wounded_animation", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;

	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "weapon_wounded_duration", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "weapon_wounded_loop_ani", false );

	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "weapon_attribute_index", 0 );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "weapon_attribute_resist_index", 0 );


	// float weapon info
	m_fRevisionRate        = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );
	m_fWeaponGravityRate   = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower    = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponMinSpeed      = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fUpLimitHeight       = rkLoader.LoadFloat_e( "weapon_up_limit_height", 30.0f );
	m_fUpLimitRange        = rkLoader.LoadFloat_e( "weapon_up_limit_range", 0.0f );

	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_dwProtectTime = (DWORD)rkLoader.LoadInt_e( "protect_time", 0 );
}

ioItem* ioObjectChargeShotItem::Clone()
{
	return new ioObjectChargeShotItem( *this );
}

ioObjectItem::ObjectSubType ioObjectChargeShotItem::GetObjectSubType() const
{
	return OST_CHARGE_SHOT;
}

void ioObjectChargeShotItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();
	
	pOwner->SetState( CS_ATTACK );
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( true );

	m_AttackState = AS_PREDELAY;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_PreDelayAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fPreDelayAniRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fPreDelayAniRate;
}

void ioObjectChargeShotItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_AttackState )
	{
	case AS_PREDELAY:
		OnPreDelay( pOwner );
		break;
	case AS_ATTACK:
		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
		OnAttack( pOwner );
		break;
	case AS_ATTACK_END:
		OnAttackEnd( pOwner );
		break;
	}
}

void ioObjectChargeShotItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	
	switch( iState )
	{
	case SS_ATTACK_END:
		{
			ChangeToAttackEnd( pOwner );
		}
		break;
	case SS_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;
				switch( iDir )
				{
				case ioUserKeyInput::DKI_UP:
					m_CurDirKey = ioUserKeyInput::DKI_UP;
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
					break;
				case ioUserKeyInput::DKI_RIGHT:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
					break;
				case ioUserKeyInput::DKI_RIGHTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
					break;
				case ioUserKeyInput::DKI_DOWN:
					m_CurDirKey = ioUserKeyInput::DKI_DOWN;
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
					break;
				case ioUserKeyInput::DKI_LEFT:
					m_CurDirKey = ioUserKeyInput::DKI_LEFT;
					break;
				case ioUserKeyInput::DKI_LEFTUP:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
					break;
				default:
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
					break;
				}
			}
		}
		break;
	}
}

void ioObjectChargeShotItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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
}

bool ioObjectChargeShotItem::IsProtectState()
{
	if( m_dwAttackStartTime > 0 && m_dwAttackStartTime + m_dwProtectTime > FRAMEGETTIME() )
		return true;

	return false;
}

bool ioObjectChargeShotItem::EnableReduceGaugeOnSwitchBuff()
{
	if( m_dwAttackStartTime == 0 )
		return false;

	return true;
}

bool ioObjectChargeShotItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioObjectChargeShotItem::IsEnableDash() const
{
	return true;
}

bool ioObjectChargeShotItem::IsEnableDefense() const
{
	return m_bEnableObjectDefense;
}

void ioObjectChargeShotItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && m_dwMarkEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwMarkEffectID );
		m_dwMarkEffectID = -1;
	}
}

void ioObjectChargeShotItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( m_AttackState == AS_ATTACK )
		return;

	m_AttackState = AS_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_AttackAnimation );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fAttackAniRate );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();
		vPos += ( vDir * m_fMarkFrontOffset );
		vPos.y = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false ) + m_fMarkHeightOffset;
		m_vMarkPosition = vPos;
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		m_fMarkCurRange = m_fMarkFrontOffset;

		if( !m_MarkEffect.IsEmpty() )
		{
			ioBaseChar *pChar = pStage->GetCameraTargetChar();
			if( pChar && pOwner->GetTeam() == pChar->GetTeam())
			{
				ioMapEffect *pEffect = pStage->CreateMapEffect( m_MarkEffect, m_vMarkPosition, vScale );
				if( pEffect )
					m_dwMarkEffectID = pEffect->GetUniqueID();
			}
		}
	}

	if( m_dwProtectTime > 0 && m_dwAttackStartTime == 0 )
		m_dwAttackStartTime = FRAMEGETTIME();
}

void ioObjectChargeShotItem::ChangeToAttackEnd( ioBaseChar *pOwner )
{
	if( m_AttackState == AS_ATTACK_END )
		return;

	m_AttackState = AS_ATTACK_END;

	pOwner->SetChargingState( false );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_AttackEndAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fAttackEndAniRate );

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fAttackEndAniRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_ATTACK_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && m_dwMarkEffectID != -1 )
	{
		pStage->DestroyMapEffect( m_dwMarkEffectID );
		m_dwMarkEffectID = -1;
	}

}

void ioObjectChargeShotItem::OnPreDelay( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		ChangeToAttackFire( pOwner );
	}
}

void ioObjectChargeShotItem::OnAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToAttackEnd( pOwner );
			return;
		}
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fDist = m_fMarkSpeed * fTimePerSec;
		m_fMarkCurRange += fDist;
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vDir = pOwner->GetMoveDir();

		m_vMarkPosition = vPos;
		m_vMarkPosition += ( vDir * m_fMarkCurRange );
		m_vMarkPosition.y = pStage->GetMapHeight( m_vMarkPosition.x, m_vMarkPosition.z, NULL, false ) + m_fMarkHeightOffset;

		if( m_fMarkCurRange >= m_fMarkRange )
		{
			m_fMarkCurRange = m_fMarkFrontOffset;
		}

		ioMapEffect *pEffect = pStage->FindMapEffect( m_dwMarkEffectID );
		if( pEffect )
			pEffect->SetWorldPosition( m_vMarkPosition );
	}

	// 발사 처리.
	if( m_dwWeaponFireCheckTime < FRAMEGETTIME() )
	{
		CheckWeaponFloatInfo( pOwner );
		CreateWeapon( pOwner );

		m_dwWeaponFireCheckTime = FRAMEGETTIME() + m_dwWeaponFireTime;
	}
}

void ioObjectChargeShotItem::OnAttackEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetState( CS_DELAY );
	}
}

void ioObjectChargeShotItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
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

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioObjectChargeShotItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = ( fTimePerSec * 1000 ) / m_dwRotateTime;
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

void ioObjectChargeShotItem::CheckWeaponFloatInfo( ioBaseChar *pOwner )
{
	// WeaponMove
	D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir = m_vMarkPosition - vCharPos;
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
	if( m_fMarkRange > 0.0f )
	{
		fRate = fRange / m_fMarkRange;

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
		float fGapHeight2 = vCharPos.y - m_vMarkPosition.y;
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

float ioObjectChargeShotItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
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

float ioObjectChargeShotItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
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

void ioObjectChargeShotItem::CreateWeapon( ioBaseChar *pOwner )
{
	pOwner->NormalAttackFireExplicit( m_WeaponInfo );

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && !m_MarkAfterEffect.IsEmpty() )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		pStage->CreateMapEffect( m_MarkAfterEffect, m_vMarkPosition, vScale );
	}
}

bool ioObjectChargeShotItem::EnableReleaseObject( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	return false;
}