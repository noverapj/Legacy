
#include "stdafx.h"

#include "ioCrowDummy.h"

#include "ioVoiceChat/Fourier.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "SkillDefine.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioCrowDummy::ioCrowDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_dwAreaWeaponIndex = 0;
}

ioCrowDummy::~ioCrowDummy()
{
	m_vRandomAngleList.clear();

	if( m_dwAreaWeaponIndex > 0 )
		g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
}

void ioCrowDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_vRandomAngleList.clear();
	int iAngleCnt = rkLoader.LoadInt_e( "random_angle_cnt", 0 );

	float fAngle = 0.0f;
	for( int i=0; i < iAngleCnt; ++i )
	{
		wsprintf_e( szKey, "random_angle%d", i+1 );

		fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_vRandomAngleList.push_back( fAngle );
	}

	m_fRandomMoveSpeed = rkLoader.LoadFloat_e( "random_move_speed", 0.0f );
	m_fRandomRotateSpeed = rkLoader.LoadFloat_e( "random_rotate_speed", 0.0f );

	int iMin = rkLoader.LoadInt_e( "random_move_min_time", 0 );
	int iMax = rkLoader.LoadInt_e( "random_move_max_time", 0 );

	if( iMin <= iMax )
	{
		m_dwRandomMinTime = max( 0, iMin );
		m_dwRandomMaxTime = max( 0, iMax );
	}
	else
	{
		m_dwRandomMinTime = max( 0, iMax );
		m_dwRandomMaxTime = max( 0, iMin );
	}

	//
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
}

void ioCrowDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioCrowDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START || m_State == DCS_DELAY )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	D3DXVECTOR3 vNewPos = GetWorldPosition();
	D3DXQUATERNION qtCurRot = GetWorldOrientation();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// process targetrot
	if( m_fRandomRotateSpeed > 0.0f && !m_bTargetRot )
	{
		float fFrameGap = g_FrameTimer.GetCurFrameGap();
		float fRate = fFrameGap / m_fRandomRotateSpeed;
		float fRotateAngle = 360.0f * fRate;

		D3DXQUATERNION qtTarget;
		ioMath::CalcDirectionQuaternion( qtTarget, -m_vTargetDir );

		float fDotValue = D3DXVec3Dot( &vCurMoveDir, &m_vTargetDir );
		float fGapAngle = RADtoDEG( acosf(fDotValue) );

		if( fDotValue >= 0.98f )
		{
			m_bTargetRot = true;
			SetWorldOrientation( qtTarget );

			vCurMoveDir = m_vTargetDir;
		}
		else
		{
			D3DXVECTOR3 vAxis;
			if( fDotValue <= -0.98f )
			{
				vAxis = ioMath::UNIT_Y;
			}
			else
			{
				D3DXVec3Cross( &vAxis, &m_vTargetDir, &vCurMoveDir );
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
			SetWorldOrientation( qtNewRot );

			vCurMoveDir = vNewDir;
		}
	}

	// process movespeed
	float fMoveDist = m_fRandomMoveSpeed * fTimePerSec;
	vNewPos += vCurMoveDir * fMoveDist;

	SetWorldPosition( vNewPos );

	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_dwAreaWeaponIndex );
	if( pAreaWeapon && pAreaWeapon->GetAreaWeaponMoveSpeed() == 0.0f )
	{
		pAreaWeapon->SetAreaPosition( vNewPos, m_pModeParent->GetCreator() );
	}

	m_dwProcessTime = dwCurTime;
}

void ioCrowDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	// CheckRandom Time
	if( m_dwCheckRandomTime == 0 || m_dwCheckRandomTime < dwCurTime )
	{
		CheckRandomTime();
	}
}

void ioCrowDummy::SetDelayState( bool bNet )
{
	m_State = DCS_DELAY;
	m_iReserveAttackCode = 0;

	m_dwDelayStartTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	SetMoveState();

	ioBaseChar *pOwner = GetOwner();
	if( bNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCrowDummy::SetMoveState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_MOVE;

	CheckChargeWeaponDead();

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );

			float fTimeRate = m_fMoveAniRate;
			if( fTimeRate <= 0.0f )
				fTimeRate = FLOAT1;

			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		}
	}

	m_dwBaseSeed = pOwner->GetRandomSeed();
	m_TimeRandom.SetRandomSeed( m_dwBaseSeed );
	m_AngleRandom.SetRandomSeed( m_dwBaseSeed+1 );

	m_dwCheckRandomTime = 0;
	m_dwAreaWeaponIndex = 0;

	SetWorldOrientation( pOwner->GetTargetRot() );

	CreateAreaWeapon( GetWorldPosition() );
}

void ioCrowDummy::CheckRandomTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	// time
	DWORD dwGapTime = m_dwRandomMaxTime - m_dwRandomMinTime;
	DWORD dwRand = m_TimeRandom.Random( dwGapTime );

	m_dwCheckRandomTime = dwCurTime + m_dwRandomMaxTime + dwRand;

	// angle
	m_bTargetRot = false;
	int iAngleSize = m_vRandomAngleList.size();
	int iRand = m_AngleRandom.Random( iAngleSize );

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	if( COMPARE( iRand, 0, iAngleSize ) )
	{
		float fAngle = m_vRandomAngleList[iRand];
		fAngle = ioMath::ArrangeHead( fAngle );

		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
		
		m_vTargetDir = qtRot * vCurDir;
		D3DXVec3Normalize( &m_vTargetDir, &m_vTargetDir );
	}
}

void ioCrowDummy::CreateAreaWeapon( const D3DXVECTOR3 &vPos )
{
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage ) return;

	if( m_AreaWeaponName.IsEmpty() ) return;

	if( m_dwAreaWeaponIndex != 0 )
		return;

	D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( GetOwnerName(),
																  "",
																  "",
																  m_AreaWeaponName,
																  vPos,
																  qtRot,
																  ioAreaWeapon::CT_STRUCT );
	if( pAreaWeapon )
	{
		m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
	}
}







