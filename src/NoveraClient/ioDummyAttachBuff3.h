#pragma once

#include "ioBuff.h"
#include "ioSpecialStateBase.h"

class ioBaseChar;
class ioINILoader;

class ioDummyAttachBuff3 : public ioBuff
{
protected:
	enum BuffState
	{
		BS_None,
		BS_Move,
		BS_End,
		BS_CreateDummy,
	};

protected:
	BuffState m_BuffState;	
	DWORD m_dwCheckGaugeTime;
	bool m_bSetReduceGauge;

protected:
	ioHashString m_szDummyName;
	D3DXVECTOR3 m_vDummyOffset;

	float m_fEndJumpPower;
	
	ioHashStringVec m_OwnerBuffList;

	DWORD m_dwGaugeTicTime;
	float m_fUseGauge;

	bool m_bEnableWeaponSkill;
	bool m_bEnableArmorSkill;
	bool m_bEnableHelmetSkill;
	bool m_bEnableCloakSkill;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );

protected:
	void CreateDummy( ioBaseChar *pOwner );

protected:
	void SetMoveState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );
	
protected:
	bool CheckOwnerState( ioBaseChar *pOwner );
	void CheckGauge( ioBaseChar *pOwner );
	void CheckMove( ioBaseChar *pOwner );

public:
	bool CheckEnableUseSkill( int iSlot );
	void SetReduceStartGauge();

public:
	bool CheckDummy();
	D3DXVECTOR3 GetDummyCharPos();

public:
	ioDummyAttachBuff3();
	ioDummyAttachBuff3( const ioDummyAttachBuff3 &rhs );
	virtual ~ioDummyAttachBuff3();
};

inline ioDummyAttachBuff3* ToDummyAttachBuff3( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_ATTACH3 )
		return NULL;

	return dynamic_cast< ioDummyAttachBuff3* >( pBuff );
}

//-----------------------------------------------------------------------------------------------------------

class ioHelicopterSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HELICOPTER"; }
	virtual const CharState GetState(){ return CS_HELICOPTER; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioHelicopterSpecialState();
	virtual ~ioHelicopterSpecialState();
};

inline ioHelicopterSpecialState* ToHelicopterSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HELICOPTER )
		return NULL;

	return dynamic_cast< ioHelicopterSpecialState* >( pState );
}