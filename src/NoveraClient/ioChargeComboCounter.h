#pragma once

#include "ioCounterAttack.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

enum CounterExtendType
{
	CET_Default,
	CET_ShadowRanger,
	CET_LeeSunSin,
	CET_RedHood,
	CET_EXTEND,
};

class ExtendCounter;
class ioChargeComboCounter : public ioCounterAttack
{
	struct CounterBranchAniInfo
	{
		ioHashString Ani;
		float		 rate;
	};

public:
	enum ChargeComboState
	{
		CCS_None,
		CCS_Delay,
		CCS_Delay_Charge,
		CCS_Normal_Charge_Attack,
		CCS_Normal_Combo,
		CCS_Special_Charge_Attack,
		CCS_Special_Process,
		CCS_End,
		CCS_End_Jump,
		CCS_Branch_Ani,
	};

protected:
	CounterExtendType	m_CounterExtendType;
	ExtendCounter*		m_pExtendCounter;

	std::vector<CounterBranchAniInfo> m_vBranch;

protected:
	AttackAttribute		m_CounterDelay;
	AttackAttribute		m_CounterCharge;
	AttributeList		m_CounterComboAttList;
	int					m_nCounterMaxCombo;
	DWORD				m_dwChargeTime;

	float				m_fEndJumpPower;
	float				m_fJumpHeightGap;

protected:
	ChargeComboState	m_ChargeComboState;
	DWORD				m_dwCounterStartTime;
	DWORD				m_dwRedHoodComboStartTime;
	DWORD				m_dwComboStartTime;
	DWORD				m_dwEnableReserveKeyTime;
	DWORD				m_dwMotionEndTime;

	bool				m_bAttackKeyReserved;
	bool				m_bDefenseKeyReserved;
	bool				m_bJumpKeyReserved;

	//////////////////////////////////////////////////////////////////////////
	DWORD				m_dwRedHoodSpecialMoveMacroTime_S;
	DWORD				m_dwRedHoodSpecialMoveMacroTime_E;

public:
	int					m_nCurCounterCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CounterAttackType GetType() const;
	virtual CounterExtendType GetExtendType() const;
	virtual ioCounterAttack* Clone();

public:
	virtual ExtendCounter* GetExtendCounter() const;

public:
	virtual bool StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState );
	virtual void ProcessCAT( ioBaseChar *pOwner );
	virtual void ProcessSpecialCAT( ioBaseChar *pOwner );
	virtual void ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void ApplySpecialCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	bool IsNoDropState() const; 
	void SetRedHoodMacro( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );

protected:
	void Init();
	bool SetCounterDelayState( ioBaseChar *pOwner );
	bool SetChargeState( ioBaseChar *pOwner );
	void SetNormalComboState( ioBaseChar *pOwner );
	void SetNormalChargeAttackState( ioBaseChar *pOwner );

protected:
	void CheckCharging( ioBaseChar *pOwner );
	void CheckchargingRedHood( ioBaseChar *pOwner );
	void CheckComboCharging( ioBaseChar *pOwner );
	bool CheckComboInput( ioBaseChar *pOwner );

public:
	void ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump );
	void CheckReserveState( ioBaseChar *pOwner );

protected:	
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );
	bool IsFloatState( ioBaseChar *pOwner );

	void SetBranChAni( ioBaseChar *pOwner, int nIndex, bool bSendNet );

public:
	void SetChargeComboState( ioChargeComboCounter::ChargeComboState eChargeComboState ){ m_ChargeComboState = eChargeComboState; }
	void SetEnableReserveKeyTime( DWORD dwEnableReserveKeyTime ){ m_dwEnableReserveKeyTime = dwEnableReserveKeyTime; }
	void SetAttackKeyReserved( bool bAttackKeyReserved ){ m_bAttackKeyReserved = bAttackKeyReserved; }
	void SetDefenseKeyReserved( bool bDefenseKeyReserved ){ m_bDefenseKeyReserved = bDefenseKeyReserved; }
	void SetJumpKeyReserved( bool bJumpKeyReserved ){ m_bJumpKeyReserved = bJumpKeyReserved; }

public:
	ioChargeComboCounter::ChargeComboState GetChargeComboState(){ return m_ChargeComboState; }
	DWORD GetEnableReserveKeyTime(){ return m_dwEnableReserveKeyTime; }
	bool GetAttackKeyReserved(){ return m_bAttackKeyReserved; }
	bool GetDefenseKeyReserved(){ return m_bDefenseKeyReserved; }
	bool GetJumpKeyReserved(){ return m_bJumpKeyReserved; }

public:
	ioChargeComboCounter();
	ioChargeComboCounter( const ioChargeComboCounter &rhs );
	virtual ~ioChargeComboCounter();
};

inline ioChargeComboCounter* ToChargeComboCounter( ioCounterAttack *pCounter )
{
	if( !pCounter || pCounter->GetType() != CAT_CHARGE_COMBO )
		return NULL;

	return dynamic_cast< ioChargeComboCounter* >( pCounter );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 반격 특별 기본 클래스

class ExtendCounter
{
protected:
	CounterExtendType	m_CounterExtendType;

protected:
	DWORD				m_dwMotionEndTime;

public:
	virtual ExtendCounter* Clone();

public:
	virtual void LoadProperty( ioINILoader &rkLoader ){};

public:
	virtual bool StartCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter ){  return false; }
	virtual void ProcessCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter ){}
	virtual void ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter ){}	
	virtual bool SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter ){ return false; }

protected:
	virtual void CounterAttack(  ioBaseChar *pOwner, ioChargeComboCounter* pCounter, const AttackAttribute* pAttr, bool bAmtReset = true );

public:
	virtual bool IsNoDropState() const {  return false; }

public:
	virtual void ApplyCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket ){}
	virtual void ApplySpecialCAT( ioBaseChar *pOwner,ioChargeComboCounter* pCounter, SP2Packet &rkPacket ){}

public:
	ExtendCounter();
	ExtendCounter( const ExtendCounter &rhs );
	virtual ~ExtendCounter();

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//쉐도우 레인져
class ioShadowChargeComboCounter : public ExtendCounter
{
protected:
	enum ShadowSpecialState
	{
		SSS_None,
		SSS_Move,
		SSS_Combo,
	};

	AttackAttribute		m_ShadowRangerTeleportMove;
	D3DXVECTOR3			m_vShadowRangerTeleportOffset;
	AttributeList		m_ShadowRangerComboAttList;
	int					m_nShadowRangerTeleportComboMaxCnt;

protected:
	ShadowSpecialState	m_ShadowSpecialState;
	int					m_nShadowRangerTeleportComboCurCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ExtendCounter* Clone();

protected:
	virtual void ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );
	virtual bool SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );

	void SetShadowRangerComboState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );
	bool CheckShadowRangerComboInput( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );

	virtual void ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket );
	virtual bool IsNoDropState() const;

public:
	ioShadowChargeComboCounter();
	ioShadowChargeComboCounter( const ioShadowChargeComboCounter &rhs );
	~ioShadowChargeComboCounter();
};

inline ioShadowChargeComboCounter* ToShadowChargeComboCounter( ioChargeComboCounter *pCounter )
{
	if( !pCounter || pCounter->GetExtendType() != CET_ShadowRanger )
		return NULL;
	
	return dynamic_cast< ioShadowChargeComboCounter* >( pCounter->GetExtendCounter() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//이순신
class ioLeeSunSinExtendCounter : public ExtendCounter
{
protected:
	AttackAttribute m_ExtendCounter;
	AttackAttribute m_FullExtendCounter;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ExtendCounter* Clone();

protected:
	virtual void ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );
	virtual bool SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );

	virtual void ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket );

public:
	ioLeeSunSinExtendCounter();
	ioLeeSunSinExtendCounter( const ioLeeSunSinExtendCounter &rhs );
	~ioLeeSunSinExtendCounter();
};

inline ioLeeSunSinExtendCounter* ToLeeSunSinExtendCounter( ioChargeComboCounter *pCounter )
{
	if( !pCounter || pCounter->GetExtendType() != CET_LeeSunSin )
		return NULL;

	return dynamic_cast< ioLeeSunSinExtendCounter* >( pCounter->GetExtendCounter() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//RedHood
class ioRedHoodExtendCounter : public ExtendCounter
{
protected:
	AttributeList m_ChargeAttackList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ExtendCounter* Clone();

protected:
	virtual void ProcessSpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );
	virtual bool SetChargeState( ioBaseChar *pOwner, ioChargeComboCounter* pCounter );

	virtual void ApplySpecialCAT( ioBaseChar *pOwner, ioChargeComboCounter* pCounter, SP2Packet &rkPacket );

public:
	ioRedHoodExtendCounter();
	ioRedHoodExtendCounter( const ioRedHoodExtendCounter &rhs );
	~ioRedHoodExtendCounter();
};

inline ioRedHoodExtendCounter* ToRedHoodExtendCounter( ioChargeComboCounter *pCounter )
{
	if( !pCounter || pCounter->GetExtendType() != CET_RedHood )
		return NULL;

	return dynamic_cast< ioRedHoodExtendCounter* >( pCounter->GetExtendCounter() );
}