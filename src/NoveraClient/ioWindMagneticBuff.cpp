
#include "stdafx.h"

#include "ioWindMagneticBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"



ioWindMagneticBuff::ioWindMagneticBuff()
{
	m_BuffState = BS_NONE;
}

ioWindMagneticBuff::ioWindMagneticBuff( const ioWindMagneticBuff &rhs )
: ioBuff( rhs ),
m_fPullRange( rhs.m_fPullRange ),
m_fPullMinRange( rhs.m_fPullMinRange ),
m_fPullSpeed( rhs.m_fPullSpeed ),
m_szPullAni( rhs.m_szPullAni ),
m_szPullExtendAni( rhs.m_szPullExtendAni ),
m_PullEndType( rhs.m_PullEndType ),
m_PullEndWeaponInfo( rhs.m_PullEndWeaponInfo ),
m_fTargetOffsetFr( rhs.m_fTargetOffsetFr ),
m_fTargetOffsetHe( rhs.m_fTargetOffsetHe ),
m_fTargetOffsetSi( rhs.m_fTargetOffsetSi ),
m_szPullControlBuff( rhs.m_szPullControlBuff ),
m_bNotUseOwnerBuff( rhs.m_bNotUseOwnerBuff )
{
	m_BuffState = BS_NONE;
}

ioWindMagneticBuff::~ioWindMagneticBuff()
{
}

void ioWindMagneticBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fPullRange = rkLoader.LoadFloat_e( "pull_range", 0.0f );
	m_fPullMinRange = rkLoader.LoadFloat_e( "pull_min_range", 0.0f );
	m_fPullSpeed = rkLoader.LoadFloat_e( "pull_speed", FLOAT1 );

	rkLoader.LoadString_e( "pull_ani", "", szBuf, MAX_PATH );
	m_szPullAni = szBuf;

	rkLoader.LoadString_e( "pull_extend_ani", "", szBuf, MAX_PATH );
	m_szPullExtendAni = szBuf;

	m_PullEndType = (PullEndType)rkLoader.LoadInt_e( "pull_end_type", PET_NORMAL );

	//
	m_PullEndWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "pull_attack_index", NO_WEAPON_ATTRIBUTE );
	m_PullEndWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "pull_attack_index_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "pull_attack_wounded", "", szBuf, MAX_PATH );
	m_PullEndWeaponInfo.m_WoundedAnimation = szBuf;

	m_PullEndWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "pull_attack_wounded_time", 0 );
	m_PullEndWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "pull_attack_wounded_loop_ani", false );

	//
	m_fTargetOffsetFr = rkLoader.LoadFloat_e( "target_offset_fr", 0.0f );
	m_fTargetOffsetHe = rkLoader.LoadFloat_e( "target_offset_he", 0.0f );
	m_fTargetOffsetSi = rkLoader.LoadFloat_e( "target_offset_si", 0.0f );

	//
	rkLoader.LoadString_e( "pull_control_buff", "", szBuf, MAX_PATH );
	m_szPullControlBuff = szBuf;

	m_bNotUseOwnerBuff = rkLoader.LoadBool_e( "not_use_owner_buff", false );
}

ioBuff* ioWindMagneticBuff::Clone()
{
	return new ioWindMagneticBuff( *this );
}

void ioWindMagneticBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetPullingState();
}

bool ioWindMagneticBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	SetPullingState();

	return true;
}

void ioWindMagneticBuff::SetPullingState()
{
	m_BuffState = BS_PULLING;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szPullAni, FLOAT100 );

	pOwner->SetState( CS_WIND_MAGNETIC );
}

void ioWindMagneticBuff::SetPullEndState()
{
	m_BuffState = BS_PULL_END;

	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = GetCreator();

	if( !pOwner || !pTarget )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	DWORD dwWeaponIndex = pTarget->IncreaseWeaponIndexBase() + 1;

	m_vWeaponIndexList.push_back( dwWeaponIndex );

	ioWeapon *pWeapon = pTarget->DummyExplicit( m_PullEndWeaponInfo,
												vPos,
												dwWeaponIndex,
												false,
												m_OwnerName,
												m_Name,
												m_bUseCallAttWeaponCol );

	if( pWeapon )
	{
		D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate() - vPos;
		pWeapon->SetMoveDir( vDir );
	}
}

void ioWindMagneticBuff::SetPullExtendState()
{
	m_BuffState = BS_PULL_EXTEND;

	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = GetCreator();
	
	if( !pOwner || !pTarget )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szPullExtendAni, FLOAT100 );


	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSide;
	D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

	vPos += vDir * m_fTargetOffsetFr;
	vPos.y += m_fTargetOffsetHe;
	vPos += vSide * m_fTargetOffsetSi;

	pOwner->SetWorldPosition( vPos );
}

void ioWindMagneticBuff::SetPullExtendEndState()
{
	m_BuffState = BS_PULL_EXTEND_END;

	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = GetCreator();

	if( !pOwner || !pTarget )
		return;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	DWORD dwWeaponIndex = pTarget->IncreaseWeaponIndexBase() + 1;

	m_vWeaponIndexList.push_back( dwWeaponIndex );

	ioWeapon *pWeapon = pTarget->DummyExplicit( m_PullEndWeaponInfo,
												vPos,
												dwWeaponIndex,
												false,
												m_OwnerName,
												m_Name,
												m_bUseCallAttWeaponCol );

	if( pWeapon )
	{
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &vDir, &vDir );

		pWeapon->SetMoveDir( vDir );
	}
}

void ioWindMagneticBuff::SetEndState()
{
	m_BuffState = BS_END;

	SetReserveEndBuff();
}

void ioWindMagneticBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( pOwner->GetState() == CS_WIND_MAGNETIC )
		{
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		}
	}

	ioBuff::EndBuff();
}

void ioWindMagneticBuff::ProcessBuff( float fTimePerSec )
{
	if( !CheckValidateTarget() )
		return;

	if( !CheckBuffState( fTimePerSec ) )
		return;

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

bool ioWindMagneticBuff::IsCharCollisionSkip() const
{
	if( m_BuffState == BS_PULL_EXTEND )
		return true;

	return ioBuff::IsCharCollisionSkip();
}

bool ioWindMagneticBuff::IsCollisionAvailableWeapon() const
{
	if( m_BuffState == BS_PULL_EXTEND )
		return false;

	return ioBuff::IsCollisionAvailableWeapon();
}

bool ioWindMagneticBuff::CheckBuffState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return false;
	}

	switch( m_BuffState )
	{
	case BS_PULLING:
		if( !CheckMagneticRange() )
			return false;

		ProcessBuffMove( fTimePerSec );
		break;
	case BS_PULL_EXTEND:
		break;
	case BS_PULL_END:
	case BS_PULL_EXTEND_END:
		if( pOwner->GetState() != CS_WIND_MAGNETIC )
		{
			SetReserveEndBuff();
			return false;
		}
		break;
	case BS_END:
		break;
	}

	return true;
}

bool ioWindMagneticBuff::CheckMagneticRange()
{
	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = GetCreator();

	if( !pOwner || !pTarget )
	{
		SetReserveEndBuff();
		return false;
	}

	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSide;
	D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

	vTargetPos += vDir * m_fTargetOffsetFr;
	vTargetPos.y += m_fTargetOffsetHe;
	vTargetPos += vSide * m_fTargetOffsetSi;

	D3DXVECTOR3 vDiff = pOwner->GetMidPositionByRate() - vTargetPos;
	float fRange = D3DXVec3Length( &vDiff );

	if( COMPARE( fRange, 0.0f, m_fPullMinRange) )
	{
		switch( m_PullEndType )
		{
		case PET_NORMAL:
			SetPullEndState();	
			break;
		case PET_EXTEND:
			SetPullExtendState();
			break;
		default:
			SetReserveEndBuff();
			break;
		}

		return false;
	}
	else if( !COMPARE( fRange,m_fPullMinRange, m_fPullRange) )
	{
		SetEndState();
		return false;
	}

	return true;
}

void ioWindMagneticBuff::ProcessBuffMove( float fTimePerSec )
{
	if( m_fPullSpeed == 0.0f )
		return;

	ioBaseChar *pOwner = GetOwner();
	ioBaseChar *pTarget = GetCreator();
	
	if( !pOwner || !pTarget )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSide;
	D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

	vPos += vDir * m_fTargetOffsetFr;
	vPos.y += m_fTargetOffsetHe;
	vPos += vSide * m_fTargetOffsetSi;

	D3DXVECTOR3 vDiff = vPos - pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vMoveDir = vDiff;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	pOwner->SetTargetRotToDir( vMoveDir, false );

	//
	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = vMoveDir * m_fPullSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			pOwner->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vNewPos ) )
		pOwner->SetWorldPosition( vNewPos );
}

bool ioWindMagneticBuff::CheckValidateTarget()
{
	ioBaseChar *pTarget = GetCreator();
	if( !pTarget )
	{
		SetReserveEndBuff();
		return false;
	}

	bool bTargetValidate = true;

	if( !pTarget->CheckGrapplingExceptionState() )
		bTargetValidate = false;

	if ( !m_bNotUseOwnerBuff )
	{
		if( !m_szPullControlBuff.IsEmpty() && !pTarget->HasBuff( m_szPullControlBuff ) )
			bTargetValidate = false;
	}

	if( !bTargetValidate )
	{
		if( m_BuffState == BS_PULL_EXTEND )
		{
			SetPullExtendEndState();
			return true;
		}
		else
		{
			SetReserveEndBuff();
			return false;
		}
	}

	return true;
}



