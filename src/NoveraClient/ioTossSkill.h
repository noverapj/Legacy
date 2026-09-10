#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioTossSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_UP,
		SS_DOWN,
		SS_FINISH_DOWN,
		SS_END,
	};

	enum SyncType
	{
		SCT_NORMAL,
		SCT_DIR,
	};

protected:
	SkillState m_SkillState;

	// up
	ioHashString m_TossUpAni;
	CEncrypt<float> m_fTossUpAniRate;
	CEncrypt<float> m_fUpJumpPowerAmt;

	// down
	AttackAttribute m_TossDownAttack;
	AttackAttribute m_TossFinishAttack;

	DWORD m_dwAttackFireTime;

	// end
	ioHashString m_TossEndAni;
	float m_fTossEndAniRate;

	// move
	CEncrypt<float> m_fMoveSpeed;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// etc
	CEncrypt<int> m_iMaxTossCnt;
	CEncrypt<int> m_iCurTossCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void SetTossUpState( ioBaseChar *pChar );
	void SetTossDownState( ioBaseChar *pChar );
	void SetTossFinishState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );

	void OnTossUpState( ioBaseChar *pChar );
	void OnTossDownState( ioBaseChar *pChar );
	void OnTossFinishState( ioBaseChar *pChar );

	void ProcessFire( ioBaseChar *pChar );
	void ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckMoveDir( ioBaseChar *pChar );
	bool CheckDown( ioBaseChar *pChar );
	bool CheckLanding( ioBaseChar *pChar );
	
public:
	ioTossSkill();
	ioTossSkill( const ioTossSkill &rhs );
	virtual ~ioTossSkill();
};

inline ioTossSkill* ToTossSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_TOSS_SKILL )
		return NULL;

	return dynamic_cast< ioTossSkill* >(pAttack);
}

