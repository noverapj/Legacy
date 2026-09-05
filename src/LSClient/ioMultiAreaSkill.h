#pragma once

#include "ioNormalSkill.h"
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioMultiAreaSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
		SS_TELEPORT,
	};

	enum SkillSyncState
	{
		SSS_ACTION,
		SSS_TELEPORT,
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

	CEncrypt<int> m_iMaxAreaWeaponCnt;
	CEncrypt<int> m_iCurAreaWeaponCnt;

	DWORD m_dwFireStartTime;
	DWORD m_dwActionStartTime;

	// magic circle
	float m_fCircleRange;
	float m_fCircleMoveSpeed;
	float m_fCircleHeightGap;
	float m_fCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_ReadyCircle;
	ioHashString m_RedGuidCircle;
	ioHashString m_BlueGuidCircle;
	//ioHashString

	//
	float m_fEndMotionRate;
	ioHashString m_SkillEndMotion;

	ioHashString	m_LoopAnimation;
	CEncrypt<DWORD>	m_dwLoopProtectTime;
	DWORD			m_dwLoopStartTime;
	DWORD			m_dwFirstLoopStartTime;

	CEncrypt<bool>	m_bDisableActionProtect;

	//
	ioHashString m_StartEffect;
	ioHashString m_EndEffect;

	CEncrypt<bool>	m_bPreventRecoveryGauge;

	AttackAttribute m_TeleportInfo;
	bool m_bEnableDefenceKey;
	bool m_bCheckMaxAreaCnt;
	bool m_bSetLoopTime;
	DWORD m_dwLoopTime;
	bool m_bForceReduceGauge;

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
	void CheckCreateAreaWeapon( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetMagicCircle( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

public:
	bool CheckKOEffect() { return m_bPreventRecoveryGauge; }

protected:
	void SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	ioMultiAreaSkill();
	ioMultiAreaSkill( const ioMultiAreaSkill &rhs );
	virtual ~ioMultiAreaSkill();
};

inline ioMultiAreaSkill* ToMultiAreaSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_MULTI_AREA )
		return NULL;

	return dynamic_cast< ioMultiAreaSkill* >( pSkill );
}

