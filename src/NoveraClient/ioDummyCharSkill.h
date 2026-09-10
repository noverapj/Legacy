#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioPlayStage;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioDummyCharSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<DWORD> m_dwFireStartTime;

	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

	BuffInfoList m_OwnerBuffList;

	bool m_bSkillMotionLoop;
	bool m_bSkillMotionLoopCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadOwnerBuffList( ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

	bool SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage );
	
	bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	void SkillMotionEndByBuff( ioBaseChar* pOwner, ioHashString& szBuffName );
public:
	ioDummyCharSkill();
	ioDummyCharSkill( const ioDummyCharSkill &rhs );
	virtual ~ioDummyCharSkill();
};

inline ioDummyCharSkill* ToDummyCharSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_DUMMY_CHAR )
		return NULL;

	return dynamic_cast< ioDummyCharSkill* >(pAttack);
}





//////////////////////////////////////////////////////////////////////////
// npc spawn skill
//////////////////////////////////////////////////////////////////////////


class ioNpcSpawnSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
	};

protected:
	SkillState m_SkillState;
	CEncrypt<DWORD> m_dwFireStartTime;

	struct stNpc
	{
		DWORD			dwNpcCode;
		DWORD			dwStartTime;
		float			fOffset;
		float			fSideOffset;
		float			fStartAngle;
	};

	std::vector <stNpc> m_vecNpc;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void SendSpawnNpcMsg( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioNpcSpawnSkill();
	ioNpcSpawnSkill( const ioNpcSpawnSkill &rhs );
	virtual ~ioNpcSpawnSkill();
};
