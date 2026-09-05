

#include "stdafx.h"

#include "ioSmartBombWeapon.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

ActionStopType ioSmartBombWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSmartBombWeapon::m_DieActionStopType = AST_NONE;

ioSmartBombWeapon::ioSmartBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION | WP_ONE_TIME_USE );

	m_pFireMesh = NULL;

	m_fCurMoveRange = 0.0f;
	m_fDownSpeed = 0.0f;
	m_State = TS_MOVE;

	m_bWounedWaeponFire = false;
	m_iCurPingPongCnt = 0;


	m_bCharProtectedCollision = false;

	m_bArleadyCallWeapon = false;

	m_dwCallAttrStartTime = 0;
}

ioSmartBombWeapon::~ioSmartBombWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioSmartBombWeapon::~ioSmartBombWeapon - UnReleased Resource" );
	}
}

bool ioSmartBombWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();		
		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioSmartBombWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;
		
	m_fWoundedFireRange = rkLoader.LoadFloat_e( "wounded_waepon_range", 0.0f );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwExplosionTime = rkLoader.LoadInt_e( "explosion_time", 0 );

	m_fPingPongPower = rkLoader.LoadFloat_e( "ping_pong_power", 400.0f );
	m_fCurPingPongPower = m_fPingPongPower;
	m_iMaxPingPongCnt = rkLoader.LoadFloat_e( "max_ping_pong_cnt", 0 );

	LoadCallAttributeList( rkLoader );
}

void ioSmartBombWeapon::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

void ioSmartBombWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXVec3Cross( &m_vRollAxis,
		&D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
		&m_vMoveDir );
}

bool ioSmartBombWeapon::SetFireMesh( const ioHashString &rkMeshName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( rkMeshName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();

	m_fExtentsOffSet = m_LocalAttackBox.GetExtents( 1 );

	AddEffectEvent( m_pFireMesh );

	return true;
}

void ioSmartBombWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_AreaWeaponName.IsEmpty() &&
		m_dwCreatedTime+m_dwAttachAreaWeaponTime < FRAMEGETTIME() )
	{
		CreateAttachAreaWeapon();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_State )
	{
	case TS_MOVE:
		OnMoveState( fTimePerSec, pStage );	
		break;
	case TS_DOWN:
		OnDownState( fTimePerSec, pStage );		
		break;
	case TS_PINGPONG:		
		OnPingPongState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION_WAIT:
		if( 0 < m_dwExplosionStart && m_dwExplosionStart + m_dwExplosionTime < dwCurTime )
		{				
			CallAttribute();
			SetWeaponDead();
		}
		if( m_pFireMesh )
		{
			m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
		}
		
		break;
	case TS_EXPLOSION:		
		OnWaeponDead( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
	RollWeapon( fTimePerSec );
}

void ioSmartBombWeapon::SetState( ThrowState eState )
{
	m_State = eState;

	switch( eState )
	{
	case TS_DOWN:
		m_fDownSpeed = 0.0f;
		break;
	}
}

void ioSmartBombWeapon::OnMoveState(  float fTimePerSec, ioPlayStage *pStage )
{
	float fMoveDist = GetAttribute()->m_fMoveSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetAttribute()->m_fMoveRange;

	if( m_bWounedWaeponFire )
		fMaxRange = m_fWoundedFireRange;

	if( m_fCurMoveRange >= fMaxRange )
	{	
		SetState( TS_DOWN );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
	UpdateAttachAreaWeapon( pStage );
}

void ioSmartBombWeapon::OnDownState( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos = GetPosition();
	vPos += m_vMoveDir * ( GetAttribute()->m_fMoveSpeed * fTimePerSec );

	m_fDownSpeed += pStage->GetRoomGravity() * fTimePerSec;
	vPos.y -= m_fDownSpeed * fTimePerSec;
	SetPosition( vPos );
	
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		SetState(TS_PINGPONG);
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
	UpdateAttachAreaWeapon( pStage );
}

void ioSmartBombWeapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vMove = m_vMoveDir * ( GetAttribute()->m_fMoveSpeed * fTimePerSec * 0.3f );
	float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;

	vMove.y += m_fCurPingPongPower * fTimePerSec;
	m_fCurPingPongPower -= fCurGravity;
	GetParentSceneNode()->Translate( vMove );

	D3DXVECTOR3 vPos = GetPosition();

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		SetPosition( vPos );				
		OnExplosionState( fTimePerSec, pStage );		
	}
	else
	{
		SetPosition( vPos );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
	UpdateAttachAreaWeapon( pStage );
}

void ioSmartBombWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
{	
	if( m_State == TS_EXPLOSION_WAIT || m_State == TS_EXPLOSION )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwExplosionStart = dwCurTime;

	m_State = TS_EXPLOSION_WAIT;
}

void ioSmartBombWeapon::OnWaeponDead( float fTimePerSec, ioPlayStage *pStage )
{	
	DWORD dwCurTime = FRAMEGETTIME();
	if( 0 < m_dwCallAttrStartTime && m_dwCallAttrStartTime + 100 < dwCurTime)
	{	
		if( m_pFireMesh )
		{
			m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
		}
		SetWeaponDead();
	}
}

void ioSmartBombWeapon::CallAttribute()
{	
	if( m_bArleadyCallWeapon )	return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;	
	
	DWORD dwCurGenerateTime;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwCallAttrStartTime = dwCurTime;
	dwCurGenerateTime = dwCurTime;

	D3DXVECTOR3	vPos = GetPosition();

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
											  pInfo,
											  vPos,
											  m_vMoveDir,
											  dwCurGenerateTime + pInfo.m_dwCallTime,
											  m_dwWeaponIdx );		
	}
	m_bArleadyCallWeapon = true;
}


float ioSmartBombWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioSmartBombWeapon::RollWeapon( float fTimePerSec )
{	
	if( m_State == TS_EXPLOSION_WAIT || m_State == TS_EXPLOSION )
		return;

	if( m_pFireMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}
}

void ioSmartBombWeapon::SetCurMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

void ioSmartBombWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioSmartBombWeapon::GetType() const
{
	return WT_SMART_BOAM_WEAPON;
}

ActionStopType ioSmartBombWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSmartBombWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioSmartBombWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioSmartBombWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioSmartBombWeapon::SetWoundedFire(bool bWounded)
{
	m_bWounedWaeponFire = bWounded;
}

void ioSmartBombWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
			ioMath::UNIT_ALL,
			GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
			ioMath::UNIT_ALL,
			GetParentSceneNode()->GetDerivedOrientation() );
	}

}

void ioSmartBombWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	if( m_State == TS_EXPLOSION_WAIT || m_State == TS_EXPLOSION)
		return;

	if( m_iMaxPingPongCnt > 0 && m_iCurPingPongCnt >= m_iMaxPingPongCnt )
		return;

	D3DXVECTOR3 vMoveDir = m_vMoveDir;
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fDotValue = D3DXVec3Dot( &vMoveDir, &vFaceNrm );
	if( fDotValue > 0.1f ) return;

	D3DXVECTOR3 vReflect = vMoveDir - 2.0f * fDotValue * vFaceNrm;
	vReflect.y = 0.0f;
	D3DXVec3Normalize( &vReflect, &vReflect );

	SetCurMoveDir( vReflect );

	m_State = TS_PINGPONG;

	m_iCurPingPongCnt++;

	if( m_iMaxPingPongCnt > 0 )
		m_fCurPingPongPower = m_fPingPongPower / m_iCurPingPongCnt;
}

bool ioSmartBombWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State == TS_EXPLOSION )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	SetWallCollision( vNormal );
	return true;
}

void ioSmartBombWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{	
	if( m_State == TS_EXPLOSION_WAIT || m_State == TS_EXPLOSION )
		return;
	
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( pTargetChar && pTargetChar->GetCharName() == GetOwnerName() )
		return;

	if( pTargetChar && pTargetChar->GetTeam() == GetTeam() )
		return;

	if( m_State == TS_PINGPONG && m_bObjectCollision )
		return;

	const ioOrientBox &kBombBox = GetWorldAttackBox();
	const ioOrientBox &kWoundedBox = pTarget->GetWorldCollisionBox();

	if( !pTarget->IsWoundedCollision( kBombBox ) )
		return;
	
	bool bCollision = false;

	bool bDefenceModeNpc = false;
	ioNpcChar *pNpc = ToNpcChar( pTarget );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )	
	{		
		bDefenceModeNpc = true;
	}

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) || 
		ToDummyChar(pTarget) || ToWallZone(pTarget) || bDefenceModeNpc )
	{
		m_bObjectCollision = true;
		
		D3DXVECTOR3 vDiff = kBombBox.GetCenter() - kWoundedBox.GetCenter();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		SetWallCollision( vDiff );
		return;
	}
	
	bool bCharCollision = false;

	//무적 체크
	if( pTargetChar && pTargetChar->IsProtectState() )
		bCharCollision = true;

	if( !bCharCollision )
		return;

	D3DXVECTOR3 vDiff = kBombBox.GetCenter() - kWoundedBox.GetCenter();
	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );

	SetWallCollision( vDiff );
	m_bCharProtectedCollision = true;

}

bool ioSmartBombWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{	
	if( m_State == TS_EXPLOSION_WAIT || m_State == TS_EXPLOSION )
		return false;

	if( !pEntity )
		return false;

	if( !ToBaseChar(pEntity) )
		return false;

	ioNpcChar *pNpc = ToNpcChar( pEntity );
	if( pNpc && COMPARE( pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )
		return false;
	
	if( m_bCharProtectedCollision )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}


bool ioSmartBombWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION_WAIT )
		return true;

	return false;
}

DWORD ioSmartBombWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION_WAIT )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioSmartBombWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}
