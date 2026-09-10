#pragma once

#include "IORandom.h"
#include "ioAttackSkill.h"

class ioBaseChar;
class ioWeapon;

class ioFullShotSkill2 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_ACTION_END,
		SS_CANCEL,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	D3DXVECTOR3 m_vSkillDir;

	int m_iMaxWeaponWay;
	int m_iWeaponWay;

	BulletInfoList m_BulletInfoList;

protected:
	CEncrypt<bool> m_bUseDependencyWeapon;

	DWORD m_dwStartSkillProtectedTime;
	DWORD m_dwSkillProtectedTime;
	DWORD m_dwStateChangeTime;

	ioHashString m_LoopStateAniName;
	float m_fLoopStateAniRate;
	DWORD m_dwLoopStateDuration;

	ioHashString m_ActionEndAniName;
	float m_fActionEndAniRate;

	ioHashString m_ActionCancelAniName;
	float m_fActionCancelAniRate;

public:
	virtual ioSkill* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsUseAutoTargetValue();
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void SetSkillLoopState( ioBaseChar *pChar );
	void SetSkillActionEndState( ioBaseChar *pChar );
	void SetSkillCancelState( ioBaseChar *pChar );
	void SetSkillEndState( ioBaseChar *pChar );

	bool CheckDependencyWeapon( ioBaseChar *pChar );
	
public:
	ioFullShotSkill2();
	ioFullShotSkill2( const ioFullShotSkill2 &rhs );
	virtual ~ioFullShotSkill2();
};

inline ioFullShotSkill2* ToFullShot2( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FULL_SHOT2 )
		return NULL;

	return dynamic_cast< ioFullShotSkill2* >( pAttack );

}


