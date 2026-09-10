#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioNormalSkill.h"
#include "ioAttackAttribute.h"

class ioDummyComboFireSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_LOOP,
		SS_END,
	};

	enum SyncType
	{
		ST_ACTION,
		ST_CREATE_DUMMY,
		ST_REMOVE_DUMMY,
		ST_REMOVE_ALL_DUMMY,
	};
protected:
	SkillState m_SkillState;
	CEncrypt<DWORD> m_dwSkillProtectTime;
	
	ioHashString m_DummyCharName;
	IntVec	m_iDummyIndexList;
	float m_fDummyCharOffset;
	ioHashStringVec m_vDestroyDummyCharList;
	
	DWORD m_dwDummyCreateGap;

	//
	ioHashString	m_LoopDelayAnimation;
	CEncrypt<float>	m_fLoopDelayAnimationRate;

	ioHashString	m_LoopMoveAnimation;
	CEncrypt<float>	m_fLoopMoveAnimationRate;

	ioHashString	m_EndAnimation;
	CEncrypt<float>	m_fEndAnimationRate;

	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;

	DWORD m_dwLoopStartTime;
	bool m_bSetMoveAni;

	// 스킬 게이지 감소
	CEncrypt<bool>	m_bDisableGrowth;
	CEncrypt<float>	m_fGuagePerTic;
	CEncrypt<DWORD>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	int m_iMaxDummyCnt;
	int m_iDummyActiveCnt;
	BuffInfoList m_OwnerBuffList;

	int m_iRandomCnt;
	DWORD m_dwLifeTime;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	void ClearData();
	void LoadOwnerBuffList( ioINILoader &rkLoader );
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );
	
protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar );
	void CheckNormalMoveAni( ioBaseChar* pChar );
	void CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void RemoveAllDummy( ioBaseChar *pChar );
	void RemoveDummy( ioBaseChar *pChar );

	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	
public:
	bool HasOwnerBuff( const ioHashString &szBuffName );
public:
	ioDummyComboFireSkill();
	ioDummyComboFireSkill( const ioDummyComboFireSkill &rhs );
	virtual ~ioDummyComboFireSkill();
};

inline ioDummyComboFireSkill* ToDummyComboFireSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_DUMMY_COMBO_FIRE )
		return NULL;

	return dynamic_cast< ioDummyComboFireSkill* >( pSkill );
}