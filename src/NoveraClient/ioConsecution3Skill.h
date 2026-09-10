#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioConsecution3Skill : public ioAttackSkill
{
protected:

	enum SkillState
	{
		SS_PRE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum HandState
	{
		HS_LEFT,
		HS_RIGHT,
	};

	enum MotionState
	{
		MS_PRE,
		MS_RUN,
	};

	enum SkillSync
	{	
		SSC_ACTION,
		SSC_RESERVE_ACTION,
	};

	SkillState m_State;
	HandState m_HandState;

	// 스킬 게이지 감소
	CEncrypt<bool> m_bDisableGrowth;
	CEncrypt<float> m_fGuagePerTic;
	CEncrypt<DWORD> m_dwTicTime;

	DWORD m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	MotionState m_PreMotionState;
	MotionState m_CurMotionState;

	ioHashString m_szPreLoopMotion;
	ioHashString m_szRunLoopMotion;

	ioHashString m_szLeftHandAction;
	ioHashString m_szRightHandAction;
	float m_fLeftHandActionRate;
	float m_fRightHandActionRate;

	ioHashString m_SkillMotionFr;
	ioHashString m_SkillMotionBk;
	ioHashString m_SkillMotionLt;
	ioHashString m_SkillMotionRt;
	float m_fSkillMotionRate;
	
	WeaponInfo m_LeftWeapon;
	WeaponInfo m_RightWeapon;

	int m_iWeaponFireCnt;
	
	DWORD m_dwFireMotionStartTime;
	DWORD m_dwFireMotionLoopCnt;
	DWORD m_dwCurFireMotionLoopCnt;

	DWORD m_dwEnableReserveEndTime;
	DWORD m_dwEnableReserveStartTime;

	bool m_bRetrunLoop;
	bool m_bMotionSetted;
	bool m_bFirstMoveMotion;
	ioUserKeyInput::DirKeyInput m_StartDirKey;

	CEncrypt<float> m_fMaxOverHeatGauge;	  //최대 과열수치
	CEncrypt<float> m_fDisableOverHeatGauge;  //무기 사용을 비활성화 할 수치
	
	CEncrypt<float> m_fIncreaseOverHeatGauge; // 공격중일 떄 증가할 과열 수치
	CEncrypt<float> m_fDecreaseOverHeatGauge; // 공격중일 떄 증가할 과열 수치

	float m_fCurOverHeatGauge;		//현재 과열수치

	CEncrypt<DWORD> m_dwSkillProtectedTime;
	DWORD m_dwStartSkillProtectedTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool CheckUseItemMesh();

protected:
	void LoadAttackAttribute( ioINILoader &rkLoader );

protected:
	virtual void AttackSkillFire( ioBaseChar *pChar );

protected:
	void ClearData();
	
	void SetActionState( ioBaseChar* pChar, ioPlayStage* pStage );
	void SetLoopState( ioBaseChar* pChar, ioPlayStage* pStage );

	void CheckActionMove( ioBaseChar *pChar );
	void CheckMoveAndDelay( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	
	inline float GetCurOverHeatCnt(){ return m_fCurOverHeatGauge; }
	inline float GetMaxOverHeatCnt(){ return m_fMaxOverHeatGauge; }
	inline float GetChangeUIOverHeatCnt(){ return m_fDisableOverHeatGauge; }

	inline EquipSlot GetOwerItemEquipSlot(){ return ES_CLOAK; }	

public:
	ioConsecution3Skill();
	ioConsecution3Skill( const ioConsecution3Skill &rhs );
	virtual ~ioConsecution3Skill();
};

inline ioConsecution3Skill* ToConsecution3Skill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONSECUTION3 )
		return NULL;

	return dynamic_cast< ioConsecution3Skill* >( pAttack );
}

