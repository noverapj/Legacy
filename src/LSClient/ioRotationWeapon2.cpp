
#include "stdafx.h"

#include "ioRotationWeapon2.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioRotationWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioRotationWeapon2::m_DieActionStopType = AST_NONE;

ioRotationWeapon2::ioRotationWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	m_pFireBall = NULL;

	m_iCreateItemIndex = 0;

	m_bEnableDownState = false;
	m_bRemoveByWoundState = false;

	m_bSetGraceClose = false;
	m_bFirst = true;
	m_stPassTargetName.Clear();
	m_State = WS_ROTATE;
}

ioRotationWeapon2::~ioRotationWeapon2()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioRotationWeapon2::~ioRotationWeapon2 - UnReleased Resouce" );
	}
}

bool ioRotationWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioRotationWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_bOnlyCharCollision	= rkLoader.LoadBool_e( "only_char_collision", false );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_fRotationTime = rkLoader.LoadFloat_e( "rotation_time", 0.0f );

	m_bEnableDownState = rkLoader.LoadBool_e( "enable_down_state", false );

	m_bRemoveBySkillEnd = rkLoader.LoadBool_e( "remove_by_skill_end", false );

	m_bIndependenceChar = rkLoader.LoadBool_e( "independence_char", false );
}

bool ioRotationWeapon2::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioRotationWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_vPreWorldPosition = GetPosition();

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;
	ioBaseChar *Target = pOwner->GetBaseChar( m_stPassTargetName );
	if ( !Target )
		return;
	
	m_vTargetPos = Target->GetMidPositionByRate();
	m_vCenterPos = m_vTargetPos;
	m_vDiffPos = vPos - m_vTargetPos;

	m_qtStartRot = Target->GetTargetRot();
	
}

void ioRotationWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioRotationWeapon2::SetLiveTime( DWORD dwTime )
{
}

void ioRotationWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();
	}

	if( m_bRemoveBySkillEnd )
	{
		ioHashString szSkillName = GetWeaponBySkillName();
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			ioSkill *pSkill = pOwner->GetCurActiveSkill();
			if( !pSkill )
			{
				SetWeaponDead();
			}
			else if( pSkill && pSkill->GetName() != szSkillName )
			{
				SetWeaponDead();
			}
		}
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		if ( m_State != WS_EXPLOSION )
		{
			UpdateWorldAttackBox( fTimePerSec, false );
			if( m_pFireBall )
			{
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );
			}
			SetWeaponDead();
		}
		
		return;
	}

	if( !CheckOwnerState() )
	{
		SetWeaponDead();
		return;
	}

	if ( !CheakTargetState() )
	{
		SetWeaponDead();
		return;
	}

	switch( m_State )
	{
	case WS_ROTATE:
		{
			OnRotation( fTimePerSec );
		}
		break;
	case WS_EXPLOSION:
		{
			OnExplosion();
		}
		break;
	}

	
	m_bFirst = false;

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioRotationWeapon2::OnRotation( float fTimePerSec )
{
	if( !m_bFirst )
	{
		// 캐릭터 기준으로 위치 갱신
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			SetWeaponDead();
			return;
		}
		ioBaseChar *Target = pOwner->GetBaseChar( m_stPassTargetName );
		if ( !Target )
			return;
		m_vTargetPos = Target->GetMidPositionByRate();
		D3DXVECTOR3 vCurCencter = m_vTargetPos;
		if( m_bIndependenceChar )
			vCurCencter = m_vCenterPos;

		// 회전처리
		if( m_fRotationTime > 0.0f )
		{
			float fGapTime = fTimePerSec * FLOAT1000;
			float fRotationRate = fGapTime / m_fRotationTime;
			float fAngle = 360.0f * fRotationRate;

			fAngle = ioMath::ArrangeHead( fAngle );

			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );
			SetOrientation( qtAngle );

			m_vDiffPos = qtAngle * m_vDiffPos;

			D3DXVECTOR3 vNewPos = vCurCencter + m_vDiffPos;
			SetPosition( vNewPos );
		}
		else
		{
			D3DXQUATERNION qtCurRot = Target->GetTargetRot();

			D3DXVECTOR3 vStartDir = m_qtStartRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vStartDir.y = 0.0f;
			D3DXVec3Normalize( &vStartDir, &vStartDir );

			D3DXVECTOR3 vCurDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vCurDir.y = 0.0f;
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			D3DXVECTOR3 vDiff = m_vDiffPos;

			if( vCurDir != vStartDir )
			{
				float fDotValue = D3DXVec3Dot( &vStartDir, &vCurDir );
				float fGapAngle = acosf(fDotValue);

				D3DXVECTOR3 vAxis = ioMath::UNIT_Y;
				if( COMPARE( fDotValue, -0.98f, 0.98f ) )
				{
					D3DXVec3Cross( &vAxis, &vStartDir, &vCurDir );
					D3DXVec3Normalize( &vAxis, &vAxis );
				}

				D3DXQUATERNION qtNewRot;
				D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, fGapAngle );

				vDiff = qtNewRot * m_vDiffPos;
			}

			D3DXVECTOR3 vNewPos = vCurCencter + vDiff;
			SetPosition( vNewPos );

			SetOrientation( qtCurRot );
		}

		UpdateWorldAttackBox( fTimePerSec, false );
	}

}

void ioRotationWeapon2::OnExplosion()
{
	if( FRAMEGETTIME() - m_dwExplosionStart > 100 )
	{
		SetWeaponDead();	
	}
}

void ioRotationWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioRotationWeapon2::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	D3DXVECTOR3 vAttackDir;

	if( m_fRotationTime > 0.0f )
	{
		D3DXVec3Cross( &vAttackDir,
			&D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
			&m_vDiffPos );
	}
	else
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVec3Normalize( &vAttackDir, &vAttackDir );

	return vAttackDir;
}

const ioOrientBox& ioRotationWeapon2::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioRotationWeapon2::GetType() const
{
	return WT_ROTATE2;
}

ActionStopType ioRotationWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRotationWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioRotationWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
			GetScale(),
			GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
			GetScale(),
			GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioRotationWeapon2::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );
}

const WeaponAttribute* ioRotationWeapon2::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

bool ioRotationWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	if( m_State != WS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	float fCurExplosionRange = m_fExplosionRange;
	if( m_fOwnerChargeExplosionRate > FLOAT1 )
		fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;

	if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
	{		
		return true;
	}
	
	return false;
}

bool ioRotationWeapon2::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioRotationWeapon2::SetWeaponDead()
{
	StopLoopSound();
	m_bLive = false;
	m_State = WS_ROTATE;
}

bool ioRotationWeapon2::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	if( !m_bEnableDownState && pOwner->IsApplyDownState( false ) )
		return false;

	return true;
}

bool ioRotationWeapon2::CheakTargetState()
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return false;
	
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_stPassTargetName );
	if ( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;
		
	return true;
}

void ioRotationWeapon2::SetPassTargetName( const ioHashString& stPassName )
{
	m_stPassTargetName = stPassName;
}

void ioRotationWeapon2::SetExplosionState( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	m_State = WS_EXPLOSION;

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vScale = GetScale();

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect,
			GetPosition(),
			vScale );

		ioWeapon::PlayExplosionSound();
	}

	m_dwExplosionStart = FRAMEGETTIME();
}

void ioRotationWeapon2::CheckPreCollision( ioPlayEntity *pEntity, ioPlayStage *pStage )
{
	if( m_State == WS_EXPLOSION )
		return;

	bool bCollision = false;
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( pEntity->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( pTarget && pTarget->IsNeedProcess() )
		{
			if( !m_bOnlyCharCollision &&
				( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) ||
				ToDummyChar(pEntity) || ToWallZone(pEntity) ) )
			{
				bCollision = true;
			}
			else if( pTarget && pTarget->IsNeedProcess() )
			{
				if( GetTeam() != pTarget->GetTeam() )
				{
					bCollision = true;
				}
				else if( GetOwner() != pTarget )
				{
					if( pTarget->IsEnableTeamCollision( this ) )
					{
						bCollision = true;
					}
				}
			}
		}
	}

	if( bCollision )
	{
		SetExplosionState( pStage );
		if( pTarget && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << m_State;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRotationWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;
	switch ( iState )
	{
	case WS_EXPLOSION:
		SetExplosionState( pStage );
		break;
	}	
}