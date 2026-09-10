

#include "stdafx.h"

#include "ioRotateTargetWeapon.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioRotateTargetWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioRotateTargetWeapon::m_DieActionStopType = AST_NONE;

ioRotateTargetWeapon::ioRotateTargetWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_DEF_NO_SOUND );

	m_TargetInfoList.clear();
	m_BallState = BS_MOVE;
}

ioRotateTargetWeapon::~ioRotateTargetWeapon()
{
}

bool ioRotateTargetWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		m_LocalAttackBox.SetExtents( 0, m_fCollisionRange );
		m_LocalAttackBox.SetExtents( 1, m_fCollisionRange );
		m_LocalAttackBox.SetExtents( 2, m_fCollisionRange );
		UpdateWorldAttackBox( 0.0f, false );

		return true;
	}

	return false;
}

void ioRotateTargetWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	m_dwRotateSpeed = (float)rkLoader.LoadFloat_e( "target_rotate_speed", 0.0f );

	m_TargetEndBuff.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "target_end_buff_count", 0 );
	for( int i=0; i<iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "target_end_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetEndBuff.push_back( szBuf );
	}

	m_fTargetOffset       = rkLoader.LoadFloat_e( "target_offset", 0.0f );
	m_fTargetHeightOffset = rkLoader.LoadFloat_e( "target_height_offset", 0.0f );
	m_fCollisionRange     = rkLoader.LoadFloat_e( "collision_range", 0.0f );
}

void ioRotateTargetWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioRotateTargetWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
	m_LocalAttackBox.SetExtents( 0, m_fCollisionRange );
	m_LocalAttackBox.SetExtents( 1, m_fCollisionRange );
	m_LocalAttackBox.SetExtents( 2, m_fCollisionRange );
	UpdateWorldAttackBox( 0.0f, false );
}

void ioRotateTargetWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	m_vAttackDir = m_vMoveDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioRotateTargetWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_BallState )
	{
	case BS_MOVE:
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;
				D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
				if( m_iDummyIndex > 0 )
				{
					ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
					if( pDummy )
					{
						vPos = pDummy->GetMidPositionByRate();
						vDir = pDummy->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
						vPos += vDir;
					}
					else
					{
						SetEndState( pStage );
						return;
					}
				}
				else
				{
					vPos = pOwner->GetMidPositionByRate();
					vDir = pOwner->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					vPos += vDir;
				}
				SetPosition( vPos );
			}
			else
			{
				SetEndState( pStage );
				return;
			}

			UpdateWorldAttackBox( fTimePerSec, false );
			CheckTarget( pStage );
		}
		break;
	case BS_END:
		SetWeaponDead();
		break;
	}
}

void ioRotateTargetWeapon::DestroyResource( ioPlayStage *pStage )
{
}

ioWeapon::WeaponType ioRotateTargetWeapon::GetType() const
{
	return WT_ROTATE_TARGET_WEAPON;
}

ActionStopType ioRotateTargetWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRotateTargetWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioRotateTargetWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vAttackDir;
}

const ioOrientBox& ioRotateTargetWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioRotateTargetWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	bool bCollision = CheckCollision( pEntity );
	CheckPiercing( bCollision );

	if( bCollision && !IsAlreadyCollisioned(pEntity) )
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
				vDiff.y = 0;

				D3DXVec3Normalize( &vDiff, &vDiff );
				kInfo.m_vDir = vDiff;

				m_TargetInfoList.push_back( kInfo );

				if( pTarget->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_WEAPON_SYNC );
					kPacket << GetOwnerName();
					kPacket << m_dwWeaponIdx;
					kPacket << SS_ADD_TARGET;
					kPacket << kInfo.m_TargetName;
					kPacket << kInfo.m_vDir;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	return bCollision;
}

void ioRotateTargetWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
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
	case SS_EXCEPTION:
		{
			ioHashString szTargetName;
			rkPacket >> szTargetName;

			TargetInfoList::iterator iter = m_TargetInfoList.begin();
			while( iter != m_TargetInfoList.end() )
			{
				TargetInfo kInfo = *iter;
				if( kInfo.m_TargetName == szTargetName )
					iter = m_TargetInfoList.erase( iter );
				else
					++iter;
			}
		}
		break;
	}	
}

void ioRotateTargetWeapon::SetEndState( ioPlayStage *pStage )
{
	if( !pStage ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_BallState = BS_END;
	
	int iCnt = m_TargetInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetInfoList[i].m_TargetName );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			int iBuffCnt = m_TargetEndBuff.size();
			for( int j=0; j<iBuffCnt; ++j )
			{
				pTarget->AddNewBuff( m_TargetEndBuff[j], pOwner->GetCharName(), "", NULL );
			}

			if( pTarget->IsNeedSendNetwork() )
			{
				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -WeaponAddBuff7 : %d, %d", GetAttributeIndex(), FRAMEGETTIME() );
				}
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pTarget->GetCharName();
				kPacket << GetOwnerName();
				kPacket << GetCreateItem();
				kPacket << pTarget->GetRandomSeed();

				kPacket << false;		// 타격횟수 체크
				kPacket << 0.0f;
				kPacket << 0.0f;
				kPacket << 0.0f;

				kPacket << COWT_WEAPON;			// Check CloseOrderBuff(12.07.13 수정)
				kPacket << GetOwnerName();
				kPacket << GetWeaponIndex();

				kPacket << iBuffCnt;

				for( int j=0; j<iBuffCnt; ++j )
				{
					kPacket << m_TargetEndBuff[j];
				}

				kPacket << false;					// Use Force

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioRotateTargetWeapon::CheckTarget( ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( m_TargetInfoList.empty() )	return;

	TargetInfoList::iterator iter = m_TargetInfoList.begin();
	while( iter != m_TargetInfoList.end() )
	{
		TargetInfo &kInfo = *iter;
		ioBaseChar *pTarget = pStage->GetBaseChar( kInfo.m_TargetName );
		if( !pTarget )
		{
			iter = m_TargetInfoList.erase( iter );
			continue;
		}

		if( !pTarget->CheckGrapplingExceptionState() && pTarget->IsNeedProcess() )
		{
			iter = m_TargetInfoList.erase( iter );
			SendExceptionTarget( pTarget );
			continue;
		}

		if( pTarget->GetState() != CS_SKIP_STUN && pTarget->IsNeedProcess() )
		{
			iter = m_TargetInfoList.erase( iter );
			SendExceptionTarget( pTarget );
			continue;
		}

		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
		float fRotateAngle = 360.0f * fRate;
		fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

		D3DXQUATERNION	qtRot;
		D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fRotateAngle) );

		D3DXVECTOR3 vDir = kInfo.m_vDir;
		vDir = qtRot * vDir;
		D3DXVec3Normalize( &vDir, &vDir );
		kInfo.m_vDir = vDir;

		// 위치
		D3DXVECTOR3 vNewPos = GetPosition() + ( vDir * m_fTargetOffset );
		vNewPos.y += m_fTargetHeightOffset;
		pTarget->SetWorldPosition( vNewPos );

		// 위치에 따른 캐릭터 회전
		D3DXQUATERNION	qtNewRot;
		vDir.y = 0.0f;
		ioMath::CalcDirectionQuaternion( qtNewRot, vDir );
		pTarget->SetWorldOrientation( qtNewRot );

		++iter;
	}
}

bool ioRotateTargetWeapon::CheckCollision( ioPlayEntity *pEntity )
{
	if( !pEntity )
		return false;

	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();
	float fCollisionRange = m_fCollisionRange * m_fOwnerChargeRangeRate;
	D3DXVECTOR3 vPoint = ioMath::VEC3_ZERO;
	if( pEntity->CheckSpherePoint( vPos, fCollisionRange, vPoint ) )
		return true;

	return false;
}

void ioRotateTargetWeapon::SendExceptionTarget( ioBaseChar *pTarget )
{
	if( !pTarget )
		return;
	
	if( pTarget->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SS_EXCEPTION;
		kPacket << pTarget->GetCharName();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}