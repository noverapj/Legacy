#pragma once

#include "ioDummyChar.h"
#include "ioSpecialStateBase.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioBunkerDummy : public ioDummyChar
{			
protected:
	enum DummyBunkerState
	{
		DBS_DELAY,
		DBS_START,
		DBS_CREATE_WEAPON,
	};

protected:
	DummyBunkerState m_DummyBunkerState;
	DWORD m_dwAttackTime;

	bool m_bHasOwner;
	ioHashString m_stAttachOwnerName;

	ioHashStringVec m_stOwnerAddBuffs;
	//ioHashStringVec m_stOwnerRemoveBuffs;

	//ioHashString m_stOwnerAddBuff;
	//ioHashString m_stOwnerRemoveBuff;

	float m_fEndJumpPower;
	ioHashString m_AimedTarget;

	ioHashString m_stAttackSound;

protected:
	ioHashString m_szCheckBuff;
	ioHashString m_szDelayAniName;
	ioHashString m_szFireAniName;

	DWORD m_dwCheckOwnerDelayTime;
	DWORD m_dwCheckAttackTime;
	DWORD m_dwNextAttackTime;

	int m_nWeaponType;

	bool m_bCreatorBunker;
	float m_fOwnerFindRange;

	float m_fAttackBulletHeightOffset;

	float m_fAttackFindAngle;
	float m_fAttackFindRange;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual bool IsActivateWeapon();
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	virtual bool IsNeedCheckMapCollision() const;
	virtual int DontMoveEntityLevel() const;

protected:
	void SetState( ioHashString szAniName, DummyBunkerState eState );
	void ProcessDelayState();
	void ProcessFireState();
	void ProcessEnd();

protected:
	void FindAttackSearchTarget( ioBaseChar *pOwner );

	bool CheckOwnerState( ioBaseChar* pAttachOwner );
	void CreateWeapon( ioBaseChar* pAttachOwner );

	void FindBunkerOwner();
	void SetOwner( ioBaseChar* pOwner );

public:
	ioBunkerDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioBunkerDummy();
};

inline ioBunkerDummy* ToDummyBunker( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_BUNKER )
		return NULL;

	return dynamic_cast< ioBunkerDummy* >( pDummy );
}


//-----------------------------------------------------------------------------------------------------------

class ioBunkerAttachSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_BUNKER_STATE"; }
	virtual const CharState GetState(){ return CS_BUNKER_STATE; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioBunkerAttachSpecialState();
	virtual ~ioBunkerAttachSpecialState();
};

inline ioBunkerAttachSpecialState* ToBunkerAttachSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BUNKER_STATE )
		return NULL;

	return dynamic_cast< ioBunkerAttachSpecialState* >( pState );
}