

#include "stdafx.h"

#include "ioAdhesiveMissileWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioAdhesiveMissileWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioAdhesiveMissileWeapon::m_DieActionStopType = AST_NONE;

ioAdhesiveMissileWeapon::ioAdhesiveMissileWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	ClearData();

	m_pFireBall = NULL;

	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	
	m_bSetGraceClose = false;
	m_bFirst = true;

	m_pSecondAttr = NULL;

	m_bArleadyCallWeapon = false;

	m_fChangeAngleH = 0.0f;
	m_fChangeAngleV = 0.0f;
}

ioAdhesiveMissileWeapon::~ioAdhesiveMissileWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioAdhesiveMissileWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}
	m_vSecondAttrList.clear();
}

void ioAdhesiveMissileWeapon::ClearData()
{
	UpdateWorldAttackBox(0.0f, true );
	UpdateTargetCollisionBox( true );
	m_szTargetsName.clear();
}

bool ioAdhesiveMissileWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

void ioAdhesiveMissileWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bCollisionMap = rkLoader.LoadBool_e( "check_collision_map", false );

	m_vExtents.x = rkLoader.LoadFloat_e( "collisionbox_extents_x", 9.0f );
	m_vExtents.y = rkLoader.LoadFloat_e( "collisionbox_extents_y", 50.0f );
	m_vExtents.z = rkLoader.LoadFloat_e( "collisionbox_extents_z", 9.0f );

	m_fStartOffset_x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_fStartOffset_y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_fStartOffset_z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_CollisionBoxType = (CollisionBoxType)rkLoader.LoadInt_e( "collsion_box_type", CBT_TARGET );

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

	LoadCallAttributeList( rkLoader );

	m_BombEffectPositionType = (BombEffectPositionType)rkLoader.LoadInt_e( "bomb_effect_position_type", BEPT_TARGET );
	m_fChangeAngleH = rkLoader.LoadFloat_e( "change_angle_h", 0.0f );
	m_fChangeAngleV = rkLoader.LoadFloat_e( "change_angle_v", 0.0f );

	m_bWorldAxiCallWeapon = rkLoader.LoadBool_e( "call_weapon_world_axi_type", false );
}

void ioAdhesiveMissileWeapon::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();
	m_CallWeaponRotateAngleList.clear();
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );
	m_CallWeaponRotateAngleList.reserve( iCnt );
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

		wsprintf_e( szKey, "call_attribute%d_angle", i+1 );
		float fRotateAngle = rkLoader.LoadFloat( szKey, 0.0f );
		m_CallWeaponRotateAngleList.push_back( fRotateAngle );
	}
}

bool ioAdhesiveMissileWeapon::CreateFireBall( const ioHashString &szFileName )
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
 
void ioAdhesiveMissileWeapon::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioAdhesiveMissileWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	RotateMoveDir( m_fChangeAngleH, m_fChangeAngleV );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vMoveDir = qtRot * m_vMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vMoveDir = qtRot * m_vMoveDir;

	//qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
	//SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	D3DXVECTOR3 vStartPos = vPos;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		D3DXVECTOR3 vStartOffsetPos( m_fStartOffset_x, m_fStartOffset_y, m_fStartOffset_z );
		vStartOffsetPos = pOwner->GetWorldOrientation() * vStartOffsetPos;
		vStartPos = vPos + vStartOffsetPos;
	}

	SetPosition( vStartPos );
	m_StartPos = GetPosition();
}

bool ioAdhesiveMissileWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	
	if( m_szTargetsName.empty() )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{
			ExplosionNoWeaponDead( NULL, pStage );
			return true;
		}
	}
	else
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

void ioAdhesiveMissileWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;

		D3DXVECTOR3 vMove = m_vMoveDir * fMoveDist;

		GetParentSceneNode()->Translate( vMove );

		// 발사체의 이동만큼 피격대상이 따라가도록 처리.
		int iCnt = m_szTargetsName.size();
		for( int i=0; i<iCnt; ++i )
		{
			ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetsName[i] );
			
			if( pTarget && pTarget->GetState() == CS_SKIP_STUN )
				pTarget->Translate( vMove );
			/*
			if( pTarget && pTarget->GetState() == CS_SKIP_STUN )
			{
				D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
				vTargetPos += vMove;
				float fHeight = pStage->GetMapHeight( vTargetPos.x, vTargetPos.z, pTarget, false );
				if ( fHeight > vTargetPos.y )
				{
					vTargetPos.y = fHeight;
				}
				pTarget->SetWorldPosition( vTargetPos );
			}
			*/
		}

		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		
		if( fMaxRange > 0.0f )
			m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

		if( m_fCurMoveRange < fMaxRange )
		{
			UpdateWorldAttackBox( fTimePerSec, false );
			if( !m_CollisionBox.IsNull() )	UpdateTargetCollisionBox( vMove, false );
		}
		else
		{
			int iCharCnt = (int)m_szTargetsName.size();
			for( int i=0; i < iCharCnt; ++i )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetsName[i] );
				if( !pTarget ) continue;

				int iBuffCnt = (int)m_szTargetWoundBuff.size();
				for( int j=0; j < iBuffCnt; ++j )
				{
					pTarget->AddNewBuff( m_szTargetWoundBuff[j], GetOwner()->GetCharName(), "", NULL);
				}
			}

			SetForceClose();
			SetWeaponDead();
			ClearData();
		}		
	}

	m_bFirst = false;

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioAdhesiveMissileWeapon::SetForceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioAdhesiveMissileWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioAdhesiveMissileWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioAdhesiveMissileWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioAdhesiveMissileWeapon::GetType() const
{
	return WT_ADHESIVE_MISSILE;
}

float ioAdhesiveMissileWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioAdhesiveMissileWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

ActionStopType ioAdhesiveMissileWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAdhesiveMissileWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAdhesiveMissileWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fScaleRate = FLOAT1 + (GetAttribute()->m_fScaleRate * m_fCurMoveRate );
	
	float fExtentsX = m_WorldAttackBox.GetExtents(0) * fScaleRate;
	float fExtentsY = m_WorldAttackBox.GetExtents(1) * fScaleRate;

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

void ioAdhesiveMissileWeapon::UpdateTargetCollisionBox( D3DXVECTOR3 &vMove, bool bSetNull )
{
	if( m_CollisionBox.IsNull() ) return;

	if( bSetNull )
	{
		m_CollisionBox.SetNull();
		return;
	}

	m_CollisionBox.SetCenter( m_CollisionBox.GetCenter() + vMove );
}

void ioAdhesiveMissileWeapon::UpdateTargetCollisionBox( bool bSetNull )
{
	if( bSetNull )	m_CollisionBox.SetNull();
}

void ioAdhesiveMissileWeapon::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioAdhesiveMissileWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );

	if( m_bResistance ) return;

	int iSize = m_vSecondAttrList.size();
	if( iSize > 0 )
	{
		for( int i=0; i < iSize-1; ++i )
		{
			if( m_fCurMoveRange < m_vSecondAttrList[i].m_fLimiteValue )
			{
				m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[i].m_iAttributeIndex );
				return;
			}
		}

		m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[iSize-1].m_iAttributeIndex );
		return;
	}

	m_pSecondAttr = NULL;
	return;
}

const WeaponAttribute* ioAdhesiveMissileWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	if( m_pSecondAttr )
		return m_pSecondAttr;

	return m_pConstAttr;
}

void ioAdhesiveMissileWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_szTargetsName.empty() )
	{
		if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) && m_bCollisionMap )
		{
			if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
				ToDummyChar(pTarget) || ToWallZone(pTarget) )
			{
				if( GetOwner() )
				{
					if( ToDummyChar(pTarget) && ToDummyChar(pTarget)->IsCharCollisionSkipState(GetOwner()->GetTeam()) )
						return;
				}

				ExplosionNoWeaponDead( pTarget, pStage );
				return;
			}
		}

		return;
	}

	if( pTarget->IsWoundedCollision( m_CollisionBox ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			if( GetOwner() )
			{
				if( ToDummyChar(pTarget) && ToDummyChar(pTarget)->IsCharCollisionSkipState(GetOwner()->GetTeam()) )
					return;
			}

			Explosion( pStage );
			return;
		}
	}
}

bool ioAdhesiveMissileWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
 	if( ToBaseChar(pEntity) && !ToBaseChar(pEntity)->IsNeedProcess() )
 		return false;

	if( m_szTargetsName.empty() )
	{
		if( pEntity->IsWoundedCollision( GetWorldAttackBox( ) ) )
		{
			ioBaseChar* pTarget = ToBaseChar(pEntity);
			
			if( pTarget )
			{
				if( IsProtectState( pTarget ) )	return false;

				m_szTargetsName.push_back( pTarget->GetCharName() );

				if ( m_CollisionBoxType == CBT_TARGET )
					m_CollisionBox = pTarget->GetWorldCollisionBox();
				else if ( m_CollisionBoxType == CBT_WEAPON )
				{
					m_CollisionBox = GetWorldAttackBox( false );
					m_CollisionBox.SetCenter( GetPosition() );
				}

				m_CollisionBox.SetExtents( 0, m_vExtents.x );
				m_CollisionBox.SetExtents( 1, m_vExtents.y );
				m_CollisionBox.SetExtents( 2, m_vExtents.z );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << STC_ONE_COLLISION;
					kPacket << pTarget->GetCharName();
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				return true;
			}
			else if( ToBallStruct(pEntity) )
			{
				return true;
			}
			else if( GetOwner() )
			{
				if( ToDummyChar(pEntity) &&
					ToDummyChar(pEntity)->IsCharCollisionSkipState(GetOwner()->GetTeam()) )
					return false;
			}
		}
	}
	else
	{
		ioBaseChar* pTarget = ToBaseChar( pEntity );

		if( pTarget )
		{
			if( IsProtectState( pTarget ) )	return false;
			if( m_CollisionBox.IsNull() )	return false;

			if( pTarget->IsWoundedCollision( m_CollisionBox ) )
			{
				m_szTargetsName.push_back( pTarget->GetCharName() );

				// 끌고가는 타겟 수 만큼 충돌체의 z축만 사이즈를 늘려준다.
				int iOffset = m_szTargetsName.size() * 2 - 1;
				float fExtentsZ = m_vExtents.z * (float)iOffset;
				m_CollisionBox.SetExtents( 2, fExtentsZ );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << STC_MULTI_COLLISION;
					kPacket << pTarget->GetCharName();
					kPacket << fExtentsZ;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				return true;
			}
		}
		else if( GetOwner() )
		{
			if( ToDummyChar(pEntity) &&
				ToDummyChar(pEntity)->IsCharCollisionSkipState(GetOwner()->GetTeam()) )
				return false;
		}
	}

	return false;
}

void ioAdhesiveMissileWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case STC_NONE:
		break;
	case STC_ONE_COLLISION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
			{
				m_szTargetsName.push_back( szTargetName );

				if ( m_CollisionBoxType == CBT_TARGET )
					m_CollisionBox = pTarget->GetWorldCollisionBox();
				else if ( m_CollisionBoxType == CBT_WEAPON )
				{
					m_CollisionBox = GetWorldAttackBox( false );
					m_CollisionBox.SetCenter( GetPosition() );
				}
				m_CollisionBox.SetExtents( 0, m_vExtents.x );
				m_CollisionBox.SetExtents( 1, m_vExtents.y );
				m_CollisionBox.SetExtents( 2, m_vExtents.z );
			}
		}
		break;
	case STC_MULTI_COLLISION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
			if( pTarget )
				m_szTargetsName.push_back( szTargetName );

			float fExtentsZ;
			rkPacket >> fExtentsZ;
			// 끌고가는 타겟 수 만큼 충돌체의 z축만 사이즈를 늘려준다.
			m_CollisionBox.SetExtents( 2, fExtentsZ );
		}
		break;
	case STC_TEST_MAP_COLLISION:
		{
			Explosion( pStage );
		}
		break;
	}
}

void ioAdhesiveMissileWeapon::Explosion( ioPlayStage *pStage )
{
	// 폭발 효과와 함께 사실상 웨폰의 프로세스를 끝낸다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  ioMath::VEC3_ZERO;
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		int iCnt = m_szTargetsName.size();
		if( iCnt != 0 )
		{
			if ( m_BombEffectPositionType == BEPT_WEAPON )
			{
				vPos = GetWorldAttackBox().GetCenter();
			}
			else if ( m_BombEffectPositionType == BEPT_TARGET )
			{
				ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetsName[iCnt-1] );
				if( pTarget )
					vPos = pTarget->GetWorldPosition();
				else
					vPos = GetWorldAttackBox().GetCenter();
			}
		}
		else
		{
			vPos = GetWorldAttackBox().GetCenter();
		}

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

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
					LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff1 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
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

	CallAttribute();

	ClearData();
	SetForceClose();
	SetWeaponDead();
}

bool ioAdhesiveMissileWeapon::IsProtectState( ioBaseChar* pChar )
{
	if( pChar->HasBuff( BT_STATE_PROTECTION ) || pChar->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
		return true;
	
	return pChar->IsProtectState();
}

void ioAdhesiveMissileWeapon::ExplosionNoWeaponDead( ioPlayEntity *pTarget, ioPlayStage* pStage )
{
	// 폭발 효과만 준다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		int iCnt = m_szTargetsName.size();
		if( iCnt != 0 )
		{
			if ( m_BombEffectPositionType == BEPT_WEAPON )
			{
				vPos = GetWorldAttackBox().GetCenter();
			}
			else if ( m_BombEffectPositionType == BEPT_TARGET )
			{
				ioBaseChar* pTarget = pStage->GetBaseChar( m_szTargetsName[iCnt-1] );
				if( pTarget )
					vPos = pTarget->GetWorldPosition();
				else
					vPos = GetWorldAttackBox().GetCenter();
			}
		}
		else
		{
			vPos = GetWorldAttackBox().GetCenter();
		}

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}

	CallAttribute();

	if( m_bCollisionMap )
	{
		if( pTarget )
		{
			CheckCurAttribute( pTarget );

			ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
			pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

			AddCollisionEntity( pTarget );
		}

		ClearData();
		SetForceClose();
		SetWeaponDead();
	}
}

void ioAdhesiveMissileWeapon::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();

	for( int i=0 ; i<(int)m_CallWeaponInfoList.size() ; ++i )
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;

		D3DXVECTOR3 vTempDir = m_vMoveDir;
		vTempDir.y = 0.f;
		D3DXVec3Normalize( &vTempDir, &vTempDir );

		if( m_bWorldAxiCallWeapon )
			CallWeaponRotateMoveDir( vDir, vTempDir, m_CallWeaponRotateAngleList[i] );
		else
			CallWeaponRotateMoveDir( vDir, m_vMoveDir, m_CallWeaponRotateAngleList[i] );
		
		

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			m_CallWeaponInfoList[i],
			GetPosition(),
			vDir,
			dwCurGenerateTime + m_CallWeaponInfoList[i].m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}

void ioAdhesiveMissileWeapon::CallWeaponRotateMoveDir( D3DXVECTOR3& vOutMoveDir, D3DXVECTOR3 vCurMoveDir, float fAngle)
{
	fAngle = ioMath::ArrangeHead( fAngle );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );
	vOutMoveDir = qtRot * vCurMoveDir;
}