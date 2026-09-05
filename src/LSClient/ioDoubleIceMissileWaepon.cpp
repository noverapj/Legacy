

#include "stdafx.h"

#include "ioDoubleIceMissileWaepon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioDoubleIceMissileWaepon::m_ActionStopType = AST_NONE;
ActionStopType ioDoubleIceMissileWaepon::m_DieActionStopType = AST_NONE;

ioDoubleIceMissileWaepon::ioDoubleIceMissileWaepon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
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

	m_fCurMoveSpeed = 0.0f;
	m_fTotalMoveAmount = 0;

}

ioDoubleIceMissileWaepon::~ioDoubleIceMissileWaepon()
{
	if( m_pIceCoreMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioDoubleIceMissileWaepon::~ioDoubleIceMissileWaepon - UnReleased Resouce" );
	}
}

bool ioDoubleIceMissileWaepon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetIceCoreMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioDoubleIceMissileWaepon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwLiveTime = (DWORD)rkLoader.LoadInt_e( "live_time", 0 );

	m_bPlayWoundSound = rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );

	m_fExtraOffsetX			= rkLoader.LoadFloat_e( "extra_offset_x", 0.0f );
	m_fExtraOffsetY			= rkLoader.LoadFloat_e( "extra_offset_y", 0.0f );
	m_fExtraOffsetZ			= rkLoader.LoadFloat_e( "extra_offset_z", 0.0f );

	LoadCallAttribute( rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );
	LoadChangeDirInfoList( rkLoader );

	m_bIgnoreKeyInput = rkLoader.LoadBool_e( "ignore_key_input", false );
}

void ioDoubleIceMissileWaepon::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
		
	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );
}

void ioDoubleIceMissileWaepon::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
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

void ioDoubleIceMissileWaepon::LoadChangeDirInfoList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt > 0 )
	{
		m_ChangeDirInfoList.clear();
		m_ChangeDirInfoList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			ChangeDirInfo kInfo;

			wsprintf_e( szBuf, "change_range%d", i+1 );
			kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_speed%d", i+1 );
			kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_h%d", i+1 );
			kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

			wsprintf_e( szBuf, "change_angle_v%d", i+1 );
			kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

			m_ChangeDirInfoList.push_back( kInfo );
		}
		std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
	}
}


bool ioDoubleIceMissileWaepon::SetIceCoreMesh( const ioHashString &szFileName )
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

void ioDoubleIceMissileWaepon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pIceCoreMesh )
	{
		g_EffectFactory.DestroyEffect( m_pIceCoreMesh );
		m_pIceCoreMesh = NULL;
	}
}

const ioOrientBox& ioDoubleIceMissileWaepon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioDoubleIceMissileWaepon::GetType() const
{
	return WT_DOUBLE_ICE_MISSILE_WEAPON;
}

ActionStopType ioDoubleIceMissileWaepon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioDoubleIceMissileWaepon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioDoubleIceMissileWaepon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	switch(m_OrientationState)
	{
	case OS_NONE:
		ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
		break;
	case OS_ORIENTATION:
		qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
		break;
	}
	SetOrientation( qtRot );
}

void ioDoubleIceMissileWaepon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pIceCoreMesh && m_pIceCoreMesh->GetSkeletonName().IsEmpty() && m_pIceCoreMesh->HasLinkedSystem() )
		return;

	D3DXVECTOR3 vNewPos = vPos;
	if( m_fExtraOffsetX != 0.0f || m_fExtraOffsetY != 0.0f || m_fExtraOffsetZ != 0.0f )
	{
		ioBaseChar *pOwner = GetOwner();
		if( m_pIceCoreMesh && pOwner )
		{
			vNewPos = pOwner->GetWorldPosition();
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();

			D3DXVECTOR3 vZDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vZDir, &vZDir );

			D3DXVECTOR3 vXDir;
			D3DXVec3Cross( &vXDir, &vZDir, &ioMath::UNIT_Y );
			D3DXVec3Normalize( &vXDir, &vXDir );

			D3DXVECTOR3 vYDir;
			D3DXVec3Cross( &vYDir, &vXDir, &vZDir );
			D3DXVec3Normalize( &vYDir, &vYDir );

			vNewPos += m_fExtraOffsetX * vXDir;
			vNewPos += m_fExtraOffsetY * vYDir;
			vNewPos += m_fExtraOffsetZ * vZDir;
		}
	}

	SetPosition( vNewPos );
}

void ioDoubleIceMissileWaepon::PlayWoundedSound( ioPlayEntity *pEntity )
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

void ioDoubleIceMissileWaepon::Process( float fTimePerSec, ioPlayStage *pStage )
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

			CheckChangeDir();

			if( CheckKeyInput( pOwner, pStage ) )
				Explosion( pStage );
			
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

bool ioDoubleIceMissileWaepon::CheckKeyInput( ioBaseChar* pOwner, ioPlayStage *pStage )
{
	if ( m_bIgnoreKeyInput )
		return false;

	//키입력 여부 검사
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		bReserveSkill = true;
	}

	if( pOwner->IsAttackKey() || pOwner->IsDefenseKey() || pOwner->IsJumpKey() && !bReserveSkill )
	{	
		if( GetOwner()->IsNeedProcess() )
		{
			if( GetOwner()->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );			
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << WSC_ORDER_EXPLOSION;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}				
		}

		return true;
	}

	return false;
}

bool ioDoubleIceMissileWaepon::CheckMoveRange( ioBaseChar* pOwner, ioPlayStage *pStage, float fTimePerSec )
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

bool ioDoubleIceMissileWaepon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

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

void ioDoubleIceMissileWaepon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
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

void ioDoubleIceMissileWaepon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioDoubleIceMissileWaepon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioDoubleIceMissileWaepon::Explosion( ioPlayStage *pStage )
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

bool ioDoubleIceMissileWaepon::IsFloating() const
{
	if( m_IceState == DIS_FLOATING )
		return true;

	return false;
}

bool ioDoubleIceMissileWaepon::IsExplosion() const
{
	if( m_IceState == DIS_EXPLOSION || m_IceState == DIS_EXPLOSION_END )
		return true;

	return false;
}

void ioDoubleIceMissileWaepon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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
	if ( GetPiercingType() == CPT_NONE )
		pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioDoubleIceMissileWaepon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
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

DWORD ioDoubleIceMissileWaepon::GetCollisionEnableTime() const
{
	if( m_IceState == DIS_EXPLOSION || m_IceState == DIS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

float ioDoubleIceMissileWaepon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioDoubleIceMissileWaepon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioDoubleIceMissileWaepon::CallAttribute()
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

void ioDoubleIceMissileWaepon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
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

void ioDoubleIceMissileWaepon::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
		m_dwLiveTime = dwTime;
}

void ioDoubleIceMissileWaepon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fTotalMoveAmount >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioDoubleIceMissileWaepon::RotateMoveDir( float fAngleH, float fAngleV )
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

	switch(m_OrientationState)
	{
	case OS_NONE:
		ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
		break;
	case OS_ORIENTATION:
		qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
		break;
	}
	SetOrientation( qtRot );
}
