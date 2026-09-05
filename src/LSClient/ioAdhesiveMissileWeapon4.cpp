

#include "stdafx.h"

#include "ioAdhesiveMissileWeapon4.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

ActionStopType ioAdhesiveMissileWeapon4::m_ActionStopType = AST_NONE;
ActionStopType ioAdhesiveMissileWeapon4::m_DieActionStopType = AST_NONE;

ioAdhesiveMissileWeapon4::ioAdhesiveMissileWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	ClearData();

	m_pFireBall = NULL;		
	m_bSetGraceClose = false;

	m_State = WS_INIT;

	m_bUseCol = true;
	m_bCollisionMapForSkill = true;
}

ioAdhesiveMissileWeapon4::~ioAdhesiveMissileWeapon4()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioAdhesiveMissileWeapon4::~ioAdhesiveMissileWeapon4 - UnReleased Resouce" );
	}
}

void ioAdhesiveMissileWeapon4::ClearData()
{
	UpdateWorldAttackBox(0.0f, true );

	m_TargetInfoList.clear();

	m_vPreAttackDir = ioMath::VEC3_ZERO;
	m_bMapCollision = false;
}

bool ioAdhesiveMissileWeapon4::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

void ioAdhesiveMissileWeapon4::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	//맵 충돌에 의해 충돌체가 사라진 경우 걸어줄 버프
	int iCnt = rkLoader.LoadInt_e( "target_cancel_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "target_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szTargetMapCollisionCancelBuff.push_back( szBuf );
	}

	//웨폰이 종료 됬을때 호출될 버프
	iCnt = rkLoader.LoadInt_e( "owner_wounded_buff_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szKey, "owner_wounded_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szOwnerWoundedBuff.push_back( szBuf );
	}

	m_fLimitAngle = rkLoader.LoadFloat_e( "limit_angle", 0.0f );

	m_bCollisionMap = rkLoader.LoadBool_e( "check_collision_map", false );
}

bool ioAdhesiveMissileWeapon4::CreateFireBall( const ioHashString &szFileName )
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
		
	if( m_pFireBall->GetSkeletonName().IsEmpty() &&
		m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	AddEffectEvent( m_pFireBall );

	return true;
}
 
void ioAdhesiveMissileWeapon4::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioAdhesiveMissileWeapon4::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioPlayEntityHelp::CheckOrientation( m_vMoveDir, qtRot );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioAdhesiveMissileWeapon4::GetMoveDir()
{
	return m_vMoveDir;
}

void ioAdhesiveMissileWeapon4::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon4::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	return;
}

bool ioAdhesiveMissileWeapon4::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !m_bCollisionMap )
		return true;

	if( !m_bCollisionMapForSkill )
		return true;

	if( GetOwner()->IsNeedProcess() )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{
			m_bMapCollision = true;
			Explosion( pStage );

			if( GetOwner()->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << STC_TEST_MAP_COLLISION;
				kPacket << m_bMapCollision;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}

		if( m_State == WS_CHOP )
		{
			int iSize = m_TargetInfoList.size();
			for( int i=0; i<iSize; ++i )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_szTargetName );
				if( pTarget )
				{
					bool bFront = false;

					// 앵글 체크
					ioBaseChar *pOwner = GetOwner();
					if( !pOwner )
						continue;

					D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
					D3DXVECTOR3 vOwnerDir = pOwner->GetMoveDir();
					D3DXVec3Normalize( &vTargetPos, &vTargetPos );
					float fDot = D3DXVec3Dot( &vOwnerDir, &vTargetPos );
					if( fDot > 0.0f )
						bFront = true;

					if( bFront && pShape->TestIntersection( pShapeWorldMat, pTarget->GetWorldCollisionBox() ) )
					{
						m_bMapCollision = true;
						Explosion( pStage );

						if( GetOwner()->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_WEAPON_SYNC );
							kPacket << GetOwnerName();
							kPacket << m_dwWeaponIdx;
							kPacket << STC_TEST_MAP_COLLISION;
							kPacket << m_bMapCollision;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
						return true;
					}
				}
			}
		}
	}

	return false;
}

void ioAdhesiveMissileWeapon4::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 타겟이 유효한지 체크.
	TargetInfoList::iterator iter = m_TargetInfoList.begin();
	while( iter != m_TargetInfoList.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( (*iter).m_szTargetName );
		if( pTarget )
		{
			if( !pTarget->CheckGrapplingExceptionState() )
				iter = m_TargetInfoList.erase(iter);
			else
				++iter;
		}
		else
			iter = m_TargetInfoList.erase(iter);
	}

	switch( m_State )
	{	
	case WS_INIT:
		{
		}
		break;
	case WS_ATTACH_MOVE:
		ProcessAttachMove( fTimePerSec, pStage );
		break;
	case WS_CHOP:				
		ProcessChop( fTimePerSec, pStage );
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioAdhesiveMissileWeapon4::ProcessAttachMove( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		Explosion( pStage );
		return;
	};

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() &&
		m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();					
		}
	}
	
	SetMoveDir( vDir );
	SetPosition( vLinkPosOffset );	
	UpdateWorldAttackBox( fTimePerSec, false );

	D3DXVECTOR3 vSideAxis;
	D3DXVec3Cross( &vSideAxis, &vDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vSideAxis, &vSideAxis );

	D3DXQUATERNION qtNewRot = GetOrientation();

	TargetInfoList::iterator iter;
	for( iter = m_TargetInfoList.begin(); iter != m_TargetInfoList.end(); ++iter )
	{
		TargetInfo pInfo = (*iter);
		ioBaseChar* pTarget = pStage->GetBaseChar( pInfo.m_szTargetName );
		if( pTarget && pTarget->HasBuff(BT_SKIP_STUN) )
		{
			D3DXVECTOR3 vPos = GetPosition() + ( vDir * pInfo.m_fLength );			
			vPos.y -= pInfo.m_fHeightGap;
			vPos += vSideAxis * -m_WorldAttackBox.GetExtents(0);

			pTarget->SetWorldPosition( vPos );
			pTarget->SetWorldOrientation( qtNewRot );
		}
	}	
}

void ioAdhesiveMissileWeapon4::ProcessChop( float fTimePerSec, ioPlayStage *pStage  )
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;
		
	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() &&
		m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();					
		}
	}

	SetPosition( vLinkPosOffset );

	D3DXVECTOR3 vOwnerPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vPos = GetPosition();
	D3DXVECTOR3 vDir = vPos - vOwnerPos;
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideAxis;
	D3DXVec3Cross( &vSideAxis, &vDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vSideAxis, &vSideAxis );
	SetMoveDir( vDir );
	
	// 각도체크
	float fDotValue = D3DXVec3Dot( &vDir, &ioMath::UNIT_Y );
	float fAngle = RADtoDEG(acosf(fDotValue));

	if( m_fLimitAngle > 0.0f && fAngle > m_fLimitAngle )
	{
		Explosion(pStage);
		return;
	}
	

	D3DXQUATERNION qtNewRot = GetOrientation();

	TargetInfoList::iterator iter;
	for( iter = m_TargetInfoList.begin(); iter != m_TargetInfoList.end(); ++iter )
	{
		TargetInfo pInfo = (*iter);
		ioBaseChar* pTarget = pStage->GetBaseChar( pInfo.m_szTargetName );
		if( pTarget && pTarget->HasBuff(BT_SKIP_STUN) )
		{
			D3DXVECTOR3 vPos = GetPosition() + ( vDir * pInfo.m_fLength );
			
			vPos.y -= pInfo.m_fHeightGap;
			vPos += vSideAxis * -m_WorldAttackBox.GetExtents(0);

			pTarget->SetWorldPosition( vPos );
			pTarget->SetWorldOrientation( qtNewRot );
		}
	}
		
	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioAdhesiveMissileWeapon4::SetForceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioAdhesiveMissileWeapon4::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioAdhesiveMissileWeapon4::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();
	
	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioAdhesiveMissileWeapon4::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioAdhesiveMissileWeapon4::GetType() const
{
	return WT_ADHESIVE_MISSILE4;
}

ActionStopType ioAdhesiveMissileWeapon4::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAdhesiveMissileWeapon4::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAdhesiveMissileWeapon4::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );			
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

const WeaponAttribute* ioAdhesiveMissileWeapon4::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

void ioAdhesiveMissileWeapon4::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
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

bool ioAdhesiveMissileWeapon4::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !m_bUseCol )
		return false;

	if( ToBaseChar(pEntity) && !ToBaseChar(pEntity)->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vColPoint = ioMath::VEC3_ZERO;
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(), &vColPoint ) )
	{
		// 타겟이 타워라면 피격만 주고 끝낸다.
		ioNpcChar *pNpc = ToNpcChar( pEntity );
		if( pNpc && COMPARE(pNpc->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )
			return true;

		//웨폰에 타겟 버프가 없으면 피격만 주고 끝낸다.
		if( GetTargetBuffList().empty() && GetTargetAirBuffList().empty() && GetTargetDownBuffList().empty() )		
			return true;		

		ioBaseChar* pTarget = ToBaseChar(pEntity);
		if( pTarget )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner && pOwner->GetTeam() == pTarget->GetTeam() )
				return true;

			int iSize = m_TargetInfoList.size();
			for( int i=0; i<iSize; ++i )
			{
				TargetInfo kInfo = m_TargetInfoList[i];
				if( kInfo.m_szTargetName == pTarget->GetCharName() )
					return false;
			}

			// 방향성, 방파 등의 옵션이 있는 상태무적관련 체크.
			if( pTarget->HasBuff(BT_STATE_PROTECTION) || pTarget->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
			{
				AttackSide eAttackSide = AS_NONE;

				CharState eState = pTarget->GetState();
				if( eState == CS_BLOW_WOUNDED || eState == CS_BOUND_BLOW_WOUNDED )
				{
					if( eState == BW_BLOW_ENDED )
					{
						eAttackSide = pTarget->GetCurAttackedSide();
					}
				}

				if( eAttackSide == AS_NONE )
				{
					D3DXVECTOR3 vDefenseDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					D3DXVECTOR3 vInvAttack = -GetAttackDir(pTarget);

					float fAttackedCosine = D3DXVec3Dot( &vDefenseDir, &vInvAttack );
					
					float fFrontAngle = DEGtoRAD(FLOAT90 );
					float fFrontSideAngleCosine = cosf( fFrontAngle );	// 좌우 대칭이기때문에 계산은 절반으로

					if( fAttackedCosine >= fFrontSideAngleCosine )
						eAttackSide = AS_FRONT;
					else
						eAttackSide = AS_BACK;
				}

				if( pTarget->IsStateProtect( eAttackSide, GetDefenseBreakType() ) )
				{
					return true;
				}
			}
			else if( pTarget->IsProtectState() )
				return true;
			else if( pTarget->HasBuff( BT_HIT_COUNT_HOLD ) )
				return true;
									
			D3DXQUATERNION qtRot = GetOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vMaxLength = vDir * (m_WorldAttackBox.GetExtents(2) * 2.0f);
			D3DXVECTOR3 vCurLength = pTarget->GetWorldPosition() - GetPosition();

			if( D3DXVec3Length(&vMaxLength) < D3DXVec3Length(&vCurLength) )
				vCurLength = vMaxLength;

			TargetInfo pInfo;
			pInfo.m_szTargetName = pTarget->GetCharName();
			pInfo.m_fLength = D3DXVec3Length( &vCurLength );
			pInfo.m_fHeightGap = GetPosition().y - pTarget->GetWorldPosition().y;

			m_TargetInfoList.push_back( pInfo );

			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << STC_CHAR_COLLISION;
				kPacket << pTarget->GetCharName();
				kPacket << D3DXVec3Length( &vCurLength );
				kPacket << GetPosition().y - pTarget->GetWorldPosition().y;

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return true;
		}
	}

	return false;
}

void ioAdhesiveMissileWeapon4::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case STC_TEST_MAP_COLLISION:
		rkPacket >> m_bMapCollision;
		Explosion( pStage );
		break;
	case STC_CHAR_COLLISION:
		{
			TargetInfo pInfo;
			rkPacket >> pInfo.m_szTargetName;
			rkPacket >> pInfo.m_fLength;
			rkPacket >> pInfo.m_fHeightGap;

			m_TargetInfoList.push_back( pInfo );
		}
		break;
	}
}

void ioAdhesiveMissileWeapon4::Explosion( ioPlayStage *pStage )
{
	if( !IsLive() )
	{		
		return;
	}

	// 폭발 효과와 함께 사실상 웨폰의 프로세스를 끝낸다.
	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}

	if( m_bMapCollision )
		SetTargetMapCollisionCancelBuff();
	else
		SetOwnerWoundedBuff();
	
	ClearData();
	SetForceClose();
	SetWeaponDead();
}

void ioAdhesiveMissileWeapon4::SetTargetMapCollisionCancelBuff()
{
	ioBaseChar* pChar = GetOwner();
	if( !pChar )
		return;

	ioPlayStage* pStage = pChar->GetCreator();
	if( !pStage )
		return;

	TargetInfoList::iterator iter;
	for( iter = m_TargetInfoList.begin(); iter != m_TargetInfoList.end(); ++iter )
	{
		TargetInfo pInfo = (*iter);
		ioBaseChar* pTarget = pStage->GetBaseChar( pInfo.m_szTargetName );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			bool bResult = false;
			ioHashStringVec vBuffList;

			if( pTarget->GetState() == CS_SKIP_STUN )
			{
				int iBuffCnt = m_szTargetMapCollisionCancelBuff.size();
				for( int i = 0; i < iBuffCnt; i++ )
				{
					ioBuff *pBuff = pTarget->AddNewBuff( m_szTargetMapCollisionCancelBuff[i], 
						GetOwner()->GetCharName(),
						"",
						NULL );
					if( pBuff )
					{
						bResult = true;
						vBuffList.push_back( m_szTargetMapCollisionCancelBuff[i] );
					}
				}
			}

			if( bResult && !vBuffList.empty() && pTarget->IsNeedSendNetwork() )
			{
				int iBuffCnt = vBuffList.size();

				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff4 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
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

				for(int i=0; i < iBuffCnt; i++ )
				{
					kPacket << vBuffList[i];
				}

				kPacket << false;

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

		}
	}
}

void ioAdhesiveMissileWeapon4::SetOwnerWoundedBuff()
{
	ioBaseChar* pChar = GetOwner();
	if( !pChar )
		return;

	ioPlayStage* pStage = pChar->GetCreator();
	if( !pStage )
		return;

	TargetInfoList::iterator iter;
	for( iter = m_TargetInfoList.begin(); iter != m_TargetInfoList.end(); ++iter )
	{
		TargetInfo pInfo = (*iter);
		ioBaseChar* pTarget = pStage->GetBaseChar( pInfo.m_szTargetName );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			bool bResult = false;
			ioHashStringVec vBuffList;

			if( pTarget->GetState() == CS_SKIP_STUN )
			{
				int iBuffCnt = m_szOwnerWoundedBuff.size();
				for( int i = 0; i < iBuffCnt; i++ )
				{					
					ioBuff *pBuff = pTarget->AddNewBuff( m_szOwnerWoundedBuff[i], 
						GetOwner()->GetCharName(),
						"",
						NULL );
					if( pBuff )
					{
						bResult = true;
						vBuffList.push_back( m_szOwnerWoundedBuff[i] );
					}
				}
			}

			if( bResult && !vBuffList.empty() && pTarget->IsNeedSendNetwork() )
			{
				int iBuffCnt = vBuffList.size();

				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff5 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
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

				for(int i=0; i < iBuffCnt; i++ )
				{
					kPacket << vBuffList[i];
				}

				kPacket << false;

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioAdhesiveMissileWeapon4::IsProtectState( ioBaseChar* pChar )
{
	if( pChar->HasBuff( BT_STATE_PROTECTION ) || pChar->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
		return true;
	
	return pChar->IsProtectState();
}

void ioAdhesiveMissileWeapon4::SetState( WeaponState State )
{
	m_State = State;
	m_vPreAttackDir = GetPosition();	
}


void ioAdhesiveMissileWeapon4::SetUseColCheck( bool bCheck )
{
	m_bUseCol= bCheck;
	if( m_pFireBall )
		m_pFireBall->SetVisible( bCheck );
}