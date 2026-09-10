#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"
#include "ioChargeCommandItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;


enum	CommandWeapon
{
	CW_NONE,
	CW_SCYTHE,
	CW_GUN
};

struct CommandWeaponInfo
{
	ioHashString m_Cmd;

	CommandWeapon m_CommandWeapon;

	CommandWeaponInfo()
	{
		Init();
	}

	void Init()
	{
		m_Cmd.Clear();
		m_CommandWeapon = CW_NONE;
	}
};

typedef std::vector< CommandWeaponInfo > CommandWeaponInfoList;


class ioMidnightItem : public ioChargeCommandItem//public ioWeaponItem
{
public:
	enum	MidnightState
	{
		MS_NONE,
		MS_DASH_CHARGE,
		MS_CANCEL_DASH,
		MS_CANCEL_DASH_ADD_ATTACK,
		MS_JUMP_DASH,
		MS_JUMP_COMMAND,
		MS_RELEASE_OBJECT,
		MS_CLAW_ATTACK
	};

	enum	ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_AUTO_ATTACK
	};

	// 새로운 싱크 타입이 필요하면 무조건 아래에 추가하도록..
	enum	SyncType
	{
		ST_ATTACK,
		ST_CMD_STATE,
		ST_JUMP_CMD_STATE,
		ST_OBJECT_EQUIP,
		ST_OBJECT_RELEASE,
		ST_AUTO_ATTACK,
		ST_ROTATE,
		ST_CANCEL_DASH,
		ST_CANCEL_DASH_ATTACK,
		ST_JUMP_DASH,
		ST_CHAIN_ATTACK_ADD,
		ST_CLAW_ATTACK,
		ST_END_STATE
	};

	enum	EquipObjectState
	{
		EOS_NONE,
		EOS_NORMAL,
		EOS_JUMP,
		EOS_DASH,
	};

	enum	ChainAttackState
	{
		CAS_NONE,
		CAS_START,
		CAS_ATTACK_HITED,
		CAS_ATTACK_ADD,
		CAS_END
	};

	enum	ObjectEquipState
	{
		OES_NONE,
		OES_GROUND,
		OES_JUMP
	};

protected:
	MidnightState		m_MidnightState;
	ChargeState			m_ChargeState;
	ChainAttackState	m_ChainAttackState;
	ObjectEquipState	m_ObjectEquipState;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	DWORD				m_dwMotionEndTime;
	DWORD				m_dwHitedTime;
	DWORD				m_dwWeaponIndex;

	float				m_fNeedChargeBullet;
	float				m_fExtraGaugeJumpInc;

	DWORD				m_dwScytheObjectItem;
	DWORD				m_dwGunObjectItem;

	// cancel dash
	DWORD				m_dwCancelDashMotionEndTime;
	DWORD				m_dwCancelDashAttackMotionEndTime;
	DWORD				m_dwCancelDashAttackTime;
	AttackAttribute		m_CancelDashAttribute;
	AttackAttribute		m_CancelDashAddAttackAttribute;
	float				m_fCancelDashEndJumpAmt;
	float				m_fCancelDashRandRate;

	// jump dash
	AttackAttribute		m_JumpDashAttribute;
	DWORD				m_dwJumpDashMotionEndTime;
	float				m_fJumpDashGravityAmt;
	bool				m_bJumpDashEnable;

	// charge
	AttackAttribute		m_ChargeAutoAttribute;
	ioHashString		m_szChargeAttackLoopingSound;
	UniqueObjID			m_ChargeAttackLoopingSoundID;
	ioHashString		m_ChargeRunningAni;
	float				m_fChargeRunningAniRate;
	DWORD				m_dwRunningEndTime;
	DWORD				m_dwChargeRunningDuration;
	float				m_fChargeRunSpeed;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;
	bool				m_bTargetRot;
	DWORD				m_dwChargeRotateTime;
	DWORD				m_dwChargeAttackEndTime;

	// gauge
	float				m_fMaxBullet;
	float				m_fCurBullet;
	float				m_fIncreseGaugeTic;

	//D꾹 관련
	ioHashStringVec		m_PreGatherAnimationList;
	ioHashString		m_szRleaseObjectEffect;

	ioHashString		m_Target;

	MidnightAttackInfo	m_CurAttackInfo;
	MidnightAttackInfo	m_ClawAttackInfo;

	ChargeCmdInfoList	m_ChargeJumpCmdInfoList;
	ChargeCmdInfo		m_CurChargeJumpCmdInfo;

	// object
	CommandWeaponInfoList	m_CommandWeapon;
	CommandWeaponInfoList	m_JumpCommandWeapon;

	bool				m_bObjectReleaseSend;

	bool				m_bObjectReleaseAutoJump;

public:
	void Init();

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();

	virtual	void	LoadCmdInfoList( ioINILoader &rkLoader );
	void	LoadClawAttackInfo( ioINILoader &rkLoader );

public:
	virtual void	OnReleased( ioBaseChar *pOwner );

	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );
	virtual void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual	void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool	IsChargeAttackState( ioBaseChar *pOwner );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool	SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void	ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void	SetUseExtraGauge( float fAmt );
	virtual void	UpdateExtraData( ioBaseChar *pOwner );		
	virtual void	SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	virtual bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
			void	SetAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir );

	virtual	void	EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	virtual	WeaponSubType	GetSubType() const;

protected:
	void	OnCharging( ioBaseChar *pOwner );

	void	ChangeToAttackFire( ioBaseChar *pOwner );

	void	ChangeAttackStart( ioBaseChar* pOwner );

	void	ChangeToChargeAttack( ioBaseChar* pOwner );
	void	OnChargeAttack( ioBaseChar* pOwner );

public:
	void	ChangeEquipObjectState( ioBaseChar* pOwner, DWORD dwObject, EquipObjectState eState = EOS_NORMAL );
	void	OnChangeEquipObjectState( ioBaseChar* pOwner, DWORD dwObject, EquipObjectState eState = EOS_NORMAL, ObjectEquipState eEquipState = OES_NONE );

public:	
	virtual int		GetMaxBullet();	
	virtual int		GetCurBullet();
	virtual void	SetCurBullet( int iCurBullet );
	virtual int		GetNeedBullet();
			void	DecreaseBullet();

	virtual float	GetMaxExtraGauge();
	virtual float	GetCurExtraGauge();
			float	GetNeedExtraGauge();

public:
	virtual void	ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	bool	IsEquipObject( ioBaseChar* pOwner );	
	void	ReleaseObject( ioBaseChar* pOwner, bool bNeedProcess = true );
	virtual void	ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject );

public:
	void	ChangeToDashChargeAttack( ioBaseChar* pOwner, const MidnightAttackInfo& pAttackInfo );
	void	ChangeToClawAttack( ioBaseChar* pOwner );

	bool	SetTarget( const ioHashString &szTargetName, ioBaseChar *pOwner );

	void	ProcessMidnightState( ioBaseChar *pOwner );

	bool	IsCollisionSkipState();
	void	ClearAttackState( ioBaseChar *pOwner );

	void	ChangeToJumpDashState( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eCurrDir );
	bool	IsJumpDashState();
	float	GetJumpDashGravityAmt();
	bool	IsJumpDashEnable();

protected:
	void	FindChainAttTarget( ioBaseChar* pOwner );
	bool	IsEnableTargetState( ioBaseChar* pTarget );
	bool	CompareTarget( ioBaseChar* pOwner );

	void	ChangeTrackingUD_Fire( ioBaseChar* pOwner );
	void	AimedFireToUp( ioEntityGroup* pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	void	AimedFireToDown( ioEntityGroup* pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	float	GetAimedUpDownWeight( const D3DXVECTOR3& vAimDir, float fMaxAngle );

	void	CheckKeyInput( ioBaseChar *pOwner );
	void	ioMidnightItem::ProcessRotate( ioBaseChar *pOwner );

protected:
	void	RemoveWeapon( ioBaseChar* pOwner );
	void	SetChangeEndState( ioBaseChar* pOwner );
	void	CheckAttackHited( ioBaseChar* pOwner );
	void	ChangeAttackAdd( ioBaseChar* pOwner );
	void	ChangeAttackHited( ioBaseChar* pOwner );

	void	ProcessWeaponLine( ioBaseChar* pOwner );

	bool	IsMidnightLockState( ioBaseChar* pOwner );

	void	ChangeReserveKeyInputState( ioBaseChar* pOwner, int iCurCombo = 0 );
	void	ProcessCancelDashState( ioBaseChar* pOwner );
	void	ProcessCancelDashAttackState( ioBaseChar* pOwner );
	void	ProcessJumpDash( ioBaseChar* pOwner );

public:
	bool	CheckJumpCmdInputState( ioBaseChar* pOwner );

	MidnightState	GetMidnightState()		{	return	m_MidnightState;	};

	void	ClearJumpCmdInfo();

	bool	IsEquipScytheObject( ioBaseChar *pOwner );
	bool	IsEquipGunObject( ioBaseChar *pOwner );
	inline	ObjectEquipState	GetObjectEquipState()		{	return	m_ObjectEquipState;		};
	void	ResetObjectEquipState( ioBaseChar* pOwner, int iState = CS_DELAY );

	void	OnReleaseObject( ioBaseChar* pOwner );

	void	SetObjectReleseAutoJump( bool bJump )			{	m_bObjectReleaseAutoJump	= bJump;	};

protected:
	bool	ChangeToJumpCmdSet( ioBaseChar* pOwner );
	bool	ChangeToNewJumpCmdSet( ioBaseChar* pOwner );
	bool	ChangeToComboJumpCmdSet( ioBaseChar* pOwner );
	void	SetJumpCmdAttack( ioBaseChar* pOwner, const AttackAttribute &rkAttr, float fExtraAniRate=1.0f, float fExtraForceRate=1.0f );
	bool	CheckJumpCmdInput( ioBaseChar* pOwner );

	virtual void	SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate=1.0f, float fExtraForceRate=1.0f );

	DWORD	GetCommandWeapon( bool bJump );

	void	ApplyCmdAttack( ioBaseChar* pOwner, ObjectEquipState objectEquipState );

public:
	ioMidnightItem();
	ioMidnightItem( const ioMidnightItem &rhs );
	virtual ~ioMidnightItem();
};

inline ioMidnightItem*	ToMidnightItem( ioItem *pItem )
{
	ioWeaponItem	*pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MIDNIGHT_ITEM )
		return NULL;

	return dynamic_cast< ioMidnightItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMidnightSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MIDNIGHT_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MIDNIGHT_SPECIAL; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioMidnightSpecialState();
	virtual ~ioMidnightSpecialState();
};

inline ioMidnightSpecialState* ToMidnightSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MIDNIGHT_SPECIAL )
		return NULL;

	return dynamic_cast< ioMidnightSpecialState* >( pState );
}

