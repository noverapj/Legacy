

#include "stdafx.h"

#include "ioMachineCannon2.h"

#include "ioVoiceChat/Fourier.h"

#include "ioPlayStage.h"
#include "ioPlayMode.h"
#include "ioBaseChar.h"


#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioMachineCannon2::ioMachineCannon2( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioMachineStruct( pGrp, pMode )
{
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bAttackKey = false;

	m_dwChangeStateTime = 0;
	m_dwSetTakeTime = 0;

	m_dwFireTime = 0;
	m_dwFireEndTime = 0;
	m_iWeaponFireCnt = 0;

	m_CannonState = MCS_NONE;
}

ioMachineCannon2::~ioMachineCannon2()
{
}

void ioMachineCannon2::LoadProperty( ioINILoader &rkLoader )
{
	ioMachineStruct::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "cannon_none_ani", "", szBuf, MAX_PATH );
	m_NoneAnimation = szBuf;

	rkLoader.LoadString_e( "cannon_delay_ani", "", szBuf, MAX_PATH );
	m_DelayAnimation = szBuf;

	rkLoader.LoadString_e( "cannon_charging_ani", "", szBuf, MAX_PATH );
	m_ChargingAnimation = szBuf;

	rkLoader.LoadString_e( "cannon_fire_ani", "", szBuf, MAX_PATH );
	m_FireAnimation = szBuf;
	m_fFireAniRate = rkLoader.LoadFloat_e( "cannon_fire_ani_rate", FLOAT1 );

	m_dwRotateTime = rkLoader.LoadInt_e( "cannon_rotate_speed", 0 );

	m_ReleaseOffSet.x = rkLoader.LoadFloat_e( "release_char_offset_x", 0.0f );
	m_ReleaseOffSet.y = rkLoader.LoadFloat_e( "release_char_offset_y", 0.0f );
	m_ReleaseOffSet.z = rkLoader.LoadFloat_e( "release_char_offset_z", 0.0f );

	rkLoader.LoadString_e( "weapon_wounded_animation", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "weapon_wounded_duration", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "weapon_wounded_loop_ani", false );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "weapon_attribte_index", 0 );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "weapon_attribute_resist_index", 0 );

	m_fWeaponOffSet = rkLoader.LoadFloat_e( "weapon_offset", 0.0f );
	m_fWeaponSideOffSet = rkLoader.LoadFloat_e( "weapon_side_offset", 0.0f );
	m_fWeaponHeightOffSet = rkLoader.LoadFloat_e( "weapon_height_offset", 0.0f );
}

MachineStructType ioMachineCannon2::GetType() const
{
	return MCT_CANNON2;
}

void ioMachineCannon2::InitMachineStructInfo( int iStructNum, int iArrayNum, DWORD dwCreateGapTime, DWORD dwSeed,
											  const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale )
{
	ioMachineStruct::InitMachineStructInfo( iStructNum, iArrayNum, dwCreateGapTime, dwSeed, vPos, vScale );

	m_CannonState = MCS_NONE;
	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	if( !m_NoneAnimation.IsEmpty() )
		m_pGroup->SetLoopAni( m_NoneAnimation, FLOAT100 );
}

bool ioMachineCannon2::IsTakeEnableState()
{
	if( !m_TakeCharName.IsEmpty() )
		return false;

	if( m_CannonState != MCS_NONE )
		return false;

	return true;
}

void ioMachineCannon2::SetTakeState( const ioHashString &szTakeCharName )
{
	m_TakeCharName = szTakeCharName;
	m_CannonState = MCS_DELAY;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwChangeStateTime = dwCurTime;
	m_dwSetTakeTime = dwCurTime;

	m_dwFireTime = 0;
	m_dwFireEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bAttackKey = false;

	// charging
	m_dwChargingStartTime = 0;
	m_fCurChargeRate = 0.0f;

	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	m_iBaseWeaponIndex = pOwner->GetWeaponIndexBase();
	m_iWeaponFireCnt = 0;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	if( !m_DelayAnimation.IsEmpty() )
		m_pGroup->SetLoopAni( m_DelayAnimation, FLOAT100 );

	if( !m_CameraBuff.IsEmpty() )
		pOwner->AddNewBuff( m_CameraBuff, "", "", NULL );
}

void ioMachineCannon2::ReleaseState()
{
	ioBaseChar *pOwner = NULL;
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( pPlayStage )
	{
		pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	}

	m_TakeCharName.Clear();
	m_CannonState = MCS_NONE;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bTargetRot = false;
	m_bAttackKey = false;

	m_dwChangeStateTime = 0;
	m_dwSetTakeTime = 0;

	m_dwFireTime = 0;
	m_dwFireEndTime = 0;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	if( !m_NoneAnimation.IsEmpty() )
		m_pGroup->SetLoopAni( m_NoneAnimation, FLOAT100 );

	m_iWeaponFireCnt = 0;

	if( pOwner && !m_CameraBuff.IsEmpty() )
		pOwner->RemoveBuff( m_CameraBuff );
}

void ioMachineCannon2::SetStructDie( ioBaseChar *pChar, bool bEffect )
{
	m_CannonState = MCS_NONE;

	bool bSend = false;
	if( !m_TakeCharName.IsEmpty() )
	{
		if( pChar->IsNeedSendNetwork() )
			bSend = true;
	}

	if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		if( bSend )
		{
			SendDieMessageToServer( bEffect );
		}
	}
	else
	{
		StructDie();
	}
}

void ioMachineCannon2::StructDie( bool bEffect )
{
	m_CannonState = MCS_NONE;

	m_bLive = false;
	PlaySound( m_DestroySound );

	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	D3DXVECTOR3 vPos = GetWorldPosition() + m_ReleaseOffSet;
	pOwner->SetMachineRelease( this, vPos );
}

void ioMachineCannon2::ProcessState( float fTimePerSec )
{
	switch( m_CannonState )
	{
	case MCS_NONE:
		ProcessNone( fTimePerSec );
		break;
	case MCS_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case MCS_CHARGING:
		ProcessCharging( fTimePerSec );
		break;
	case MCS_FIRE:
		ProcessFire( fTimePerSec );
		break;
	}
}

void ioMachineCannon2::ProcessNone( float fTimePerSec )
{
}

void ioMachineCannon2::ProcessDelay( float fTimePerSec )
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	// takechar check
	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner )
	{
		ReleaseState();
		return;
	}

	if( pOwner->GetState() != CS_MACHINE_TAKE )
	{
		ReleaseState();
		return;
	}

	// 점프키 체크
	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() && pOwner->IsJumpKey() &&
		m_dwSetTakeTime > 0 && m_dwSetTakeTime+100 < dwCurTime )
	{
		D3DXVECTOR3 vPos = GetWorldPosition() + m_ReleaseOffSet;
		pOwner->SetMachineRelease( this, vPos );

		ReleaseState();

		if( pOwner->IsNeedSendNetwork() )
		{
			/*
			// tcp sync
			SP2Packet kPacket( CTPK_MACHINESTRUCT );
			kPacket << MACHINE_RELEASE;
			kPacket << GetMachineArrayIdx();
			kPacket << vPos;
			TCPNetwork::SendToServer( kPacket );
			*/
		}
		return;
	}

	// 방향키 체크
	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	// 공격키 체크
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsAttackKey() )
		{
			ChangeToChargingState();
			return;
		}
	}
}

void ioMachineCannon2::ChangeToFireState()
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	// 공격상태로 변경
	DWORD dwCurTime = FRAMEGETTIME();
	m_CannonState = MCS_FIRE;
	m_dwChangeStateTime = dwCurTime;

	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	float fTimeRate = m_fFireAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = m_pGroup->GetAnimationIdx( m_FireAnimation );
	m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwFireTime = m_dwChangeStateTime + m_pGroup->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireEndTime = m_dwChangeStateTime + m_pGroup->GetAnimationFullTime( iAniID ) * fTimeRate;

	pOwner->SetTargetRotToRotate( GetWorldOrientation(), true, false );

	// check charging
	DWORD dwGapTime = dwCurTime - m_dwChargingStartTime;
	if( m_dwChargingMaxTime > 0 )
	{
		m_fCurChargeRate = (float)dwGapTime / m_dwChargingMaxTime;
		m_fCurChargeRate = min( (max(0.0f, m_fCurChargeRate)), FLOAT1 );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		// udp sync
		SP2Packet kPacket( CUPK_MACHINESTRUCT );
		kPacket << GetMachineArrayIdx();
		kPacket << MACHINE_STATE_U;
		kPacket << m_CannonState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMachineCannon2::ProcessFire( float fTimePerSec )
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireEndTime > 0 && m_dwFireEndTime < dwCurTime )
	{
		m_CannonState = MCS_DELAY;
		m_dwChangeStateTime = dwCurTime;

		m_CurDirKey = ioUserKeyInput::DKI_NONE;

		m_bTargetRot = false;
		m_bAttackKey = false;

		m_dwFireTime = 0;
		m_dwFireEndTime = 0;

		// charging
		m_dwChargingStartTime = 0;
		m_fCurChargeRate = 0.0f;

		m_pGroup->ClearAllActionAni( FLOAT100, true );
		m_pGroup->ClearAllLoopAni( FLOAT100, true );

		if( !m_DelayAnimation.IsEmpty() )
			m_pGroup->SetLoopAni( m_DelayAnimation, FLOAT100 );
		return;
	}

	if( m_dwFireTime > 0 && m_dwFireTime < dwCurTime )
	{
		D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vCurDir, &vCurDir );

		D3DXVECTOR3 vCurPos = GetMidPositionByRate();
		vCurPos = vCurPos + (vCurDir * m_fWeaponOffSet);

		int iWeaponIndex = m_iBaseWeaponIndex + m_iWeaponFireCnt;

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;
		kFireTime.dwDuration = 1;

		kFireTime.iAttributeIdx = m_WeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_WeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_WeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_WeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_WeaponInfo.m_bWoundedLoopAni;

		kFireTime.m_iExtraValue = GetMachineArrayIdx();

		kFireTime.dwWeaponIdx = iWeaponIndex;
		kFireTime.eCollisionType = ACT_SPECIAL;
		kFireTime.eFireTimeType = FTT_MACHINE_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
		m_iWeaponFireCnt++;

		m_dwFireTime = 0;

		if( !m_WeaponFireEffect.IsEmpty() )
			m_pGroup->AttachEffect( m_WeaponFireEffect, NULL );

		if( !m_WeaponFireSound.IsEmpty() )
			PlaySound( m_WeaponFireSound );
	}
}

void ioMachineCannon2::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_dwRotateTime <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
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
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true );
		SetWorldOrientation( qtRotate );
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
	SetWorldOrientation( qtRotate );
}

void ioMachineCannon2::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_MACHINESTRUCT );
			kPacket << GetMachineArrayIdx();
			kPacket << MACHINE_DIR_U;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioMachineCannon2::ApplyMachineStructExtraInfo( SP2Packet &rkPacket )
{
	if( m_CannonState == MCS_NONE )
		return;

	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case MACHINE_DIR_U:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			m_bTargetRot = false;
			pOwner->SetTargetRotToRotate( qtRot, true );

			int iDir;
			rkPacket >> iDir;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	case MACHINE_STATE_U:
		ApplyMachineState( rkPacket );
		break;
	}
}

void ioMachineCannon2::ApplyMachineState( SP2Packet &rkPacket )
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case MCS_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
			if( pOwner )
				pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToFireState();
		}
		break;
	case MCS_CHARGING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
			if( pOwner )
				pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToChargingState();
		}
		break;
	}
}

bool ioMachineCannon2::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon ) return false;

	return false;

	if( !IsCollisionAvailableRange( pWeapon ) )
		return false;

	if( pWeapon->IsAlreadyCollisioned( this ) )
		return false;

	return true;
}

bool ioMachineCannon2::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return false;
}

ApplyWeaponExp ioMachineCannon2::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioMachineCannon::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( P2PNetwork::IsNetworkPlaying() &&
		m_pModeParent->IsNetworkMode() &&
		!pAttacker->IsNeedProcess() )
		return AWE_EXPLOSION;

	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = pWeapon->GetDamage( this );
	float fPushPower = pWeapon->GetDefensePushPower( fDamage );
	// damage, push no

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
	{
		bContactType = true;
	}

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );

			bOnlyEmoticon = false;
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_HP.m_fCurValue > 0.0f )
	{
		if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_PUSHSTRUCT_DAMAGE );
			kPacket << pWeapon->GetOwnerName();
			kPacket << m_iArrayIndex;
			kPacket << m_HP.m_fCurValue;
			kPacket << m_HP.m_fMaxValue;
			kPacket << bOnlyEmoticon;
			kPacket << vAttackDir;
			kPacket << fDamage;
			kPacket << fPushPower;
			kPacket << (int)eActionStop;
			kPacket << iShakeCamera;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( fPreCurHP > 0.0f )
		{
			m_HP.m_fCurValue = 0.0f;
			m_KillerChar = pWeapon->GetOwnerName();

			if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
			{
				ioBaseChar *pKillChar = m_pModeParent->GetCreator()->GetBaseChar( m_KillerChar );
				if( pKillChar )
				{
					if( pKillChar->IsNeedSendNetwork() )
					{
						SendDieMessageToServer( true );
					}
				}
				else
				{
					if( P2PNetwork::IsHost() )
					{
						SendDieMessageToServer( true );
					}
				}
			}
			else
			{
				StructDie();
			}
		}
	}

	if( pAttacker->IsOwnerChar() )
	{
		// ChangeJumpping
		if( pWeapon && pWeapon->IsChangeJumpping( false, false ) )
		{
			int iLimitCnt = pWeapon->GetChangeJumppingLimitCnt();
			pAttacker->SetChangeJumppingState( pWeapon->GetChangeJumppingPowerRate(), true, iLimitCnt );
		}
	}

	return AWE_EXPLOSION;
}

void ioMachineCannon2::ProcessCharging( float fTimePerSec )
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	// takechar check
	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner )
	{
		ReleaseState();
		return;
	}

	if( pOwner->GetState() != CS_MACHINE_TAKE )
	{
		ReleaseState();
		return;
	}

	// 방향키 체크
	CheckKeyInput( pOwner );
	ProcessRotate( pOwner );

	// 공격키 체크
	if( pOwner->IsNeedProcess() )
	{
		if( !pOwner->IsAttackKeyDown() )
		{
			ChangeToFireState();
			return;
		}
	}
}

void ioMachineCannon2::ChangeToChargingState()
{
	ioPlayStage *pPlayStage = g_MachineStructMgr.GetPlayStage();
	if( !pPlayStage ) return;

	ioBaseChar *pOwner = pPlayStage->GetBaseChar( m_TakeCharName );
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_CannonState = MCS_CHARGING;

	m_dwChargingStartTime = dwCurTime;
	m_fCurChargeRate = 0.0f;

	m_pGroup->ClearAllLoopAni( FLOAT100, true );
	m_pGroup->ClearAllActionAni( FLOAT100, true );

	if( !m_ChargingAnimation.IsEmpty() )
		m_pGroup->SetLoopAni( m_ChargingAnimation, FLOAT100 );

	if( pOwner->IsNeedSendNetwork() )
	{
		// udp sync
		SP2Packet kPacket( CUPK_MACHINESTRUCT );
		kPacket << GetMachineArrayIdx();
		kPacket << MACHINE_STATE_U;
		kPacket << m_CannonState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


