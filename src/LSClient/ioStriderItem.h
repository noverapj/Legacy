#pragma once
#include "ioSpecialStateBase.h"

class ioStriderItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_CHARGING_ATTACK,
		SS_CANCEL_DASH,
		SS_WAIT,
		SS_RUN,
		SS_JUMP,
		SS_LANDING,
		SS_END,
		SS_JUMP_DASH_ADD_ATTACK,
	};

	enum StriderSyncType
	{
		SST_NORMAL_ATTACK,
		
		SST_CHARGING_ATTACK,
		SST_CHARGING_ATTACK_CREAT_DUMMY,
		
		SST_CANCEL_DASH,

		SST_EXTEND_DASH_WAIT,
		SST_EXTEND_DASH_JUMP,
		SST_EXTEND_DASH_JUMP_END_ATTACK,
		SST_EXTEND_DASH_CHANGE_DIR_KEY,
		SST_EXTEND_DASH_CHANGE_JUMP_DIR_KEY,
		SST_EXTEND_DASH_LANDING,
		SST_EXTEND_DASH_END,
		SST_EXTEND_DASH_END_ATTACK,
		SST_EXTEND_DASH_SEND_TARGET,

		SST_JUMP_DASH_ADD_ATTACK,
	};

protected:
	//State
	ChargeState						m_ChargeState;
	SpecialState					m_SpecialState;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;;
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Cancel Dash
	AttackAttribute					m_CancelDashAttribute;
	float							m_fCancelDashEndJumpAmt;
	float							m_fCancelDashRandRate;

	//Charge Attack
	AttackAttribute					m_CurChargeAttack;
	AttackAttribute					m_ChargeAttack;
	AttackAttribute					m_JumpChargeAttack;
	float							m_fChargeAttackEndJumpAmt;
	float							m_fChargeAttackRandRate;
	

	ioHashString					m_szCurChargeAttackDummyName;
	ioHashString					m_szChargeAttackDummyName;
	ioHashString					m_szFlyChargeAttackDummyName;
	DWORD							m_dwChargeAttackDummyTime;
	DWORD							m_dwFlyChargeAttackDummyTime;
	DWORD							m_dwChargeAttackDummyCreateTime;

	//Jump Dash Add Attack
	AttackAttribute					m_JumpDashAddAttack;
	float							m_fJumpDashAddAttackAngle;
	float							m_fJumpDashAddAttackGravity;

	//Extend Dash
	ioHashString					m_ExtendDashWaitAni;
	float							m_fExtendDashWaitAniRate;
	ioHashString					m_ExtendDashRunningAni;
	float							m_fExtendDashRunningAniRate;
	float							m_fExtendDashRunSpeed;
	AttackAttribute					m_ExtendDashJumpAttack;
	ioHashString					m_ExtendDashLandingAni;
	float							m_fExtendDashLandingAniRate;
	float							m_fExtendDashJumpPower;
	float							m_fExtendDashJumpLookForce;
	DWORD							m_dwExtendDashJumpRotateTime;
	bool							m_bExtendDashJumpTargetRot;
	bool							m_bExtendDashJumpLeftRot;
	float							m_fExtendDashJumpEndJumpPower;
	ioHashString					m_ExtendDashEndAni;
	float							m_fExtendDashEndAniRate;
	
	//Target
	ioHashStringVec					m_vAddTargetList;
	ioHashStringVec					m_vRemoveTargetList;
	
	typedef std::map< ioHashString,DWORD > TargetEffectList;
	TargetEffectList				m_TargetList;

	ioHashString					m_szExtendDashBlueTargetMarker;
	ioHashString					m_szExtendDashRedTargetMarker;
	TargetWoundType					m_ExtendDashTargetWoundType;
	float							m_fExtendDashTargetAngle;
	float							m_fExtendDashTargetMaxRange;
	float							m_fExtendDashTargetMinRange;
	DWORD							m_dwExtendDashTargetMaxCnt;

	//Dummy
	ioHashString					m_szExtendDashDummyName;
	ioHashString					m_szExtendDashJumpDummyName;

	//Gauge
	float							m_fMaxBullet;
	float							m_fCurBullet;

	float							m_fIncreseGaugeTic;
	float							m_fDecreaseExtendDashTic;
	float							m_fChargeAttackNeedBullet;
	float							m_fCancelDashNeedBullet;
	float							m_fJumpDashAttackNeedGauge;
	float							m_fJumpDashAddAttackNeedBullet;
	float							m_fExtendDashNeedGauge;
	
	float							m_fExtendDashAttackGauge;
	float							m_fExtendDashJumpAttackGauge;

	//JumppingAni
	ioHashString					m_JumpDashAddJumppingAnimation;
	ioHashString					m_CancelDashJumppingAnimation;
	
protected:
	void ClearData();

	void LoadExtendDashInfo( ioINILoader &rkLoader );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeExtendDashWaitState( ioBaseChar *pOwner );
	void ChangeExtendDashRunningState( ioBaseChar *pOwner );
	void ChangeExtendDashJumpAttackState( ioBaseChar *pOwner );
	void ChangeExtendDashJumpEndAttackState( ioBaseChar *pOwner );
	void ChangeExtendDashLandingState( ioBaseChar *pOwner );
	void ChangeExtendDashEndState( ioBaseChar *pOwner, bool bSend = true );
	void ChangeExtendDashEndAttackState( ioBaseChar *pOwner );
	void ChangeJumpDashAddAttack( ioBaseChar *pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessCancelDashState( ioBaseChar *pOwner );
	void ProcessChargeAttackState( ioBaseChar *pOwner );
	void ProcessExtendDashWaitState( ioBaseChar *pOwner );
	void ProcessExtendDashRunState( ioBaseChar *pOwner );
	void ProcessExtendDashJumpState( ioBaseChar *pOwner );
	void ProcessExtendDashLandingState( ioBaseChar *pOwner );
	void ProcessExtendDashEndState( ioBaseChar *pOwner );
	void ProcessExtendDashRotate( ioBaseChar *pOwner );
	void ProcessJumpDashAddAttackState( ioBaseChar *pOwner );

	void CheckExtendDashRunDirKey( ioBaseChar *pOwner );
	void CheckExtendDashJumpDirKey( ioBaseChar *pOwner );
	bool CheckCharLanding( ioBaseChar *pOwner );
	void CheckAttTarget( ioBaseChar *pOwner );
	
	bool IsEnableTargetState( ioBaseChar *pTarget );

	//Atarget
	void AddTarget( ioPlayStage* pStage, ioBaseChar* pTarget );
	void RemoveTarget( ioPlayStage* pStage, ioHashString szTargetName );
	void RemoveAllTarget( ioBaseChar* pOwner );
	void UpdateAttTarget( ioBaseChar *pOwner );
	void SendTargetList( ioBaseChar *pOwner );

	//Maker
	DWORD CreateTargetMaker( ioPlayStage* pStage, ioBaseChar* pTarget );
	void DestroyTragetMaker( ioPlayStage* pStage, DWORD& dwEffectID );

	//Create Dummy
	void CreateChargeAttackDummy( ioBaseChar *pOwner, int iIndex = -1 );
	void CreateExtendDashDummy( ioBaseChar *pOwner, int iIndex );
	void CreateExtendDashJumpDummy( ioBaseChar *pOwner, int iIndex );

	//Gauge
	void DecreaseGauge( float fNeedGauge );

public:
	void ClearStriderState( ioBaseChar *pOwner );

	void SetStriderChargeAttackState( ioBaseChar *pOwner, bool bJumpState );
	void SetStriderExtendDashState( ioBaseChar *pOwner );
	void SetJumpDashAddAttackState( ioBaseChar *pOwner );

	void ProcessStriderSpecialState( ioBaseChar *pOwner );

	bool IsStriderSpecialNoDropState();
	bool IsStriderChargeAttackGauge();
	bool IsStriderJumpDashAddAttackGauge();
	bool IsStriderJumpDashAttackGauge();
	bool IsStriderSpecialEnableBlowWounded();
	bool IsStriderExtendDashGauge();

	void DecreaseJumpDashAttGauge();

	virtual int GetNeedBullet();
	void GetStriderStateGravity( float &fCurGravity );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );	

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void InitExtraGauge();
	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual const ioHashString& GetExtendJumppingAnimation() const;

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

public:
	ioStriderItem(void);
	ioStriderItem( const ioStriderItem &rhs );
	virtual ~ioStriderItem(void);
};

inline ioStriderItem* ToStriderItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_STRIDER_ITEM )
		return NULL;

	return dynamic_cast< ioStriderItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioStriderSpecialState : public ioSpecialStateBase
{
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_STRIDER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_STRIDER_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );
public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual bool IsEnableDashState( ioBaseChar* pOwner ) { return true; }
	virtual bool IsNoDropState( ioBaseChar* pOwner );
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
public:
	ioStriderSpecialState();
	virtual ~ioStriderSpecialState();
};