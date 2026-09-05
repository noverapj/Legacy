

#include "stdafx.h"

#include "ioBombMissileWeapon3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ItemDefine.h"

ActionStopType ioBombMissileWeapon3::m_ActionStopType = AST_NONE;
ActionStopType ioBombMissileWeapon3::m_DieActionStopType = AST_NONE;

ioBombMissileWeapon3::ioBombMissileWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION );

	m_pBombMesh = NULL;
	m_BombState = BS_FLOATING_AIR;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_bArleadyCallWeapon	= false;
	m_bDsiableChangeDir		= false;
	m_bSetGraceClose		= false;

	m_dwEndEffectID = -1;
	m_OrientationState = OS_NONE;
}

ioBombMissileWeapon3::~ioBombMissileWeapon3()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombMissileWeapon::~ioBombMissileWeapon - UnReleased Resouce" );
	}
}

bool ioBombMissileWeapon3::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		ioHashString szBombMesh = GetAttribute()->m_WeaponEffect;
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			TeamType eCurTeamType = pOwner->GetTeam();
			TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();
			TeamType eTeamType = TEAM_NONE;
			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				eTeamType = TEAM_BLUE;
				break;
			case TEAM_RED:
				eTeamType = TEAM_RED;
				break;
			default:
				if( eCurSingleTeamType == TEAM_BLUE )
					eTeamType = TEAM_BLUE;
				else
					eTeamType = TEAM_RED;
				break;
			}

			if( !m_szWeaponEffect_Blue.IsEmpty() && eTeamType == TEAM_BLUE )
				szBombMesh = m_szWeaponEffect_Blue;
			else if( !m_szWeaponEffect_Red.IsEmpty() && eTeamType == TEAM_RED )
				szBombMesh = m_szWeaponEffect_Red;
		}

		return SetBombMesh( szBombMesh );
	}

	return false;
}

void ioBombMissileWeapon3::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bEnableMapCollision = rkLoader.LoadBool_e( "enable_map_collision", false );	
	m_bEndExplosion = rkLoader.LoadBool_e( "end_explosion", false );
	m_bCallExplosion = rkLoader.LoadBool_e( "call_explosion", false );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_OwnerRotate = (OwnerRotType)rkLoader.LoadInt_e( "owner_rotate", ORT_NONE );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );
	m_fRotateEndRange = rkLoader.LoadFloat_e( "rotate_end_range", 0.0f );

	m_fStartSpeed = rkLoader.LoadFloat_e( "start_speed", 0.0f );
	m_fMaxSpeed = rkLoader.LoadFloat_e( "max_speed", 0.0f );
	m_fAccelSpeed = rkLoader.LoadFloat_e( "accel_speed", 0.0f );

	m_fEnableMoveGap = rkLoader.LoadFloat_e( "enable_move_gap_height", FLOAT100 );
	m_fDefaultGapHeight = rkLoader.LoadFloat_e( "defalut_height_gap", 0 );

	m_MapHeightCheckType = (MapHeightCheckType)rkLoader.LoadInt_e( "map_height_check_type", MHCT_BOUNDBOX );
	m_bSetOreintationFloatingAir = rkLoader.LoadBool_e( "set_oreintation_floating_air", false );

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

	rkLoader.LoadString_e( "weapon_effect_blue", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Blue = szBuf;
	rkLoader.LoadString_e( "weapon_effect_red", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Red = szBuf;

	m_bRotationBox = rkLoader.LoadBool( "rotation_collision_box", false );
	LoadCallAttributeList( rkLoader );

	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );
}

void ioBombMissileWeapon3::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

bool ioBombMissileWeapon3::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )
		return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pBombMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pBombMesh->GetCollisionBox();

	AddEffectEvent( m_pBombMesh );
	return true;
}

void ioBombMissileWeapon3::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

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
}

void ioBombMissileWeapon3::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() )
		return;

	if( m_bDsiableChangeDir )
		return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioBombMissileWeapon3::RotateMoveDir( float fAngleH, float fAngleV )
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

void ioBombMissileWeapon3::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pBombMesh && m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
	ioBaseChar *pOwner = GetOwner();
}

void ioBombMissileWeapon3::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	switch( m_BombState )
	{
	case BS_FLOATING_AIR:
	case BS_FLOATING_LAND:
		OnFloating( fTimePerSec, pStage );
		break;
	case BS_CLOSE:
		OnClosing( fTimePerSec, pStage );
		break;
	case BS_WAIT_CLOSE:
		m_BombState = BS_CLOSE;
		break;
	case BS_EXPLOSION_END:
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pBombMesh )
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
}

bool ioBombMissileWeapon3::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( !m_bEnableMapCollision && m_BombState != BS_FLOATING_AIR && m_BombState != BS_FLOATING_LAND )
		return true;

	if( m_BombState == BS_FLOATING_LAND && GetPiercingType() == CPT_ALL )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		if( m_BombState == BS_FLOATING_AIR )
		{
			m_BombState = BS_FLOATING_LAND;
			m_vMoveDir.y = 0;
			D3DXVec3Normalize(&m_vMoveDir, &m_vMoveDir);

			D3DXVECTOR3 vPos = GetPosition();
			float fHeight = 0.0f;
			if ( m_MapHeightCheckType == MHCT_BOUNDBOX )
				fHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
			else if ( m_MapHeightCheckType == MHCT_POSITION )
				fHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z );
			
			vPos.y = fHeight + m_fDefaultGapHeight;
			SetPosition( vPos );
			
			if ( m_bSetOreintationFloatingAir )
			{
				switch(m_OrientationState)
				{
				case OS_NONE:
					{
						D3DXQUATERNION qtRot;
						ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
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
			}
		}
		else if( m_BombState == BS_FLOATING_LAND )
		{
			if( m_bEndExplosion )
			{
				Explosion( pStage );
			}
			else
			{
				m_BombState = BS_CLOSE;				
			}
			g_WeaponMgr.AddWeaponEvent( GetWeaponIndex(), WET_MAP_COLL );			
		}
		else
			assert( false );

		return true;
	}

	return false;
}

void ioBombMissileWeapon3::SetGraceClose()
{
	if( m_bSetGraceClose )
		return;

	if( m_pBombMesh )
	{
		if( m_bEndEffectGrace )
			m_pBombMesh->EndEffectGrace();
		else
			m_pBombMesh->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioBombMissileWeapon3::OnClosing( float fTimePerSec, ioPlayStage *pStage )
{
	SetGraceClose();

	if( m_dwEndEffectID = -1 && !GetWeaponEndEffect().IsEmpty() )
	{
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
		if( pMapEffect )
			m_dwEndEffectID = pMapEffect->GetUniqueID();
	}

	if( !m_pBombMesh || !m_pBombMesh->IsLive() )
		SetWeaponDead();
}

 
void ioBombMissileWeapon3::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckChangeDir();

		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			switch( m_OwnerRotate )
			{
			case ORT_NORMAL:
				CheckOwnerRotateDir( fTimePerSec );
				break;
			case ORT_STAFF3:
				{
					ioMagicStaffItem3 *pStaff3 = ToMagicStaffItem3( pOwner->GetPriorityItem( SPT_ATTACK ) );
					if( pStaff3 && pStaff3->GetCurFireState() == ioMagicStaffItem3::FS_GATHERING )
					{
						CheckOwnerRotateDir( fTimePerSec );
					}
				}
				break;
			}
		}

		float fCurSpeed = CheckCurSpeed( fTimePerSec );
		float fMoveDist = fCurSpeed * fTimePerSec;

		if( m_BombState == BS_FLOATING_AIR )
		{
			GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
		}
		else if( m_BombState == BS_FLOATING_LAND )
		{
			D3DXVECTOR3 vPrePos = GetPosition();
			D3DXVECTOR3 vNewPos = vPrePos + (m_vMoveDir * fMoveDist);
			float fHeight = 0.0f;
			if ( m_MapHeightCheckType == MHCT_BOUNDBOX )
				fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z, vNewPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
			else if ( m_MapHeightCheckType == MHCT_POSITION )
				fHeight = pStage->GetMapHeightNoneEntity( vNewPos.x, vNewPos.z );
			if( fabs( vPrePos.y - fHeight ) < m_fEnableMoveGap )
			{
				vNewPos.y = fHeight + m_fDefaultGapHeight;
			}
			/*else
			{
				m_BombState = BS_CLOSE;
				return;
			}*/

			SetPosition( vNewPos );
		}
		else
			assert( false );

		m_fCurMoveRange += fMoveDist;

		float fMaxRange = GetMaxRange() + m_fGrowthMoveRange;
		if( m_fCurMoveRange >= fMaxRange )
		{
			if( m_bEndExplosion )
			{
				g_WeaponMgr.AddWeaponEvent( GetWeaponIndex(), WET_NORMAL_DEAD );
				Explosion( pStage );
				return;
			}
			else
			{
				g_WeaponMgr.AddWeaponEvent( GetWeaponIndex(), WET_NORMAL_DEAD );
				m_BombState = BS_CLOSE;
			}
		}
	}

	m_bFirst = false;

	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBombMissileWeapon3::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombMissileWeapon3::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombMissileWeapon3::GetType() const
{
	return WT_BOMB_MISSILE3;
}

ActionStopType ioBombMissileWeapon3::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombMissileWeapon3::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioBombMissileWeapon3::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = m_fStartSpeed;

	return m_fCurMoveSpeed;
}

float ioBombMissileWeapon3::GetMaxRange()
{
	return GetAttribute()->m_fMoveRange * m_fOwnerChargeRangeRate;
}

void ioBombMissileWeapon3::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
			if( m_bRotationBox )
			{
				D3DXVECTOR3 vDir = GetParentSceneNode()->GetDerivedOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
				vDir.y = 0.0f;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );

				D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
				m_WorldAttackBox.Transform( GetPosition(), vScale, qtRot );
				m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, qtRot );
			}
			else
			{
				D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
				m_WorldAttackBox.Transform( GetPosition(), vScale, ioMath::QUAT_IDENTITY );
				m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, ioMath::QUAT_IDENTITY );
			}
		}
		else
		{
			if( m_bRotationBox )
			{
				D3DXVECTOR3 vDir = GetParentSceneNode()->GetDerivedOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
				vDir.y = 0.0f;
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
				m_WorldAttackBox.Transform( GetPosition(), GetScale(), qtRot );
				m_WorldAttackBoxForMap.Transform( GetPosition(), GetScale(), qtRot );
			}
			else
			{
				m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
				m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
			}
		}
	}
}

bool ioBombMissileWeapon3::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBombMissileWeapon3::Explosion( ioPlayStage *pStage )
{
	if( m_BombState == BS_WAIT_CLOSE || m_BombState == BS_EXPLOSION_END )
		return;

	m_BombState = BS_WAIT_CLOSE;

	if( m_bCallExplosion )
	{
		CallAttribute();
	}
	else
	{
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
}

bool ioBombMissileWeapon3::IsFloating() const
{
	if( m_BombState == BS_FLOATING_AIR )
		return true;

	return false;
}

bool ioBombMissileWeapon3::IsExplosion() const
{
	if( m_BombState == BS_WAIT_CLOSE || m_BombState == BS_EXPLOSION_END )
		return true;

	return false;
}

void ioBombMissileWeapon3::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_BombState != BS_FLOATING_AIR && m_BombState != BS_FLOATING_LAND )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
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

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget ) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		if( m_bEndExplosion )
		{
			Explosion( pStage );
			g_WeaponMgr.AddWeaponEvent( GetWeaponIndex(), WET_MAP_COLL );
		}
		else
		{
			m_BombState = BS_WAIT_CLOSE;
			g_WeaponMgr.AddWeaponEvent( GetWeaponIndex(), WET_MAP_COLL );
		}
		
		return;
	}

	CheckCurAttribute( pTarget );
	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );

	if( m_bEndExplosion )
		pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

bool ioBombMissileWeapon3::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{

	//폭발이 아닌경우 현재 충돌 박스로 테스트
	if( m_bEndExplosion == false )
	{
		bool bCollision = false;
		if( pEntity->IsWoundedCollision( GetWorldAttackBox() ) )
		{
			if( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) ||
				ToDummyChar(pEntity) || ToWallZone(pEntity) )
			{
				bCollision = true;
			}
			else if( GetTeam() != pEntity->GetTeam() )
			{
				bCollision = true;
			}
			else if( GetOwner() != pEntity )
			{
				ioBaseChar *pChar = ToBaseChar(pEntity);
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
			return false;

		if( ToBaseChar(pEntity) || ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) ||
			ToDummyChar(pEntity ) || ToWallZone(pEntity) )
		{
			return true;
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//폭발인경우
	if( m_BombState != BS_WAIT_CLOSE && m_BombState != BS_EXPLOSION_END )
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
	{
		return true;
	}

	return false;

}

DWORD ioBombMissileWeapon3::GetCollisionEnableTime() const
{
	if( m_BombState == BS_WAIT_CLOSE || m_BombState == BS_EXPLOSION_END )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioBombMissileWeapon3::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioBombMissileWeapon3::CheckOwnerRotateDir( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_dwRotateTime == 0 ) return;
	if( m_fCurMoveRange > m_fRotateEndRange )
		return;

	D3DXQUATERNION	qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	float fDotValue = D3DXVec3Dot( &vTargetDir, &m_vMoveDir );
	if( fDotValue >= 0.98f )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &m_vMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	float fAngleGap = RADtoDEG( acosf(fDotValue) );
	float fRate = (fTimePerSec*1000) / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	if( fAngleGap < fRotateAngle )
	{
		SetOrientation( qtRot );
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}
	else
	{
		D3DXQUATERNION	qtNewRot;
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

		m_vMoveDir = qtNewRot * m_vMoveDir;

		switch(m_OrientationState)
		{
		case OS_NONE:
			{
				ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );
				SetOrientation( qtNewRot );
			}
			break;
		case OS_ORIENTATION:
			{
				qtNewRot = Help::ConvertDirToQuaternion( m_vMoveDir );
				SetOrientation( qtNewRot );
			}
			break;
		}
	}
}

float ioBombMissileWeapon3::CheckCurSpeed( float fTimePerSec )
{
	float fCurSpeed = GetMoveSpeed();

	if( m_fStartSpeed != m_fMaxSpeed || m_fAccelSpeed > 0.0f )
	{
		float fGapTime = (float)(FRAMEGETTIME() - m_dwCreatedTime);
		float fDTime = fGapTime / FLOAT1000;
		float fCurAccel = m_fAccelSpeed * fDTime * fDTime;

		fCurSpeed += fCurAccel;
		fCurSpeed = min( fCurSpeed, m_fMaxSpeed );
	}

	m_fCurMoveSpeed = fCurSpeed;
	return fCurSpeed * m_fOwnerChargeSpeedRate;
}

void ioBombMissileWeapon3::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();

	WeaponInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			pInfo,
			GetPosition(),
			m_vMoveDir,
			dwCurGenerateTime + pInfo.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}
