

#include "stdafx.h"

#include "ioFormalAttackGrp.h"

ioFormalAttackGrp::ioFormalAttackGrp()
{
}

ioFormalAttackGrp::~ioFormalAttackGrp()
{
}

void ioFormalAttackGrp::LoadProperty( ioINILoader &rkLoader, ioItem *pOwnerItem )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "max_combo", 1 );

	int i = 0;
	m_NormalAttackList.clear();
	for(i = 0;i < iMaxCombo;i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}
	
	// Counter Attack
	m_CounterAttack.Init();
	LoadAttackAttribute_e( "counter_attack", m_CounterAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_CounterAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_CounterAttack.m_AttackEndAnimation );
	}

	// Dash Attack
	m_DashAttack.Init();
	LoadAttackAttribute_e( "dash_attack", m_DashAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_DashAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_DashAttack.m_AttackEndAnimation );
	}

	// extradash attack
	int iMaxAttackCnt = rkLoader.LoadInt_e( "extra_dash_max_cnt", 1 );

	m_ExtraDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_dash_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// extraotherdash attack
	iMaxAttackCnt = rkLoader.LoadInt_e( "extra_other_dash_max_cnt", 1 );

	m_ExtraOtherDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_other_dash_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraOtherDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// Jump Attack & Extend Jump Attack
	m_JumpAttack.Init();
	LoadAttackAttribute_e( "jump_attack", m_JumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_JumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_JumpAttack.m_AttackEndAnimation );
	}

	m_ExtendJumpAttack.Init();
	LoadAttackAttribute_e( "extend_jump_attack", m_ExtendJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackEndAnimation );
	}

	m_LandJumpAttack.Init();
	LoadAttackAttribute_e( "land_jump_attack", m_LandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackEndAnimation );
	}

	m_ExtendLandJumpAttack.Init();
	LoadAttackAttribute_e( "extend_land_jump_attack", m_ExtendLandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackEndAnimation );
	}

	m_HighJumpAttack.Init();
	LoadAttackAttribute_e( "high_jump_attack", m_HighJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackEndAnimation );
	}
}


void ioFormalAttackGrp::LoadProperty_Extend( ioINILoader &rkLoader, ioItem *pOwnerItem, int nIndex )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	wsprintf( szKey, "max_extend%d_combo", nIndex );

	int iMaxCombo = rkLoader.LoadInt( szKey, 1 );

	int i = 0;
	m_NormalAttackList.clear();
	for(i = 0;i < iMaxCombo;i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_extend%d_attack0%d", nIndex, i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackList.push_back( kAttribute );
		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// Counter Attack
	m_CounterAttack.Init();
	wsprintf( szKey, "counter_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_CounterAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_CounterAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_CounterAttack.m_AttackEndAnimation );
	}

	// extradash attack
	wsprintf( szKey, "extra_dash_extend%d_max_cnt", nIndex );
	int iMaxAttackCnt = rkLoader.LoadInt( szKey, 1 );
	m_ExtraDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_dash_extend%d_attack%d", nIndex, i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// extraotherdash attack
	wsprintf( szKey, "extra_other_dash_extend%d_max_cnt", nIndex );
	iMaxAttackCnt = rkLoader.LoadInt( szKey, 1 );
	m_ExtraOtherDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_other_dash_extend%d_attack%d", nIndex, i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraOtherDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// Jump Attack & Extend Jump Attack
	m_JumpAttack.Init();
	wsprintf( szKey, "jump_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_JumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_JumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_JumpAttack.m_AttackEndAnimation );
	}

	m_ExtendJumpAttack.Init();
	wsprintf( szKey, "extend_jump_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_ExtendJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackEndAnimation );
	}

	m_LandJumpAttack.Init();
	wsprintf( szKey, "land_jump_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_LandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackEndAnimation );
	}

	m_ExtendLandJumpAttack.Init();
	wsprintf( szKey, "extend_land_jump_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_ExtendLandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackEndAnimation );
	}

	m_HighJumpAttack.Init();
	wsprintf( szKey, "high_jump_extend%d_attack", nIndex );
	LoadAttackAttribute( szKey, m_HighJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackEndAnimation );
	}
}


void ioFormalAttackGrp::LoadProperty_Only_Monk( ioINILoader &rkLoader, ioItem *pOwnerItem )
{
	char szBuf[MAX_PATH];
	int iMaxCombo = rkLoader.LoadInt_e( "max_combo_monk", 1 );

	int i = 0;
	m_NormalAttackList.clear();
	for(i = 0;i < iMaxCombo;i++)
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "normal_monk_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_NormalAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// Counter Attack
	m_CounterAttack.Init();
	LoadAttackAttribute_e( "counter_attack", m_CounterAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_CounterAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_CounterAttack.m_AttackEndAnimation );
	}

	// Dash Attack
	m_DashAttack.Init();
	LoadAttackAttribute_e( "dash_monk_attack", m_DashAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_DashAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_DashAttack.m_AttackEndAnimation );
	}

	// extradash attack
	int iMaxAttackCnt = rkLoader.LoadInt_e( "extra_dash_monk_max_cnt", 1 );

	m_ExtraDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_dash_monk_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// extraotherdash attack
	iMaxAttackCnt = rkLoader.LoadInt_e( "extra_other_dash_monk_max_cnt", 1 );

	m_ExtraOtherDashAttackList.clear();
	for( i=0; i < iMaxAttackCnt; i++ )
	{
		AttackAttribute kAttribute;

		wsprintf_e( szBuf, "extra_other_dash_monk_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );		
		m_ExtraOtherDashAttackList.push_back( kAttribute );

		// Ani
		if( !pOwnerItem )
		{
			PrepareLoadingAnimation( kAttribute.m_AttackAnimation );
			PrepareLoadingAnimation( kAttribute.m_AttackEndAnimation );
		}
	}

	// Jump Attack & Extend Jump Attack
	m_JumpAttack.Init();
	LoadAttackAttribute_e( "jump_monk_attack", m_JumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_JumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_JumpAttack.m_AttackEndAnimation );
	}

	m_ExtendJumpAttack.Init();
	LoadAttackAttribute_e( "extend_jump_monk_attack", m_ExtendJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendJumpAttack.m_AttackEndAnimation );
	}

	m_LandJumpAttack.Init();
	LoadAttackAttribute_e( "land_jump_monk_attack", m_LandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_LandJumpAttack.m_AttackEndAnimation );
	}

	m_ExtendLandJumpAttack.Init();
	LoadAttackAttribute_e( "extend_land_jump_monk_attack", m_ExtendLandJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_ExtendLandJumpAttack.m_AttackEndAnimation );
	}

	m_HighJumpAttack.Init();
	LoadAttackAttribute_e( "high_jump_monk_attack", m_HighJumpAttack, rkLoader );
	if( !pOwnerItem )
	{
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackAnimation );
		PrepareLoadingAnimation( m_HighJumpAttack.m_AttackEndAnimation );
	}
}


bool ioFormalAttackGrp::HasAttack( AttackType eType, int iSubIdx ) const
{
	switch( eType )
	{
	case AT_NORMAL:
		if( !COMPARE( iSubIdx, 0, GetNormalMaxCombo() ) )
			return false;
		break;
	case AT_COUNTER_ATTACK:
		if( m_CounterAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_DASH_ATTACK:
		if( m_DashAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTRA_DASH_ATTACK:
		if( !COMPARE( iSubIdx, 0, GetExtraDashAttackMax() ) )
			return false;
		break;
	case AT_EXTRA_OTHER_DASH_ATTACK:
		if( !COMPARE( iSubIdx, 0, GetExtraOtherDashAttackMax() ) )
			return false;
		break;
	case AT_JUMP_ATTACK:
		if( m_JumpAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTEND_JUMP_ATTACK:
		if( m_ExtendJumpAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_LAND_JUMP_ATTACK:
		if( m_LandJumpAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTEND_LAND_JUMP_ATTACK:
		if( m_ExtendLandJumpAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	case AT_HIGH_JUMP_ATTACK:
		if( m_HighJumpAttack.m_AttackAnimation.IsEmpty() )
			return false;
		break;
	}

	return true;
}

bool ioFormalAttackGrp::HasAttackEnd( AttackType eType, int iSubIdx ) const
{
	switch( eType )
	{
	case AT_NORMAL:
		if( !COMPARE( iSubIdx, 0, GetNormalMaxCombo() ) )
			return false;
		if( m_NormalAttackList[iSubIdx].m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_COUNTER_ATTACK:
		if( m_CounterAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_DASH_ATTACK:
		if( m_DashAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTRA_DASH_ATTACK:
		if( !COMPARE( iSubIdx, 0, GetExtraDashAttackMax() ) )
			return false;
		if( m_ExtraDashAttackList[iSubIdx].m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTRA_OTHER_DASH_ATTACK:
		if( !COMPARE( iSubIdx, 0, GetExtraOtherDashAttackMax() ) )
			return false;
		if( m_ExtraOtherDashAttackList[iSubIdx].m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_JUMP_ATTACK:
		if( m_JumpAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTEND_JUMP_ATTACK:
		if( m_ExtendJumpAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_LAND_JUMP_ATTACK:
		if( m_LandJumpAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_EXTEND_LAND_JUMP_ATTACK:
		if( m_ExtendLandJumpAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	case AT_HIGH_JUMP_ATTACK:
		if( m_HighJumpAttack.m_AttackEndAnimation.IsEmpty() )
			return false;
		break;
	}

	return true;
}

int ioFormalAttackGrp::GetNormalMaxCombo() const
{
	return m_NormalAttackList.size();
}

int ioFormalAttackGrp::GetExtraDashAttackMax() const
{
	return m_ExtraDashAttackList.size();
}

int ioFormalAttackGrp::GetExtraOtherDashAttackMax() const
{
	return m_ExtraOtherDashAttackList.size();
}

const AttackAttribute* ioFormalAttackGrp::GetAttribute( AttackType eType, int iSubIdx ) const
{
	if( !HasAttack( eType, iSubIdx ) )
		return NULL;

	switch( eType )
	{
	case AT_NORMAL:
		return &m_NormalAttackList[iSubIdx];
	case AT_COUNTER_ATTACK:
		return &m_CounterAttack;
	case AT_DASH_ATTACK:
		return &m_DashAttack;
	case AT_EXTRA_DASH_ATTACK:
		return &m_ExtraDashAttackList[iSubIdx];
	case AT_EXTRA_OTHER_DASH_ATTACK:
		return &m_ExtraOtherDashAttackList[iSubIdx];
	case AT_JUMP_ATTACK:
		return &m_JumpAttack;
	case AT_EXTEND_JUMP_ATTACK:
		return &m_ExtendJumpAttack;
	case AT_LAND_JUMP_ATTACK:
		return &m_LandJumpAttack;
	case AT_EXTEND_LAND_JUMP_ATTACK:
		return &m_ExtendLandJumpAttack;
	case AT_HIGH_JUMP_ATTACK:
		return &m_HighJumpAttack;
	}

	LOG.PrintTimeAndLog( 0, "ioFormalAttackGrp::GetAttribute %d unknown AttackType", eType );

	return NULL;
}

const ioHashString& ioFormalAttackGrp::GetAniFile( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_AttackAnimation;

	return m_NormalAttackList[0].m_AttackAnimation;
}

float ioFormalAttackGrp::GetAttackAniRate( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_fAttackAniRate;

	return FLOAT1;
}

const ioHashString& ioFormalAttackGrp::GetEndAniFile( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_AttackEndAnimation;

	return m_NormalAttackList[0].m_AttackEndAnimation;
}

float ioFormalAttackGrp::GetAttackEndAniRate( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_fAttackEndAniRate;

	return FLOAT1;
}

DWORD ioFormalAttackGrp::GetAttackPreDelay( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_dwPreDelay;

	return 0;
}

DWORD ioFormalAttackGrp::GetAttackAnimationDelay( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_dwEndDelay;

	return 0;
}

const WeaponInfoList& ioFormalAttackGrp::GetAttackAttributeIdx( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_vWeaponInfoList;

	static WeaponInfoList vEmtpyList;
	return vEmtpyList;
}

const ForceInfoList& ioFormalAttackGrp::GetForceInfoList( AttackType eType, int iSubIdx ) const
{
	const AttackAttribute *pAttr = GetAttribute( eType, iSubIdx );
	if( pAttr )
		return pAttr->m_vForceInfoList;

	static ForceInfoList vForceEmptyList;
	return vForceEmptyList;
}

void ioFormalAttackGrp::PrepareLoadingAnimation( const ioHashString &rkAniName )
{
	if( rkAniName.IsEmpty() ) return;

	char szExt[MAX_PATH] = "";
	_splitpath( rkAniName.c_str(), NULL, NULL, NULL, szExt );
	if( strcmp( szExt, ".ani" ) != 0 )
		return;

	g_AnimationMgr.PrepareLoading( rkAniName, true );

	// 여성 애니가 따로 존재하면 포함
	ioHashString kFemaleAniName = Help::ConvertTailNameAndFileCheck( rkAniName, false );
	if( !kFemaleAniName.IsEmpty() )
	{
		g_AnimationMgr.PrepareLoading( kFemaleAniName, true );
	}
}


