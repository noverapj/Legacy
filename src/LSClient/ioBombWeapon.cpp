
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioBombWeapon.h"

ActionStopType ioBombWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioBombWeapon::m_DieActionStopType = AST_NONE;

ioBombWeapon::ioBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pBombMesh = NULL;

	m_dwExplosionTime = 0;

	m_bResistance = false;

	m_BombState = BS_FLOATING;
}

ioBombWeapon::~ioBombWeapon()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombWeapon::~ioBombWeapon - UnReleased Resource" );
	}
}

void ioBombWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	LoadAreaWeaponProperty( rkLoader );
	LoadDummyCharProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	DWORD dwDuration = rkLoader.LoadFloat_e( "bomb_drop_duration", 0.0f );
	m_dwExplosionTime = FRAMEGETTIME() + dwDuration;

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fFloatingPower = GetAttribute()->m_fFloatPower;

	m_bDestroyDummy = rkLoader.LoadBool_e( "destroy_dummy", true );
}

bool ioBombWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetBombMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioBombWeapon::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

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


void ioBombWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioBombWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_BombState )
	{
	case BS_FLOATING:
		OnFloating( fTimePerSec, pStage );
		break;
	case BS_EXPLOSION_WAIT:
		Explosion( pStage );
		break;
	case BS_EXPLOSION:
		CreateAreaWeaponbyExplosion( GetOwner() );
		CreateDummyCharbyExplosion( GetOwner() );
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioBombWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionTime > FRAMEGETTIME() )
	{
		float fRotateAngle = 900.0f * fTimePerSec;
		GetParentSceneNode()->PitchX( fRotateAngle, ioNode::TS_LOCAL );

		D3DXVECTOR3 vMove = m_vMoveDir * ( GetMoveSpeed() * fTimePerSec );
		vMove.y += m_fFloatingPower * fTimePerSec;
		m_fFloatingPower -= pStage->GetRoomGravity() * fTimePerSec;
		GetParentSceneNode()->Translate( vMove );

		if( GetCollisionEnableTime() < FRAMEGETTIME() )
		{
			D3DXVECTOR3 vPos = GetPosition();
			float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
			if( vPos.y <= 0.0f || vPos.y <= fMapHeight )
			{
				if( vPos.y <= 0.0f )
					vPos.y = 0.0f;
				else
					vPos.y = fMapHeight;

				SetPosition( vPos );
				SetOrientation( D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );
				m_fFloatingPower = 0.0f;

				Explosion( pStage );
			}
		}
	}
	else
	{
		Explosion( pStage );
	}

	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

float ioBombWeapon::GetMoveSpeed() const
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioBombWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_BombState == BS_EXPLOSION )
		return;

	m_BombState = BS_EXPLOSION;
	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}
		
		vPos.y += FLOAT1;

		pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		ioWeapon::PlayExplosionSound();
	}
}

bool ioBombWeapon::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioBombWeapon::IsExplosion() const
{
	if( m_BombState == BS_EXPLOSION )
		return true;

	return false;
}

void ioBombWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
}

bool ioBombWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_BombState != BS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	float fExplosionRange = m_fExplosionRange + m_fGrowthExplosionRange;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, fExplosionRange ) )
		return true;

	return false;
}

void ioBombWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombWeapon::GetType() const
{
	return WT_BOMB;
}

ActionStopType ioBombWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioBombWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioBombWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
	}
}

void ioBombWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_BombState != BS_FLOATING )
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
			if( m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay < FRAMEGETTIME() )
				bCollision = true;
		}
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

DWORD ioBombWeapon::GetCollisionEnableTime() const
{
	if( m_BombState == BS_FLOATING || m_BombState == BS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioBombWeapon::LoadAreaWeaponProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_AreaWeaponInfoList.clear();

	int nMaxInfo = rkLoader.LoadInt_e( "max_area_weapon_cnt", 0 );
	for ( int i=0; i<nMaxInfo; i++ )
	{
		CreateInfo sAreaWeaponInfo;
		wsprintf_e( szKey, "area_weapon%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sAreaWeaponInfo.m_szCreateName = szBuf;
		
		wsprintf_e( szKey, "area_weapon%d_x_offset", i+1 );
		sAreaWeaponInfo.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "area_weapon%d_y_offset", i+1 );
		sAreaWeaponInfo.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "area_weapon%d_z_offset", i+1 );
		sAreaWeaponInfo.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "area_weapon%d_angle", i+1 );
		sAreaWeaponInfo.m_fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AreaWeaponInfoList.push_back( sAreaWeaponInfo );
	}
}

void ioBombWeapon::CreateAreaWeaponbyExplosion( ioBaseChar *pOwner )
{
	if ( !pOwner || m_AreaWeaponInfoList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = m_vMoveDir;
	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vMoveDir );
	D3DXVECTOR3 vPos = GetPosition();

	int nMaxCnt = (int)m_AreaWeaponInfoList.size();
	for ( int i=0; i<nMaxCnt; i++ )
	{		
		D3DXVECTOR3 vNewPos = vPos + qtRot * m_AreaWeaponInfoList[i].m_vPos;
		//vNewPos.y = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(m_AreaWeaponInfoList[i].m_fAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vRotDir = qtAngle * vMoveDir;
		qtAngle = Help::ConvertDirToQuaternion( vRotDir );

		g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "", m_AreaWeaponInfoList[i].m_szCreateName, vNewPos, qtAngle, ioAreaWeapon::CT_NORMAL );
	}
}

void ioBombWeapon::LoadDummyCharProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_DummyCharInfoList.clear();

	int nMaxInfo = rkLoader.LoadInt_e( "max_dummy_cnt", 0 );
	for ( int i=0; i<nMaxInfo; i++ )
	{
		CreateInfo sDummyInfo;
		wsprintf_e( szKey, "dummy%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sDummyInfo.m_szCreateName = szBuf;

		wsprintf_e( szKey, "dummy%d_x_offset", i+1 );
		sDummyInfo.m_vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_y_offset", i+1 );
		sDummyInfo.m_vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "dummy%d_z_offset", i+1 );
		sDummyInfo.m_vPos.z = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "dummy%d_angle", i+1 );
		sDummyInfo.m_fAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_DummyCharInfoList.push_back( sDummyInfo );
	}
}

void ioBombWeapon::CreateDummyCharbyExplosion( ioBaseChar *pOwner )
{
	if ( !pOwner || m_DummyCharInfoList.empty() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vMoveDir = m_vMoveDir;
	D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vMoveDir );
	D3DXVECTOR3 vPos = GetPosition();

	int nMaxCnt = (int)m_DummyCharInfoList.size();
	for ( int i=0; i<nMaxCnt; i++ )
	{	
		if( m_bDestroyDummy )
			g_DummyCharMgr.DestroyDummyCharByName( pOwner, m_DummyCharInfoList[i].m_szCreateName );
		
		if ( vPos.y <= 0.0f )
			continue;

		D3DXVECTOR3 vNewPos = vPos + qtRot * m_DummyCharInfoList[i].m_vPos;
		//vNewPos.y = fMapHeight;

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(m_DummyCharInfoList[i].m_fAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vRotDir = qtAngle * vMoveDir;
		qtAngle = Help::ConvertDirToQuaternion( vRotDir );

		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_DummyCharInfoList[i].m_szCreateName, iCurIndex, pOwner->GetCharName(), vNewPos, m_DummyCharInfoList[i].m_fAngle, 0, true );
		if ( pDummy )
		{
			pDummy->SetWorldPosition( vNewPos );
			pDummy->SetWorldOrientationAndDir( qtAngle );
		}
	}
}


bool ioBombWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	if ( m_BombState == BS_EXPLOSION_WAIT || m_BombState == BS_EXPLOSION )
		return true;

	if( GetCollisionEnableTime() >= FRAMEGETTIME() )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetExplosionWait();
		return true;
	}

	return false;
}

void ioBombWeapon::SetExplosionWait()
{
	m_BombState = BS_EXPLOSION_WAIT;
}
