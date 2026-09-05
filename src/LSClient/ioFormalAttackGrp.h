#pragma once

class ioINILoader;

#include "ioAttackAttribute.h"
class ioItem;
class ioFormalAttackGrp
{
private:
	AttributeList m_NormalAttackList;
	AttackAttribute m_CounterAttack;
	AttackAttribute m_DashAttack;
	AttackAttribute m_JumpAttack;
	AttackAttribute m_ExtendJumpAttack;
	AttackAttribute m_LandJumpAttack;
	AttackAttribute m_ExtendLandJumpAttack;
	AttackAttribute m_HighJumpAttack;
	AttributeList m_ExtraDashAttackList;
	AttributeList m_ExtraOtherDashAttackList;
	
public:
	void LoadProperty( ioINILoader &rkLoader, ioItem *pOwnerItem );
	void LoadProperty_Only_Monk( ioINILoader &rkLoader, ioItem *pOwnerItem );
	void LoadProperty_Extend( ioINILoader &rkLoader, ioItem *pOwnerItem, int nIndex = 1);
	
public:
	bool HasAttack( AttackType eType, int iSubIdx = 0 ) const;
	bool HasAttackEnd( AttackType eType, int iSubIdx = 0 ) const;
	int GetNormalMaxCombo() const;
	int GetExtraDashAttackMax() const;
	int GetExtraOtherDashAttackMax() const;

	const AttackAttribute* GetAttribute( AttackType eType, int iSubIdx = 0 ) const;
	const ioHashString& GetAniFile( AttackType eType, int iSubIdx = 0 ) const;
	float GetAttackAniRate( AttackType eType, int iSubIdx = 0 ) const;
	const ioHashString& GetEndAniFile( AttackType eType, int iSubIdx = 0 ) const;
	float GetAttackEndAniRate( AttackType eType, int iSubIdx = 0 ) const;
	DWORD GetAttackPreDelay( AttackType eType, int iSubIdx = 0 ) const;
	DWORD GetAttackAnimationDelay( AttackType eType, int iSubIdx = 0 ) const;

public:
	const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;

public:
	void PrepareLoadingAnimation( const ioHashString &rkAniName );

public:
	ioFormalAttackGrp();
	~ioFormalAttackGrp();
};

