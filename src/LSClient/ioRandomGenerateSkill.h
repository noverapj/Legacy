#pragma once


#include "ioAttackSkill.h"

class ioINILoader;
class ioBaseChar;
class SP2Packet;

class ioRandomGenerateSkill : public ioAttackSkill
{
protected:
	float m_fCircleOffSet;
	float m_fCircleRadius;
	float m_fGenerateStartHeight;
	DWORD m_dwGenerateStartTime;
	DWORD m_dwMinGenerateTime;
	DWORD m_dwMaxGenerateTime;
	DWORD m_dwNextGenerateTime;

	CEncrypt<int> m_iCurGenerateCnt;
	
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;

	DWORD m_dwCircleDuration;
	DWORD m_dwCircleEnd;

	UniqueObjID m_dwCurMapEffect;
	D3DXVECTOR3 m_vCirclePos;

	GenerateList m_vGenerateList;
	bool m_bSendGenerateList;

	DWORD m_dwSeed;
	D3DXVECTOR3 m_GeneratePos;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void GenerateBlizzard( ioBaseChar *pChar );

public:
	void OnGenerateList( ioBaseChar *pChar, SP2Packet &rkPacket );

public:
	ioRandomGenerateSkill();
	ioRandomGenerateSkill( const ioRandomGenerateSkill &rhs );
	virtual ~ioRandomGenerateSkill();
};

inline ioRandomGenerateSkill* ToRandomGenerate( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_RANDOM_GENERATE )
		return NULL;

	return dynamic_cast< ioRandomGenerateSkill* >(pAttack);
}

