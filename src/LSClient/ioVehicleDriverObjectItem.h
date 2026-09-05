#pragma once

#include "ioObjectItem.h"
#include "ioSpecialStateBase.h"

class ioWeapon;
class ioBaseChar;
class ioPlayStage;
class ioVehicleDummy;

class ioVehicleDriverObjectItem : public ioObjectItem
{
public:
	enum VehicleDriverState
	{
		VDS_NONE,
		VDS_DELAY,
		VDS_RUN,
		VDS_JUMP_READY,
		VDS_JUMPPING,
		VDS_LADING,
		VDS_DRIFT,
		VDS_END,
	};

	enum SyncState
	{
		SS_RUN,
		SS_JUMP_READY,
		SS_DRIFT,
		SS_NONE_AIMMING_ATTACK,
		SS_AIMMING_ATTACK,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;
	typedef ioTargetMarker::MarkerState AimState;

protected:
	VehicleDriverState	m_eVehicleDriverState;
	AimState			m_AimState;

protected:
	ioHashString	m_szRidingDummyCharName;
	int				m_iRidingDummyCharIndex;

protected:
	ioHashString	m_szRideStartAnimation;
	float			m_fRideStartTimeRate;

	DWORD			m_dwRideStartEndTime;

protected:
	ioHashString	m_szRideRunLoopAnimation;
	float			m_fRideRunLoopTimeRate;
	float			m_fRideRunSpeed;

	DWORD			m_dwRideRunLoopStartTime;
	DWORD			m_dwRideRunLoopDuration;
	DWORD			m_dwRideRotateTime;

	DirKey			m_CurDirKey;
	float			m_fTargetYaw;
	bool			m_bLeftRot;

protected:
	ioHashString	m_szRideJumpReadyAnimation;
	float			m_fJumpReadyTimeRate;
	float			m_fRideJumpPower;
	float			m_fRideJumpReadySpeed;

	DWORD			m_dwRideJumpRotateTime;
	DWORD			m_dwJumpReadyEndTime;

protected:
	ioHashString	m_szRideJumppingAnimation;
	float			m_fRideJumppingTimeRate;	
	float			m_fRideJumppingSpeed;
	
	DWORD			m_dwRideJumppingStartTime;

protected:
	ioHashString	m_szRideLandingAnimation;
	float			m_fRideLandingTimeRate;
	float			m_fRideChangeToLandHeight;
	float			m_fRideLandingSpeed;

	DWORD			m_dwRideLandingEndTime;
	WeaponInfo		m_LandingWeaponInfo;
	DWORD			m_dwLadingWeaponBaseIndex;

protected:
	AttackAttribute	m_DriftLeftAttribute;
	AttackAttribute	m_DriftRightAttribute;
	float			m_fDriftAngle;
	float			m_fDriftSpeed;
	float			m_fDriftForce;
	float			m_fDriftRotateTime;

	DWORD			m_dwRideDriftEndTime;

protected:
	ioHashString	m_szRideEndAnimation;
	float			m_fRideEndTimeRate;

	DWORD			m_dwRideEndTime;
	DWORD			m_dwDriftDuration;

	DWORD			m_dwDriftAttackStartTime;
	DWORD			m_dwDriftAttackGapTime;

protected:
	int				m_iTargetMax;
	float			m_fTargetAngle;
	float			m_fTargetMaxRange;
	float			m_fTargetMinRange;
	float			m_fTargetMaxAngle;
	
	DWORD			m_dwAimmingStartTime;
	DWORD			m_dwZeroAimedStartTime;
	DWORD			m_dwAimmingCheckTime;
	DWORD			m_dwZeroAimedCheckTime;
	DWORD			m_dwAimmingFireGapTime;
	DWORD			m_dwAimmingFireStartTime;

	BaseCharList	m_TargetedList;
	WeaponInfo		m_AimedTargetFireWeaponInfo;
	WeaponInfo		m_ZeroAimedTargetFireWeaponInfo;

protected:
	float			m_fNoneTargetMaxRange;
	float			m_fNoneTargetYOffset;

	WeaponInfo		m_NoneTargetFireWeaponInfo;

protected:
	int				m_iPassengerMax;
	ioHashString	m_szPassengerBuff;
	ioHashString	m_szPassengerKickBuff;

protected:
	WeaponInfoList m_AttachWeaponInfoList;
	DWORDVec m_vAttachWeaponIndexList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual ioItem* Clone();

public:
	void Init();

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDefense() const;
	virtual bool IsEnableDash() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual ObjectSubType GetObjectSubType() const;

protected:
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, OUT DWORD& dwStartTime, DWORD dwPreDelay );
	int SetLoopAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate );
	void AttackFire( IN ioBaseChar* pOwner, IN const WeaponInfo& rkAttribute, IN const D3DXVECTOR3& vFireDir, IN const D3DXVECTOR3& vFirePos );

	void SetAttachWeapon( ioBaseChar *pOwner );

protected:
	void ChangeToDelayState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckDelayState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ChangeToRunState( ioPlayStage *pStage, ioBaseChar* pOwner, bool bStateValueReset = true );
	void CheckRunState( ioPlayStage *pStage, ioBaseChar* pOwner );
	
	void ChangeToJumpReadyState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckJumpReadyState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ChangeToJumppingState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckJumppingState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ChangeToRandingState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckLandingChangeState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ChangeToDriftState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckDriftState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void ChangeToEndState( ioPlayStage *pStage, ioBaseChar* pOwner );
	void CheckEndState( ioPlayStage *pStage, ioBaseChar* pOwner );

	void CheckAimming( ioBaseChar* pOwner );
	void CheckMultipleTarget( ioBaseChar* pOwner );
	void CheckNewMultipleTarget( ioBaseChar* pOwner, BaseCharList& TargetList );
	void UpdateTargetState( ioBaseChar* pOwner, int iMarkerIndex );

	void ClearNewMultipleTarget( ioBaseChar* pOwner, BaseCharList& TargetList );

	void AimmingAttackFire( ioBaseChar* pOwner );
	void NoneTargetAimmingAttackFire( ioBaseChar* pOwner );

protected:
	void CreateDummy( ioBaseChar* pOwner );
	void DestroyDummy( ioBaseChar* pOwner );	
	ioVehicleDummy* GetVehicle( ioBaseChar* pOwner );	

protected:
	void SetTargeted( ioBaseChar* pOwner, int iMarkerIndex );

public:
	void SetRidingDummyCharIndex( ioBaseChar *pOwner, int iCurIndex );

protected:
	void CheckRotateKeyInput( ioPlayStage *pStage, ioBaseChar *pOwner );
	void CheckJumpKeyInput( ioPlayStage *pStage, ioBaseChar *pOwner );
	void CheckJumpKeyInputByDropZone( ioPlayStage *pStage, ioBaseChar *pOwner );
	bool CheckDefenceInput( ioPlayStage *pStage, ioBaseChar *pOwner );
	void CheckAttackInput( ioPlayStage *pStage, ioBaseChar *pOwner );
	bool IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey );

	void ProcessRotate( IN ioBaseChar *pOwner, IN DWORD dwRotateTime, IN float fTargetYaw );
	void ProcessDriftRotate( IN ioBaseChar *pOwner, IN DWORD dwRotateTime, IN float fTargetYaw );

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	
public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	void CheckPassenger( ioBaseChar* pOwner, ioPlayStage* pStage );

public:
	ioVehicleDriverObjectItem();
	ioVehicleDriverObjectItem( const ioVehicleDriverObjectItem &rhs );
	virtual ~ioVehicleDriverObjectItem();
};

inline ioVehicleDriverObjectItem* ToVehicleDriverObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_VEHICLE_DRIVER )
		return NULL;


	return dynamic_cast< ioVehicleDriverObjectItem* >( pObject );
}

typedef ioVehicleDriverObjectItem::VehicleDriverState DriverState;

//-----------------------------------------------------------------------------------------------------------

class ioVehicleDirverState : public ioSpecialStateBase
{
protected:
	DriverState m_eDriverState;
	bool		m_bDriftLeft;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_VEHICLE_DRIVER"; }
	virtual const CharState GetState(){ return CS_VEHICLE_DRIVER; }
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
										DWORD dwStartTime,
										DWORD dwEndTime,
										DWORD dwSkipType,
										bool bTeamOnly,
										bool bDefense,
										TeamType eOwnerTeam,
										TeamType eOtherTeam ) const;

	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const;

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	void CheckPassenger( ioBaseChar* pOwner, ioPlayStage* pStage );

public:
	inline void SetDriverState( DriverState eState ){ m_eDriverState = eState; }
	inline DriverState GetDriverState(){ return m_eDriverState; }
	inline void SetDriftLeft( bool bLeft ){ m_bDriftLeft = bLeft; }
	inline bool GetDriftLeft(){ return m_bDriftLeft; }

public:
	ioVehicleDirverState();
	virtual ~ioVehicleDirverState();
};

inline ioVehicleDirverState* ToVehicleDirverState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_VEHICLE_DRIVER )
		return NULL;

	return dynamic_cast< ioVehicleDirverState* >( pState );
}