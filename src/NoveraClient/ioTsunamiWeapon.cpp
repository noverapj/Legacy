

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioTsunamiWeapon.h"

ActionStopType ioTsunamiWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioTsunamiWeapon::m_DieActionStopType = AST_NONE;

ioTsunamiWeapon::ioTsunamiWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER );

	m_pMeteor = NULL;
	m_FlameState = FS_FALL;

	m_bResistance = false;
	m_dwCurMapEffect = -1;
	m_dwExplosionStartTime = 0;
}

ioTsunamiWeapon::~ioTsunamiWeapon()
{
	if( m_pMeteor )
	{
		LOG.PrintTimeAndLog( 0, "ioTsunamiWeapon::~ioTsunamiWeapon - UnReleased Resource" );
	}
}

bool ioTsunamiWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateMeteor( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioTsunamiWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_TsunamiType = (TsunamiType)rkLoader.LoadInt_e( "tsunami_type", TT_NORMAL );
	m_TsunamiMoveType = (TsunamiMoveType)rkLoader.LoadInt_e( "tsunami_move_type", TMT_NORMAL );

	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_fExplosionInRange = rkLoader.LoadFloat_e( "explosion_in_range", 0.0f );

	m_fTsunamiSpeed = rkLoader.LoadFloat_e( "tsunami_speed", 0.0f );
	m_fTsunamiAngle = rkLoader.LoadFloat_e( "tsunami_angle", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "weapon_up_height", 0.0f );
	m_fUnderHeight = rkLoader.LoadFloat_e( "weapon_under_height", 0.0f );
	
	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "tsunami_effect", "", szBuf, MAX_PATH );
	m_TsunamiEffect = szBuf;
	rkLoader.LoadString_e( "tsunami_end_effect", "", szBuf, MAX_PATH );
	m_TsunamiEndEffect = szBuf;
}

bool ioTsunamiWeapon::CreateMeteor( const ioHashString &szFileName )
{
	m_pMeteor = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMeteor )
		return false;

	GetParentSceneNode()->AttachObject( m_pMeteor );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	AddEffectEvent( m_pMeteor );
	return true;
}

void ioTsunamiWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioTsunamiWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	D3DXVECTOR3 vNewPos( vPos.x, vPos.y+m_fStartHeight, vPos.z );
	m_StartPos = vNewPos;
	SetPosition( vNewPos );

	if( GetOwner() )
	{
		m_qtStartRot = GetOwner()->GetTargetRot();
		SetOrientation( m_qtStartRot );
	}
	else
	{
		m_qtStartRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
	}
}

void ioTsunamiWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_TsunamiType )
	{
	case TT_NORMAL:
		if( m_TsunamiMoveType == TMT_REVERSE )
		{
			if( m_FlameState == FS_FALL )
			{
				Explosion( pStage );
			}
		}
		break;
	case TT_DIRECT:
		if( m_FlameState == FS_FALL )
			Explosion( pStage );
		break;
	}

	switch( m_FlameState )
	{
	case FS_FALL:
		OnFalling( fTimePerSec, pStage );
		break;
	case FS_EXPLOSION:
		OnExplosion( fTimePerSec, pStage );
		break;
	}
}

void ioTsunamiWeapon::OnFalling( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPrePos = GetPosition();
	float fPreMapHeight = pStage->GetMapHeight( vPrePos.x, vPrePos.z, NULL, true, vPrePos.y+5.0f );

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist );

	if( m_pMeteor )
	{
		m_pMeteor->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	D3DXVECTOR3 vPos = GetPosition();
	if( vPos.y < fPreMapHeight )
	{
		vPos.y = fPreMapHeight;
		SetPosition( vPos );
		SetOrientation( m_qtStartRot );

		Explosion( pStage );
	}
}

void ioTsunamiWeapon::OnExplosion( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwExplosionStartTime > 0 && m_dwExplosionStartTime + m_dwLiveTime < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	if( m_fTsunamiSpeed <= 0.0f )
	{
		m_fCurExpRange = m_fExplosionRange;
		m_fCurExpInRange = m_fCurExpRange - m_fExplosionInRange;
		return;
	}

	float fGapTime = (dwCurTime - m_dwExplosionStartTime) / FLOAT1000;
	if( fGapTime <= 0.0f )
	{
		m_fCurExpRange = m_fExplosionRange;
		m_fCurExpInRange = m_fCurExpRange - m_fExplosionInRange;
		return;
	}

	float fValue = m_fTsunamiSpeed * fGapTime;

	switch( m_TsunamiMoveType )
	{
	case TMT_NORMAL:
		{
			m_fCurExpRange = m_fExplosionRange + fValue;
			m_fCurExpInRange = m_fCurExpRange - m_fExplosionInRange;
		}
		break;
	case TMT_REVERSE:
		{
			m_fCurExpRange = m_fExplosionRange - fValue;
			m_fCurExpRange = max( 0.0f, m_fCurExpRange );
			m_fCurExpInRange = m_fCurExpRange - m_fExplosionInRange;
			m_fCurExpInRange = max( 0.0f, m_fCurExpInRange );
		}
		break;
	}

#ifndef SHIPPING
	CheckTestCollisionBox();
#endif
}

void ioTsunamiWeapon::Explosion( ioPlayStage *pStage )
{
	if( !GetOwner() )
		return;
	if( m_FlameState == FS_EXPLOSION )
		return;

	//ClearCollisionList();
	UpdateWorldAttackBox( 0.0f, false );

	if( m_pMeteor )
	{
		g_EffectFactory.DestroyEffect( m_pMeteor );
		m_pMeteor = NULL;
	}

	m_FlameState = FS_EXPLOSION;
	m_dwExplosionStartTime = FRAMEGETTIME();

	m_fCurExpRange = m_fExplosionRange;
	m_fCurExpRange = max( 0.0f, m_fCurExpRange );
	m_fCurExpInRange = m_fCurExpRange - m_fExplosionInRange;
	m_fCurExpInRange = max( 0.0f, m_fCurExpInRange );

	if( pStage )
	{
		if( !m_BoomEffect.IsEmpty() && pStage )
		{
			pStage->CreateMapEffect( m_BoomEffect,
									 GetPosition(),
									 GetScale() );

		}

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_TsunamiEffect,
														   GetPosition(),
														   GetScale() );

		if( pMapEffect )
		{
			m_dwCurMapEffect = pMapEffect->GetUniqueID();
			pMapEffect->SetWorldOrientation( m_qtStartRot );
		}

		ioWeapon::PlayExplosionSound();
	}

	if( m_bSkillDependency )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner ) return;

		pOwner->SetSkillCreateWeaponDependency( GetWeaponBySkillName() );
	}
}

void ioTsunamiWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMeteor )
	{
		g_EffectFactory.DestroyEffect( m_pMeteor );
		m_pMeteor = NULL;
	}

	if( !pStage ) return;

	ioMapEffect *pEndMapEffect = pStage->CreateMapEffect( m_TsunamiEndEffect,
													   GetPosition(),
													   GetScale() );
	if( pEndMapEffect && GetOwner() )
	{
		pEndMapEffect->SetWorldOrientation( m_qtStartRot );
	}

	if( m_dwCurMapEffect != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwCurMapEffect );
		if( pMapEffect )
			pMapEffect->EndEffectForce();

		m_dwCurMapEffect = -1;
	}
}

const ioOrientBox& ioTsunamiWeapon::GetLocalAttackBox() const
{
	if( m_pMeteor )
		return m_pMeteor->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTsunamiWeapon::GetType() const
{
	return WT_TSUNAMI;
}

ActionStopType ioTsunamiWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTsunamiWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioTsunamiWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioTsunamiWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
	}
}

bool ioTsunamiWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_FlameState != FS_EXPLOSION )
		return false;

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = GetPosition();
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fCurExpRange );
	vCenter = kCylinder.GetCenter();

	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff = vCollisionPoint - vCenter;
	vDiff.y = 0.0f;

	float fTargetDist = D3DXVec3Length( &vDiff );

	if( !COMPARE( fTargetDist, m_fCurExpInRange, m_fCurExpRange ) )
		return false;

	// 수평 각 체크
	ioBaseChar *pOwner = GetOwner();
	if( pOwner && m_fTsunamiAngle > 0.0f )
	{
		D3DXVECTOR3 vSightDir = m_qtStartRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vSightDir, &vSightDir );

		D3DXVECTOR3 vDiffDir;
		float fHalfCosine = cosf( DEGtoRAD( m_fTsunamiAngle ) * FLOAT05 );
		D3DXVec3Normalize( &vDiffDir, &vDiff );

		if( D3DXVec3Dot( &vSightDir, &vDiffDir ) < fHalfCosine )
			return false;
	}

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

D3DXVECTOR3 ioTsunamiWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( pEntity )
	{
		D3DXVECTOR3 vMoveDir = pEntity->GetWorldPosition() - GetPosition();
		vMoveDir.y = 0.0f;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		return vMoveDir;
	}

	return D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
}

void ioTsunamiWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_FlameState != FS_FALL )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		ioBaseChar *pChar = ToBaseChar( pTarget );
		if( pChar )
		{
			if( GetTeam() != pTarget->GetTeam() )
				bCollision = true;
			else if( pChar->GetCharName() == GetOwnerName() )
				bCollision = true;
		}
		else
			bCollision = true;
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		Explosion( pStage );
		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

DWORD ioTsunamiWeapon::GetCollisionEnableTime() const
{
	if( m_FlameState == FS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioTsunamiWeapon::CheckTestCollisionBox()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_fTsunamiAngle < 0.0f )
		return;
	else if( m_fTsunamiAngle == 0.0f )
		m_fTsunamiAngle = 360.0f;

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = GetPosition();
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fCurExpRange );
	vCenter = kCylinder.GetCenter();

	float fHalfAngle = m_fTsunamiAngle * FLOAT05;

	if( m_fTsunamiAngle == 360.0f )
	{
		D3DXVECTOR3 vMinPos, vMaxPos;
		vMaxPos = vMinPos = vCenter;

		vMaxPos.x += m_fCurExpRange;
		vMaxPos.y += m_fUpHeight;
		vMaxPos.z += m_fCurExpRange;

		vMinPos.x -= m_fCurExpRange;
		vMinPos.y -= m_fUnderHeight;
		vMinPos.z -= m_fCurExpRange;

		ioOrientBox kBuffBox;
		kBuffBox.SetBoxByMinMax( vMinPos, vMaxPos );

#ifndef SHIPPING
		kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif
	}
	else
	{
		// RightDir
		D3DXQUATERNION qtRotR;
		D3DXQuaternionRotationAxis( &qtRotR, &ioMath::UNIT_Y, DEGtoRAD(fHalfAngle) );

		D3DXVECTOR3 vRightDir = qtRotR * vSightDir;
		D3DXVec3Normalize( &vRightDir, &vRightDir );

		// LeftDir
		D3DXQUATERNION qtRotL;
		D3DXQuaternionRotationAxis( &qtRotL, &(-ioMath::UNIT_Y), DEGtoRAD(fHalfAngle) );

		D3DXVECTOR3 vLeftDir = qtRotL * vSightDir;
		D3DXVec3Normalize( &vLeftDir, &vLeftDir );

#ifndef SHIPPING
		ioBaseChar::m_vRangeBoxArrayForTest[0] = vCenter + vRightDir*m_fCurExpRange;
		ioBaseChar::m_vRangeBoxArrayForTest[0].y += m_fUpHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[3] = vCenter + vRightDir*m_fCurExpInRange;
		ioBaseChar::m_vRangeBoxArrayForTest[3].y += m_fUpHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[4] = vCenter + vRightDir*m_fCurExpRange;
		ioBaseChar::m_vRangeBoxArrayForTest[4].y -= m_fUnderHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[7] = vCenter + vRightDir*m_fCurExpInRange;
		ioBaseChar::m_vRangeBoxArrayForTest[7].y -= m_fUnderHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[1] = vCenter + vLeftDir*m_fCurExpRange;
		ioBaseChar::m_vRangeBoxArrayForTest[1].y += m_fUpHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[2] = vCenter + vLeftDir*m_fCurExpInRange;
		ioBaseChar::m_vRangeBoxArrayForTest[2].y += m_fUpHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[5] = vCenter + vLeftDir*m_fCurExpRange;
		ioBaseChar::m_vRangeBoxArrayForTest[5].y -= m_fUnderHeight;

		ioBaseChar::m_vRangeBoxArrayForTest[6] = vCenter + vLeftDir*m_fCurExpInRange;
		ioBaseChar::m_vRangeBoxArrayForTest[6].y -= m_fUnderHeight;
#endif
	}
}


