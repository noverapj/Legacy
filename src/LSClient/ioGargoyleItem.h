#pragma once
#include "ioSpecialStateBase.h"

class ioGargoyleItem  : public ioWeaponItem
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
		SS_START,
		SS_MOVE,
		SS_NORMAL_ATTACK,
		SS_CHARGE,
		SS_CHARGE_MOVE,
		SS_CHARGE_ATTACK,
		SS_JUMP,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_SET_GARGOYLE_STATE,
		ST_GARGOYLE_CHARGE,
		ST_GARGOYLE_NOR_ATTACK,
		ST_GARGOYLE_MOVE,
		ST_GARGOYLE_CHARGE_MOVE,
		ST_GARGOYLE_CHANGE_DIR,
		ST_GARGOYLE_CHARGE_ATTACK,
		ST_GARGOYLE_JUMP,
		ST_GARGOYLE_CHARGE_JUMP,
		ST_GARGOYLE_TARGET,
	};
protected:
	ChargeState						m_ChargeState;
	SpecialState					m_SpecialState;
	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Special State
protected:
	ioHashString					m_DummyCharName;
	int								m_DummyCharIndex;

	float							m_fCurGargoyleStartAngle;
	DWORD							m_dwCurGargoyleStartDuration;
	float							m_fCurMaxGargoyleStartRange;

	float							m_fGargoyleStartAngle;
	DWORD							m_dwGargoyleStartDuration;
	float							m_fMaxGargoyleStartRange;

	float							m_fJumpGargoyleStartAngle;
	DWORD							m_dwJumpGargoyleStartDuration;
	float							m_fMaxJumpGargoyleStartRange;

	float							m_fCurGargoyleStartRange;
	float							m_fGargoyleMoveSpeed;
	float							m_fGargoyleMoveGravity;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;
	DWORD							m_dwGargoyleChargeStartTime;
	DWORD							m_dwMaxGargoyleChargeTime;
	DWORD							m_dwGargoyleChargeJumpEnableTime;
	DWORD							m_dwGargoyleChargeJumpTicTime;
	float							m_fGargoyleEndJumpPower;
	bool							m_fGargoyleEndMustJump;
	bool							m_bEnableDownAim;	
	float							m_fAimAngle;
	float							m_fAimRange;
	float							m_fMinRange;
	ioHashString					m_AimedTarget;
	float							m_fGargoyleJumpPower;

	//Gague
	float							m_fNeedGargoyleStateGauge;
	float							m_fNeedGargoyleChargeStateGauge;
	float							m_fGargoyleStateStartGauge;
	float							m_fGargoyleChargeAttackGauge;
	float							m_fGargoyleAttackGauge;
	float							m_fGargoyleGaugeCheckHeight;
	float							m_fGargoyleHighHeightDecreaseTic;
protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToGargoyleMoveState( ioBaseChar *pOwner, bool bNet = true );
	void ChangeToGargoyleChargeState( ioBaseChar *pOwner );
	void ChangeToGargoyleAttackState( ioBaseChar *pOwner );
	void ChangeToGargoyleChargeMoveState( ioBaseChar *pOwner );
	void ChangeToGargoyleChargeAttackState( ioBaseChar *pOwner );
	void ChangeToGargoyleJumpState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessGargoyleStartState( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	void ProcessGargoyleMoveState( ioBaseChar *pOwner );
	void ProcessGargoyleChargeState( ioBaseChar *pOwner );
	void ProcessGargoyleAttackState( ioBaseChar *pOwner );
	void ProcessGargoyleChargeMoveState( ioBaseChar *pOwner );
	void ProcessGargoyleChargeAttackState( ioBaseChar *pOwner );
	void ProcessGargoyleJumpState( ioBaseChar *pOwner );

	void CheckDirKey( ioBaseChar *pOwner );
	void CheckFindTarget( ioBaseChar *pOwner );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pOwner );

	void CreateDummy( ioBaseChar *pOwner );
	void DestroyDummy( ioBaseChar* pOwner );
public:

	void SetGargoyleSpecialState( ioBaseChar *pOwner, bool bJump = false );
	void ClearSpecialState( ioBaseChar *pOwner );

	void ProcessGargoyleSpecialState( ioBaseChar *pOwner );
	bool ProcessGargoyleStateMove( ioBaseChar *pOwner );

	void GetGargoyleGravity( float &fCurGravity );

	bool IsNoDropState();
	bool IsEnableGargoyleSpecialState();

	virtual int GetNeedBullet();
	void DecreaseGauge( float fNeedGauge );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	//Gauge
	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioGargoyleItem(void);
	ioGargoyleItem( const ioGargoyleItem &rhs );
	~ioGargoyleItem(void);
};

inline ioGargoyleItem* ToGargoyleItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GARGOYLE_ITEM )
		return NULL;

	return dynamic_cast< ioGargoyleItem* >( pItem );
}

class ioGargoyleSpecialState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_GARGOYLE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_GARGOYLE_SPECIAL; }
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight ){}

	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
public:
	ioGargoyleSpecialState();
	virtual ~ioGargoyleSpecialState();
};