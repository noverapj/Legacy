
#include "stdafx.h"

#include "ioConveyer.h"

#include "ioPlayMode.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

#include "ioTeleportZone.h"

#include "FindListPredicateImpl.h"

ioConveyer::ioConveyer( ioEntityGroup *pGrp, ioPlayMode *pMode, int iCreateIndex )
: ioPlayEntity( pGrp, pMode )
{
	m_iCreateIndex = iCreateIndex;

	m_vMoveDir = D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 );
	m_fMoveSpeed = FLOAT100;

	m_bEnableFunction = true;
}

ioConveyer::~ioConveyer()
{
}

void ioConveyer::LoadProperty( ioINILoader &rkLoader )
{
	m_vMoveDir.x = rkLoader.LoadFloat_e( "move_dir_x", 0.0f );
	m_vMoveDir.y = rkLoader.LoadFloat_e( "move_dir_y", 0.0f );
	m_vMoveDir.z = rkLoader.LoadFloat_e( "move_dir_z", FLOAT1 );
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "move_speed", FLOAT100 );

	m_bNotMovePushStruct = rkLoader.LoadBool_e( "not_move_push_struct", 0 );
}

void ioConveyer::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	if( !m_bEnableFunction )	return;

	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( !pStage )	return;

	D3DXVECTOR3 vMoveAmt = m_vMoveDir * ( m_fMoveSpeed * fTimePerSec );

	FL_CollisionPlayEntity kFind( GetWorldCollisionBox(), this );
	if( pStage->FindEntityList( kFind ) )
	{
		const PlayEntityList &rkEntityList = kFind.GetResultList();

		PlayEntityList::const_iterator iter = rkEntityList.begin();
		for( ; iter!=rkEntityList.end() ; ++iter )
		{
			if( m_bNotMovePushStruct && ToPushStruct( (*iter) ) )
				continue;

			if( ToTeleportZone( (*iter) ) )
				continue;

			ioBaseChar* pChar = ToBaseChar( (*iter) );
			if ( pChar )
			{
				if( pChar->GetConveyerDisable() )
					continue;
			}

			(*iter)->TranslateByConveyer( vMoveAmt );
		}
	}

	MountedWeaponList vMountedWeaponList;
	if( pStage->UpdateMountedWeaponListByBox( this, vMountedWeaponList ) )
	{
		MountedWeaponList::iterator iMount = vMountedWeaponList.begin();
		for( ; iMount!=vMountedWeaponList.end() ; ++iMount )
		{
			ioMineWeapon *pMine = ToMineWeapon( *iMount );
			ioMineWeapon2 *pMine2 = ToMineWeapon2( *iMount );
			ioMineWeapon3 *pMine3 = ToMineWeapon3( *iMount );
			ioMineWeapon4 *pMine4 = ToMineWeapon4( *iMount );
			ioMineWeapon6 *pMine6 = ToMineWeapon6( *iMount );
			ioBoundBombWeapon *pBound = ToBoundBombWeapon( *iMount );
			ioMapAttachWeapon *pAttach = ToMapAttachWeapon( *iMount );

			if( pMine )
				pMine->TranslateByConveyer( vMoveAmt );
			else if( pMine2 )
				pMine2->TranslateByConveyer( vMoveAmt );
			else if( pMine3 )
				pMine3->TranslateByConveyer( vMoveAmt );
			else if( pMine4 )
				pMine4->TranslateByConveyer( vMoveAmt );
			else if( pMine6 )
				pMine6->TranslateByConveyer( vMoveAmt );
			else if( pBound )
				pBound->TranslateByConveyer( vMoveAmt );
			else if( pAttach )
				pAttach->TranslateByConveyer( vMoveAmt );
		}
	}
}

void ioConveyer::UpdateAfter( float fFrameGap )
{
	if( m_pGroup )
	{
		m_pGroup->Update( fFrameGap );
	}

	ioPlayEntity::UpdateAfter( fFrameGap );
}

bool ioConveyer::IsMountingAvailable() const
{
	return false;
}

void ioConveyer::UpdateBottomHeight() const
{
	m_fCurBottomHeight = GetWorldPosition().y;
}

ApplyWeaponExp ioConveyer::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioConveyer::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioConveyer::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}

bool ioConveyer::IsWoundedCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint ) const
{
	return false;
}

ioPlayEntity::PlaySubType ioConveyer::GetSubType() const
{
	return PST_CONVEYER;
}

ArmorType ioConveyer::GetArmorType() const
{
	return AT_UNARMOR;
}

int ioConveyer::DontMoveEntityLevel() const
{
	return 6;
}

void ioConveyer::EnableWorldFunction( bool bEnable )
{
	m_bEnableFunction = bEnable;
}