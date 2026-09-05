#pragma once

// ioChargingFireSkill.h
// 차징중 이동이 가능한 형태.

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

struct CHARGING_INFO
{
	DWORD			dwChargingTime;
	float			fReduceGaugeRate;
	ioHashString	szWeaponEffectName;
	ioHashString	szEffectName;
	ioHashString	szAttackEffectName;
	DWORD			dwWeaponEffectID;
	DWORD			dwAttackEffectID;
	AttackAttribute AttackAttribute;
};

typedef	std::vector<CHARGING_INFO> ChargingInfoList;

class ioChargingFireSkill5 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_PREDELAY,
		SS_LOOP,
		SS_ACTION,
		SS_LOOP_AFTER_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
	};

protected:
	CEncrypt<int> m_State;

	ioHashString	m_szNormalDelayAnimation;
	ioHashString	m_szNormalMoveAnimation;
	CEncrypt<bool>	m_bDirection;
	bool			m_bSetMoveAni;

	// Attack
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;

	DWORD			m_dwNormalStartTime;
	DWORD			m_dwAttackEndTime;
	CEncrypt<DWORD>	m_dwNormalDuration;
	CEncrypt<DWORD>	m_dwSkillProtectTime;
	DWORD			m_dwCurSkillProtectTime;

	ioHashString	m_szLoopMotion;
	CEncrypt<DWORD>	m_dwLoopDuration;
	CEncrypt<float>	m_fLoopMotionRate;

	// Charging

	UniqueObjID			m_dwChargingEffect;
	CEncrypt<int>		m_iTotalChargingCount;
	ChargingInfoList	m_ChargingInfoList;
	int					m_iCurChargingPhase;
	float				m_fTotalReduceGauge;
	float				m_fLimitReduceGauge;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );
	void SetLoopAfterActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void CheckCurChargingPhase( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckNormalMoveAni( ioBaseChar *pChar );

	bool CheckControlWeapon( ioBaseChar *pChar );

public:
	bool IsWeaponControl( ioBaseChar *pChar );
	const D3DXVECTOR3& GetWeaponPos( ioBaseChar *pChar );

public:
	ioChargingFireSkill5();
	ioChargingFireSkill5( const ioChargingFireSkill5 &rhs );
	virtual ~ioChargingFireSkill5();
};

inline ioChargingFireSkill5* ToChargingFireSkill5( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE5 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill5* >( pSkill );
}

