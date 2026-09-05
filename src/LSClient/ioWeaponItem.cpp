

#include "stdafx.h"

#include "ioWeaponItem.h"
#include "ioAniCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

#include "ioBaseChar.h"


ioWeaponItem::ioWeaponItem()
{
	m_pAniCollisionGrp = NULL;
	m_pChangeAniCollisionGrp = NULL;
	m_pCurAniCollisionGrp = NULL;
}

ioWeaponItem::ioWeaponItem( const ioWeaponItem &rhs )
: ioAttackableItem( rhs ),
 m_EquipMesh( rhs.m_EquipMesh ),
 m_ChangeEquipMesh( rhs.m_ChangeEquipMesh ),
 m_szReadyMarker( rhs.m_szReadyMarker ),
 m_szAimingMarker( rhs.m_szAimingMarker ),
 m_szAimedMarker( rhs.m_szAimedMarker ),
 m_szZeroAimedMarker( rhs.m_szZeroAimedMarker ),
 m_StartMotion( rhs.m_StartMotion ),
 m_WinMotion( rhs.m_WinMotion ),
 m_LoseMotion( rhs.m_LoseMotion ),
 m_DrawMotion( rhs.m_DrawMotion ),
 m_SpecialMotion( rhs.m_SpecialMotion ),
 m_dwSpecialMotionCamera( rhs.m_dwSpecialMotionCamera ),
 m_DieMotion( rhs.m_DieMotion ),
 m_DieBackMotion( rhs.m_DieBackMotion ),
 m_BlowDieAction( rhs.m_BlowDieAction ),
 m_BackBlowDieAction( rhs.m_BackBlowDieAction ),
 m_DownBlowDieAction( rhs.m_DownBlowDieAction ),
 m_BackdownBlowDieAction( rhs.m_BackdownBlowDieAction ),
 m_KillEffect( rhs.m_KillEffect )
{
	if( rhs.m_pAniCollisionGrp )
		m_pAniCollisionGrp = rhs.m_pAniCollisionGrp->Clone();
	else
		m_pAniCollisionGrp = NULL;

	if( rhs.m_pChangeAniCollisionGrp )
		m_pChangeAniCollisionGrp = rhs.m_pChangeAniCollisionGrp->Clone();
	else
		m_pChangeAniCollisionGrp = NULL;

	m_pCurAniCollisionGrp = m_pAniCollisionGrp;

	m_fCollisionBoxScale  = rhs.m_fCollisionBoxScale;
	m_fCollisionBoxOffset = rhs.m_fCollisionBoxOffset;

	m_dwChargeTime = rhs.m_dwChargeTime;

	m_iWeaponSoundType = rhs.m_iWeaponSoundType;

	int iAwardAniSize = rhs.m_AwardMotionList.size();
	for(int i = 0;i < iAwardAniSize;i++)
		m_AwardMotionList.push_back( rhs.m_AwardMotionList[i] );

	m_CurEquipMesh = rhs.m_EquipMesh;
}

ioWeaponItem::~ioWeaponItem()
{
	SAFEDELETE( m_pAniCollisionGrp );
	SAFEDELETE( m_pChangeAniCollisionGrp );

	m_EquipMeshMap.clear();
	m_AwardMotionList.clear();
}

void ioWeaponItem::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackableItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH];

	m_EquipMeshMap.clear();
	int iDecoCnt = 0;
	while( true )
	{
		sprintf_e( szTitle, "equip_mesh%d", iDecoCnt );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_EquipMeshMap.insert( DecoInfoMap::value_type( iDecoCnt++, szBuf ) );
	}

	rkLoader.LoadString_e( "change_equip_mesh", "", szBuf, MAX_PATH );
	m_ChangeEquipMesh = szBuf;

	SetItemType();      //Default.

	rkLoader.LoadString_e( "ready_marker", "", szBuf, MAX_PATH );
	m_szReadyMarker = szBuf;
	rkLoader.LoadString_e( "aiming_marker", "", szBuf, MAX_PATH );
	m_szAimingMarker = szBuf;
	rkLoader.LoadString_e( "aimed_marker", "", szBuf, MAX_PATH );
	m_szAimedMarker = szBuf;
	rkLoader.LoadString_e( "zero_aimed_marker", "", szBuf, MAX_PATH );
	m_szZeroAimedMarker = szBuf;

	rkLoader.LoadString_e( "start_motion", "", szBuf, MAX_PATH );
	m_StartMotion = szBuf;

	rkLoader.LoadString_e( "win_motion", "", szBuf, MAX_PATH );
	m_WinMotion = szBuf;
	rkLoader.LoadString_e( "lose_motion", "", szBuf, MAX_PATH );
	m_LoseMotion = szBuf;
	rkLoader.LoadString_e( "draw_motion", "", szBuf, MAX_PATH );
	m_DrawMotion = szBuf;

	rkLoader.LoadString_e( "special_motion", "", szBuf, MAX_PATH );
	m_SpecialMotion = szBuf;

	m_dwSpecialMotionCamera = (DWORD)rkLoader.LoadInt_e( "special_motion_camera", 0 );

	rkLoader.LoadString_e( "shop_motion", "", szBuf, MAX_PATH );
	m_UIMotion    = szBuf;

	m_AwardMotionList.clear();
	int iAwardAniCnt = 0;
	while( true )
	{
		sprintf_e( szTitle, "award_motion_%d", iAwardAniCnt++ );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_AwardMotionList.push_back( szBuf );
	}

	rkLoader.LoadString_e( "collision_mesh", "", szBuf, MAX_PATH );
	if( !m_pAniCollisionGrp && strcmp( szBuf, "" ) )
	{
		m_pAniCollisionGrp = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szBuf );
	}

	rkLoader.LoadString_e( "change_collision_mesh", "", szBuf, MAX_PATH );
	if( !m_pChangeAniCollisionGrp && strcmp( szBuf, "" ) )
	{
		m_pChangeAniCollisionGrp = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szBuf );
	}

	m_fCollisionBoxScale = rkLoader.LoadFloat_e( "collision_mesh_scale", FLOAT1 );
	m_fCollisionBoxOffset = rkLoader.LoadFloat_e( "collision_mesh_offset", 0.0f );
	
	m_dwChargeTime = rkLoader.LoadInt_e( "charge_time", 300 );

	m_iWeaponSoundType = rkLoader.LoadInt_e( "weapon_sound_type", 0 );

	rkLoader.LoadString_e( "die_motion", "", szBuf, MAX_PATH );
	m_DieMotion = szBuf;

	rkLoader.LoadString_e( "die_back_motion", "", szBuf, MAX_PATH );
	m_DieBackMotion = szBuf;

	rkLoader.LoadString_e( "blow_die_motion", "", szBuf, MAX_PATH );
	m_BlowDieAction = szBuf;

	rkLoader.LoadString_e( "back_blow_die_motion", "", szBuf, MAX_PATH );
	m_BackBlowDieAction = szBuf;

	rkLoader.LoadString_e( "down_blow_die_motion", "", szBuf, MAX_PATH );
	m_DownBlowDieAction = szBuf;

	rkLoader.LoadString_e( "backdown_blow_die_motion", "", szBuf, MAX_PATH );
	m_BackdownBlowDieAction = szBuf;

	rkLoader.LoadString_e( "kill_effect", "", szBuf, MAX_PATH );
	m_KillEffect = szBuf;
}

const ioHashString& ioWeaponItem::GetAwardMotion( int iAniCode ) const
{
	int iAwardAniSize = m_AwardMotionList.size();
	if( COMPARE( iAniCode, 0, iAwardAniSize ) )
		return m_AwardMotionList[iAniCode];
	return m_UIMotion;        // 예외 발생시 상점 애니 작렬
}

const ioHashString& ioWeaponItem::FindEquipMesh() const
{
	// Default Return
	DecoInfoMap::const_iterator iter = m_EquipMeshMap.find( 0 );
	if( iter != m_EquipMeshMap.end() )
		return iter->second;		

	LOG.PrintTimeAndLog( 0, "ioWeaponItem::FindEquipMesh : Unknown Default" );
	return m_EquipMesh;
}

void ioWeaponItem::SetItemType()
{
	ioItem::SetItemType();

	m_EquipMesh = FindEquipMesh();
	m_CurEquipMesh = m_EquipMesh;
}

void ioWeaponItem::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );

	m_CurEquipMesh = m_EquipMesh;
	m_pCurAniCollisionGrp = m_pAniCollisionGrp;
}

void ioWeaponItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
}

bool ioWeaponItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioWeaponItem::IsEnableJumpKeyEtcAction( ioBaseChar *pOwner )
{
	return true;
}

void ioWeaponItem::AddGlowLine( ioBaseChar *pOwner )
{
	pOwner->AddGlowLine( m_CurEquipMesh, m_GlowColor, 2.0f );
}

void ioWeaponItem::RemoveGlowLine( ioBaseChar *pOwner )
{
	pOwner->RemoveGlowLine( m_CurEquipMesh );
}

ioItem::ItemType ioWeaponItem::GetType() const
{
	return IT_WEAPON;
}

void ioWeaponItem::SetChangeEquipMesh( bool bChangeEquip )
{
	if( bChangeEquip && !m_CurEquipMesh.IsEmpty() )
	{
		m_CurEquipMesh = m_ChangeEquipMesh;
		m_pCurAniCollisionGrp = m_pChangeAniCollisionGrp;
	}
	else
	{
		m_CurEquipMesh = m_EquipMesh;
		m_pCurAniCollisionGrp = m_pAniCollisionGrp;
	}
}

const ioHashString& ioWeaponItem::GetEquipMesh( ioBaseChar *pOwner ) const
{
	return m_EquipMesh;
}


void ioWeaponItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{

}


void ioWeaponItem::WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar )
{

}

bool ioWeaponItem::IsCanSendControl() const
{
	return true;
}


bool ioWeaponItem::IsCmdCheckEnableTime()
{
	return false;
}


bool ioWeaponItem::IsCmdSuccess()
{
	return false;
}