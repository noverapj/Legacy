

#include "stdafx.h"

#include "ioAttackAttribute.h"

void AttackAttribute::Init()
{
	m_fAttackAniRate = FLOAT1;
	m_fAttackEndAniRate = FLOAT1;
	m_dwPreDelay = 0;
	m_dwEndDelay = 0;

	m_AttackAnimation.Clear();
	m_AttackEndAnimation.Clear();

	m_vWeaponInfoList.clear();
	m_vForceInfoList.clear();
}

void LoadAttackAttribute( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader )
{
	rkAttribute.Init();

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	rkLoader.LoadString( szAttack, "", szBuf, MAX_PATH );
	rkAttribute.m_AttackAnimation = szBuf;

	if( rkAttribute.m_AttackAnimation.IsEmpty() )
		return;

	wsprintf_e( szKey, "%s_time_rate", szAttack );
	rkAttribute.m_fAttackAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_end_animation", szAttack );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkAttribute.m_AttackEndAnimation = szBuf;

	wsprintf_e( szKey, "%s_end_time_rate", szAttack );
	rkAttribute.m_fAttackEndAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

	wsprintf_e( szKey, "%s_pre_delay", szAttack );
	rkAttribute.m_dwPreDelay = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_delay", szAttack );
	rkAttribute.m_dwEndDelay = rkLoader.LoadInt( szKey, 0 );


	wsprintf_e( szKey, "%s_force_cnt", szAttack );
	int iForceCnt = rkLoader.LoadInt( szKey, 0 );
	if( iForceCnt > 0 )
		rkAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "%s_force_move%d", szAttack, i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_force_friction%d", szAttack, i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s_force_angle%d", szAttack, i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		rkAttribute.m_vForceInfoList.push_back( kInfo );
	}


	wsprintf_e( szKey, "%s_type_count", szAttack );
	int iAttackTypeCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "%s_type%d", szAttack, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szAttack, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szAttack, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack, i+1 );
        kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szAttack, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}


void LoadForceInfo( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH];

	wsprintf_e( szKey, "%s_force_cnt", szAttack );
	int iForceCnt = rkLoader.LoadInt( szKey, 0 );
	if( iForceCnt > 0 )
		rkAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "%s_force_move%d", szAttack, i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_force_friction%d", szAttack, i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "%s_force_angle%d", szAttack, i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		rkAttribute.m_vForceInfoList.push_back( kInfo );
	}
}

void LoadWeaponInfo( const char *szAttack, AttackAttribute &rkAttribute, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_type_count", szAttack );
	int iAttackTypeCnt = rkLoader.LoadInt( szKey, 0 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "%s_type%d", szAttack, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szAttack, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szAttack, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack, i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_wounded_loop_ani", szAttack, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void LoadWeaponInfo( const char *szWeapon, WeaponInfo &rkWeaponInfo, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_type", szWeapon );
	rkWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "%s_type_resist", szWeapon );
	rkWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "%s_type_wounded", szWeapon );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	rkWeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "%s_type_wounded_time", szWeapon );
	rkWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "%s_type_wounded_loop_ani", szWeapon );
	rkWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
}

void LoadWeaponInfoList( const char *szAttack, WeaponInfoList &WeaponInfoList, ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "%s_type_count", szAttack );
	int iCnt = rkLoader.LoadInt( szKey, 0 );
	WeaponInfoList.reserve( iCnt );
	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "%s_type%d", szAttack, i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_resist", szAttack, i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_type%d_wounded", szAttack, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_type%d_loop_ani", szAttack, i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "%s_type%d_wounded_time", szAttack, i+1 );
		kInfo.m_dwWoundedDuration = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_type%d_call_time", szAttack, i+1 );
		kInfo.m_dwCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		WeaponInfoList.push_back( kInfo );
	}
}
