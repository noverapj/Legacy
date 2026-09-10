#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioNataItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_STATE_INIT_COMBO,
		WS_SPECIAL_STATE_INIT_FLY,
		WS_SPECIAL_STATE_PROCESS_COMBO,
		WS_SPECIAL_STATE_PROCESS_FLY,
	};

	enum NataComboSpecialState
	{
		NCSS_None,
		NCSS_Combo_Attack,
		NCSS_Combo_Check,
		NCSS_End,
	};

	enum NataFlySpecialState
	{
		NFSS_None,
		NFSS_Fly_Pre,
		NFSS_Fly_Delay,
		NFSS_Fly_Move,
		NFSS_Find_Target,
		NFSS_Target_Attack,
		NFSS_Target_Move,
		NFSS_Target_Attach,
		NFSS_Check_Attack,
		NFSS_Charge_Move,
		NFSS_Combo_Attack,
		NFSS_Return_Move,
		NFSS_Normal_Attack,
	};

protected:
	struct DistInfo
	{
		ioHashString szCharName;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};
	typedef std::vector< DistInfo > SortList;

protected:			
	ioHashStringVec m_AttackReadyAniList;	
	AttributeList m_vNormalChargeComboAttList;		
	int m_nNormalChargeMaxCombo;
	float m_fNormalEndJumpPower;
	float m_fNormalUseGauge;

	AttributeList m_vDashChargeComboAttList;
	int m_nDashChargeMaxCombo;	
	float m_fDashEndJumpPower;
	float m_fDashUseGauge;

	AttributeList m_vJumpChargeComboAttList;
	int m_nJumpChargeMaxCombo;
	float m_fJumpEndJumpPower;
	float m_fJumpUseGauge;

	float m_fMaxGauge;
	float m_fNeedGauge;
	float m_fIncreaseGauge;

protected:
	WeaponItemState m_WeaponItemState;
	NataComboSpecialState m_NataComboSpecialState;

	float m_fCurGauge;

	int m_nNormalCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwComboEndMotionEndTime;	
	DWORD m_dwComboReserveTime;

	int m_nCurCombo;
	int m_nCurMaxCombo;
	AttributeList m_vCurChargeComboAttList;

	float m_fCurEndJumpPower;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;	

protected:
	ioHashString m_szFlyCameraBuff;
	float m_fFlyUseGauge;
	float m_fFlyDecGauge;

	AttackAttribute m_FlyPre;

	ioHashString m_szFlyDelayAni;
	float m_fFlyDelayAniRate;

	ioHashString m_szFlyMoveFrAni;
	float m_fFlyMoveFrAniRate;
	ioHashString m_szFlyMoveBkAni;
	float m_fFlyMoveBkAniRate;
	ioHashString m_szFlyMoveLtAni;
	float m_fFlyMoveLtAniRate;
	ioHashString m_szFlyMoveRtAni;
	float m_fFlyMoveRtAniRate;

	float m_fFlyMoveForceAmt;
	float m_fFlyMoveForceFric;

	bool m_bFlyEnableDownAim;	
	float m_fFlyAimAngle;
	float m_fFlyAimRange;
	float m_fFlyMinRange;

	float m_fFlyMarkerRange;
	ioHashString m_szFlyReadyMarker;
	ioHashString m_szFlyAimingMarker;
	ioHashString m_szFlyAimedMarker;

	AttackAttribute m_FlyTargetAttack;

	ioHashString m_szFlyTargetMoveAni;
	float m_fFlyTargetMoveAniRate;
	float m_fMaxTargetMoveDist;
	float m_fTargetMoveSpeed;
	float m_fTargetCheckDist;

	ioHashString m_szFlyTargetAttachLeftAni;
	ioHashString m_szFlyTargetAttachRightAni;
	float m_fFlyTargetAttachAniRate;

	DWORD m_dwTargetAttachRotateSpeed;
	D3DXVECTOR3 m_vTargetAttachOffset;
	float m_fTargetAttachEndDist;

	DWORD m_dwFlyChargeAttackTime;
	
	int m_nFlyMaxCombo;
	float m_fFlyChargeComboUseGauge;
	AttributeList m_vFlyChargeComboAttList;
	float m_fFlyChargeComboReserveEndDist;
	float m_fFlyNormalAttackUseGauge;
	AttackAttribute m_FlyNormalAttack;
	float m_fFlyNormalAttackReserveEndDist;
	
	float m_fFlyChangeTargetUseGauge;
	float m_fFlyChangeTargetColRange;
	float m_fFlyChangeTargetColAngle;
	float m_fFlyChangeTargetColUpHeight;
	float m_fFlyChangeTargetColUnderHeight;
	bool m_bFlyChangeEnableTargetDown;

	float m_fFlyHighDecreaseFlyTic;
	float m_fFlyHighDecreaseHeight;

	DWORD m_dwChangeAttactDirTime;

	ioHashString m_szFlyChargeMoveAni;
	float m_fFlyChargeMoveAniRate;
	float m_fMaxChargeMoveDist;
	float m_fChargeMoveSpeed;

	ioHashString m_szFlyReturnMoveAni;
	float m_fFlyReturnMoveAniRate;
	float m_fMaxReturnMoveDist;
	float m_fReturnMoveSpeed;
	float m_fReturnMoveCheckDist;
	float m_fReturnMoveEndDist;

	float m_fFlyEndJumpPower;

	ioHashString m_szTargetAttachEffect;
	float m_fTargetAttachEffectHeight;
	
protected:
	NataFlySpecialState m_NataFlySpecialState;
	DWORD m_dwFlyEndMotionEndTime;
	DWORD m_dwFlyForceTime;
	D3DXVECTOR3 m_vFlyForceDir;
	float m_fCurTargetMoveDist;
	float m_fCurTargetAttachAngle;
	DWORD m_dwFlyAttackStartTime;

	DWORD m_dwFlyComboEndMotionEndTime;	
	DWORD m_dwFlyComboReserveTime;
	int m_nFlyCurCombo;

	bool m_bFlyAttachLeftDir;
	bool m_bFlyAttachRightDir;

	DWORD m_dwCheckChangeAttactDirTime;

	float m_fCurChargeMoveDist;
	float m_fCurReturnMoveDist;
	bool m_bSetReturnMoveCheck;

	float m_fCurFlyEndDist;

	DWORD m_dwCurChargeComboForceTime;
	float m_fCurChargeComboForceAmt;
	float m_fCurChargeComboForceFric;

	bool m_bFlyStartJump;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void Init();
	void LoadAttackProperty( ioINILoader &rkLoader );
	void RecvComboSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void InitExtraGauge();
	virtual int GetNeedBullet();

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void DecreaseGauge( float fUseGauge );
	bool IsEnableComboGauge( bool bNormal, bool bDash, bool bJump );
	bool IsEnableFlyGauge();

public:
	void ClearState( ioBaseChar *pOwner );
	void SetNataComboSpecialState( ioBaseChar *pOwner, bool bNormal, bool bDash, bool bJump );
	void ProcessNataComboSpecialState( ioBaseChar *pOwner );
	
protected:
	void SetComboAttackState( ioBaseChar *pOwner );
	bool CheckComboInput( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );
	
protected:
	void LoadFlyProperty( ioINILoader &rkLoader );
	void RecvFlySpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	void SetNataFlySpecialState( ioBaseChar *pOwner, bool bStartJump );
	void ProcessNataFlyState( ioBaseChar *pOwner );
	bool IsFlyCollisionSkipState();
	bool IsFlySendControlInfoToUser();

protected:
	void SetFlyPreState( ioBaseChar *pOwner );
	void ProcessFlyPreState( ioBaseChar *pOwner );

protected:
	void SetFlyDelayState( ioBaseChar *pOwner );
	void ProcessFlyDelayState( ioBaseChar *pOwner );
	bool CheckMoveInput( ioBaseChar *pOwner );
	bool CheckFlyEndInput( ioBaseChar *pOwner );

protected:
	void SetFlyMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir );
	void SetFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ProcessFlyMoveState( ioBaseChar *pOwner );

protected:
	bool FindTarget( ioBaseChar *pOwner );
	bool CompareAimedTarget( ioBaseChar *pOwner );

protected:
	void SetFlyTargetAttackState( ioBaseChar *pOwner );
	void ProcessFlyTargetAttackState( ioBaseChar *pOwner );

protected:
	void SetFlyTargetMoveState( ioBaseChar *pOwner );
	void ProcessFlyTargetMoveState( ioBaseChar *pOwner );
	bool CheckTargetMove( ioBaseChar *pOwner, float &fCurMoveDist, float fMoveSpeed, D3DXVECTOR3 vMoveDir );

protected:
	void SetFlyTargetAttachState( ioBaseChar *pOwner, bool bAttachEffect );
	void ProcessFlyTargetAttachState( ioBaseChar *pOwner );
	bool CheckTargetAttachPos( ioBaseChar *pOwner );
	bool CheckAttachAttackInput( ioBaseChar *pOwner );
	bool CheckAttachDefenceInput( ioBaseChar *pOwner );
	bool CheckEnableAttachTarget( ioBaseChar *pOwner );
	bool CheckChangeAttachDir( ioBaseChar *pOwner );
	
protected:
	void SetFlyCheckAttackState( ioBaseChar *pOwner );
	void ProcessFlyCheckAttackState( ioBaseChar *pOwner );
	void CheckFlyAttackInput( ioBaseChar *pOwner );

protected:
	void SetFlyChargeMoveState( ioBaseChar *pOwner );
	void ProcessFlyChargeMoveState( ioBaseChar *pOwner );

protected:
	void SetFlyComboAttackState( ioBaseChar *pOwner );
	void ProcessFlyComboAttackState( ioBaseChar *pOwner );
	bool CheckFlyComboAttackInput( ioBaseChar *pOwner );

protected:
	void SetFlyReturnMoveState( ioBaseChar *pOwner, bool bCheckDist );
	void ProcessFlyReturnMoveState( ioBaseChar *pOwner );

protected:
	void SetFlyNormalAttackState( ioBaseChar *pOwner );
	void ProcessFlyNormalAttackState( ioBaseChar *pOwner );

protected:
	void SetFlyTargetChangeState( ioBaseChar *pOwner );
	bool FindChangeTarget( ioBaseChar *pOwner );
	bool SortChangeTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList );
	bool IsChangeEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget );

protected:
	void FlyReserveInputKey( ioBaseChar *pOwner, bool bAttack, bool bDefence, bool bJump );
	bool CheckFlyReserveState( ioBaseChar *pOwner );
	void SetFlyReserveAttack( ioBaseChar *pOwner );
	void SetFlyReserveDefence( ioBaseChar *pOwner );
	void SetFlyReserveJump( ioBaseChar *pOwner );

public:
	ioNataItem();
	ioNataItem( const ioNataItem &rhs );
	virtual ~ioNataItem();
};

inline ioNataItem* ToNataItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NATA_ITEM )
		return NULL;

	return dynamic_cast< ioNataItem* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioNataComboSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NATA_COMBO"; }
	virtual const CharState GetState(){ return CS_NATA_COMBO; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioNataComboSpecialState();
	virtual ~ioNataComboSpecialState();
};

inline ioNataComboSpecialState* ToNataComboSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NATA_COMBO )
		return NULL;

	return dynamic_cast< ioNataComboSpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioNataFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NATA_FLY"; }
	virtual const CharState GetState(){ return CS_NATA_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner );

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
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioNataFlySpecialState();
	virtual ~ioNataFlySpecialState();
};

inline ioNataFlySpecialState* ToNataFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NATA_FLY )
		return NULL;

	return dynamic_cast< ioNataFlySpecialState* >( pState );
}