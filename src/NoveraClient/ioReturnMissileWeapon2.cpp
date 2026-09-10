

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioReturnMissileWeapon2.h"

ActionStopType ioReturnMissileWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioReturnMissileWeapon2::m_DieActionStopType = AST_NONE;


ioReturnMissileWeapon2::ioReturnMissileWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_ROTATE;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_bResistance = false;

	m_vMoveDir = ioMath::VEC3_ZERO;

	m_bAddedBuffList = false;
}

ioReturnMissileWeapon2::~ioReturnMissileWeapon2()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioReturnMissileWeapon2::~ioReturnMissileWeapon2 - UnReleased Resource" );
	}
}

void ioReturnMissileWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "fire_sound", "", szBuf, MAX_PATH );
	m_FireSound = szBuf;

	//최대 life time
	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	//최대 이동거리
	m_fMaxMoveRange = rkLoader.LoadFloat_e( "max_range", 0.f );

	//이 거리만큼 접근해 있다면 맞은걸로 간주
	m_fCheckMaxRange = rkLoader.LoadFloat_e( "touch_range", 0.f );

	m_fCurMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.f );

	int nBuffCnt = rkLoader.LoadInt_e( "buff_cnt", 0 );
	m_AddBuffList.reserve( nBuffCnt );
	for( int i = 0; i < nBuffCnt; i++ )
	{
		sprintf_e( szKey, "add_buff%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString stBuffName = szBuf;
		m_AddBuffList.push_back( stBuffName );
	}
}

bool ioReturnMissileWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioReturnMissileWeapon2::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

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

void ioReturnMissileWeapon2::SetLiveTime( DWORD dwTime )
{
}

void ioReturnMissileWeapon2::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	//if( m_vMoveDir != ioMath::VEC3_ZERO )
		//return;

	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;

	qtRot = Help::ConvertDirToQuaternion( vDir );

	//D3DXQUATERNION qtRot;
	//ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioReturnMissileWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );

	m_vPrePos = vPos;

	if( !GetOwner() )
	{
		SetWeaponDead();
		return;
	}

	D3DXVECTOR3 vDiff = GetOwner()->GetMidPositionByRate() - vPos;
	D3DXVec3Normalize( &vDiff, &vDiff );
	m_vMoveDir = vDiff;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
	SetOrientation( qtRot );
}

void ioReturnMissileWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !GetOwner() )
	{
		SetWeaponDead();
		return;
	}
	// 생존시간을 넘길 경우 Weapon을 소멸시킨다.
	if( (m_dwCreatedTime + m_dwLiveTime) < FRAMEGETTIME() )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pFireBall )
			m_pFireBall->Update( fTimePerSec * FLOAT1000 );

		SetWeaponDead();
		return;
	}

	if( m_fCurMoveRange >= m_fMaxMoveRange )
	{
		SetWeaponDead();
		return;
	}

	D3DXVECTOR3 vPos= GetPosition();
	D3DXVECTOR3 vCreatorPos = GetOwner()->GetMidPositionByRate();
	D3DXVECTOR3 vDiff = vCreatorPos - vPos;
	float fLenthDiff = D3DXVec3Length( &vDiff );
	if( fLenthDiff < m_fCheckMaxRange )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pFireBall )
			m_pFireBall->Update( fTimePerSec * FLOAT1000 );

		SetWeaponDead();
		return;
	}

	float fMoveDist = m_fCurMoveSpeed * fTimePerSec;
	D3DXVec3Normalize( &m_vMoveDir, &vDiff );

	//회전때문에
	SetMoveDir( m_vMoveDir );

	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	UpdateWorldAttackBox( fTimePerSec, false );


	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}


void ioReturnMissileWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioReturnMissileWeapon2::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioReturnMissileWeapon2::GetType() const
{
	return WT_RETURN_MISSILE_WEAPON2;
}


bool ioReturnMissileWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	ioBaseChar *pTarget = ToBaseChar( pEntity );
	if( pTarget && pTarget == GetOwner() )
	{
		bool bCollision = false;

		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
			bCollision = true;

		CheckPiercing( bCollision );
		return bCollision;
	}

	return false;
}


void ioReturnMissileWeapon2::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
}

void ioReturnMissileWeapon2::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioReturnMissileWeapon2::SetWeaponDead()
{
	if( m_State == TS_ROTATE && GetOwner() && GetOwner()->GetCreator() )
	{
		GetOwner()->GetCreator()->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox().GetCenter(), GetScale() );
		PlayExplosionSound();
	}

	StopLoopSound();
	m_bLive = false;

	if( GetOwner() && !m_bAddedBuffList )
	{
		m_bAddedBuffList = true;
		for( int i = 0; i < (int)m_AddBuffList.size(); i++ )
		{
			ioHashString stBuffName = m_AddBuffList[i];
			GetOwner()->AddNewBuff( stBuffName, GetOwner()->GetCharName(), "", NULL );
		}
	}

}

void ioReturnMissileWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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


bool ioReturnMissileWeapon2::IsIgnoreMapCollision() const
{
	if( m_State == TS_ROTATE )
		return true;
	return false;
}

ActionStopType ioReturnMissileWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioReturnMissileWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}