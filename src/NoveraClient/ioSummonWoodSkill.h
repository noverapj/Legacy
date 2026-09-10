#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioSummonWoodSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum SkillSyncState
	{
		SSS_CREATE_DUMMY,
		SSS_CREATE_AREA_WEAPON,
		SSS_ACTION,
		SSS_CREATE_COUNT_DUMMY,
	};

	enum DummyCreateType
	{
		DCT_DIRECT,
		DCT_COUNT,
	};

protected:
	CEncrypt<int> m_State;

	bool			m_bCheckDropZone;

	// team circle
	ioHashString	m_RedCircle;
	ioHashString	m_BlueCircle;
	UniqueObjID		m_dwCurMapEffect;

	// SkillGauge
	CEncrypt<float>	m_fGaugePerTic;
	CEncrypt<float>	m_dwTicTime;
	DWORD			m_dwCheckTime;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;

	// areaweapon
	ioHashString m_AreaWeaponName;

	CEncrypt<int> m_iSummonCnt;
	CEncrypt<int> m_iCurSummonCnt;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// magic circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_OnCircle;
	ioHashString m_OffCircle;
	ioHashString m_CurCircleEffect;

	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;
	
	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwSkillProtectTime;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	DWORD			m_dwFirstLoopStartTime;

	CEncrypt<bool>	m_bDisableActionProtect;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	CEncrypt<bool>	m_bPreventRecoveryGauge;

	//
	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;
	
	float m_fCircleToCircleOffset;

	bool m_bOffCircle;
	bool m_bEnableSummon;

	DummyCreateType m_DummyCreateType;
	bool			m_bUseDestroyDummy;
	DWORD			m_dwLoopDuration;
	float			m_fReduceSkillGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool CheckUseItemMesh();

public:
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	
	void ClearData();

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void CheckDummyChar( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage );
	void ChangeCircleDropZoneAndPositionGap( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	bool CheckKOEffect() { return m_bPreventRecoveryGauge; }

public:
	ioSummonWoodSkill();
	ioSummonWoodSkill( const ioSummonWoodSkill &rhs );
	virtual ~ioSummonWoodSkill();
};

inline ioSummonWoodSkill* ToSummonWoodSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_SUMMON_WOOD )
		return NULL;

	return dynamic_cast< ioSummonWoodSkill* >( pSkill );
}

