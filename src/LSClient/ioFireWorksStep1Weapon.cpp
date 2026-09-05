

#include "stdafx.h"

#include "ioFireWorksStep1Weapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioFireWorksStep1Weapon::m_ActionStopType = AST_NONE;
ActionStopType ioFireWorksStep1Weapon::m_DieActionStopType = AST_NONE;

ioFireWorksStep1Weapon::ioFireWorksStep1Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION |
		WP_WOUND_NO_SOUND |
		WP_DEF_NO_SOUND );

	m_pIceCoreMesh = NULL;
	m_IceState = DIS_FLOATING;

	m_bFirst = true;
	m_bResistance = false;
	m_bArleadyCallWeapon = false;
	m_bSetCallAxisChange = false;

	m_fCurMoveSpeed = 0.0f;
	m_fTotalMoveAmount = 0;

	m_OrientationState = OS_NONE;
}

ioFireWorksStep1Weapon::~ioFireWorksStep1Weapon()
{
	if( m_pIceCoreMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioFireWorksStep1Weapon::~ioFireWorksStep1Weapon - UnReleased Resouce" );
	}
}

bool ioFireWorksStep1Weapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetIceCoreMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFireWorksStep1Weapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwLiveTime = (DWORD)rkLoader.LoadInt_e( "live_time", 0 );
	m_fMaxMoveLenght = rkLoader.LoadFloat_e( "max_move_length", 0 );
	m_fAngleV = rkLoader.LoadFloat_e( "angle_v", 0 );
	m_fAngleH = rkLoader.LoadFloat_e( "angle_h", 0 );


	m_bPlayWoundSound = rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );

	LoadCallAttribute( rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );

	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );
}

void ioFireWorksStep1Weapon::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );
}

void ioFireWorksStep1Weapon::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "call_attribute_fire_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_CallAttFireList.clear();
	m_CallAttFireList.reserve( iCnt );

	for( int i= 0; i < iCnt; ++i )
	{
		ChangeDirInfo kInfo;

		wsprintf_e( szBuf, "call_attribute_fire%d_range", i+1 );
		kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_speed", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_h", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_angle_v", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_CallAttFireList.push_back( kInfo );
	}

	std::sort( m_CallAttFireList.begin(), m_CallAttFireList.end(), ChangeDirSort() );
}

bool ioFireWorksStep1Weapon::SetIceCoreMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pIceCoreMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pIceCoreMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pIceCoreMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pIceCoreMesh->GetSkeletonName().IsEmpty() && m_pIceCoreMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pIceCoreMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pIceCoreMesh->GetCollisionBox();

	AddEffectEvent( m_pIceCoreMesh );
	return true;
}

void ioFireWorksStep1Weapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pIceCoreMesh )
	{
		g_EffectFactory.DestroyEffect( m_pIceCoreMesh );
		m_pIceCoreMesh = NULL;
	}
}

const ioOrientBox& ioFireWorksStep1Weapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioFireWorksStep1Weapon::GetType() const
{
	return WT_FIRE_WORKS_STEP1_WEAPON;
}

ActionStopType ioFireWorksStep1Weapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFireWorksStep1Weapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioFireWorksStep1Weapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	//m_vMoveDir = vDir;

	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 vDirRotate = vDir;
	D3DXVec3Normalize(&vDirRotate, &vDirRotate);
	float fAngleH = ioMath::ArrangeHead( m_fAngleH );
	float fAngleV = ioMath::ArrangeHead( m_fAngleV );

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vDirRotate, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vDirRotate = qtRot * vDirRotate;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	vDirRotate = qtRot * vDirRotate;
	//////////////////////////////////////////////////////////////////////////

	m_vMoveDir = vDirRotate;


	switch(m_OrientationState)
	{
	case OS_NONE:
		{
			ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	case OS_ORIENTATION:
		{
			qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	}
}

void ioFireWorksStep1Weapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pIceCoreMesh && m_pIceCoreMesh->GetSkeletonName().IsEmpty() && m_pIceCoreMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioFireWorksStep1Weapon::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) && !m_bPlayWoundSound )
		return;

	const ioHashString &szSound = GetAttribute()->m_WoundedSound;

	if( ToBaseChar( pEntity ) )
	{
		std::string szNewSound = ToBaseChar(pEntity)->GetWoundedSoundName( szSound );
		g_SoundMgr.PlaySound( ioHashString(szNewSound.c_str()), GetParentSceneNode() );
	}
	else
	{
		g_SoundMgr.PlaySound( szSound, GetParentSceneNode() );
	}

	StopLoopSound();
}

void ioFireWorksStep1Weapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar* pOwner = GetOwner();

	switch( m_IceState )
	{
	case DIS_FLOATING:
		{
			DWORD dwLooseTime = 0;
			if( pOwner )
				dwLooseTime = GetOwner()->GetActionStopCurAdd();

			if( CheckMoveRange( pOwner, pStage, fTimePerSec ) )
				Explosion( pStage );

			if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < dwCurTime )
				Explosion( pStage );
			else
				OnFloating( fTimePerSec, pStage );
		}
		break;
	case DIS_EXPLOSION:
		m_IceState = DIS_EXPLOSION_END;
		break;
	case DIS_EXPLOSION_END:
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}


bool ioFireWorksStep1Weapon::CheckMoveRange( ioBaseChar* pOwner, ioPlayStage *pStage, float fTimePerSec )
{	
	float fMaxRange = GetMaxRange();

	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;

	float fMoveAmt = GetMoveSpeed() * fTimePerSec;
	m_fTotalMoveAmount += fMoveAmt;

	if( fMaxRange =! 0 && fMaxRange < m_fTotalMoveAmount )
		return true;

	return false;
}

bool ioFireWorksStep1Weapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	// 
	if( m_IceState != DIS_FLOATING )
		return true;

	if( m_bDisableMapCollision )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{	
		m_bCollision = true;
		Explosion( pStage );
		return true;
	}

	return false;
}

void ioFireWorksStep1Weapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;

		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}

	m_bFirst = false;

	if( m_pIceCoreMesh )
	{
		m_pIceCoreMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioFireWorksStep1Weapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioFireWorksStep1Weapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioFireWorksStep1Weapon::Explosion( ioPlayStage *pStage )
{	
	if( m_IceState == DIS_EXPLOSION || m_IceState == DIS_EXPLOSION_END )
		return;

	CallAttribute();

	m_IceState = DIS_EXPLOSION;
	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos =  GetPosition();
		D3DXVECTOR3 vScale = ioMath::UNIT_ALL;

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect, vPos, vScale );

		ioWeapon::PlayExplosionSound();
	}
}

bool ioFireWorksStep1Weapon::IsFloating() const
{
	if( m_IceState == DIS_FLOATING )
		return true;

	return false;
}

bool ioFireWorksStep1Weapon::IsExplosion() const
{
	if( m_IceState == DIS_EXPLOSION || m_IceState == DIS_EXPLOSION_END )
		return true;

	return false;
}

void ioFireWorksStep1Weapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_IceState != DIS_FLOATING )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget))
		{
			bCollision = true;
		}
		else if( GetTeam() != pTarget->GetTeam() )
		{
			bCollision = true;
		}
		else if( GetOwner() != pTarget )
		{
			ioBaseChar *pChar = ToBaseChar(pTarget);
			if( pChar )
			{
				if( pChar->IsEnableTeamCollision( this ) )
				{
					bCollision = true;
				}
			}
		}
	}

	if( !bCollision )
		return;

	if( !m_bDisableMapCollision )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget ) || ToWallZone(pTarget) ||
			GetPiercingType() == CPT_NONE )
		{
			Explosion( pStage );
			return;
		}
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioFireWorksStep1Weapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_IceState != DIS_EXPLOSION && m_IceState != DIS_EXPLOSION_END )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	D3DXVECTOR3 vDiff = pEntity->GetWorldPosition() - vCenter;
	float fLength = D3DXVec3Length( &vDiff );

	float fCurExplosionRange = m_fExplosionRange;
	if( m_fOwnerChargeExplosionRate > FLOAT1 )
	{
		fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;
	}

	if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, fCurExplosionRange ) )
		return true;

	return false;
}

DWORD ioFireWorksStep1Weapon::GetCollisionEnableTime() const
{
	if( m_IceState == DIS_EXPLOSION || m_IceState == DIS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

float ioFireWorksStep1Weapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioFireWorksStep1Weapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioFireWorksStep1Weapon::CallAttribute()
{	
	if( m_bArleadyCallWeapon )	return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;	

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;

	for( ChangeDirInfoList::size_type i = 0; i < m_CallAttFireList.size(); ++i )
	{
		D3DXVECTOR3 vDir = m_vMoveDir;
		D3DXVec3Normalize(&vDir, &vDir);
		float fAngleH = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleH );
		float fAngleV = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleV );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;

		if ( m_bSetCallAxisChange )
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) );
		else
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		vDir = qtRot * vDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		vDir = qtRot * vDir;

		D3DXVECTOR3	vPos = GetPosition();

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			m_CallWeaponInfo,
			vPos,
			-vDir,											
			dwCurGenerateTime,
			m_dwWeaponIdx );

		dwCurGenerateTime += 10;
	}

	m_bArleadyCallWeapon = true;

}

void ioFireWorksStep1Weapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case WSC_ORDER_EXPLOSION:
		Explosion( pStage );
		break;
	}
}

void ioFireWorksStep1Weapon::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
		m_dwLiveTime = dwTime;
}

void ioFireWorksStep1Weapon::SetCallAxisChange( bool bSetCallAxisChange )
{
	m_bSetCallAxisChange = bSetCallAxisChange;
}