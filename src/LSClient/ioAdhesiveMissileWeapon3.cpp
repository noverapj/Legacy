

#include "stdafx.h"

#include "ioAdhesiveMissileWeapon3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioAdhesiveMissileWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioAdhesiveMissileWeapon3::m_DieActionStopType = AST_NONE;

ioAdhesiveMissileWeapon3::ioAdhesiveMissileWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	ClearData();

	m_pFireBall = NULL;

	m_fCurMoveRange = 0.0f;
	m_fCurMoveRate = 0.0f;
	
	m_bSetGraceClose = false;
	m_bFirst = true;

	m_bOwnerAttach = false;
	m_bCharColState = false;
	m_bArleadyCallWeapon = false;

	m_iDummyIndex = 0;
}

ioAdhesiveMissileWeapon3::~ioAdhesiveMissileWeapon3()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioAdhesiveMissileWeapon3::~ioAdhesiveMissileWeapon3 - UnReleased Resouce" );
	}
}

void ioAdhesiveMissileWeapon3::ClearData()
{
	UpdateWorldAttackBox(0.0f, true );
}

bool ioAdhesiveMissileWeapon3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}
	return false;
}

void ioAdhesiveMissileWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bCollisionMap = rkLoader.LoadBool_e( "check_collision_map", false );

	m_fStartOffset_x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_fStartOffset_y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_fStartOffset_z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	//
	int iCount = rkLoader.LoadInt_e( "first_col_buff_cnt", 0 );
	m_FirstColBuffList.reserve( iCount );

	for( int i=0; i<iCount; i++ )
	{
		wsprintf_e( szKey, "first_col_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_FirstColBuffList.push_back( tBuffInfo );
			}
		}
	}

	LoadCallAttribute(  rkLoader );
	LoadCallAttributeFireDirectionList( rkLoader );
}

void ioAdhesiveMissileWeapon3::LoadCallAttribute( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	m_CallWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "call_attribute_index", 0 );		
	m_CallWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "call_attribute_resistance", 0 );		

	rkLoader.LoadString_e( "call_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_CallWeaponInfo.m_WoundedAnimation = szBuf;		

	m_CallWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_attribute_wound_duration", 0 );
	m_CallWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_attribute_wound_loop_ani", false );
}

void ioAdhesiveMissileWeapon3::LoadCallAttributeFireDirectionList( ioINILoader &rkLoader )
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

		wsprintf_e( szBuf, "call_attribute_fire%d_offset_x", i+1 );
		kInfo.m_vFireOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_offset_y", i+1 );
		kInfo.m_vFireOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "call_attribute_fire%d_offset_z", i+1 );
		kInfo.m_vFireOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

		m_CallAttFireList.push_back( kInfo );
	}

	std::sort( m_CallAttFireList.begin(), m_CallAttFireList.end(), ChangeDirSort() );
}

bool ioAdhesiveMissileWeapon3::CreateFireBall( const ioHashString &szFileName )
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
	D3DXVECTOR3 vPosOffset = pOwner->GetTargetRot() * vStartPos;
	D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + vPosOffset;

	m_StartPos = GetPosition();
	AddEffectEvent( m_pFireBall );

	return true;
}
 
void ioAdhesiveMissileWeapon3::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioAdhesiveMissileWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioAdhesiveMissileWeapon3::GetMoveDir()
{
	return m_vMoveDir;
}

void ioAdhesiveMissileWeapon3::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioAdhesiveMissileWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	return;
}

bool ioAdhesiveMissileWeapon3::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !m_bCollisionMap )
		return true;

	if( GetOwner()->IsNeedProcess() )
	{
		if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
		{
			if( GetOwner()->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_WEAPON_SYNC );			
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << STC_TEST_MAP_COLLISION;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			CallAttribute();
			Explosion( pStage );
			return true;
		}
	}

	return false;
}

void ioAdhesiveMissileWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		if( m_bOwnerAttach )
			ProcessAttachMove( fTimePerSec, pStage );
		else
			ProcessNormalMove( fTimePerSec, pStage );
	}

	m_bFirst = false;

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioAdhesiveMissileWeapon3::ProcessAttachMove( float fTimePerSec, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		Explosion( pStage );
		return;
	}

	if( m_iDummyIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
		if( pDummy )
		{
			D3DXQUATERNION qtRot = pDummy->GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vStartPos( m_fStartOffset_x, m_fStartOffset_y, m_fStartOffset_z );
			D3DXVECTOR3 vPosOffset = qtRot * vStartPos;

			D3DXVECTOR3 vPos = pDummy->GetWorldPosition() + vPosOffset;	// offset 설정 필요

			SetPosition( vPos );
			SetOrientation( qtRot );
			SetMoveDir( vDir );

			UpdateWorldAttackBox( fTimePerSec, false );
		}
		else
		{
			Explosion( pStage );
			return;
		}
	}
	else
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vStartPos( m_fStartOffset_x, m_fStartOffset_y, m_fStartOffset_z );
		D3DXVECTOR3 vPosOffset = qtRot * vStartPos;

		D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + vPosOffset;	// offset 설정 필요

		SetPosition( vPos );
		SetOrientation( qtRot );
		SetMoveDir( vDir );

		UpdateWorldAttackBox( fTimePerSec, false );
	}
}

void ioAdhesiveMissileWeapon3::ProcessNormalMove( float fTimePerSec, ioPlayStage *pStage )
{
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	D3DXVECTOR3 vMove = m_vMoveDir * fMoveDist;
	GetParentSceneNode()->Translate( vMove );

	m_fCurMoveRange += fMoveDist;

	float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
	if( fMaxRange > 0.0f )
		m_fCurMoveRate = m_fCurMoveRange / fMaxRange;

	if( m_fCurMoveRange < fMaxRange )
	{
		UpdateWorldAttackBox( fTimePerSec, false );
	}
	else
	{
		Explosion( pStage );
	}
}

void ioAdhesiveMissileWeapon3::SetForceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioAdhesiveMissileWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioAdhesiveMissileWeapon3::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioAdhesiveMissileWeapon3::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioAdhesiveMissileWeapon3::GetType() const
{
	return WT_ADHESIVE_MISSILE3;
}

float ioAdhesiveMissileWeapon3::GetMoveSpeed()
{
	if( m_bOwnerAttach )
		return 0.0f;

	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

float ioAdhesiveMissileWeapon3::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

ActionStopType ioAdhesiveMissileWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioAdhesiveMissileWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioAdhesiveMissileWeapon3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

const WeaponAttribute* ioAdhesiveMissileWeapon3::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

void ioAdhesiveMissileWeapon3::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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

			CallAttribute();
			Explosion( pStage );
			return;
		}
	}
}

bool ioAdhesiveMissileWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

void ioAdhesiveMissileWeapon3::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case STC_TEST_MAP_COLLISION:
		Explosion( pStage );
		break;
	}
}

void ioAdhesiveMissileWeapon3::Explosion( ioPlayStage *pStage )
{
	if( !IsLive() )
		return;

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

	ClearData();
	SetForceClose();
	SetWeaponDead();
}

bool ioAdhesiveMissileWeapon3::IsProtectState( ioBaseChar* pChar )
{
	if( pChar->HasBuff( BT_STATE_PROTECTION ) || pChar->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
		return true;
	
	return pChar->IsProtectState();
}

const BuffInfoList& ioAdhesiveMissileWeapon3::GetTargetAirBuffList() const
{
	if( m_bCharColState )
		return GetAttribute()->m_TargetAirBuffList;

	return m_FirstColBuffList;
}

const BuffInfoList& ioAdhesiveMissileWeapon3::GetTargetBuffList() const
{
	if( m_bCharColState )
		return GetAttribute()->m_TargetBuffList;

	return m_FirstColBuffList;
}

const BuffInfoList& ioAdhesiveMissileWeapon3::GetTargetDownBuffList() const
{
	if( m_bCharColState )
		return GetAttribute()->m_TargetDownBuffList;

	return m_FirstColBuffList;
}

void ioAdhesiveMissileWeapon3::CheckWeaponCollision()
{
	m_bCharColState = true;
}

void ioAdhesiveMissileWeapon3::CheckOwnerAttach( bool bAttach )
{
	m_bOwnerAttach = bAttach;
}

void ioAdhesiveMissileWeapon3::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->SetSkillCreateWeaponDependency( GetWeaponBySkillName() );
	}	
}

void ioAdhesiveMissileWeapon3::CallAttribute()
{	
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallAttFireList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;	

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;

	for( ChangeDirInfoList::size_type i = 0; i < m_CallAttFireList.size(); ++i )
	{
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize(&vDir, &vDir);
		float fAngleH = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleH );
		float fAngleV = ioMath::ArrangeHead( m_CallAttFireList[i].m_fChangeAngleV );

		//Offset
		D3DXVECTOR3	vPos = GetPosition();
		ioPlayEntityHelp::MoveOffset( m_CallAttFireList[i].m_vFireOffset, pOwner->GetTargetRot(), vPos );

		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		vDir = qtRot * vDir;

		// horizon
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		vDir = qtRot * vDir;

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