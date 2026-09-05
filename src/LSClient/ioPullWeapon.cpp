

#include "stdafx.h"

#include "ioPullWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ActionStopType ioPullWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioPullWeapon::m_DieActionStopType = AST_NONE;

ioPullWeapon::ioPullWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	ClearData();	
}

ioPullWeapon::~ioPullWeapon()
{
}

void ioPullWeapon::ClearData()
{
	m_fZoneRange		= 0.0f;
	m_fZoneInnerRange	= 0.0f;
	m_fZoneAngle		= 0.0f;

	m_fCurMoveRange = 0.0f;

	m_vAttackDir = ioMath::VEC3_ZERO;
	m_vCallStartPosition = ioMath::VEC3_ZERO;
	m_vCollisionDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_szTargetsName.clear();

	m_bSetOwnerOrderWeaponDead = true;
	m_bArleadyCallWeapon = false;

	m_State = WS_INIT;

	m_dwStartTime = 0;

	m_dwPreStartTime = 0;
	m_dwPreDuration = 0;
}

void ioPullWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "zone_live_time", 0 );

	m_fZoneRange		= rkLoader.LoadFloat_e( "zone_range", 0.0f );
	m_fZoneMinRange		= rkLoader.LoadFloat_e( "zone_min_range", 0.0f );
	m_fZoneMaxRange		= rkLoader.LoadFloat_e( "zone_max_range", 0.0f );
	m_fZoneInnerRange	= rkLoader.LoadFloat_e( "zone_inner_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "zone_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "zone_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "zone_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "zone_width", 0.0f );
	m_fMaxWidth		= rkLoader.LoadFloat_e( "zone_max_width", 0.0f );

	m_fOffSet		= rkLoader.LoadFloat_e( "zone_offset", 0.0f );
	m_fHeightOffSet	= rkLoader.LoadFloat_e( "zone_height_offset", 0.0f );

	m_bDisableColGrap	= rkLoader.LoadBool_e( "zone_collision_disable_grap", 0 );
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "zone_wound_type", 0 );

	m_bEnableProtectCol = rkLoader.LoadBool_e( "enable_protect_col", false );

	// 끝까지 끌려간 후 걸어줄 버프
	int iCnt = rkLoader.LoadInt_e( "target_wound_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i)
	{
		wsprintf_e( szKey, "target_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szTargetWoundBuff.push_back( szBuf );
	}
	
	m_dwPullCnt = rkLoader.LoadInt_e("pull_cnt", 1 );

	//Call Attribute
	int iAttackTypeCnt = rkLoader.LoadInt_e( "call_weapon_type_count", 0 );

	m_CallWeaponInfoList.clear();

	if( 0 < iAttackTypeCnt )
		m_CallWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "call_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_time", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kInfo );
	}

	rkLoader.LoadString_e( "remove_skip_stun_buff_name", "", szBuf, MAX_PATH );
	m_szRemoveSkipStunBuffName = szBuf;
}

void ioPullWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_vStartOffSet = vPos - pOwner->GetWorldPosition();
	}
	else
	{
		m_vStartOffSet = ioMath::VEC3_ZERO;
	}

	SetPosition( vPos );
	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );
}

void ioPullWeapon::SetCollisionDir( D3DXVECTOR3 &vDir )
{
	m_vCollisionDir = vDir;
}

void ioPullWeapon::SetCallStartPosition( const D3DXVECTOR3& vPos )
{
	m_vCallStartPosition = vPos;
}

void ioPullWeapon::SetCollisionWoundType( int iWoundType )
{
	m_TargetWoundType = (TargetWoundType)iWoundType;
}

D3DXVECTOR3 ioPullWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( m_vAttackDir == ioMath::VEC3_ZERO )
		return ioWeapon::GetAttackDir( pEntity );
	else
		return m_vAttackDir;
}

void ioPullWeapon::SetAttackDir(  D3DXVECTOR3 &vDir )
{
	m_vAttackDir = vDir;
}

ioWeapon::WeaponType ioPullWeapon::GetType() const
{
	return WT_PULL_WEAPON;
}

ActionStopType ioPullWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioPullWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioPullWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case WS_INIT:
		{	
			if( 0 < m_dwPreStartTime && m_dwPreStartTime + m_dwPreDuration < dwCurTime )
			{
				m_State = WS_MOVE;
				m_dwStartTime = dwCurTime;
			}
		}
		break;
	case WS_MOVE:
		{
			if( fTimePerSec == 0.0f )
				fTimePerSec = 0.001f;

			float fMaxRange = GetAttribute()->m_fMoveRange;
			float fMoveDist = GetAttribute()->m_fMoveSpeed * fTimePerSec;

			D3DXVECTOR3 vMove = m_vMoveDir * fMoveDist;
						
			m_fCurMoveRange += fMoveDist;

			if( m_fCurMoveRange < fMaxRange )
			{
				GetParentSceneNode()->Translate( vMove );
				int iCnt = m_szTargetsName.size();

				for( int i=0; i< iCnt; ++i )
				{
					ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetsName[i] );

					if( pTarget && pTarget->GetState() == CS_SKIP_STUN )
					{
						pTarget->Translate( vMove );
					}					
				}			
				UpdateWorldAttackBox( fTimePerSec, false );				
			}
			else
			{
				m_State = WS_WAIT;
			}
		}
		break;
	case WS_WAIT:
		{
			if( !m_bSetOwnerOrderWeaponDead )
			{
				SetWeaponDead();
				SetTargetBuff( pStage );
			}
		}
		break;
	}

	CallAttribute();
}

void ioPullWeapon::SetTargetBuff( ioPlayStage* pStage )
{
	if( !pStage )
		return;

	int iCharCnt = (int)m_szTargetsName.size();
	for( int i=0; i < iCharCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetsName[i] );
		if( !pTarget ) continue;

		int iBuffCnt = (int)m_szTargetWoundBuff.size();
		for( int j=0; j < iBuffCnt; ++j )
		{
			pTarget->AddNewBuff( m_szTargetWoundBuff[j], GetOwner()->GetCharName(), "", NULL );
		}
	}
}

bool ioPullWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{	
	if( m_State == WS_INIT )
		return false;

	if( !pEntity )
		return false;

	if( !ToBaseChar(pEntity) )
		return false;

	if( !ToBaseChar(pEntity)->IsNeedProcess() )
		return false;

	bool bCollision = CheckCollisionBox( pEntity );
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	
	if( bCollision && pEntity->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( !pTarget ) return false;

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pTarget->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( m_bDisableColGrap && pTarget->IsApplyDownState() ||
				( pTarget->GetState() == CS_GRAPPLING_WOUNDED ||
				pTarget->GetState() == CS_GRAPPLING_PUSHED ||
				pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
				pTarget->GetState() == CS_GRAPPLING_WOUNDED_SWING ||
				pTarget->GetState() == CS_GRAPPLING_WOUNDED_ITEM ||
				pTarget->GetState() == CS_GRAPPLING_WOUNDED_BUFF ) )
			{
				return false;
			}
			else if( pTarget->IsApplyDownState() )
			{
				return false;
			}

		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pTarget->IsFloating() )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( !pTarget->IsApplyDownState() || pTarget->IsFloating() )
			{
				return false;
			}
		}
	}
	
	if( bCollision )
	{
		bCollision = CheckCollisionLine( pEntity, pEntity->GetWorldPosition() );
		if( bCollision )
		{
			if( m_szTargetsName.size() < m_dwPullCnt )
			{
				DWORD dwCurTime = FRAMEGETTIME();

				m_szTargetsName.push_back( pTarget->GetCharName() );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << SS_COLLISION;
					kPacket << pTarget->GetCharName();
					P2PNetwork::SendToAllPlayingUser( kPacket );				
				}

				return true;
			}		
		}
	}

	return false;
}

bool ioPullWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	DWORD dwCurTime = FRAMEGETTIME();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	vCenter += m_vCollisionDir * m_fZoneRange * FLOAT05;
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenter -= vOffset;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

 	// 유저 방향으로 사각형 회전
	kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
 
 	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
 	kBuffBox.SetCenter( vCenter );
#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	if( !pEntity )
		return false;

	if( !pEntity->IsWoundedCollision( kBuffBox ) )
		return false;

	return true;
}

void ioPullWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		m_fCurRange = 0.0f;
		m_fCurWidth = 0.0f;
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
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
	}
}

const ioOrientBox& ioPullWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioPullWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return true;
	}

	return false;
}

void ioPullWeapon::SetWeaponDead()
{
	m_bLive = false;
	ClearData();
}

bool ioPullWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	D3DXVECTOR3 vPos = GetPosition();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vPos, vColPoint, true );

	return false;
}

float ioPullWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioPullWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioPullWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioPullWeapon::CallAttribute()
{
	DWORD dwCurTime = FRAMEGETTIME();	
	DWORD dwCurGenerateTime = dwCurTime;

	if( m_bArleadyCallWeapon ) return;
	if( 0 == m_dwStartTime || dwCurTime < m_dwStartTime + 50 ) return;	
	if( m_CallWeaponInfoList.empty() ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;
		
	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
			pInfo,
			m_vCallStartPosition,
			m_vMoveDir,
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );

	}
	m_bArleadyCallWeapon = true;	
}

ioHashStringVec ioPullWeapon::GetTargetList()
{
	return m_szTargetsName;
}

bool ioPullWeapon::isMoveComplete()
{	
	if( m_State == WS_WAIT )
		return true;
	else
		return false;
}


void ioPullWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SS_COLLISION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
			{				
				m_szTargetsName.push_back( szTargetName );	
			}		
		}
		break;
	}
}

void ioPullWeapon::ApplyExtraReserveWeaponByWeaponInfo( ioWeapon* pWeapon )
{
	if( !pWeapon )
		return;

	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		pZone->SetWeaponByDummyChar( m_iDummyIndex );
		pZone->SetZoneValue( false, false );
		pZone->SetCollisionDir( D3DXVECTOR3(0, FLOAT1, 0 ) );
		
		ioHashStringVec pList = GetTargetList();
		ioHashStringVec::iterator iter = pList.begin();

		for( ; iter != pList.end(); ++iter )
		{
			pZone->SetIgnoreCollsionChar( (*iter) );
		}
	}
}

void ioPullWeapon::SetWeaponPreInfo( DWORD dwPreStartTime, DWORD dwDuration )
{
	m_dwPreStartTime = dwPreStartTime;
	m_dwPreDuration = dwDuration;
}