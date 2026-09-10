#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioObjectBankruptcyItem :	public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,			
		CS_CHARGING,
		CS_GATHERING,
		CS_RELEASE,
		CS_NORMAL_ATTACK,
		CS_MOVING_SHIELD,
		CS_JUMP_GRAP,				// 공중 잡기
		CS_JUMP_CHARGE,				
		CS_JUMP_GRAP_LAND_ATTACK,
	};
protected:

	enum SpecialState
	{
		SS_NONE,
		SS_START,
		SS_DELAY,
		SS_DELAY_SYNC,
		SS_END,
		SS_END_SYNC,
		SS_SPECIAL_DASH,
		SS_SPECIAL_DASH_ADD_ATTACK,
		SS_JUMP_CHARGE,
		SS_JUMP_CHARGE_ATTACK,
		SS_JUMP_CHARGE_END_ATTACK,
		SS_JUMP_CHARGE_CANCEL_SYNC
	};

	// 공중 잡기
	enum GrapItemCancelSync
	{
		GICS_GRAP_JUMP,
		GICS_DIR,
	};

protected:
	// normal attack
	ChargeState			m_ChargeState;
	DWORD				m_dwAttackStartTime;
	DWORD				m_dwInputSpecialAttackTime;
	int					m_iCurCombo;
	ioHashStringVec		m_AttackReadyAniList;
	DWORD				m_dwMotionEndTime;

	// cancel jump
	D3DXVECTOR3			m_vSpecialJumpMoveDir;
	float				m_fCurSpecialJumpMoveSpeed;
	float				m_fCurSpecialJumpEndJumpPower;

	// cancel dash
	AttackAttribute		m_NormalCancelDashAttribute;
	AttackAttribute		m_CancelDashAttribute;
	AttackAttribute		m_CancelDashAddAttackAttribute;

	DWORD				m_dwCancelDashAttackMotionEndTime;
	DWORD				m_dwCancelDashAttackTime;		
	int					m_iChargeComboDashState;

	// jump charge
	AttackAttribute		m_JumpChargeAttribute;
	AttackAttribute		m_JumpChargeEndAttribute;

	D3DXVECTOR3			m_vMagicCirclePos;

	ioHashString		m_szJumpChargeLoopingAni;
	float				m_fJumpChargeLoopingAniRate;

	float				m_fCurCharMoveSpeed;
	float				m_fJumpChargeMinHeight;

	//Gauge
	float				m_fMaxBullet;
	float				m_fCurBullet;
	float				m_fNeedBullet;

	float				m_fRecoverGaugeDelay;
	float				m_fRecoverGaugeRun;
	float				m_fRecoverGaugeAttack;
	float				m_fRecoverGaugeWound;
	float				m_fReduceGaugeRate;

	float				m_fJumpChargingNeedBullet;

	// special state
	SpecialState		m_SpecialState;
	CyvogueFlyInfo		m_SpecialStateAniInfo;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

	// magic circle
	ioHashString		m_ReadyCircle;
	ioHashString		m_EnableCircle;

	// gravity
	float				m_fGravity;
	float				m_fAttackGravity;

	float				m_fRange;
	float				m_fMoveSpeed;
	float				m_fHeightGap;

	// 공중 잡기 
	ItemJumpAttackInfo	m_CurJumpAttackInfo;
	ioHashString		m_GrapplingTargetName;
	float				m_fGrpplingCharToCharGap;
	float				m_fGrpplingCharMidPosRate;

	// 변신 파산신공
	ioHashString		m_GatherAnimation;
	DWORD				m_dwGatheringStartTime;
	ioHashString		m_MaxChangeDamageEffect;
	UniqueObjID			m_dwChangeDamageEffect;
	float				m_fCurChangeDamage;
	float				m_fGatherAniRate;
	bool				m_bEnableMaxCharge;

	// 변신 파산신공 후 tic 단위로 게이지가 감소
	float				m_fIncreaseChangeDamage;	
	float				m_fDownDecreaseChangeDamage;
	float				m_fDecreaseChangeDamage;

	// 2020-06-08
	bool				m_bEnableDefenseRelease;
	bool				m_bCheckReleaseWereWolf;
	bool				m_bEnableSpeedChange;

public:
	virtual void	LoadProperty( ioINILoader &rkLoader );
	virtual ioItem*	Clone();
	virtual ObjectSubType GetObjectSubType() const;

	// 변신 D~ 해제 할때 값을 false 로 해줘야한다. 
	virtual bool	IsOneTimeWeaponItem() const;
	virtual bool	IsEnableDash() const;
	virtual	void	UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void	CheckNormalAttack( ioBaseChar *pOwner );

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool	ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual	bool	IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void	SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	virtual	void	CheckExtendDashState( ioBaseChar *pOwner );

	virtual void	CheckWoundedState( ioBaseChar *pOwner );

	virtual bool	IsEnableDefense() const;	// 2020-06-22

protected:
	void	OnCharging( ioBaseChar* pOwner );
	void	OnGathering( ioBaseChar *pOwner );

	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ProcessNormalAttack( ioBaseChar* pOwner );

	// 변신 파산신공 부분
	void	ChangeToGathering( ioBaseChar *pOwner );
	bool	CheckOwnerDownState( ioBaseChar *pOwner );

protected:	// 변신 파산신공
	virtual bool CheckChangeWereWolfState( ioBaseChar *pChar );
	virtual void SetChangeWereWolfState( ioBaseChar *pChar );

public:	// 변신 파산신공
	void ReleaseEnableChangeDamageEffect( ioBaseChar *pChar );
	void SetEnableChangeDamageEffect( ioBaseChar *pChar );
	float GetCurChangeDamage();
	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );
	virtual void DecreaseChangeDamage( ioBaseChar *pOwner );

	// 변신 해제
	virtual void ReleseWereWolfState( ioBaseChar *pChar );

	void ChangeToRelease( ioBaseChar *pOwner );

protected:
	void	ChangeToAttackAfterSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir );
	void	ProcessSpecialDash( ioBaseChar* pOwner );
	void	ProcessSpecialDashAddAttack( ioBaseChar* pOwner );
	void	ProcessJumpCharge( ioBaseChar* pOwner );
	void	ProcessJumpChargeAttack( ioBaseChar* pOwner );
	void	ProcessJumpChargeEndAttack( ioBaseChar* pOwner );

public:
	void	ProcessState( ioBaseChar* pOwner );

	bool	IsEnableMoveState();
	void	ClearSpecialState( ioBaseChar* pOwner );

	void	ChangeToJumpChargeState( ioBaseChar* pOwner );
	void	ChangeToJumpChargeAttack( ioBaseChar* pOwner );
	void	ChangeToJumpChargeEndAttack( ioBaseChar* pOwner );

protected:
	void	ChangeToSpecialState( ioBaseChar* pOwner );
	void	ChangeToStartSpecialState( ioBaseChar* pOwner );
	void	ChangeToDelaySpecialState( ioBaseChar* pOwner );
	void	ChangeToEndSpecialState( ioBaseChar* pOwner );

	void	ProcessDelaySpecialState( ioBaseChar* pOwner );

	void	CheckKeyInput( ioBaseChar* pOwner );

	void	ChangeMoveAni( ioBaseChar *pOwner );
	void	ChangeMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate );

	void	ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

public:
	virtual	int	GetNeedBullet();
	virtual	int	GetMaxBullet();
	virtual	int	GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

	inline bool IsEnableDefenseRelease() { return m_bEnableDefenseRelease; }
	inline bool IsReleaseWereWolf() const { return m_bCheckReleaseWereWolf; }

	bool	IsEnoughGaugeJumpCharge();

	bool	IsJumpChargeState();
	
	float	GetGravity( ioBaseChar* pOwner );

protected:
	void	SetJumpChargeAttackForce( ioBaseChar* pOwner, D3DXVECTOR3& vMagicCirclePos );
	float	GetFlightDuration( const D3DXVECTOR3 &vCharPos, const D3DXVECTOR3 &vMagicCirclePos, float fCurGravity );


	//공중잡기//////////////////////////////////////////////////////////////////////////
public:
		void SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info );
		bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
		float GetGrapGravity();
		bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
		D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

		void ProcessGrapplerJumpGrapMove( ioBaseChar *pOwner );
		bool ProcessGrapplerFlyLandAttack( ioBaseChar *pOwner );
		int GetState();

protected:
	bool CheckLanding( ioBaseChar *pChar );
	void StartLandAttack( ioBaseChar *pOwner );
	//////////////////////////////////////////////////////////////////////////

public:
	ioObjectBankruptcyItem(void);
	ioObjectBankruptcyItem( const ioObjectBankruptcyItem &rhs );
	virtual ~ioObjectBankruptcyItem(void);
};

inline ioObjectBankruptcyItem* ToObjectBankruptcyItem( ioItem *pItem )
{
	ioObjectItem *pObject	= ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_GWONWANG )
	{
		return	NULL;
	}

	return	dynamic_cast< ioObjectBankruptcyItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioObjectBankruptcySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_OBJECT_BANKRUPTCY_SPECIAL"; }
	virtual const CharState GetState(){ return CS_OBJECT_BANKRUPTCY_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioObjectBankruptcySpecialState();
	virtual ~ioObjectBankruptcySpecialState();
};

inline ioObjectBankruptcySpecialState* ToObjectBankruptcySpecialState( ioObjectBankruptcySpecialState *pState )
{
	if( !pState || pState->GetState() != CS_OBJECT_BANKRUPTCY_SPECIAL )
	{
		return NULL;
	}

	return dynamic_cast< ioObjectBankruptcySpecialState* >( pState );
}