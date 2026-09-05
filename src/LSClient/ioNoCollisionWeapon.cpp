

#include "stdafx.h"

#include "ioNoCollisionWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioNoCollisionWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioNoCollisionWeapon::m_DieActionStopType = AST_NONE;

ioNoCollisionWeapon::ioNoCollisionWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;

	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	m_fCurMoveSpeed = 0.0f;

	m_WeaponState = WS_MOVE;

	m_vTargetInfo.clear();
}

ioNoCollisionWeapon::~ioNoCollisionWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioNoCollisionWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}
}

bool ioNoCollisionWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioNoCollisionWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fMaxMoveRange = rkLoader.LoadFloat_e( "max_move_range", 0.0f );
	m_fSencingRange = rkLoader.LoadFloat_e( "sencing_range", 0.0f );
	m_dwSencingTime = rkLoader.LoadInt_e( "sencing_time", 0 );

	m_iMaxTargetCnt = rkLoader.LoadInt_e( "max_target_cnt", 0 );
	m_iMaxFireCnt = rkLoader.LoadInt_e( "max_fire_cnt", 1 );
	m_dwFireTime = rkLoader.LoadInt_e( "fire_time", 0 );

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_type", NO_WEAPON_ATTRIBUTE );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString( "attack_type_wounded", "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_type_wounded_time", 0 );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_type_wounded_loop_ani", false );
}

bool ioNoCollisionWeapon::CreateFireBall( const ioHashString &szFileName )
{
	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );
	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioNoCollisionWeapon::CreateFileBall - Not Has Owner" );
		return false;
	}

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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
 
void ioNoCollisionWeapon::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioNoCollisionWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioNoCollisionWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioNoCollisionWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_WeaponState )
	{
	case WS_MOVE:
		{
			float fMoveDist = GetMoveSpeed() * fTimePerSec;

			D3DXVECTOR3 vMove = m_vMoveDir * fMoveDist;

			GetParentSceneNode()->Translate( vMove );
			m_fCurMoveRange += fMoveDist;

			if( m_fCurMoveRange < m_fMaxMoveRange )
			{
				UpdateWorldAttackBox( fTimePerSec, false );
			}
			else
			{
				UpdateWorldAttackBox( fTimePerSec, true );

				m_WeaponState = WS_SENCING;
				m_dwSencingEndTime = FRAMEGETTIME() + m_dwSencingTime;
			}
		}
		break;
	case WS_SENCING:
		{
			if( m_dwSencingEndTime < FRAMEGETTIME() )
			{
				ProcessFire();

				if( IsEnableWeaponDie() )
				{
					SetGraceClose();
					SetWeaponDead();

					if( pStage )
						pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );

					return;
				}
			}
			else
			{
				CheckTarget();
				ProcessFire();
			}
		}
		break;
	}
	

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioNoCollisionWeapon::SetGraceClose()
{
	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
}

void ioNoCollisionWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioNoCollisionWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioNoCollisionWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioNoCollisionWeapon::GetType() const
{
	return WT_NO_COLLISION_WEAPON;
}

float ioNoCollisionWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;

	return m_fCurMoveSpeed;
}

float ioNoCollisionWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioNoCollisionWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
	
	if( m_bSkillDependency )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner ) return;

		pOwner->SetSkillCreateWeaponDependency( GetWeaponBySkillName() );
	}
}

ActionStopType ioNoCollisionWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioNoCollisionWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioNoCollisionWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	return;
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

void ioNoCollisionWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	return;
}

bool ioNoCollisionWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	return false;
}

void ioNoCollisionWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iCnt = 0;
	rkPacket >> iCnt;

	for( int i=0; i<iCnt; ++i )
	{
		ioHashString szTargetName;
		rkPacket >> szTargetName;
		if( !IsAlreadyTarget( szTargetName ) )
		{
			m_vTargetInfo.push_back( TargetInfo( szTargetName ) );
		}
	}
}

void ioNoCollisionWeapon::CheckTarget()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	int iCnt = m_vTargetInfo.size();
	if( m_iMaxTargetCnt <= iCnt )
		return;

	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( !pStage )
		return;

	ioHashStringVec szNewTarget;
	szNewTarget.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		if( m_iMaxTargetCnt <= (int)m_vTargetInfo.size() )
			continue;

		ioBaseChar *pTarget = *iter;
		if( !pTarget )
			continue;

		if( pTarget->GetCharName() == m_OwnerName )
			continue;

		if( pTarget->GetTeam() == GetTeam() )
			continue;

		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			continue;

		if( !pTarget->IsEnableTarget() )
			continue;

		if( IsAlreadyTarget( pTarget->GetCharName() ) )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - GetPosition();
		float fDistance = D3DXVec3Length( &vDiff );

		if( fDistance > m_fSencingRange )
			continue;

		m_vTargetInfo.push_back( TargetInfo( pTarget->GetCharName() ) );
		szNewTarget.push_back( pTarget->GetCharName() );
	}

	if( !szNewTarget.empty() )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			int iCnt = szNewTarget.size();

			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << iCnt;
			for( int i=0; i<iCnt; ++i )
			{
				kPacket << szNewTarget[i];
			}
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioNoCollisionWeapon::ProcessFire()
{
	int iCnt = m_vTargetInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vTargetInfo[i].is_fire_end )
			continue;

		if( m_vTargetInfo[i].next_fire_time < FRAMEGETTIME() )
		{
			FireWeapon( m_vTargetInfo[i].name );

			m_vTargetInfo[i].next_fire_time = FRAMEGETTIME() + m_dwFireTime;
			m_vTargetInfo[i].fire_cnt++;

			if( m_vTargetInfo[i].fire_cnt >= m_iMaxFireCnt )
				m_vTargetInfo[i].is_fire_end = true;
		}
	}
}

void ioNoCollisionWeapon::FireWeapon( ioHashString szTargetName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	if( !pTarget )
		return;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_WeaponInfo.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_WeaponInfo.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_WeaponInfo.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_WeaponInfo.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_WeaponInfo.m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_SKILL_ATTACK;

	float fRate = 0.5f;

	if( CheckDownState(pTarget)  )
	{
		fRate = 0.0f;
	}

	D3DXVECTOR3 vDir = pTarget->GetMidPositionByRate( fRate ) - GetPosition();
	D3DXVec3Normalize( &vDir, &vDir );
	pOwner->ExtendFireExplicit( kFireTime, GetPosition(), vDir, GetCreateItem() );
}


bool ioNoCollisionWeapon::IsAlreadyTarget( ioHashString szName )
{
	auto FindTarget = [szName]( TargetInfo kInfo )->bool{ return kInfo.name == szName; };
	vTargetInfo::iterator itTarget = std::find_if( m_vTargetInfo.begin(), m_vTargetInfo.end(), FindTarget );
	if( itTarget != m_vTargetInfo.end() )
	{
		return true;
	}

	return false;
}

bool ioNoCollisionWeapon::IsEnableWeaponDie()
{
	int iCnt = m_vTargetInfo.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( !m_vTargetInfo[i].is_fire_end )
			return false;
	}

	return true;
}

bool ioNoCollisionWeapon::CheckDownState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}