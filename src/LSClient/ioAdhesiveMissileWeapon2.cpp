

#include "stdafx.h"

#include "ioAdhesiveMissileWeapon2.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioAdhesiveMissileWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioAdhesiveMissileWeapon2::m_DieActionStopType = AST_NONE;

ioAdhesiveMissileWeapon2::ioAdhesiveMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	ClearData();

	m_pFireBall = NULL;
	m_MissileState = MS_MOVE;

	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	
	m_bSetGraceClose = false;
	m_bFirst = true;

	m_dwCurRotateTime = 0;

	m_bArleadyCallWeapon = false;
}

ioAdhesiveMissileWeapon2::~ioAdhesiveMissileWeapon2()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioAdhesiveMissileWeapon2::~ioMissileWeapon - UnReleased Resouce" );
	}
}

void ioAdhesiveMissileWeapon2::ClearData()
{
	UpdateWorldAttackBox(0.0f, true );
	UpdateTargetCollisionBox( true );
	m_szTargetsName.clear();
}

bool ioAdhesiveMissileWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

void ioAdhesiveMissileWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_vExtents.x = rkLoader.LoadFloat_e( "collisionbox_extents_x", 9.0f );
	m_vExtents.y = rkLoader.LoadFloat_e( "collisionbox_extents_y", 50.0f );
	m_vExtents.z = rkLoader.LoadFloat_e( "collisionbox_extents_z", 9.0f );

	m_fStartOffset_x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_fStartOffset_y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_fStartOffset_z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	// 끝까지 끌려간 후 걸어줄 버프
	int iCnt = rkLoader.LoadInt_e( "target_wound_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i)
	{
		wsprintf_e( szKey, "target_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szTargetWoundBuff.push_back( szBuf );
	}

	// 충돌에 의해 충돌체가 사라진 경우 걸어줄 버프
	iCnt = rkLoader.LoadInt_e( "target_cancel_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i)
	{
		wsprintf_e( szKey, "target_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szTargetCancelBuff.push_back( szBuf );
	}

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	//
	m_fRotateStartRange = rkLoader.LoadFloat_e( "rotate_start_range", 0.0f );
	m_fRotateEndRange = rkLoader.LoadFloat_e( "rotate_end_range", 0.0f );

	m_dwMinRotateTime = (DWORD)rkLoader.LoadInt_e( "min_rotate_speed", 0 );
	m_dwMaxRotateTime = (DWORD)rkLoader.LoadInt_e( "max_rotate_speed", 0 );

	LoadCallAttributeList( rkLoader );
}


void ioAdhesiveMissileWeapon2::LoadCallAttributeList( ioINILoader &rkLoader )
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


void ioAdhesiveMissileWeapon2::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		float fHeight =	pStage->GetMapHeight( GetPosition().x, GetPosition().z );
		//pStage->GetMapHeightOnlyTerrain( GetPosition().x, GetPosition().z );
		D3DXVECTOR3 vWorldPos = GetPosition();
		vWorldPos.y = fHeight;

		WeaponInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			pInfo,
			//GetPosition(),
			vWorldPos,
			m_vMoveDir,
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}


bool ioAdhesiveMissileWeapon2::CreateFireBall( const ioHashString &szFileName )
{
	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );
	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioAdhesiveMissileWeapon::CreateFileBall - Not Has Owner" );
		return false;
	}

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	D3DXVECTOR3 vStartPos( m_fStartOffset_x, m_fStartOffset_y, m_fStartOffset_z );
	D3DXVECTOR3 vPosOffset = pOwner->GetWorldOrientation() * vStartPos;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			vLinkPosOffset += vPosOffset;
			SetPosition( vLinkPosOffset );
		}
	}

	m_StartPos = GetPosition();

	AddEffectEvent( m_pFireBall );

	m_CollisionBox.SetNull();

	return true;
}
 
void ioAdhesiveMissileWeapon2::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioAdhesiveMissileWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon2::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_StartPos = GetPosition();
}

bool ioAdhesiveMissileWeapon2::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_MissileState == MS_MOVE )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{
			ExplosionNoWeaponDead( pStage );
			CallAttribute();
			return true;
		}
	}
	else if( m_MissileState == MS_COL_MOVE )
	{
		if( GetOwner()->IsNeedProcess() )
		{
			if( pShape->TestIntersection( pShapeWorldMat, m_CollisionBox ) )
			{
				if( GetOwner()->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );			
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << STC_TEST_MAP_COLLISION;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				Explosion( pStage );
				return true;
			}
		}
	}

	return false;
}

void ioAdhesiveMissileWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckOwnerRotateDir( fTimePerSec, pStage );
		ProcessMove( fTimePerSec, pStage );
	}

	m_bFirst = false;

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioAdhesiveMissileWeapon2::ProcessMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_MissileState == MS_COL_MOVE && m_szTargetsName.empty() )
	{
		SetForceClose();
		SetWeaponDead();
		ClearData();
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;

	D3DXVECTOR3 vMove = m_vMoveDir * fMoveDist;
	GetParentSceneNode()->Translate( vMove );

	// 발사체의 이동만큼 피격대상이 따라가도록 처리.
	ioHashStringVec::iterator iter = m_szTargetsName.begin();
	while( iter != m_szTargetsName.end() )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( *iter );
		if( pTarget )
		{
			if( pTarget->GetState() == CS_SKIP_STUN )
			{
				pTarget->Translate( vMove );
				++iter;
			}
			else
			{
				iter = m_szTargetsName.erase( iter );
			}
		}
		else
		{
			iter = m_szTargetsName.erase( iter );
		}
	}

	int iCnt = m_szTargetsName.size();
	for( int i=0; i<iCnt; ++i )
	{

	}

	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
	if( fMaxRange > 0.0f )
		m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

	if( m_fCurMoveRange < fMaxRange )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( !m_CollisionBox.IsNull() )
			UpdateTargetCollisionBox( vMove, false );
	}
	else
	{
		if( m_MissileState == MS_COL_MOVE )
		{
			int iCharCnt = (int)m_szTargetsName.size();
			for( int i=0; i < iCharCnt; ++i )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetsName[i] );
				if( !pTarget ) continue;

				if( pTarget->GetState() != CS_SKIP_STUN )
					continue;

				if( !pTarget->IsNeedProcess() )
					continue;

				bool bResult = false;
				ioHashStringVec vBuffList;

				int iBuffCnt = m_szTargetWoundBuff.size();
				for( int j=0; j<iBuffCnt; j++ )
				{
					ioBuff *pBuff = pTarget->AddNewBuff( m_szTargetWoundBuff[j], GetOwner()->GetCharName(), "", NULL);
					if( pBuff )
					{
						bResult = true;
						vBuffList.push_back( m_szTargetWoundBuff[j] );
					}
				}

				if( bResult && !vBuffList.empty() && pTarget->IsNeedSendNetwork() )
				{
					int iBuffListCnt = vBuffList.size();

					if( Help::CheckEnableTimeGapUser() )
					{
						LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff2 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
					}
					SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
					kPacket << pTarget->GetCharName();
					kPacket << GetOwner()->GetCharName();
					kPacket << "";
					kPacket << pTarget->GetRandomSeed();
					kPacket << false;
					kPacket << 0.0f;
					kPacket << 400.0f;
					kPacket << 0.0f;
					kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
					kPacket << iBuffListCnt;

					for(int k=0; k < iBuffListCnt; k++ )
					{
						kPacket << vBuffList[k];
					}

					kPacket << false;

					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}

		SetForceClose();
		SetWeaponDead();
		ClearData();
	}
}

void ioAdhesiveMissileWeapon2::SetForceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioAdhesiveMissileWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioAdhesiveMissileWeapon2::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioAdhesiveMissileWeapon2::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioAdhesiveMissileWeapon2::GetType() const
{
	return WT_ADHESIVE_MISSILE2;
}

float ioAdhesiveMissileWeapon2::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioAdhesiveMissileWeapon2::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

ActionStopType ioAdhesiveMissileWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAdhesiveMissileWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAdhesiveMissileWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fExtentsX = m_WorldAttackBox.GetExtents(0);
	float fExtentsY = m_WorldAttackBox.GetExtents(1);

	m_WorldAttackBox.SetExtents( 0, fExtentsX );
	m_WorldAttackBox.SetExtents( 1, fExtentsY );

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(), GetScale(), GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), GetScale(), GetOrientation() );
	}
}

void ioAdhesiveMissileWeapon2::UpdateTargetCollisionBox( D3DXVECTOR3 &vMove, bool bSetNull )
{
	if( m_CollisionBox.IsNull() ) return;

	if( bSetNull )
	{
		m_CollisionBox.SetNull();
		return;
	}

	m_CollisionBox.SetCenter( m_CollisionBox.GetCenter() + vMove );
}

void ioAdhesiveMissileWeapon2::UpdateTargetCollisionBox( bool bSetNull )
{
	if( bSetNull )
		m_CollisionBox.SetNull();
}

void ioAdhesiveMissileWeapon2::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_MissileState == MS_MOVE )
	{
		if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
		{
			if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
				ToDummyChar(pTarget) || ToWallZone(pTarget) )
			{
				CheckCurAttribute( pTarget );

				ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
				pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

				AddCollisionEntity( pTarget );

				ExplosionNoWeaponDead( pStage );
				return;
			}
		}
	}
	else if( m_MissileState == MS_COL_MOVE )
	{
		if( m_szTargetsName.empty() )	return;
		if( pTarget->IsWoundedCollision( m_CollisionBox ) )
		{
			if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
				ToDummyChar(pTarget) || ToWallZone(pTarget) )
			{
				Explosion( pStage );
				return;
			}
		}
	}
}

bool ioAdhesiveMissileWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
 	if( ToBaseChar(pEntity) && !ToBaseChar(pEntity)->IsNeedProcess() )
 		return false;

	if( m_MissileState == MS_COL_MOVE )
		return false;

	if( pEntity->IsWoundedCollision( GetWorldAttackBox( ) ) )
	{
		ioBaseChar* pTarget = ToBaseChar(pEntity);
		if( pTarget )
			return true;
		else if( ToBallStruct(pEntity) )
			return true;
	}

	return false;
}

void ioAdhesiveMissileWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case STC_NONE:
		break;
	case STC_CHAR_COLLISION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
			{
				m_MissileState = MS_COL_MOVE;
				m_szTargetsName.push_back( szTargetName );

				m_CollisionBox = pTarget->GetWorldCollisionBox();
				m_CollisionBox.SetExtents( 0, m_vExtents.x );
				m_CollisionBox.SetExtents( 1, m_vExtents.y );
				m_CollisionBox.SetExtents( 2, m_vExtents.z );
			}
		}
		break;
	case STC_TEST_MAP_COLLISION:
		{
			Explosion( pStage );
		}
		break;
	}
}

void ioAdhesiveMissileWeapon2::Explosion( ioPlayStage *pStage )
{
	// 폭발 효과와 함께 사실상 웨폰의 프로세스를 끝낸다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  ioMath::VEC3_ZERO;
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		vPos = GetWorldAttackBox().GetCenter();
		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}

	int iCnt = m_szTargetsName.size();
	for( int i=0; i<iCnt; ++i)
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetsName[i] );

		if( pTarget && pTarget->IsNeedProcess() )
		{
			bool bResult = false;
			ioHashStringVec vBuffList;

			if( pTarget->GetState() == CS_SKIP_STUN )
			{
				int iBuffCnt = m_szTargetCancelBuff.size();
				for( int j=0; j<iBuffCnt; j++ )
				{
					ioBuff *pBuff = pTarget->AddNewBuff( m_szTargetCancelBuff[j], GetOwner()->GetCharName(), "", NULL);
					if( pBuff )
					{
						bResult = true;
						vBuffList.push_back( m_szTargetCancelBuff[j] );
					}
				}
			}

			if( bResult && !vBuffList.empty() && pTarget->IsNeedSendNetwork() )
			{
				int iBuffCnt = vBuffList.size();

				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff3 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
				}
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pTarget->GetCharName();
				kPacket << GetOwner()->GetCharName();
				kPacket << "";
				kPacket << pTarget->GetRandomSeed();
				kPacket << false;
				kPacket << 0.0f;
				kPacket << 400.0f;
				kPacket << 0.0f;
				kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
				kPacket << iBuffCnt;

				for(int j=0; j < iBuffCnt; j++ )
				{
					kPacket << vBuffList[j];
				}

				kPacket << false;

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	ClearData();
	SetForceClose();
	SetWeaponDead();
}

bool ioAdhesiveMissileWeapon2::IsProtectState( ioBaseChar* pChar )
{
	if( pChar->HasBuff( BT_STATE_PROTECTION ) || pChar->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
		return true;
	
	return pChar->IsProtectState();
}

void ioAdhesiveMissileWeapon2::ExplosionNoWeaponDead( ioPlayStage* pStage )
{
	// 폭발 효과만 준다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		vPos = GetWorldAttackBox().GetCenter();
		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}

	ClearData();
	SetForceClose();
	SetWeaponDead();
}

void ioAdhesiveMissileWeapon2::SetWeaponIndex( DWORD dwIndex )
{
	m_dwWeaponIdx = dwIndex;

	// Set Seed
	m_Random.SetRandomSeed( m_dwWeaponIdx );
}

DWORD ioAdhesiveMissileWeapon2::GetRotateSpeed()
{
	if( m_dwCurRotateTime == 0 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			DWORD dwGapValue = (DWORD)(m_dwMaxRotateTime - m_dwMinRotateTime);
			DWORD dwValue = m_Random.Random( dwGapValue );

			m_dwCurRotateTime = m_dwMinRotateTime + dwValue;
		}
	}

	return m_dwCurRotateTime;
}

void ioAdhesiveMissileWeapon2::CheckOwnerRotateDir( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_dwCurRotateTime = GetRotateSpeed();

	if( m_dwCurRotateTime == 0 ) return;
	if( !COMPARE( m_fCurMoveRange, m_fRotateStartRange, m_fRotateEndRange ) )
		return;

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fAngle = D3DXVec3Dot( &vTargetDir, &m_vMoveDir );
	if( fAngle > 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else
	{
		float fAngleGap = RADtoDEG( acosf(fAngle) );
		float fRate = (fTimePerSec*1000) / m_dwCurRotateTime;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
		{
			SetOrientation( qtRot );
			m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -m_vMoveDir );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &m_vMoveDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					bLeftRot = true;
			}

			if( bLeftRot )
				fRotateAngle *= -1;

			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtNewRot;
			D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
			m_vMoveDir = qtNewRot * m_vMoveDir;

			ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
			SetOrientation( qtNewRot );

			CheckTargetRotate( pStage, fRotateAngle );
		}
	}
}

void ioAdhesiveMissileWeapon2::CheckTargetRotate( ioPlayStage *pStage, float fRotAngle )
{
	if( !pStage ) return;
	if( m_MissileState == MS_MOVE )
		return;

	D3DXQUATERNION qtNewRot;
	ioMath::CalcDirectionQuaternion( qtNewRot, m_vMoveDir );

	int iCnt = m_szTargetsName.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetsName[i] );
		if( !pTarget ) continue;
		if( pTarget->GetState() != CS_SKIP_STUN )
			continue;

		/*
		ioEntityGroup *pGrp = pTarget->GetGroup();
		pGrp->GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fRotAngle );
		*/

		pTarget->SetTargetRotToRotate( qtNewRot, true );
	}
}

void ioAdhesiveMissileWeapon2::SetColMoveState( ioBaseChar *pTarget, bool bSend )
{
	m_MissileState = MS_COL_MOVE;
	m_szTargetsName.push_back( pTarget->GetCharName() );

	m_CollisionBox = pTarget->GetWorldCollisionBox();
	m_CollisionBox.SetExtents( 0, m_vExtents.x );
	m_CollisionBox.SetExtents( 1, m_vExtents.y );
	m_CollisionBox.SetExtents( 2, m_vExtents.z );

	if( bSend && pTarget->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << STC_CHAR_COLLISION;
		kPacket << pTarget->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}





