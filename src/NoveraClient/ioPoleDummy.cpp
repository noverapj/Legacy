#include "StdAfx.h"
#include "ioPoleDummy.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioPoleDummy::ioPoleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode ): 
ioDummyChar( pGrp, pMode )
{
	m_fCurMotionRate = 0.0f;
	m_fCurYAngle = 0.0f;
}

ioPoleDummy::~ioPoleDummy(void)
{
}

void ioPoleDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "delay_down_animation", "", szBuf, MAX_PATH );
	m_szDownDelayMotion = szBuf;
}

void ioPoleDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	if ( !CheckLive() )
	{
		SendDieState( true );
		return;
	}

	switch( m_State )
	{
	case DCS_START:
		{
			DWORD dwCurTime  = FRAMEGETTIME();
			if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
			{
				SetDelayState( false );
				return;
			}
		}
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	}
}

bool ioPoleDummy::CheckLive()
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < dwCurTime && m_State != DCS_ATTACK )
		return false;

	return true;
}

void ioPoleDummy::SetDelayState( bool bNet )
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

	CheckChargeWeaponDead();

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;
			//m_pGroup->SetLoopAni( iAniID, 0.0f, FLOAT1, FLOAT1/fAniRate );
			m_pGroup->SetLoopAni( iAniID, 0.0f, FLOAT1 );//, FLOAT1/fAniRate );
		}
	}

	if( !m_szDownDelayMotion.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_szDownDelayMotion );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;
			//m_pGroup->SetLoopAni( iAniID, 0.0f, 0.0f, FLOAT1/fAniRate );
			m_pGroup->SetLoopAni( iAniID, 0.0f, FLOAT1 );//, FLOAT1/fAniRate );
		}
	}

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

void ioPoleDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return;

	CheckAniMotionRate( pOwner );

	//Look at Owner
	D3DXVECTOR3 vDir = pOwner->GetWorldPosition() - GetWorldPosition();
	vDir.y = 0;
	D3DXVec3Normalize( &vDir, &vDir );
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetWorldOrientation( qtRot );
}

void ioPoleDummy::CheckAniMotionRate( ioBaseChar* pOwner )
{
	m_fCurMotionRate = 0.0f;
	float fMinAngle = 0.0f;

	//밑에 두 사분면은 무시.
	m_fCurYAngle = ioMath::ArrangeHead(m_fCurYAngle);
	if ( m_fCurYAngle >= 0 && m_fCurYAngle < 180.0f )
		return;

	if ( m_fCurYAngle >= 180 && m_fCurYAngle < 270.0f )
		fMinAngle = 180.0f;
	else
		fMinAngle = 270.0f;

	float fAngleGap = m_fCurYAngle - fMinAngle;
	m_fCurMotionRate = fAngleGap/FLOAT90;

	ioAnimation *pCenter, *pDown;
	pCenter = pDown = NULL;

	pCenter = m_pGroup->GetLoopInPlay( m_DelayAnimation );
	pDown = m_pGroup->GetLoopInPlay( m_szDownDelayMotion );

	if ( fMinAngle == 180.0f )
	{
		if ( pCenter )
			pCenter->ForceWeight( m_fCurMotionRate );
		if ( pDown)
			pDown->ForceWeight( FLOAT1 - m_fCurMotionRate );
	}
	else
	{
		if ( pCenter )
			pCenter->ForceWeight( FLOAT1 - m_fCurMotionRate );
		if ( pDown)
			pDown->ForceWeight( m_fCurMotionRate );
	}
}

void ioPoleDummy::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{

}