#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioBlindlyShotSkill : public ioAttackSkill
{
protected:

	enum SkillState
	{
		SS_PRE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum ActionSubState
	{
		ASS_ACTION,
		ASS_LOOP,
		ASS_END,
	};

	enum MotionState
	{
		MS_PRE,
		MS_RUN,
	};

	enum SkillSync
	{	
		SSC_ACTION,
	};

protected:

	SkillState m_State;
	ActionSubState m_SubState;
	
	BuffInfoList m_OwnerBuffList;
	
	MotionState m_PreMotionState;
	MotionState m_CurMotionState;
	float m_fPreMotionRate;
	float m_fRunLoopMotionRate;

	ioHashString m_szPreLoopMotion;
	ioHashString m_szRunLoopMotion;

	float m_fNeedFireGuage;
	bool m_bSkillEnd;

	DWORD m_dwMaxBulletCnt;
	DWORD m_dwCurBulletCnt;

	float m_dwKeyReserveTime;

	//스킬 게이지 감소
	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	AttackAttribute m_PreAttackAttribute;

	bool			m_bNotUseActiveCount;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

	void LoadOwnerBuffList( ioINILoader &rkLoader );

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsCanSwitchSkill() const;

protected:

	void ClearData();

	void SetActionState( ioBaseChar *pChar, ioPlayStage* pStage, bool bSend = true );
	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetEndState( ioBaseChar* pChar, ioPlayStage* pStage );

	void CheckMove( ioBaseChar* pChar, ioPlayStage* pStage );

	bool SetOwnerBuffList( ioBaseChar *pChar, ioPlayStage* pStage );

	void SetPreAttackState( ioBaseChar *pChar, ioPlayStage* pStage );

public:
	inline DWORD GetCurBulletCnt(){ return m_dwCurBulletCnt; }
	inline DWORD GetMaxBulletCnt(){ return m_dwCurBulletCnt; }
	virtual void SetSpecialValue(){ m_dwCurBulletCnt = m_dwMaxBulletCnt; }

	inline EquipSlot GetOwerItemEquipSlot(){ return ES_ARMOR; }
	inline const bool& GetNotUseActiveCount() { return m_bNotUseActiveCount; }

public:
	ioBlindlyShotSkill();
	ioBlindlyShotSkill( const ioBlindlyShotSkill &rhs );
	virtual ~ioBlindlyShotSkill();
};

inline ioBlindlyShotSkill* ToBlindlyShotSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_BLINDLY_SHOT )
		return NULL;

	return dynamic_cast< ioBlindlyShotSkill* >(pAttack);
}

