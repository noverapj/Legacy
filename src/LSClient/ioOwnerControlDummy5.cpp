#include "StdAfx.h"
#include "ioOwnerControlDummy5.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioOwnerControlDummy5::ioOwnerControlDummy5( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_dwAttackStartTime = 0;
}

ioOwnerControlDummy5::~ioOwnerControlDummy5(void)
{
}

void ioOwnerControlDummy5::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_dwAttackTicTime = (DWORD)rkLoader.LoadInt_e( "attack_tic_time", 0 );
}

void ioOwnerControlDummy5::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState();
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimerPerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimerPerSec );
}

void ioOwnerControlDummy5::ProcessStartState()
{
	if ( m_bStartAttchWeapon )
		SetAttachWeapon();

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vScale = pOwner->GetWorldScale();
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSide;
		D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

		vPos += vDir * m_fFrontOffset;
		vPos.y += m_fHeightOffset;
		vPos += vSide * m_fSideOffset;
		SetWorldPosition( vPos );
		SetWorldScale( vScale );
		//지정된 회전 앵글을 유지 할떄
		if( m_bKeepStartAngle && m_fStartAngle != 0.0f )
		{
			float fYawD = ioMath::ArrangeHead( m_fStartAngle );
			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
			vDir = qtRotate * vDir;					
		}

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		SetWorldOrientation( qtRot );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime <  dwCurTime )
	{
		if ( m_dwAttackTicTime != 0 )
			m_dwAttackStartTime = dwCurTime + m_dwAttackTicTime;

		SetDelayState( false );
		return;
	}
}

void ioOwnerControlDummy5::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwAttackTicTime > 0 && m_dwAttackStartTime > 0 && m_dwAttackStartTime < dwCurTime )
		{
			m_dwAttackStartTime = 0;
			m_iReserveAttackCode = m_iControlNormalAttack;
			SetAttackState();
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_NORMAL_ATT;
				P2PNetwork::SendToAllPlayingUser( kPacket );
				return;
			}
		}
		bool bChange = false;

		// 방향변화 체크
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			bChange = true;
			m_CurDirKey = eNewDirKey;
		}

		// Up 체크
		if( !bChange && !m_bSetDownKey )
		{
			if( pOwner->IsJumpKeyDown() )
			{
				bChange = true;
				m_bSetUpKey = true;
			}
		}

		// Down 체크
		if( !bChange && !m_bSetUpKey && !m_bAutoDescent )
		{
			if( pOwner->IsDefenseKeyDown() )
			{
				bChange = true;
				m_bSetDownKey = true;
			}
		}

		if( bChange )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_KEY_CHANGE;
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			kPacket << true;			// setmove...
			kPacket << GetWorldPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
			SetMoveState();		
		}
	}
}

void ioOwnerControlDummy5::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() )
	{
		if ( m_dwAttackTicTime > 0 && m_dwAttackStartTime > 0 && m_dwAttackStartTime < FRAMEGETTIME() )
		{
			m_dwAttackStartTime = 0;
			m_iReserveAttackCode = m_iControlNormalAttack;
			SetAttackState();
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
				kPacket << m_OwnerName;
				kPacket << m_iCreateIndex;
				kPacket << DUMMY_CHAR_NORMAL_ATT;
				P2PNetwork::SendToAllPlayingUser( kPacket );
				return;
			}
		}

		if ( !pOwner->IsSettedDirection() && !pOwner->IsJumpKeyDown() && ( !pOwner->IsDefenseKeyDown() && !m_bAutoDescent ) )
		{
			SetDelayState( true );
			return;
		}
	}

	CheckKeyInputChange();
	CheckControlMove( fTimePerSec );
}

void ioOwnerControlDummy5::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon *pWeapon = NULL;
				pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );

				if( pWeapon )
				{
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						pZone->SetPosition( GetMidPositionByRate() );
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
							pZone->SetCollisionDir( vDir );
							pZone->SetZoneValue( true, false );
						}
					}
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//
	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		if ( m_dwAttackTicTime != 0 )
			m_dwAttackStartTime = dwCurTime + m_dwAttackTicTime;
		SetDelayState( false );
		return;
	}
}

void ioOwnerControlDummy5::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );
}

int ioOwnerControlDummy5::DontMoveEntityLevel() const
{
	if( m_bMountEnable )
		return 1;
	else
		return 3;
}

bool ioOwnerControlDummy5::IsMountingAvailable() const
{
	if( m_bMountEnable )
		return false;

	return true;
}

bool ioOwnerControlDummy5::IsNowMountEnable() const
{
	if( m_bCurMountEnable && m_bMountEnable )
		return true;

	return false;
}

void ioOwnerControlDummy5::DropZoneCheck()
{
	if( m_dwDropZoneLiveTime > 0 )
	{
		bool bCheckDropZone = false;
		if( m_dwDropZoneCheckStartTime <= 0 )
			m_dwDropZoneCheckStartTime = FRAMEGETTIME();

		D3DXVECTOR3 vCurPos = GetWorldPosition();

		if( GetOwner() &&
			GetOwner()->GetCreator() &&
			GetOwner()->GetCreator()->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
			bCheckDropZone = true;

		if( !bCheckDropZone )
			m_dwDropZoneCheckStartTime = 0;

		if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;

			SetDieState( true );
		}
	}
}

bool ioOwnerControlDummy5::IsMountAvailable() const
{
	if( m_bMountEnable )
		return true;
	return false;
}
