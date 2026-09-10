#pragma once

#include "ioBuff.h"
#include "ioSpecialStateBase.h"
#include "ioVehicleDriverObjectItem.h"

class ioBaseChar;
class ioINILoader;

class ioPassengerBuff : public ioBuff
{
public:
	enum PassengerState
	{
		PS_NONE,
		PS_GET_ON_READY,
		PS_GET_ON,
	};

	enum PassengerSyncState
	{
		PSS_GET_ON,
		PSS_ATTACK,
	};

protected:
	PassengerState	m_PassengerState;
	DriverState		m_eDriverState;
		
	int				m_iOffsetZ;
	int				m_iOffsetY;

	ioHashString	m_szGetOnEffect;
	ioHashString	m_szWeaponEffect;
	UniqueObjID		m_WeaponEffectID;

	DWORD			m_dwGetOnReadyStartTime;

	ioHashString	m_MoveAnimation;
	float			m_fMoveAniRate;

	ioHashString	m_JumpReadyAnimation;
	float			m_fJumpReadyAniRate;

	ioHashString	m_JumppingAnimation;
	float			m_fJumppingAniRate;

	ioHashString	m_JumpLandingAnimation;
	float			m_fJumpLandingAniRate;

	ioHashString	m_DriftLeftAnimation;
	float			m_fDriftLeftAniRate;

	ioHashString	m_DrifRightAnimation;
	float			m_fDrifRightAniRate;

	WeaponInfo		m_LeftWeaponInfo;
	WeaponInfo		m_RightWeaponInfo;	

	ioHashString	m_szLBipedName;
	ioHashString	m_szRBipedName;

	DWORD			m_dwFireGapTime;
	DWORD			m_dwFireStartTime;

	float			m_fEndJumpPower;
	int				m_iPassengerMax;

	DWORD			m_dwJumpKeyInputCheckTime;
	DWORD			m_dwJumpKeyInputStart;

	ioHashStringVec	m_GetOnDisableBuffList;	
	ioHashStringVec	m_GetOnAfterList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	void Init();
	void LoadCallBuffList( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	virtual void ProcessDelay( float fTimePerSec );
	void ProcessActive( float fTimePerSec );
	void ProcesssState( float fTimePerSec );

	void ChangeToGetOnReady( ioBaseChar* pOwner );
	void ChangeToGetOn( ioBaseChar* pOwner );

	void CheckGetOnReady( ioBaseChar* pOwner );
	void CheckGetOn( ioBaseChar* pOwner, ioBaseChar* pCreator );

	void CheckCratorState( ioBaseChar* pOwner, ioBaseChar* pCreator );
	void ProcessAttach( ioBaseChar* pOwner, ioBaseChar* pCreator );

protected:
	void SetRunState( ioBaseChar* pOwner );
	void SetJumpReadyState( ioBaseChar* pOwner );
	void SetJummping( ioBaseChar* pOwner );
	void SetLanding( ioBaseChar* pOwner );
	void SetDrift( ioBaseChar* pOwner, bool bLeft );

	void SetFire( ioBaseChar* pOwner, const ioHashString& szBipedName, const WeaponInfo& Weapon, const D3DXVECTOR3& vAttackDir );

protected:
	void CheckAttackInput( ioBaseChar *pOwner );
	void AttackFire( IN ioBaseChar* pOwner, IN const WeaponInfo& rkAttribute, IN const D3DXVECTOR3& vFireDir, IN const D3DXVECTOR3& vFirePos );

protected:
	bool IsEnableGetOn( ioBaseChar* pOwner, ioPlayStage* pStage );

public:
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	PassengerState GetPassengerState(){ return m_PassengerState; }

public:
	ioPassengerBuff();
	ioPassengerBuff( const ioPassengerBuff &rhs );
	virtual ~ioPassengerBuff();
};

inline ioPassengerBuff* ToPassengerBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_PASSENGER_BUFF )
		return NULL;

	return dynamic_cast< ioPassengerBuff* >( pBuff );
}

//-----------------------------------------------------------------------------------------------------------

class ioPassengerState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_PASSENGER"; }
	virtual const CharState GetState(){ return CS_PASSENGER; }
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const;
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

	virtual void ProcessKeyInput( ioBaseChar* pOwner );	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioPassengerState();
	virtual ~ioPassengerState();
};

inline ioPassengerState* ToPassengerState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_PASSENGER )
		return NULL;

	return dynamic_cast< ioPassengerState* >( pState );
}


