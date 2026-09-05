#pragma once
#include "ioSpecialStateBase.h"

class ioWindGodItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_DELAY,
		CS_FIRE_EFFECT,
		CS_FIRE_ATTACK,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_WAIT,
		SS_MOVE,
		SS_ADD,
		SS_END,
		SS_JUMP_END,
	};


private:
	ChargeState		m_ChargeState;
	SpecialState	m_SpecialState;

	//Ani
	DWORD			m_dwMotionStartTime;
	DWORD			m_dwMotionEndTime;
	//Normal Attack
	ioHashStringVec	m_AttackReadyAniList;
	int				m_iCurCombo;


	/////////////////////////////
	//D~
	ioHashString	m_szChargeLoopDelayAni;
	ioHashString	m_szChargeLoopDelayMoveAni;
	float			m_fChargeLoopDelayAniRate;
	bool			m_bChargeLoopDelayMoveAni;

	ioHashString	m_szChargeFireEffectAni;
	float			m_fChargeFireEffectAniRate;
	ioHashString	m_szChargeFireEffectName;
	DWORD			m_dwChargeFireEffectID;
	float			m_fChargeFireEffectHeightOffset;
	float			m_fMaxChargeFireEffectRange;
	float			m_fCurChargeFireEffectRange;
	float			m_fChargeFireEffectSpeed;
	
	ioHashString	m_szChargeAttackFireEffectName;
	float			m_fChargeAttackFireEffectHeightOffset;
	DWORD			m_dwChargeAttackFireEffectID;
	ioHashString	m_szChargeAttackFireAni;
	float			m_szChargeAttackFireAniRate;
	DWORD			m_dwChargeAttackFireTime;
	WeaponInfo		m_ChargeAttackWeapon;

	//Dash D~
	ioHashString	m_szWindGodDashWaitAni;
	float			m_fWindGodDashWaitAniRate;
	ForceInfoList	m_vWindGodDashWaitForceInfoList;
	ioHashString	m_szWindGodDashMoveAni;
	float			m_fWindGodDashMoveAniRate;
	float			m_fWindGodDashMoveSpeedRate;
	float			m_fWindGodDashMoveEndJumpPower;
	int				m_iWindGodDashMoveAttachType;
	ioHashString	m_szWindGodDashMoveEndAni;
	float			m_fWindGodDashMoveEndAniRate;
	AttackAttribute	m_WindGodDashMoveAddAttack;
	float			m_fWindGodDashMoveAddAttackAngle;

	//Gauge
	float			m_fMaxBullet;
	float			m_fCurBullet;
	float			m_fIncreseGaugeTic;
	float			m_fChargeAttackNeedBullet;
	float			m_fJumpChargeAttackNeedBullet;
	float			m_fDashChargeAttackNeedBullet;
	float			m_fDecreaseDashChargeAttackTic;
	float			m_fDecreaseDashChargeAttackStartGauge;
private:
	void ClearData();
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessChargeDelayState( ioBaseChar *pOwner );
	void ProcessFireEffectState( ioBaseChar *pOwner );
	void ProcessFireAttackState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToChargeDelayState( ioBaseChar *pOwner );
	void ChangeToFireEffectState( ioBaseChar *pOwner );
	void ChangeToFireAttackState( ioBaseChar *pOwner );

	void UpdateChargeDelayMoveAni( ioBaseChar *pOwner );
	void UpdateFireEffect( ioBaseChar *pOwner );
	
	void FireWeapon( ioBaseChar *pOwner );
	void CreateFireEffect( ioPlayStage *pStage, D3DXVECTOR3 vPos );
	void RemoveEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
protected:
	void ChangeToWindGodDashMoveAttack( ioBaseChar *pOwner );
	void ChangeToWindGodDashMoveEndState( ioBaseChar *pOwner );
	void ChangeToWindGodDashAddAttack( ioBaseChar *pOwner );
	void ChangeToWindGodJumpEndState( ioBaseChar *pOwner );
	void ProcessWindGodDashWaitState( ioBaseChar *pOwner );
	void ProcessWindGodDashMoveAttackState( ioBaseChar *pOwner );
	void ProcessWindGodDashMoveEndState( ioBaseChar *pOwner );
	void ProcessWindGodDashAddAttackState( ioBaseChar *pOwner );
public:
	bool IsEnableExtendDash();
	void SetWindGodDashMoveState( ioBaseChar *pOwner );
	
	void ProcessWindGodDashAttackMove( ioBaseChar *pOwner );
	bool IsCanWindGodDashKeyMove( const ioBaseChar *pOwner ) const;
	void ProcessWindGodDashKeyInput( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool IsEnableExtendJump();

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual float GetAttackSpeedRate() const;

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
public:
	//Gauge
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );
	void DecreaseGauge( float fNeedGauge );
	void DecreseChargeJumpAttackGauge();
	virtual int GetNeedBullet();
public:
	ioWindGodItem(void);
	ioWindGodItem( const ioWindGodItem &rhs );
	virtual ~ioWindGodItem(void);
};

inline ioWindGodItem* ToWindGodItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_WINDGOD_ITEM )
		return NULL;

	return dynamic_cast< ioWindGodItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------
class ioWindGodSpecialState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_WINDGOD_SPECIAL"; }
	virtual const CharState GetState(){ return CS_WINDGOD_SPECIAL; }
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
public:
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true;	}

	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight ) { return; }

	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity ) { fCurGravity = 0.0f; }

	virtual bool IsCanKeyMove( ioBaseChar* pOwner );
	virtual bool IsCanKeySpeedMove( ioBaseChar* pOwner );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsEnableMoveState( const ioBaseChar* pOwner ) const;
public:
	ioWindGodSpecialState();
	virtual ~ioWindGodSpecialState();
};