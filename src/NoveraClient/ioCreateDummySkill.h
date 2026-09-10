#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioPlayStage;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCreateDummySkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_CREATE_DUMMY,
		SS_END,
	};

protected:
	//스킬 상태
	SkillState m_SkillState;
	//무적 시간
	CEncrypt<DWORD> m_dwSkillProtectTime;
	//더미 이름
	ioHashString m_DummyCharName;
	//더미 인덱스
	int	m_iDummyIndex;		
	//더미 오프셋
	float	m_fDummyCharOffset;
	//더미 최대 갯수
	CEncrypt<int> m_nMaxCreateCnt;
	
	bool m_bSetUseActiveCnt; 

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );
	
protected:
	void CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	void SetSkillEndState( ioBaseChar *pChar );

public:
	ioCreateDummySkill();
	ioCreateDummySkill( const ioCreateDummySkill &rhs );
	virtual ~ioCreateDummySkill();
};

inline ioCreateDummySkill* ToCreateDummySkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CREATE_DUMMY )
		return NULL;

	return dynamic_cast< ioCreateDummySkill* >(pAttack);
}

