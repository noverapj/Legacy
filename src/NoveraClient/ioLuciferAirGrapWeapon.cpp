

#include "stdafx.h"

#include "ioLuciferAirGrapWeapon.h"

#include "ioBaseChar.h"

#include "SkillDefine.h"

ActionStopType ioLuciferAirGrapWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioLuciferAirGrapWeapon::m_DieActionStopType = AST_NONE;

ioLuciferAirGrapWeapon::ioLuciferAirGrapWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	m_UpdateState = CONTACT_CREATED;
	m_bFirstCheck = true;
	m_bDeadWait = false;
}

ioLuciferAirGrapWeapon::~ioLuciferAirGrapWeapon()
{
}

void ioLuciferAirGrapWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_bDisableOwnerScale = rkLoader.LoadBool_e( "disable_owner_scale", false );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
}

void ioLuciferAirGrapWeapon::SetStartCollisionBox()
{
	ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( m_iStructIndex );
	if( pPush )
	{
		m_bStructWeapon = true;
	}

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
	if( pDummy )
	{
		m_bDummyCharWeapon = true;
	}

	ioMachineStruct *pMachine = g_MachineStructMgr.FindMachineStruct( m_iMachineIndex );
	if( pMachine )
	{
		m_bMachineWeapon = true;
	}

	if( m_bStructWeapon )
		StartCollisionBoxByStruct();
	else if( m_bDummyCharWeapon )
		StartCollisionBoxByDummyChar();
	else if( m_bMachineWeapon )
		StartCollisionBoxByMachine();
	else
		StartCollisionBoxByOwner();
}

void ioLuciferAirGrapWeapon::StartCollisionBoxByOwner()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_WorldAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType, m_bDisableOwnerScale );
	}
	else
	{
		m_WorldAttackBox.SetNull();
		LOG.PrintTimeAndLog( 0, "ioLuciferAirGrapWeapon - %s Not Exist", m_OwnerName.c_str() );
	}

	m_UpdateState = CONTACT_CREATED;
}

void ioLuciferAirGrapWeapon::StartCollisionBoxByStruct()
{
	ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( m_iStructIndex );
	if( pPush )
	{
		m_WorldAttackBox = pPush->GetAttackCollisionBox( m_CollisionType );
	}
	else
	{
		m_WorldAttackBox.SetNull();
		LOG.PrintTimeAndLog( 0, "ioLuciferAirGrapWeapon - Struct Not Exist" );
	}

	m_UpdateState = CONTACT_CREATED;
}

void ioLuciferAirGrapWeapon::StartCollisionBoxByDummyChar()
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
	if( pDummy )
	{
		m_WorldAttackBox = pDummy->GetAttackCollisionBox( m_CollisionType );
	}
	else
	{
		m_WorldAttackBox.SetNull();
		LOG.PrintTimeAndLog( 0, "ioLuciferAirGrapWeapon - DummyChar Not Exist" );
	}

	m_UpdateState = CONTACT_CREATED;
}

void ioLuciferAirGrapWeapon::GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList )
{
	vBoxList.reserve( 3 );
	vBoxList.push_back( m_PreAttackBox );
	vBoxList.push_back( m_MidAttackBox );
	vBoxList.push_back( m_WorldAttackBox );
}

bool ioLuciferAirGrapWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );

	ioBaseChar * pChar = ToBaseChar( pEntity );
	if( !pChar )
		return false;

	if( pChar->GetState() == CS_GRAPPLING_WOUNDED || pChar->GetState() == CS_GRAPPLING_WOUNDED_ITEM || pChar->GetState() == CS_GRAPPLING_WOUNDED_BUFF )
		//|| pChar->GetState() == CS_BLOW_WOUNDED || pChar->GetState() == CS_BOUND_BLOW_WOUNDED )
		return false;

	if( m_UpdateState == CONTACT_FIRST )
	{
		if( pEntity->IsWoundedCollision( m_PreAttackBox, &vColPoint ) )
		{
			return CheckCollisionLine( pEntity, vColPoint );
		}
	}

	if( pEntity->IsWoundedCollision( m_MidAttackBox, &vColPoint ) )
	{
		return CheckCollisionLine( pEntity, vColPoint );
	}

	if( pEntity->IsWoundedCollision( m_WorldAttackBox, &vColPoint ) )
	{
		return CheckCollisionLine( pEntity, vColPoint );
	}

	return false;
}

bool ioLuciferAirGrapWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

ioWeapon::WeaponType ioLuciferAirGrapWeapon::GetType() const
{
	return WT_LUCIFER_AIR_GRAP;
}

ActionStopType ioLuciferAirGrapWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioLuciferAirGrapWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioLuciferAirGrapWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( m_bStructWeapon )
		UpdateAttackBoxByStruct( fTimePerSec, bSetNull );
	else if( m_bDummyCharWeapon )
		UpdateAttackBoxByDummyChar( fTimePerSec, bSetNull );
	else if( m_bMachineWeapon )
		UpdateAttackBoxByMachine( fTimePerSec, bSetNull );
	else
		UpdateAttackBoxByOwner( fTimePerSec, bSetNull );
}

void ioLuciferAirGrapWeapon::UpdateAttackBoxByOwner( float fTimePerSec, bool bSetNull )
{
	ioBaseChar *pOwner = GetOwner();

	if( bSetNull || !pOwner || !IsLive() )
	{
		m_PreAttackBox.SetNull();
		m_MidAttackBox.SetNull();
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	ioOrientBox eCharAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType, m_bDisableOwnerScale );

	m_PreAttackBox = m_WorldAttackBox;

	DWORD dwLooseTime = 0;
	if( pOwner )
	{
		dwLooseTime = pOwner->GetActionStopCurAdd();
	}

	float fFrameGap = fTimePerSec * FLOAT1000;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveEndTime = m_dwCreatedTime + m_dwLiveTime + dwLooseTime;
	DWORD dwFrameEndTime = dwCurTime + (DWORD)fFrameGap;

	float fPreTime = (float)dwCurTime - fFrameGap;
	float fEndGap = (float)dwLiveEndTime - fPreTime;

	D3DXVECTOR3 vAxis;

	if( dwLiveEndTime >= dwFrameEndTime )
	{
		m_WorldAttackBox = eCharAttackBox;
	}
	else	// 현재 프레임내에서 공격이 끝난다.
	{
		float fLerpRate = 0.0f;
		if( m_dwCreatedTime >= dwCurTime )	// 현재 프레임에서 생성되었다. 1프레임 이내의 공격.
		{
			fLerpRate = (float)(m_dwLiveTime+dwLooseTime) / (float)( dwFrameEndTime - m_dwCreatedTime );
		}
		else	// 이전프레임에서 생성되었다.
		{
			fLerpRate = fEndGap / (2 * fFrameGap);
		}

		fLerpRate = max( 0.0f, min( FLOAT1, fLerpRate ) );

		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &eCharAttackBox.GetAxis(i), fLerpRate );
			D3DXVec3Normalize( &vAxis, &vAxis );
			m_WorldAttackBox.SetAxis( i, vAxis );
		}

		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &eCharAttackBox.GetCenter(), fLerpRate );
		m_WorldAttackBox.SetCenter( vAxis );
	}

	for( int i=0 ; i<3 ; i++ )
	{
		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &m_WorldAttackBox.GetAxis(i), FLOAT05 );
		D3DXVec3Normalize( &vAxis, &vAxis );
		m_MidAttackBox.SetAxis( i, vAxis );
		m_MidAttackBox.SetExtents( i, m_WorldAttackBox.GetExtents(i) );
	}

	D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &m_WorldAttackBox.GetCenter(), FLOAT05 );
	m_MidAttackBox.SetCenter( vAxis );


	if( m_UpdateState == CONTACT_CREATED )
		m_UpdateState = CONTACT_FIRST;
	else
		m_UpdateState = CONTACT_NORMAL;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		for( int i=0 ; i<3 ; i++ )
		{
			m_PreAttackBox.SetExtents( i, eCharAttackBox.GetExtents(i) * m_fOwnerChargeScaleRate );
			m_MidAttackBox.SetExtents( i, eCharAttackBox.GetExtents(i) * m_fOwnerChargeScaleRate );
			m_WorldAttackBox.SetExtents( i, eCharAttackBox.GetExtents(i) * m_fOwnerChargeScaleRate );
		}
	}

	m_WorldAttackBoxForMap = m_WorldAttackBox;

	if( m_bFirstCheck )
	{
		m_bFirstCheck = false;
	}
}

void ioLuciferAirGrapWeapon::UpdateAttackBoxByStruct( float fTimePerSec, bool bSetNull )
{
	ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( m_iStructIndex );

	if( bSetNull || !pPush || !IsLive() )
	{
		m_PreAttackBox.SetNull();
		m_MidAttackBox.SetNull();
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_PreAttackBox = m_WorldAttackBox;

	DWORD dwLooseTime = 0;

	float fFrameGap = fTimePerSec * FLOAT1000;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveEndTime = m_dwCreatedTime + m_dwLiveTime + dwLooseTime;
	DWORD dwFrameEndTime = dwCurTime + (DWORD)fFrameGap;

	float fPreTime = (float)dwCurTime - fFrameGap;
	float fEndGap = (float)dwLiveEndTime - fPreTime;

	D3DXVECTOR3 vAxis;

	if( dwLiveEndTime >= dwFrameEndTime )
	{
		m_WorldAttackBox = pPush->GetAttackCollisionBox( m_CollisionType );
	}
	else	// 현재 프레임내에서 공격이 끝난다.
	{
		float fLerpRate = 0.0f;
		if( m_dwCreatedTime >= dwCurTime )	// 현재 프레임에서 생성되었다. 1프레임 이내의 공격.
		{
			fLerpRate = (float)(m_dwLiveTime+dwLooseTime) / (float)( dwFrameEndTime - m_dwCreatedTime );
		}
		else	// 이전프레임에서 생성되었다.
		{
			fLerpRate = fEndGap / (2 * fFrameGap);
		}

		fLerpRate = max( 0.0f, min( FLOAT1, fLerpRate ) );

		ioOrientBox kCurAttackBox = pPush->GetAttackCollisionBox( m_CollisionType );

		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &kCurAttackBox.GetAxis(i), fLerpRate );
			D3DXVec3Normalize( &vAxis, &vAxis );
			m_WorldAttackBox.SetAxis( i, vAxis );
		}

		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &kCurAttackBox.GetCenter(), fLerpRate );
		m_WorldAttackBox.SetCenter( vAxis );
	}

	for( int i=0 ; i<3 ; i++ )
	{
		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &m_WorldAttackBox.GetAxis(i), FLOAT05 );
		D3DXVec3Normalize( &vAxis, &vAxis );
		m_MidAttackBox.SetAxis( i, vAxis );
		m_MidAttackBox.SetExtents( i, m_WorldAttackBox.GetExtents(i) );
	}

	D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &m_WorldAttackBox.GetCenter(), FLOAT05 );
	m_MidAttackBox.SetCenter( vAxis );

	if( m_UpdateState == CONTACT_CREATED )
		m_UpdateState = CONTACT_FIRST;
	else
		m_UpdateState = CONTACT_NORMAL;

	m_WorldAttackBoxForMap = m_WorldAttackBox;

	if( m_bFirstCheck )
	{
		m_bFirstCheck = false;
	}
}

void ioLuciferAirGrapWeapon::UpdateAttackBoxByDummyChar( float fTimePerSec, bool bSetNull )
{
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );

	if( bSetNull || !pDummy || !IsLive() )
	{
		m_PreAttackBox.SetNull();
		m_MidAttackBox.SetNull();
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_PreAttackBox = m_WorldAttackBox;

	DWORD dwLooseTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveEndTime = m_dwCreatedTime + m_dwLiveTime + dwLooseTime;
	DWORD dwFrameEndTime = dwCurTime + (DWORD)( fTimePerSec * FLOAT1000 );
	D3DXVECTOR3 vAxis;

	if( dwLiveEndTime >= dwFrameEndTime )
	{
		m_WorldAttackBox = pDummy->GetAttackCollisionBox( m_CollisionType );
	}
	else	// 현재 프레임내에서 공격이 끝난다.
	{
		float fLerpRate = 0.0f;
		if( m_dwCreatedTime >= dwCurTime )	// 현재 프레임에서 생성되었다. 1프레임 이내의 공격.
		{
			fLerpRate = (float)(m_dwLiveTime+dwLooseTime) / (float)( dwFrameEndTime - m_dwCreatedTime );
		}
		else	// 이전프레임에서 생성되었다.
		{
			fLerpRate = (float)(dwLiveEndTime - dwCurTime)/(float)( dwFrameEndTime - dwCurTime );
		}

		fLerpRate = max( 0.0f, min( FLOAT1, fLerpRate ) );

		ioOrientBox kCurAttackBox = pDummy->GetAttackCollisionBox( m_CollisionType );

		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &kCurAttackBox.GetAxis(i), fLerpRate );
			D3DXVec3Normalize( &vAxis, &vAxis );
			m_WorldAttackBox.SetAxis( i, vAxis );
		}

		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &kCurAttackBox.GetCenter(), fLerpRate );
		m_WorldAttackBox.SetCenter( vAxis );
	}

	for( int i=0 ; i<3 ; i++ )
	{
		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &m_WorldAttackBox.GetAxis(i), FLOAT05 );
		D3DXVec3Normalize( &vAxis, &vAxis );
		m_MidAttackBox.SetAxis( i, vAxis );
		m_MidAttackBox.SetExtents( i, m_WorldAttackBox.GetExtents(i) );
	}

	D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &m_WorldAttackBox.GetCenter(), FLOAT05 );
	m_MidAttackBox.SetCenter( vAxis );

	if( m_UpdateState == CONTACT_CREATED )
		m_UpdateState = CONTACT_FIRST;
	else
		m_UpdateState = CONTACT_NORMAL;

	m_WorldAttackBoxForMap = m_WorldAttackBox;

	if( m_bFirstCheck )
	{
		m_bFirstCheck = false;
	}
}

void ioLuciferAirGrapWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime > dwCurTime )
	{
		LOG.PrintTimeAndLog( 0, "ioLuciferAirGrapWeapon::Process - Create Time Error(%d/%d)", m_dwCreatedTime, dwCurTime );
		SetWeaponDead();
		return;
	}

	if( m_bDeadWait )
	{
		SetWeaponDead();
		return;
	}

	if( !CheckPiercing() )
		return;

	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopCurAdd();
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < dwCurTime )
	{
		m_bDeadWait = true;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioLuciferAirGrapWeapon::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
	{
		m_dwLiveTime = dwTime;
	}
}

void ioLuciferAirGrapWeapon::StartCollisionBoxByMachine()
{
	ioMachineStruct *pMachine = g_MachineStructMgr.FindMachineStruct( m_iMachineIndex );
	if( pMachine )
	{
		m_WorldAttackBox = pMachine->GetAttackCollisionBox( m_CollisionType );
	}
	else
	{
		m_WorldAttackBox.SetNull();
		LOG.PrintTimeAndLog( 0, "ioLuciferAirGrapWeapon - Machine Not Exist" );
	}

	m_UpdateState = CONTACT_CREATED;
}

void ioLuciferAirGrapWeapon::UpdateAttackBoxByMachine( float fTimePerSec, bool bSetNull )
{
	ioMachineStruct *pMachine = g_MachineStructMgr.FindMachineStruct( m_iMachineIndex );

	if( bSetNull || !pMachine || !IsLive() )
	{
		m_PreAttackBox.SetNull();
		m_MidAttackBox.SetNull();
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_PreAttackBox = m_WorldAttackBox;

	DWORD dwLooseTime = 0;

	float fFrameGap = fTimePerSec * FLOAT1000;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveEndTime = m_dwCreatedTime + m_dwLiveTime + dwLooseTime;
	DWORD dwFrameEndTime = dwCurTime + (DWORD)fFrameGap;

	float fPreTime = (float)dwCurTime - fFrameGap;
	float fEndGap = (float)dwLiveEndTime - fPreTime;

	D3DXVECTOR3 vAxis;

	if( dwLiveEndTime >= dwFrameEndTime )
	{
		m_WorldAttackBox = pMachine->GetAttackCollisionBox( m_CollisionType );
	}
	else	// 현재 프레임내에서 공격이 끝난다.
	{
		float fLerpRate = 0.0f;
		if( m_dwCreatedTime >= dwCurTime )	// 현재 프레임에서 생성되었다. 1프레임 이내의 공격.
		{
			fLerpRate = (float)(m_dwLiveTime+dwLooseTime) / (float)( dwFrameEndTime - m_dwCreatedTime );
		}
		else	// 이전프레임에서 생성되었다.
		{
			fLerpRate = fEndGap / (2 * fFrameGap);
		}

		fLerpRate = max( 0.0f, min( FLOAT1, fLerpRate ) );

		ioOrientBox kCurAttackBox = pMachine->GetAttackCollisionBox( m_CollisionType );

		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &kCurAttackBox.GetAxis(i), fLerpRate );
			D3DXVec3Normalize( &vAxis, &vAxis );
			m_WorldAttackBox.SetAxis( i, vAxis );
		}

		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &kCurAttackBox.GetCenter(), fLerpRate );
		m_WorldAttackBox.SetCenter( vAxis );
	}

	for( int i=0 ; i<3 ; i++ )
	{
		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &m_WorldAttackBox.GetAxis(i), FLOAT05 );
		D3DXVec3Normalize( &vAxis, &vAxis );
		m_MidAttackBox.SetAxis( i, vAxis );
		m_MidAttackBox.SetExtents( i, m_WorldAttackBox.GetExtents(i) );
	}

	D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &m_WorldAttackBox.GetCenter(), FLOAT05 );
	m_MidAttackBox.SetCenter( vAxis );

	if( m_UpdateState == CONTACT_CREATED )
		m_UpdateState = CONTACT_FIRST;
	else
		m_UpdateState = CONTACT_NORMAL;

	m_WorldAttackBoxForMap = m_WorldAttackBox;

	if( m_bFirstCheck )
	{
		m_bFirstCheck = false;
	}
}