#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDashHandAttackSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

	enum FireType
	{
		FT_NORMAL,
		FT_WALL,		
	};

protected:
	WeaponInfo m_ContactRangeAttackInfoToChar;
	WeaponInfo m_ContactRangeAttackInfoToWall;
	CEncrypt<int> m_State;
	CEncrypt<int> m_FireType;

	// Attack
	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwFireTime;

	ioHashString m_SkillMapEffect;
	UniqueObjID m_dwSkillMapEffect;

	float m_fCharEffectOffset;
	float m_fWallEffectOffset;

	bool m_bOneTimeCharFire;
	bool m_bOneTimeWallFire;

	ioHashString m_FireSound;
	UniqueObjID m_dwFireSoundID;

	ioHashString m_WallCollsionSound;
	UniqueObjID m_dwWallCollsionSoundID;

	ioHashStringVec m_szCollsionIgoreCharList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );	
	virtual ioSkill* Clone();

	void InitSkillData();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );
	virtual void SetCollisionByApplySkill();

	virtual bool CheckUseItemMesh();

public:
	
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

protected:

	virtual void AttackSkillFire( ioBaseChar *pChar );


protected:	

	void CheckFireState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioDashHandAttackSkill();
	ioDashHandAttackSkill( const ioDashHandAttackSkill &rhs );
	virtual ~ioDashHandAttackSkill();
};

inline ioDashHandAttackSkill* ToDashHandAttackSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DASH_HAND_ATTACK )
		return NULL;

	return dynamic_cast< ioDashHandAttackSkill* >( pSkill );
}

