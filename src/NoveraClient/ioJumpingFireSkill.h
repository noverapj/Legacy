#pragma once

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioJumpingFireSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ATTACK,
		SS_CreateWeapon,
		SS_END_ACTION,
		SS_END,
	};

protected:
	//스킬 상태
	SkillState m_SkillState;

	//스킬 보호 시간
	DWORD m_dwSkillProtectTime;
	//현재 스킬 보호 시간
	DWORD m_dwCurSkillProtectTime;

	bool m_bSetFirstAttack;

	ioHashString m_szAreaWeaponName;
	D3DXVECTOR3 m_vAreaWeaponOffset;

	float m_fJumpAngle;
	float m_fJumpPower;
	
	bool m_bPreActionInit;
	ioHashString m_szPreActionName;
	float m_fPreActionRate;
	DWORD m_dwPreActionEndTime;

	bool m_bLoopActionInit;
	ioHashString m_szLoopActionName;
	DWORD m_dwLoopTime;
	DWORD m_dwCheckLoopTime;

	bool m_bInputAttackKey;
	bool m_bAttackActionInit;
	ioHashString m_szAttackActionName;
	float m_fAttackActionRate;
	DWORD m_dwAttackActionEndTime;

	bool m_bEndActionInit;
	ioHashString m_szEndActionName;
	float m_fEndActionRate;
	DWORD m_dwEndActionEndTime;

	DWORD m_dwFireReloadTime;
	DWORD m_dwCheckFireReloadTime;

	int m_nFireMaxCnt;
	int m_nFireCurCnt;
	float m_fRandomRange;

	std::vector<int> m_FireWeaponList;
	int m_nFireWeaponIndex;

	bool m_bSetReduceGauge;

	DWORD m_dwTick;
	DWORD m_dwCheckTick;

	float m_fUseGuage;

	bool m_bSetNoJump;

	ioHashStringVec m_OwnerBuffList;

	bool m_bSetChangeState;
	
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

	virtual bool IsEndState() const;
	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsJumpState() const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void CreateAreaWeapon( ioBaseChar *pChar );
	void SetCharJumping( ioBaseChar *pChar );
	void SetPreAction( ioBaseChar *pChar );
	void SetLoopAction( ioBaseChar *pChar );
	void SetAttackAction( ioBaseChar *pChar );
	void SetEndAction( ioBaseChar *pChar );
	void CheckInputKey( ioBaseChar *pChar );
	void CreateWeapon( ioBaseChar *pChar );
	void ReduceGauge( ioBaseChar *pChar );
	void SetOwnerBuff( ioBaseChar *pChar );
	void SetChangeState( ioBaseChar *pChar );

public:
	ioJumpingFireSkill();
	ioJumpingFireSkill( const ioJumpingFireSkill &rhs );
	virtual ~ioJumpingFireSkill();
};

inline ioJumpingFireSkill* ToJumpingFireSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_JUMPING_FIRE )
		return NULL;

	return dynamic_cast< ioJumpingFireSkill* >( pAttack );
}

