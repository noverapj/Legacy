

#include "stdafx.h"

#include "ioArrowWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioArrowWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioArrowWeapon::m_DieActionStopType = AST_NONE;

ioArrowWeapon::ioArrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_fCurMoveRange = 0.0f;

	m_fCurMoveRate = 0.0f;
	
	m_bSetGraceClose = false;
	m_bFirst = true;

	m_pSecondAttr = NULL;
	m_dwAreaWeaponIndex = 0;

	//추가 2014.07.08
	m_OrientationState = OS_NONE;

	m_fCurMoveSpeed = 0.0f;
}

ioArrowWeapon::~ioArrowWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioArrowWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}

	m_vSecondAttrList.clear();
}

bool ioArrowWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyFireBall();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioArrowWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_bWaitWeaponDeadForPacket = rkLoader.LoadBool_e( "wait_weapon_dead", false );

	//추가 2014.07.08
	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );

	m_nWeaponType = (WeaponItemType)rkLoader.LoadInt( "weapon_item_type", (int)WIT_NONE );

	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

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

bool ioArrowWeapon::CreateFireBall( const ioHashString &szFileName )
{
	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );
	
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		LOG.PrintTimeAndLog( 0, "ioArrowWeapon::CreateFileBall - Not Has Owner" );
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

	m_StartPos = GetPosition();

	AddEffectEvent( m_pFireBall );

	m_dwWeaponDeadTime = 0;
	return true;
}
 
void ioArrowWeapon::DestroyFireBall()
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioArrowWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	
	//변경 2014.07.08
	switch(m_OrientationState)
	{
	case OS_NONE:
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			SetOrientation( qtRot );
		}
		break;
	case OS_ORIENTATION:
		{
			D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	}	
	/*
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
	*/
}

void ioArrowWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
	
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}


void ioArrowWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXQUATERNION qtRot;

	// horizon
	if( fAngleH != 0.0f )
	{
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fAngleH );
	}

	// vertical
	if( fAngleV != 0.0f )
	{
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( vAxis, fAngleV );
	}
}

void ioArrowWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	m_StartPos = GetPosition();
}

void ioArrowWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	if( !m_bFirst )
	{
		CheckChangeDir();

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
			UpdateWorldAttackBox( fTimePerSec, true );

			SetGraseClose();

			if( !m_pFireBall || !m_pFireBall->IsLive() )
			{
				SetWeaponDead();
			}
		}
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioArrowWeapon::SetGraseClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioArrowWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioArrowWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

D3DXVECTOR3 ioArrowWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

ioWeapon::WeaponType ioArrowWeapon::GetType() const
{
	return WT_ARROW;
}

float ioArrowWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;

	return m_fCurMoveSpeed;
}

float ioArrowWeapon::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioArrowWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
	
	if( m_bSkillDependency )
	{
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner ) return;

		pOwner->SetSkillCreateWeaponDependency( GetWeaponBySkillName() );
	}

	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( pStage && m_dwAreaWeaponIndex > 0 )
	{
		pStage->DestroyAreaWeapon( m_dwAreaWeaponIndex );
	}
}

ActionStopType ioArrowWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioArrowWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioArrowWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioArrowWeapon::SetSecondAttribute( SecondAttributeList vSecondList )
{
	int iSize = vSecondList.size();
	for( int i=0; i < iSize; ++i )
	{
		SecondAttribute kSecondAttr = vSecondList[i];
		m_vSecondAttrList.push_back( kSecondAttr );
	}
}

void ioArrowWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );

	if( m_bResistance ) return;

	int iSize = m_vSecondAttrList.size();
	if( iSize > 0 )
	{
		for( int i=0; i < iSize-1; ++i )
		{
			if( m_fCurMoveRange < m_vSecondAttrList[i].m_fLimiteValue )
			{
				m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[i].m_iAttributeIndex );
				return;
			}
		}

		m_pSecondAttr = g_WeaponMgr.FindAttribute( m_vSecondAttrList[iSize-1].m_iAttributeIndex );
		return;
	}

	m_pSecondAttr = NULL;
	return;
}

const WeaponAttribute* ioArrowWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	if( m_pSecondAttr )
		return m_pSecondAttr;

	return m_pConstAttr;
}

bool ioArrowWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	return bCollision;
}

void ioArrowWeapon::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
}

bool ioArrowWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( m_StartPos, vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}


void ioArrowWeapon::SetDestroyAreaWeaponIndex( DWORD dwIndex )
{
	m_dwAreaWeaponIndex = dwIndex;
}

ioArrowWeapon::WeaponItemType ioArrowWeapon::GetWeaponItemType()
{
	return m_nWeaponType;
}

void ioArrowWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}
