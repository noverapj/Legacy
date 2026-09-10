#pragma once

#include "ioNormalSkill.h"
#include "WeaponAttribute.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;

class ioAreaStruct2Skill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_END,
	};

	enum SkillSync
	{
		SSC_KEY,
	};

protected:
	SkillState	m_SkillState;

protected:
	void ClearData();

// Area
protected:
	ioHashString m_AreaWeaponName;

	DWORD m_dwAreaWeaponIndex;

	float m_fOffSet;
	float m_fYOffSet;
	
	ioHashString m_RedCircle;
	ioHashString m_BlueCircle;
	UniqueObjID m_dwCurMapEffect;
	
	DWORD m_dwCircleDuration;
	DWORD m_dwCircleEnd;
	D3DXVECTOR3 m_vCreatePos;

	DWORD m_dwCreateTime;

// Struct
protected:
	int m_iBildStruct;

	ioHashString m_BildEffect;

	int m_iMaxStructCnt;

// etc
protected:
	ForceInfoList m_vForceInfoList;

//
protected:
	DWORD m_dwAttackEndTime;

	bool m_bEnableCheckHeight;
	float m_fUpLimitHeight;
	float m_fDownLimitHeight;

// Loop Motion
protected:
	ioHashString	m_szSkillLoopMotion;
	CEncrypt<float>	m_fSkillLoopMotionRate;
	CEncrypt<DWORD>	m_dwSkillLoopDuration;
	CEncrypt<DWORD>	m_dwSkillLoopProtectTime;
	DWORD			m_dwSkillLoopStartTime;

// Finish Attack
	AttackAttribute m_FinishAttackAttribute;

// Charging
protected:
	float			m_fCurChargingRate;
	CEncrypt<DWORD>	m_dwChargingMaxTime;
	CEncrypt<DWORD>	m_dwEnableKeyTime;
	CEncrypt<DWORD>	m_dwNextKeyTime;
	DWORD			m_dwCheckStartTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void SetActionState( ioBaseChar *pChar );
	
	void CreateArea( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateStruct( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetLoopState( ioBaseChar* pChar );
	void SetFinishAttackFire( ioBaseChar* pChar, bool bSend );

public:
	ioAreaStruct2Skill();
	ioAreaStruct2Skill( const ioAreaStruct2Skill &rhs );
	virtual ~ioAreaStruct2Skill();
};

inline ioAreaStruct2Skill* ToAreaStruct2Skill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_AREA_STRUCT2 )
		return NULL;

	return dynamic_cast< ioAreaStruct2Skill* >( pSkill );
}

