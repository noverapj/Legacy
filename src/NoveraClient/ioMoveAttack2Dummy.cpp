
#include "stdafx.h"

#include "ioMoveAttack2Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "BuffDefine.h"

ioMoveAttack2Dummy::ioMoveAttack2Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_szTargetName.Clear();
	m_dwReturnTime = 0;
	m_bDefenseKey = false;
	m_fCheckMoveRange = 0.0f;

	m_DelayState = DS_FOLLOW;
	m_MoveState  = MS_MOVE;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_vTargetList.clear();
}

ioMoveAttack2Dummy::~ioMoveAttack2Dummy()
{
}

void ioMoveAttack2Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetRange    = rkLoader.LoadFloat_e( "target_range", 0.0f );
	m_fTargetAngle    = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fDummyHeightOffset = rkLoader.LoadFloat_e( "dummy_height_offset", 0.0f );
	m_dwTargetRefreshTime = rkLoader.LoadInt_e( "refresh_target_time", 0 );

	m_dwDelayTime = (DWORD)rkLoader.LoadInt_e( "delay_time", 0 );
	m_fMoveSpeed  = rkLoader.LoadFloat_e( "move_speed", FLOAT1 );
	m_fReturnSpeed = rkLoader.LoadFloat_e( "return_speed", FLOAT1 );

	m_bDisableReturnControl = rkLoader.LoadBool_e( "disable_return_control", false );
}

void ioMoveAttack2Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	}

	CheckKeyInput();

	if( !CheckOwnerBuff() )
	{
		SetDieState( true );
	}
}

void ioMoveAttack2Dummy::ProcessStartState( float fTimePerSec )
{
	SetAttachWeapon();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetDelayState( DS_FOLLOW );
		
		return;
	}
}

void ioMoveAttack2Dummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();

	if( !pOwner )
		return;

	switch( m_DelayState )
	{
	case DS_FOLLOW:
		{
			D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
			vPos.y += m_fHeightOffset;
			SetWorldPosition( vPos );

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
			SetWorldOrientation( qtRot );
		}
		break;
	case DS_STANDBY:
		if( m_dwReturnTime > 0 && m_dwReturnTime < FRAMEGETTIME() )
		{
			SetReturnState();
		}
		break;
	}
}

void ioMoveAttack2Dummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	
	D3DXVECTOR3 vPos = GetWorldPosition();
	D3DXQUATERNION qtRot;
	
	switch( m_MoveState )
	{
	case MS_MOVE:
		{
			FindTarget();

			float fMoveValue = fTimePerSec * m_fMoveSpeed;
			vPos += m_vAttackDir * fMoveValue;
			qtRot = Help::ConvertDirToQuaternion( m_vAttackDir );

			SetWorldOrientation( qtRot );
			SetWorldPosition( vPos );

			D3DXVECTOR3 vDiff = GetWorldPosition() - pOwner->GetWorldPosition();
			float fDistanceSq = D3DXVec3LengthSq( &vDiff );

			if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
			{
				SetReturnState();
			}
		}
		break;
	case MS_RETURN:
		{
			float fMoveValue = fTimePerSec * m_fReturnSpeed;
			vPos += m_vAttackDir * fMoveValue;
			qtRot = Help::ConvertDirToQuaternion( m_vAttackDir );

			SetWorldOrientation( qtRot );
			SetWorldPosition( vPos );

			m_fCheckMoveRange -= fMoveValue;
			if( m_fCheckMoveRange <= 0.0f )
			{
				m_fCheckMoveRange = 0.0f;
				SetDelayState( DS_FOLLOW );
			}
		}
		break;
	}
}

void ioMoveAttack2Dummy::SetDelayState( DelayState eState )
{
	if( m_State == DCS_DELAY )
		return;

	m_State = DCS_DELAY;
	m_DelayState = eState;
	m_dwReturnTime = FRAMEGETTIME() + m_dwDelayTime;

	m_szTargetName.Clear();

	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioMoveAttack2Dummy::SetMoveState()
{
	if( m_State == DCS_MOVE && m_MoveState == MS_MOVE )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	m_vAttackDir = pOwner->GetMoveDir();
	
	if( m_State == DCS_DELAY && m_DelayState == DS_FOLLOW )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		vPos.y += m_fDummyHeightOffset;
		SetWorldPosition( vPos );
	}

	m_State = DCS_MOVE;
	m_MoveState = MS_MOVE;
	m_dwReturnTime = 0;

	D3DXQUATERNION qtRot;
	qtRot = Help::ConvertDirToQuaternion( -m_vAttackDir );
	SetWorldOrientation( qtRot );

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fMoveAniRate > 0.0f )
				fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioMoveAttack2Dummy::SetReturnState()
{
	if( m_State == DCS_MOVE && m_MoveState == MS_RETURN )
		return;

	m_State = DCS_MOVE;
	m_MoveState = MS_RETURN;
	m_dwReturnTime = 0;
	m_szTargetName.Clear();

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		m_vAttackDir *= -1;

	D3DXVECTOR3 vDir = pOwner->GetMidPositionByRate() - GetMidPositionByRate();
	D3DXVec3Normalize( &m_vAttackDir, &vDir );
	m_fCheckMoveRange = D3DXVec3Length( &vDir );

	D3DXQUATERNION qtRot;
	qtRot = Help::ConvertDirToQuaternion( m_vAttackDir );
	SetWorldOrientation( qtRot );

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			float fAniRate = FLOAT1;
			if( m_fMoveAniRate > 0.0f )
				fAniRate = m_fMoveAniRate;

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}
}

void ioMoveAttack2Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	SetWorldPosition( vPos );

	rkPacket >> m_bDefenseKey;

	if( m_bDefenseKey )
		SetMoveState();
	else
		SetDelayState( DS_STANDBY );

	int iDirKey;
	rkPacket >> iDirKey;
	if( iDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( iDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vAttackDir, &vDir );
	}
}

void ioMoveAttack2Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

bool ioMoveAttack2Dummy::IsActivateWeapon()
{
	if( m_State == DCS_DELAY )
		return false;

	return true;
}

void ioMoveAttack2Dummy::FindTarget()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	float fPrevDistSq = m_fTargetRange*m_fTargetRange;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	int iSize = rkTargetList.size();

	bool bNoTarget = true;
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		if( !CheckTarget( pTarget->GetCharName() ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq < fPrevDistSq )	// Enemy
		{
			D3DXVECTOR3 vDir = vDiff;
			D3DXVec3Normalize( &vDir, &vDir );

			float fHalfCosine = 0.0f;
			fHalfCosine = cosf( DEGtoRAD( m_fTargetAngle ) * FLOAT05 );

			if( D3DXVec3Dot( &m_vAttackDir, &vDir ) >= fHalfCosine )
			{
				m_szTargetName = pTarget->GetCharName();
				bNoTarget = false;
				fPrevDistSq = fDistSq;
			}
		}
	}

	if( bNoTarget )
	{
		m_szTargetName.Clear();
	}
	else
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_szTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y += m_fDummyHeightOffset;
			D3DXVECTOR3 vDir = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &m_vAttackDir, &vDir );
		}
	}
}

bool ioMoveAttack2Dummy::CheckTarget( const ioHashString& szTargetName )
{
	int iCnt = m_vTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vTargetList[i].m_TargetName == szTargetName && FRAMEGETTIME() - m_vTargetList[i].m_dwTargetTime < m_dwTargetRefreshTime )
		{
			return false;
		}
	}

	return true;
}

void ioMoveAttack2Dummy::SetTargetTime( const ioHashString& szTargetName )
{
	int iCnt = m_vTargetList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vTargetList[i].m_TargetName == szTargetName )
		{
			m_vTargetList[i].m_dwTargetTime = FRAMEGETTIME();
			return;
		}
	}

	TargetList kInfo;
	kInfo.m_TargetName = szTargetName;
	kInfo.m_dwTargetTime = FRAMEGETTIME();
	m_vTargetList.push_back( kInfo );
}

void ioMoveAttack2Dummy::CheckKeyInput()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_State == DCS_START )
		return;

	if( m_bDisableReturnControl && m_State == DCS_MOVE && m_MoveState == MS_RETURN )
		return;

	if( pOwner->IsDefenseKey() || pOwner->IsDefenseKeyDown() )
	{
		if( m_bDefenseKey )
			m_bChangeKey = false;
		else
			m_bChangeKey = true;
		
		m_bDefenseKey = true;

		if( m_bChangeKey )
		{
			SetMoveState();
		}
	}
	else
	{
		if( m_bDefenseKey )
			m_bChangeKey = true;
		else
			m_bChangeKey = false;

		m_bDefenseKey = false;

		if( m_bChangeKey )
		{
			SetDelayState( DS_STANDBY );
		}
	}

	if( pOwner->IsSettedDirection() && m_bDefenseKey )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vAttackDir, &vDir );

			m_CurDirKey = eNewDirKey;

			m_bChangeKey = true;
		}
	}

	if( pOwner->IsNeedSendNetwork() && m_bChangeKey )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << GetWorldPosition();
		kPacket << m_bDefenseKey;
		kPacket << static_cast<int>( m_CurDirKey );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioMoveAttack2Dummy::CheckOwnerBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( pOwner->HasBuff(BT_CREATE_DUMMY_CHAR) )
		return true;

	return false;
}

void ioMoveAttack2Dummy::CheckCollisionWeapon( ioBaseChar *pChar )
{
	if( m_szTargetName.IsEmpty() )
		return;

	SetTargetTime( m_szTargetName );
	m_szTargetName.Clear();

	if( pChar )
		m_vAttackDir = pChar->GetMoveDir();
}