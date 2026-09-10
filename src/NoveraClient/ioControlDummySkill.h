#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioPlayStage;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioControlDummySkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_PREACTION,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum ReduceSkillGuageKey
	{
		RK_NONE,
		RK_ATTACK_KEY,
		RK_DEFENSE_KEY,
		RK_JUMP_KEY,
	};

protected:
	SkillState m_SkillState;
	int	m_iDummyIndex;
	D3DXVECTOR3 m_vCreatePos;

	// 스킬 게이지 감소 관련
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<int>	m_iReduceSkillGuageKey;
	bool			m_bReduceSkillGuage;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwSkillProtectTime;

	ioHashString m_DummyCharName;
	D3DXVECTOR3	m_vDummyCharOffset;

	ioHashStringVec m_vDestroyDummyCharList;

	// 추가 : 점퍼 SD
	AttackAttribute m_PreActionAttribute;
	bool			m_bRemoveAllGauge;

	bool m_bUseActiveCnt;
	bool m_bSetUseCount;

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

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

public:
	bool IsDummyCharControl( ioBaseChar *pChar );
	bool IsDummyCharEnable( ioBaseChar *pChar );
	D3DXVECTOR3 GetDummyCharControlPos( ioBaseChar *pChar );

protected:
	void SetSkillActionState( ioBaseChar *pChar );
	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessLoopRotate( ioBaseChar *pChar );

	void LoadPreActionAttribute( ioINILoader &rkLoader );
	void SetPreActionState( ioBaseChar *pChar );

public:
	void SetSkillEndState( ioBaseChar *pChar );

public:
	void StartReduceGuage();

public:	
	virtual bool IsUseActiveCnt() const;
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const;
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const;
	
public:
	ioControlDummySkill();
	ioControlDummySkill( const ioControlDummySkill &rhs );
	virtual ~ioControlDummySkill();
};

inline ioControlDummySkill* ToControlDummySkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_DUMMY )
		return NULL;

	return dynamic_cast< ioControlDummySkill* >(pAttack);
}

