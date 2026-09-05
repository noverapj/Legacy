#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiDummyCharSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum CreateDummyKey
	{
		CDK_NONE,
		CDK_ATTACK,
		CDK_DEFENSE,
		CDK_JUMP,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	// dummychar
	ioHashString m_AttackKeyDummy;
	ioHashString m_DefenseKeyDummy;
	ioHashString m_JumpKeyDummy;

	CEncrypt<int> m_iMaxDummyCharCnt;
	CEncrypt<int> m_iCurDummyCharCnt;

	CreateDummyKey m_CurCreateKey;

	CEncrypt<float> m_fDummyCharOffset;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void CheckCreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, CreateDummyKey eKey );
	void SetEndState( ioBaseChar *pChar );

public:
	ioMultiDummyCharSkill();
	ioMultiDummyCharSkill( const ioMultiDummyCharSkill &rhs );
	virtual ~ioMultiDummyCharSkill();
};

inline ioMultiDummyCharSkill* ToMultiDummyCharSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_DUMMYCHAR )
		return NULL;

	return dynamic_cast< ioMultiDummyCharSkill* >( pSkill );
}

