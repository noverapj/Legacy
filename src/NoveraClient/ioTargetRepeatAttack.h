#pragma once

#include "ioAttackSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTargetRepeatAttack : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_FIND_TARGET,
		SS_TARGET_ATTACK,
		SS_REPEAT_ATTACK,
		SS_END,
	};
	
protected:
	DWORD m_dwSkillProtectTime;

	AttackAttribute m_TargetAttack;

	int m_nMaxRepeatCnt;
	AttributeList m_vRepeatAttackList;

	ioHashString m_szDelayLoopAni;
	float m_fDelayLoopAniRate;
	ioHashString m_szMoveLoopAni;
	float m_fMoveLoopAniRate;

	float m_fTargetFindScaleSpeedRate;
	float m_fTargetFindScaleMaxRate;
	ioHashString m_TargetFindEffect;
	float m_fTargetFindEffectHeightOffset;

	float m_fTargetAngle;
	float m_fTargetMinRange;
	float m_fTargetMaxRange;
	float m_fTargetRangeRate;
	bool m_bEnableTargetDown;
	ioHashString m_szTargetBuff;

	DWORD m_dwFindLoopTime;

	float m_fFindMoveSpeedRate;

protected:
	SkillState m_SkillState;
	DWORD m_dwCurSkillProtectTime;

	DWORD m_dwAttackReserveTime;
	DWORD m_dwFireStartTime;
	int m_nCurRepeatCnt;

	bool m_bSetDelay;
	bool m_bSetMove;

	DWORD m_dwTargetFindEffectID;
	float m_fCurScaleRate;

	ioHashStringVec m_szTargetList;

	float m_fTargetCurRange;

	DWORD m_dwCheckFindLoopTime;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
		
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsAttackEndState() const;
	virtual float GetSkillMoveRate() const;
	
protected:
	void Init();
	bool CheckKeyInput( ioBaseChar *pChar );

protected:
	void UpdateTargetEffect( ioBaseChar *pChar, ioPlayStage* pStage );
	bool FindTarget( ioBaseChar *pChar, ioPlayStage* pStage );
	void SortTargetList( ioBaseChar *pChar, const BaseCharList &TargetList );
	bool IsEnableTarget( ioBaseChar *pTarget );
	void CheckDelayMoveAni( ioBaseChar *pChar );
	void CreateWeapon( ioBaseChar *pChar );

protected:
	void SetFindTargetState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTargetAttackState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetRepeatAttackState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

protected:
	void AddTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage );
	void RemoveTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage, const BaseCharList &TargetList );
	void RemoveAllTargetBuff( ioBaseChar *pChar, ioPlayStage *pStage );
		
public:
	ioTargetRepeatAttack();
	ioTargetRepeatAttack( const ioTargetRepeatAttack &rhs );
	virtual ~ioTargetRepeatAttack();
};

inline ioTargetRepeatAttack* ToTargetRepeatAttack( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TARGET_REPEAT_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetRepeatAttack* >( pAttack );
}