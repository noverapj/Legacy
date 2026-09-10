#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioShuffleStarModeItem.h"

ioShuffleStarModeItem::ioShuffleStarModeItem()
{
	m_State = IS_WAIT;
	m_vMoveDirection = -ioMath::UNIT_Z;
	m_dwActivateTime = 0;
}

ioShuffleStarModeItem::ioShuffleStarModeItem( const ioShuffleStarModeItem &rhs )
	: ioModeItem( rhs ),
	m_LoopEffectName( rhs.m_LoopEffectName )
{
	m_State = IS_WAIT;
	m_vMoveDirection = -ioMath::UNIT_Z;
	m_dwActivateTime = 0;
}

ioShuffleStarModeItem::~ioShuffleStarModeItem()
{

}

void ioShuffleStarModeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioModeItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "loop_effect", "", szBuf, MAX_PATH );
	m_LoopEffectName = szBuf;
}
ioModeItem *ioShuffleStarModeItem::Clone()
{
	return new ioShuffleStarModeItem( *this );
}

void ioShuffleStarModeItem::OnCreate()
{
	if( !m_pCreator )
		return;

	ioModeItem::OnCreate();
	ioMapEffect *pEffect = m_pCreator->CreateMapEffect( m_LoopEffectName, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	if( pEffect )
	{				
		m_dwLoopEffectID = pEffect->GetUniqueID();
	}
}

void ioShuffleStarModeItem::OnDestroy()
{
	if( !m_pCreator )
		return;

	ioModeItem::OnDestroy();

	if( m_dwLoopEffectID != -1 )
	{
		m_pCreator->DestroyMapEffect( m_dwLoopEffectID );
		m_dwLoopEffectID = -1;
	}
}

void ioShuffleStarModeItem::SetItemValue( DWORD dwIndex, float fXPos, float fZPos, int iAngle, float fSpeed, float fPower, DWORD dwActivateTime )
{
	SetUniqueID( dwIndex );

	D3DXVECTOR3 vPos = D3DXVECTOR3( fXPos, 0.0f, fZPos );
	vPos.y += m_pCreator->GetMapHeight( vPos.x, vPos.z, NULL );
	SetPosition( vPos );
	
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXQUATERNION qtRot;
	float fAngle = static_cast<float>( iAngle );
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
	vDir = qtRot * vDir;
	SetDirection( vDir );

	m_fFloatingPower = fPower;
	m_fMoveSpeed = fSpeed;

	m_dwActivateTime = FRAMEGETTIME() + dwActivateTime;
}

void ioShuffleStarModeItem::SetItemValue( DWORD dwIndex, D3DXVECTOR3 vPos, int iAngle, float fSpeed, float fPower, DWORD dwActivateTime )
{
	SetUniqueID( dwIndex );

	float fHeight = m_pCreator->GetMapHeight( vPos.x, vPos.z, NULL );	
	SetPosition( vPos );

	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXQUATERNION qtRot;
	float fAngle = static_cast<float>( iAngle );
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
	vDir = qtRot * vDir;
	SetDirection( vDir );

	m_fFloatingPower = fPower;
	m_fMoveSpeed = fSpeed;

	m_dwActivateTime = FRAMEGETTIME() + dwActivateTime;
}

void ioShuffleStarModeItem::Process()
{
	ioModeItem::Process();

	switch( m_State )
	{
	case IS_WAIT:
		ProcessWait();
		break;
	case IS_MOVE:
		ProcessMove();
		break;
	case IS_PINGPONG:
		ProcessPingpong();
	case IS_DELAY:
		ProcessDelay();
		break;		
	}
}

void ioShuffleStarModeItem::ProcessWait()
{
	if( m_dwActivateTime > 0 && m_dwActivateTime < FRAMEGETTIME() )
	{
		OnCreate();
		SetMoveState();
	}
}

void ioShuffleStarModeItem::ProcessMove()
{
	if( !m_pCreator )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurGravity = m_pCreator->GetRoomGravity() * fTimePerSec;

	D3DXVECTOR3 vMove = m_vMoveDirection * ( m_fCurMoveSpeed * fTimePerSec );

	vMove.y += m_fCurFloatingPower * fTimePerSec;
	m_fCurFloatingPower -= fCurGravity;
		
	D3DXVECTOR3 vPos = GetPosition() + vMove;

	float fMapHeight = m_pCreator->GetMapHeight( vPos.x, vPos.z, NULL, false );
	if( vPos.y < fMapHeight )
	{			
		vPos.y = fMapHeight;
		SetPingpongState();
	}
	SetPosition( vPos );
	

	ioMapEffect* pMapEffect = m_pCreator->FindMapEffect( m_dwLoopEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );
	}
}

void ioShuffleStarModeItem::ProcessPingpong()
{
	if( !m_pCreator )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurGravity = m_pCreator->GetRoomGravity() * fTimePerSec;

	D3DXVECTOR3 vMove = m_vMoveDirection * ( m_fCurMoveSpeed * fTimePerSec );

	vMove.y += m_fCurFloatingPower * fTimePerSec;
	m_fCurFloatingPower -= fCurGravity;

	D3DXVECTOR3 vPos = GetPosition() + vMove;
	float fMapHeight = m_pCreator->GetMapHeight( vPos.x, vPos.z, NULL, false );
	if( vPos.y < fMapHeight )
	{			
		vPos.y = fMapHeight;

		m_iCurPingPongCnt++;
		if( m_iCurPingPongCnt >= 8 )
		{
			SetDelayState();
		}
		else
		{
			m_fCurMoveSpeed     = m_fMoveSpeed/m_iCurPingPongCnt;
			m_fCurFloatingPower = m_fFloatingPower/m_iCurPingPongCnt;
		}
	}
	SetPosition( vPos );

	ioMapEffect* pMapEffect = m_pCreator->FindMapEffect( m_dwLoopEffectID );
	if( pMapEffect )		
		pMapEffect->SetWorldPosition( vPos );
}

void ioShuffleStarModeItem::ProcessDelay()
{
}

void ioShuffleStarModeItem::SetMoveState()
{
	if( m_State == IS_MOVE )
		return;

	m_State = IS_MOVE;

	m_fCurMoveSpeed = m_fMoveSpeed;
	m_fCurFloatingPower  = m_fFloatingPower;
	m_iCurPingPongCnt = 1;
}

void ioShuffleStarModeItem::SetPingpongState()
{
	if( m_State == IS_PINGPONG )
		return;

	m_State = IS_PINGPONG;	

	m_fCurMoveSpeed = m_fMoveSpeed * FLOAT05;
	m_fCurFloatingPower  = m_fFloatingPower * FLOAT05;	
}

void ioShuffleStarModeItem::SetDelayState()
{
	if( m_State == IS_DELAY )
		return;

	m_State = IS_DELAY;
}

bool ioShuffleStarModeItem::IsCollision( ioBaseChar *pChar )
{
	if( m_State != IS_DELAY )
		return false;

	return ioModeItem::IsCollision( pChar );
}

bool ioShuffleStarModeItem::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State != IS_MOVE && m_State != IS_PINGPONG )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, GetAttackBox(), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	SetWallCollision( vNormal );
	return true;
}

void ioShuffleStarModeItem::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	D3DXVECTOR3 vMoveDir = m_vMoveDirection;
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fDotValue = D3DXVec3Dot( &vMoveDir, &vFaceNrm );
	if( fDotValue > 0.1f )
		return;

	D3DXVECTOR3 vReflect = vMoveDir - 2.0f * fDotValue * vFaceNrm;
	vReflect.y = 0.0f;
	D3DXVec3Normalize( &vReflect, &vReflect );

	m_vMoveDirection = vReflect;
	D3DXVec3Normalize( &m_vMoveDirection, &m_vMoveDirection );
}