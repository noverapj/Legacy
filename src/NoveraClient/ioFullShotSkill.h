#pragma once

#include "IORandom.h"
#include "ioAttackSkill.h"

class ioBaseChar;
class ioWeapon;

class ioFullShotSkill : public ioAttackSkill
{
protected:
	D3DXVECTOR3 m_vSkillDir;

	int m_iMaxWeaponWay;
	int m_iWeaponWay;

	BulletInfoList m_BulletInfoList;

	ioHashString m_RightSkillMotion;
	ioHashString m_LeftSkillMotion;
	ioHashString m_BackSkillMotion;

	DWORD m_dwSeed;
	IORandom m_SpeedRand;

	bool m_bUseSpeedRandom;
	float m_fMinSpeed;
	float m_fMaxSpeed;

	int m_iSecondAttributeIndex;

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
	
protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	virtual float GetAttackTimeRate() const;
	virtual float GetAttackEndTimeRate() const;

public:
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsUseAutoTargetValue();

protected:
	void SetSkillActionState( ioBaseChar *pChar );

	void CheckSkillAction( ioBaseChar *pChar );
	
public:
	ioFullShotSkill();
	ioFullShotSkill( const ioFullShotSkill &rhs );
	virtual ~ioFullShotSkill();
};

inline ioFullShotSkill* ToFullShot( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_FULL_SHOT )
		return NULL;

	return dynamic_cast< ioFullShotSkill* >( pAttack );

}


