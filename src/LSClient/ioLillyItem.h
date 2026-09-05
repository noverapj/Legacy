#pragma once
#include "ioweaponitem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioLillyItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_AIM,
		CS_AIM_TARGET,
		CS_FULL_CHARGING,
		CS_FULL_CHAEGING_FIRE,
		CS_NORMAL_FIRE,
		CS_NORMAL_AIMED_FIRE,
		CS_ZERO_AIM,
		CS_ZERO_AIM_FIRE
	};

	enum
	{
		SS_GATHERING_DIR	= 100
	};

	enum UpDownState
	{
		UDS_UP,
		UDS_DOWN,
	};

	enum JumpChargeState
	{
		JCS_NONE,
		JCS_AIM,
		JCS_NORMAL_ATTACK,
		JCS_AIM_TARGET,
		JCS_AIM_FIRE,
		JCS_AIM_NEW_TARGET,
		JCS_NEW_AIM,
		//JCS_NEW_AIM_ATTACK,
		JCS_ZERO_AIM,
		JCS_ZERO_AIM_FIRE,
		JCS_FLY_MOVE_SYNC
	};

protected:
	// normal attack
	ChargeState	m_ChargeState;
	DWORD	m_dwAttackStartTime;

	int	m_iCurCombo;
	ioHashStringVec	m_AttackReadyAniList;

	// extend attack
	AttributeList	m_vExtendAttributeList;
	int	m_iCurExtendAttack;
	DWORD	m_dwFireDelayTime;

	AttackAttribute	m_ExtendZeroAttack;

	// charge
	DWORD	m_dwAimStartTime;
	DWORD	m_dwZeroAimStartTime;
	DWORD	m_dwZeroAimedCheckTime;
	DWORD	m_dwFullChargeTime;

	DWORD	m_dwAimedFireWeaponIdx;

	bool	m_bAimDownEnable;

	// full charge
	ioHashString	m_szFullChargingGuideEffect;
	AttackAttribute	m_FullChargingAttack;

	float	m_fVertUpLimitAngle;
	float	m_fVertDownLimitAngle;

	DWORD	m_dwHorzRotateWeight;
	DWORD	m_dwVertRotateWeight;

	D3DXVECTOR3	m_vAttackDir;

	ioHashString	m_UpFullChargingAnimation;
	ioHashString	m_CenterFullChargingAnimation;
	ioHashString	m_DownFullChargingAnimation;

	ioHashString	m_UpFullChargingAttackAnimation;
	ioHashString	m_DownFullChargingAttackAnimation;

	float	m_fCurrMotionRate;

	DWORD	m_dwFullChargeWeaponIdx;

	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	// aim
	ioHashString	m_AimUp;
	ioHashString	m_AimCenter;
	ioHashString	m_AimDown;

	ioHashString	m_FireUp;
	ioHashString	m_FireCenter;
	ioHashString	m_FireDown;

	ioHashString	m_ZeroAimUp;
	ioHashString	m_ZeroAimCenter;
	ioHashString	m_ZeroAimDown;

	ioHashString	m_ZeroFireUp;
	ioHashString	m_ZeroFireCenter;
	ioHashString	m_ZeroFireDown;

	ioHashString	m_AimEffect;
	ioHashString	m_ZeroAimedEffect;

	ioUserKeyInput::DirKeyInput	m_AimStartDirKey;
	D3DXVECTOR3	m_vStartDir;

	ioHashString	m_AimMoveFront;
	ioHashString	m_AimMoveBack;
	ioHashString	m_AimMoveLeft;
	ioHashString	m_AimMoveRight;

	bool	m_bFirstAimMoveMotion;
	bool	m_bAimMotionSetted;
	bool	m_bChangeDir;

	float	m_fAimMoveAniRate;

protected:
	// angle & direction
	float	m_fAimAngle;
	float	m_fAimRange;
	float	m_fMinRange;

	float	m_fMarkerRange;

	D3DXVECTOR3	m_vAimedDir;

	bool	m_bZeroAimState;

	float	m_fCurAngle;
	DWORD	m_dwFireStateEndTime;
	DWORD	m_dwFireStartTime;
	DWORD	m_dwFireEndTime;
	DWORD	m_dwTrackingEndTime;
	DWORD	m_dwAimTime;

protected:
	// jump charge
	JumpChargeState	m_JumpChargeState;

	float	m_fFlyMoveSpeed;
	DWORD	m_dwFlyRotateSpeed;
	DWORD	m_dwFlyDurationTime;
	DWORD	m_dwFlyStartTime;
	float	m_fFlyEndJumpPower;
	float	m_fNormalAttackJumpPower;
	float	m_fAimAttackJumpPower;
	float	m_fZeroAimAttackJumpPower;

	float	m_fJumpAimAngle;
	float	m_fJumpAimRange;
	float	m_fJumpMinRange;
	DWORD	m_dwJumpAimTime;

	D3DXVECTOR3	m_vJumpChargeStartPos;
	float	m_JumpChargeMoveRange;
	ioHashString	m_szJumpChargeMoveRangeEffect;
	DWORD	m_dwJumpChargeMoveRangeEffectID;

	int	m_iMaxChargeCombo;

	ioHashString	m_szJumpAimUp;
	ioHashString	m_szJumpAimCenter;
	ioHashString	m_szJumpAimDown;
	ioHashString	m_szJumpZeroAimUp;
	ioHashString	m_szJumpZeroAimCenter;
	ioHashString	m_szJumpZeroAimDown;

	ioHashString	m_szJumpFireUp;
	ioHashString	m_szJumpFireCenter;
	ioHashString	m_szJumpFireDown;
	ioHashString	m_szJumpZeroFireUp;
	ioHashString	m_szJumpZeroFireCenter;
	ioHashString	m_szJumpZeroFireDown;

	AttributeList	m_vChargeComboAttList;
	AttackAttribute	m_JumpChargeZeroAttack;

	DWORD	m_dwJumpAimNewTargetTime;
	DWORD	m_dwNewAimDuration;
	int	m_iPreExtendAttack;

	int	m_iCurJumpCnt;
	bool	m_bJumpCharge;

	float	m_fJumpChargeVerticalGap;
	float	m_fJumpChargeVerticalMoveTime;
	float	m_fJumpChargeYPos;
	bool	m_bJumpChargeVerticalMoveUp;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();

	virtual WeaponSubType	GetSubType() const;

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual	void	OnEquiped( ioBaseChar *pOwner );
	virtual	void	OnReleased( ioBaseChar *pOwner );

public:
	virtual	void	SetNoneState( ioBaseChar *pOwner, int eNewState = 0 );
	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual	void	SetLandingState( ioBaseChar *pOwner );

	virtual	bool	CheckAimState(bool bAimMove = false);
	virtual	bool	SetAimState( ioBaseChar *pChar );

	virtual	float	GetAttackSpeedRate() const;

	virtual	void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

protected:
	void	OnCharging( ioBaseChar* pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToAimState( ioBaseChar* pOwner );
	void	ChangeToAimTargetState( ioBaseChar* pOwner );
	void	ChangeToNormalFire( ioBaseChar* pOwner );
	void	ChangeToNormalAimedFire( ioBaseChar* pOwner );
	void	ChangeToZeroAimState( ioBaseChar* pOwner );
	void	ChangeToZeroAimedFire( ioBaseChar* pOwner );
	void	ChangeToFullChargingState( ioBaseChar* pOwner );
	void	ChangeToFullChargingFire( ioBaseChar* pOwner );

	void	ProcessNormalAttack( ioBaseChar* pOwner );
	void	ProcessAimState( ioBaseChar* pOwner );
	void	ProcessAimTargetState( ioBaseChar* pOwner );
	void	ProcessNormalFire( ioBaseChar* pOwner );
	void	ProcessNormalAimedFire( ioBaseChar* pOwner );
	void	ProcessZeroAimState( ioBaseChar* pOwner );
	void	ProcessZeroAimedFire( ioBaseChar* pOwner );
	void	ProcessFullChargingState( ioBaseChar* pOwner );
	void	ProcessFullChargingFire( ioBaseChar* pOwner );

protected:
	void	Init();

	void	CheckAimMoveAniState( ioBaseChar *pOwner );

	bool	CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck, bool bJump = false, bool bDownAim = false );
	void	TrackingTarget( ioBaseChar *pOwner, bool bJump = false );
	float	GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void	UpdateAimDir( ioBaseChar *pOwner );

	void	AimedFireToUp( ioEntityGroup *pGrp, float fWeight,	float fTimeRate, DWORD dwPreDelay, bool bHold, bool bJump = false );
	void	AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold, bool bJump = false );

	bool	ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft );
	bool	ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState );
	void	ProcessRotateAnimation( ioBaseChar *pOwner );
	void	ChangeFullChargingAttackAnimation( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute );

	bool	IsLimitAngle( float fLimitAngle, float fCurrAngle );
	bool	IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey );
	bool	CheckDirectionKey( ioBaseChar* pOwner );

public:
	void	ChangeToJumpChargeAttack( ioBaseChar* pOwner );

	void	ProcessSpecialState( ioBaseChar* pOwner );

	void	ProcessFlyMove( ioBaseChar* pOwner );

	inline	bool	IsJumpCharged()		{	return	m_bJumpCharge;	}

protected:
	void	ChangeToJumpAimState( ioBaseChar* pOwner, bool bFirstAim = true );
	void	ChangeToJumpNormalAttack( ioBaseChar* pOwner );
	void	ChangeToJumpAimTarget( ioBaseChar* pOwner );
	void	ChangeToJumpAimAttack( ioBaseChar* pOwner );
	void	ChangeToJumpAimNewTarget( ioBaseChar* pOwner );
	void	ChangeToJumpNewAimState( ioBaseChar* pOwner );
	void	ChangeToJumpZeroAimState( ioBaseChar* pOwner );
	void	ChangeToJumpZeroAimAttack( ioBaseChar* pOwner );
	void	ChangeToJumpChargeEndState( ioBaseChar* pOwner, float fJumpPower );

	void	ProcessJumpAimState( ioBaseChar* pOwner );
	void	ProcessJumpNormalAttack( ioBaseChar* pOwner );
	void	ProcessJumpAimTarget( ioBaseChar* pOwner );
	void	ProcessJumpAimAttack( ioBaseChar* pOwner );
	void	ProcessJumpAimNewTarget( ioBaseChar* pOwner );
	void	ProcessJumpNewAimState( ioBaseChar* pOwner );
	void	ProcessJumpZeroAimState( ioBaseChar* pOwner );
	void	ProcessJumpZeroAimAttack( ioBaseChar* pOwner );

protected:
	void	ProcessJumpRotate( ioBaseChar* pOwner );
	void	ProcessJumpVerticalMove( ioBaseChar* pOwner );

	bool	IsCanJump();

public:
	ioLillyItem(void);
	ioLillyItem( const ioLillyItem& rhs );
	virtual ~ioLillyItem(void);
};

inline ioLillyItem* ToLillyItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LILLY_ITEM )
		return	NULL;

	return	dynamic_cast< ioLillyItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------

class ioLillySpecialState : public ioSpecialStateBase
{
public:
	virtual	void	Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual	void	Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual	const	char*	GetStateName(){ return "CS_LILLY_SPECIAL"; }
	virtual	const	CharState	GetState(){ return CS_LILLY_SPECIAL; }	
	virtual	bool	IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }	
	virtual	bool	IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual	void	CheckSpecialState( ioBaseChar* pOwner );
	virtual	bool	ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual	bool	IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual	void	GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	virtual	void	CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioLillySpecialState();
	virtual ~ioLillySpecialState();
};

inline	ioLillySpecialState*	ToLillySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_LILLY_SPECIAL )
		return	NULL;

	return	dynamic_cast< ioLillySpecialState* >( pState );
}
