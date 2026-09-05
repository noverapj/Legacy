
#include "stdafx.h"

#include "ioRotateAttachCharWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioRotateAttachCharWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioRotateAttachCharWeapon::m_DieActionStopType = AST_NONE;

ioRotateAttachCharWeapon::ioRotateAttachCharWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	
	m_TargetInfoList.clear();
	m_BallState = BS_MOVE;

	m_dwDropZoneCheckStartTime = 0;

	m_bAttachChar = false;

	m_dwCheckFireTime = 0;
}

ioRotateAttachCharWeapon::~ioRotateAttachCharWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioRotateAttachCharWeapon::~ioRotateAttachCharWeapon - UnReleased Resource" );
	}
}

bool ioRotateAttachCharWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioRotateAttachCharWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

void ioRotateAttachCharWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_dwDropZoneLiveTime = (DWORD)rkLoader.LoadInt_e( "dropzone_live_time", 0 );
	m_dwRotateSpeed = (float)rkLoader.LoadFloat_e( "target_rotate_speed", 0.0f );

	m_bRotateXAxis = rkLoader.LoadBool_e( "rotate_x_axis", false );
	m_fOffsetByRotateX = rkLoader.LoadFloat_e( "offset_by_rotate_x", 80.0f );
	m_fRotateAngle = rkLoader.LoadFloat_e( "rotate_x_angle", 0.0f );	

	m_vAttachOffset.x = rkLoader.LoadFloat_e( "attach_offset_x", 0.0f );
	m_vAttachOffset.y = rkLoader.LoadFloat_e( "attach_offset_y", 0.0f );
	m_vAttachOffset.z = rkLoader.LoadFloat_e( "attach_offset_z", 0.0f );

	m_dwFireLiveTime = rkLoader.LoadInt_e( "fire_live_time", 0 );

	m_bCollisionMap = rkLoader.LoadBool_e( "check_collision_map", false );

	m_TargetEndBuff.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "target_end_buff_count", 0 );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "target_end_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetEndBuff.push_back( szBuf );
	}

	m_TargetEndRemoveBuff.clear();
	iBuffCnt = rkLoader.LoadInt_e( "target_end_remove_buff_count", 0 );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "target_end_remove_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetEndRemoveBuff.push_back( szBuf );
	}

}

void ioRotateAttachCharWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_vPrePos = GetPosition();
}

void ioRotateAttachCharWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	m_qtOriginal = qtRot;

	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

void ioRotateAttachCharWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioRotateAttachCharWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{	
	switch( m_BallState )
	{
	case BS_MOVE:
		{			
			if ( !m_bAttachChar && m_dwCheckFireTime > 0 && m_dwCheckFireTime + m_dwFireLiveTime < FRAMEGETTIME() )
			{
				SetEndState( pStage );
				return;
			}

			if ( !IsDropZone( pStage ) )
			{
				CheckMove( fTimePerSec );			
				CheckTarget( pStage );
			}
		}
		break;
	case BS_END:
		SetWeaponDead();
		break;
	}
}

void ioRotateAttachCharWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	if( !pStage )	return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SS_ADD_TARGET:
		{
			TargetInfo kInfo;
			rkPacket >> kInfo.m_TargetName;
			rkPacket >> kInfo.m_fLength;
			rkPacket >> kInfo.m_vDir;
			
			bool bAlreadyCollision = false;
			int iCnt = m_TargetInfoList.size();
			for( int i=0; i<iCnt; ++i )
			{
				if( m_TargetInfoList[i].m_TargetName == kInfo.m_TargetName )
					bAlreadyCollision = true;				
			}

			if( !bAlreadyCollision )
			{
				m_TargetInfoList.push_back( kInfo );
			}
		}
		break;
	}	
}

bool ioRotateAttachCharWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !m_bCollisionMap )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetEndState( pStage );
		return true;
	}

	return false;
}

bool ioRotateAttachCharWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );
	
	CheckPiercing( bCollision );

	if( bCollision && !IsAlreadyCollisioned(pEntity))
	{
		ioBaseChar *pTarget = ToBaseChar( pEntity );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			bool bProtected = false;

			if( pTarget->IsProtectState() )
				bProtected = true;

			if( pTarget->IsStateProtect( AS_NONE, GetDefenseBreakType() ) )
				bProtected = true;

			if( pTarget->GetState() == CS_DEFENSE && GetDefenseBreakType() == DBT_NONE && IsIgnoreProtect() == IPT_NONE )
				bProtected = true;

			if( !bProtected )
			{
				TargetInfo kInfo;
				kInfo.m_TargetName = pTarget->GetCharName();

				D3DXVECTOR3	vTargetPos = pTarget->GetWorldPosition();
				D3DXVECTOR3 vWeaponPos = GetPosition();
				D3DXVECTOR3 vDiff = vTargetPos - vWeaponPos;
				kInfo.m_fLength = D3DXVec3Length( &vDiff );

				if( m_bRotateXAxis )
				{
					kInfo.m_fLength = min( m_fOffsetByRotateX, kInfo.m_fLength );
					kInfo.m_vDir = m_vMoveDir;
				}
				else
				{
					D3DXVec3Normalize( &vDiff, &vDiff );
					kInfo.m_vDir = vDiff;
				}

				m_TargetInfoList.push_back( kInfo );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );			
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << SS_ADD_TARGET;
					kPacket << kInfo.m_TargetName;
					kPacket << kInfo.m_fLength;
					kPacket << kInfo.m_vDir;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	
	return bCollision;
}

/////////////////////////////////////////////////////////////////////////////////////////////

ioWeapon::WeaponType ioRotateAttachCharWeapon::GetType() const
{
	return WT_ROTATE_ATTACH_CHAR;
}

ActionStopType ioRotateAttachCharWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRotateAttachCharWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioRotateAttachCharWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vAttackDir;
}

const ioOrientBox& ioRotateAttachCharWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

/////////////////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 ioRotateAttachCharWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

void ioRotateAttachCharWeapon::SetAttachChar( bool bAttachChar )
{
	m_bAttachChar = bAttachChar;

	if ( !bAttachChar )
	{
		m_dwCheckFireTime = FRAMEGETTIME();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool ioRotateAttachCharWeapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

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

	m_vPrePos = GetPosition();

	AddEffectEvent( m_pFireMesh );
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool ioRotateAttachCharWeapon::IsDropZone( ioPlayStage *pStage )
{
	if ( !pStage )
		return true;

	if( m_dwDropZoneLiveTime > 0 )
	{
		bool bCheckDropZone = false;
		if( m_dwDropZoneCheckStartTime == 0 )
			m_dwDropZoneCheckStartTime = FRAMEGETTIME();

		D3DXVECTOR3 vCurPos = GetPosition();
		if( pStage->GetMapHeightOnlyTerrain( vCurPos.x, vCurPos.z ) <= 0.0f )
			bCheckDropZone = true;

		if( !bCheckDropZone )
			m_dwDropZoneCheckStartTime = 0;

		if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
		{
			SetEndState( pStage );
			return true;
		}

		return false;
	}

	return false;
}

void ioRotateAttachCharWeapon::CheckMove( float fTimePerSec )
{
	if ( m_bAttachChar )
	{
		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
		{
			m_bAttachChar = false;
			return;
		}

		//이동방향, 공격 방향 재설정
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		//오프셋 설정 및 위치 조정		
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + qtRot * m_vAttachOffset;

		SetPosition( vPos );
		SetOrientation( qtRot );
		SetMoveDir( vDir );
	}
	else
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pFireMesh )
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
}

float ioRotateAttachCharWeapon::GetMoveSpeed()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMoveDelayTime > 0 && m_dwCreatedTime+m_dwMoveDelayTime >= dwCurTime )
		return 0.0f;
	
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioRotateAttachCharWeapon::CheckTarget( ioPlayStage *pStage )
{
	if( !pStage || m_TargetInfoList.empty() )	
		return;

	int iCnt = m_TargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			if( !pTarget->CheckGrapplingExceptionState() )
				continue;

			if( !( pTarget->GetState() == CS_SKIP_STUN ) )
				continue;

			float fTimePerSec = g_FrameTimer.GetSecPerFrame();
			float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
			float fRotateAngle = 360.0f * fRate;
			fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

			D3DXQUATERNION	qtRot;
			D3DXVECTOR3 vAxis;
			if( m_bRotateXAxis )
			{
				D3DXVec3Cross( &vAxis, &ioMath::UNIT_Y, &m_vMoveDir );
			}
			else
			{
				vAxis = ioMath::UNIT_Y;
			}
			
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fRotateAngle) );

			D3DXVECTOR3 vDir = m_TargetInfoList[i].m_vDir;
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			m_TargetInfoList[i].m_vDir = vDir;

			// 위치
			D3DXVECTOR3 vNewPos;
			if( m_bRotateXAxis )
			{
				vNewPos = GetPosition();
				vNewPos += ( vDir * m_TargetInfoList[i].m_fLength );
				pTarget->SetWorldPosition( vNewPos );
			}
			else
			{
				vNewPos = GetPosition() + ( vDir * m_TargetInfoList[i].m_fLength );
				pTarget->SetWorldPosition( vNewPos );
			}

			// 위치에 따른 캐릭터 회전
			if( m_bRotateXAxis )
			{
				D3DXVECTOR3 vRotateAxis;
				D3DXVec3Cross( &vRotateAxis, &m_vMoveDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vRotateAxis, &vRotateAxis );
				
				D3DXQUATERNION qtNewRot, qtResult;
				float fDotValue = D3DXVec3Dot( &vDir, &ioMath::UNIT_Y );
				float fAngle = acosf( fDotValue );

				vDir *= -1;
				float fDot = D3DXVec3Dot( &m_vMoveDir, &vDir );
				float fRotateRadian = DEGtoRAD( m_fRotateAngle );
				if( fDot > 0.0f )
					D3DXQuaternionRotationAxis( &qtNewRot, &vRotateAxis, fAngle + fRotateRadian );
				else
					D3DXQuaternionRotationAxis( &qtNewRot, &vRotateAxis, ( 2 * D3DX_PI ) - fAngle + fRotateRadian );

				qtResult = m_qtOriginal * qtNewRot;
				pTarget->SetWorldOrientation( qtResult );
			}
			else
			{
				D3DXQUATERNION	qtNewRot;
				vDir.y = 0.0f;
				ioMath::CalcDirectionQuaternion( qtNewRot, vDir );
				pTarget->SetWorldOrientation( qtNewRot );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ioRotateAttachCharWeapon::SetEndState( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pStage || !pOwner )
		return;

	pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
	DestroyResource( pStage );

	m_BallState = BS_END;

	int iCnt = m_TargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( pTarget )
		{
			int iBuffCnt = m_TargetEndBuff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_TargetEndBuff[j], "", "", NULL );
			}
			iBuffCnt = m_TargetEndRemoveBuff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->RemoveBuff( m_TargetEndRemoveBuff[j] );
			}
		}
	}
}
