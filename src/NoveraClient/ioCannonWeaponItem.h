#pragma once

#include "ioSpecialStateBase.h"

class ioCannonWeaponItem : public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum SpecailState
	{
		SS_NONE,
		SS_START,
		SS_CANNON_DELAY,
		SS_CANNON_CHARGE,
		SS_CANNON_ATTACK,
		SS_CANNON_ATTACK_FAIL,
		SS_END_MOTION,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_CANNON_START,
		ST_CANNON_DELAY,
		ST_CANNON_END,
		ST_CANNON_ESCAPE,
		ST_CANNON_CHARGE,
		ST_CANNON_ATTACK,
		ST_CANNON_FIRE,
		ST_CANNON_ATTACK_FAIL,
		ST_CANNON_FULL_CHARGE_EFFECT,
		ST_CANNON_DIR_CANGE,
	};
protected:
	ChargeState					m_ChargeState;
	SpecailState				m_SpecailState;
	//Ani
	DWORD						m_dwMotionStartTime;
	DWORD						m_dwMotionEndTime;
	DWORD						m_dwFireStartTime;

	//Normal Attack
	ioHashStringVec				m_AttackReadyAniList;
	int							m_iCurCombo;

	//-----Charge------
	ioHashString				m_szDisableChargeBuff;
	ioHashString				m_szCannonEndAni;
	float						m_fCannonEndAniRate;
	bool						m_bFullGauge;
	WeaponInfo					m_CannonAttack;
	WeaponInfo					m_CannonFullAttack;
	float						m_fCannonEscapeEndJumpPower;
	bool						m_bCreateFullChargeEffect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioHashString				m_szCannonAttackAni;
	float						m_fCannonAttackAniRate;
	//Dummy
	ioHashString				m_szControlDummyName;
	ioHashString				m_szStayDummyName;
	D3DXVECTOR3					m_vControlDummyOffset;
	D3DXVECTOR3					m_vStayDummyOffset;
	DWORD						m_dwControlDummyIndex;
	DWORD						m_dwStayDummyIndex;
	bool						m_bCreateDummy;
	//Magic Circle
	D3DXVECTOR3					m_vCirclePos;
	float						m_fCircleMinRange;
	float						m_fCircleMaxRange;
	float						m_fCircleMoveSpeed;
	float						m_fCircleHeightGap;
	ioHashString				m_ReadyCircle;
	DWORD						m_dwCircleEffectID;
	ioHashString				m_RedEffect;
	ioHashString				m_BlueEffect;
	UniqueObjID					m_dwCurMapEffect;
	//Weapon
	CEncrypt<float>				m_fWeaponGravityRate;
	CEncrypt<float>				m_fWeaponFloatPower;
	CEncrypt<float>				m_fWeaponFloatMinPower;
	CEncrypt<float>				m_fWeaponMinSpeed;
	float						m_fUpLimitHeight;
	float						m_fUpLimitRange;
	float						m_fRevisionRate;
	D3DXVECTOR3					m_vWeaponDir;
	float						m_fCurWeaponMoveSpeed;
	float						m_fCurWeaponFloatPower;
	//Buff
	ioHashStringVec				m_vOwnerBuffList;
	//Gauge
	float						m_fCannonAttackNeedGauge;
	D3DXVECTOR3 m_vStartPos;

protected:
	void ClearData();

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCannonDelayState( ioBaseChar *pOwner, bool bStart );
	void ChangeToEndMotionState( ioBaseChar *pOwner );
	void ChangeToCannonEscape( ioBaseChar *pOwner );
	void ChangeToCannonCharge( ioBaseChar *pOwner );
	void ChangeToCannonAttack( ioBaseChar *pOwner );
	void ChangeToCannonAttackFailState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessCannonDelayState( ioBaseChar *pOwner );
	void ProcessCannonEndMotionState( ioBaseChar *pOwner );
	void ProcessCannonChargeState( ioBaseChar *pOwner );
	void ProcessCannonAttackState( ioBaseChar *pOwner );
	void ProcessControlCannonRot( ioBaseChar *pOwner );
	void ProcessCannonAttackFailState( ioBaseChar *pOwner );

	void CreateCannonWeapon( ioBaseChar *pOwner );

	bool EnableCannonSpecialState( ioBaseChar *pOwner );

	void SetCannonSpecialState( ioBaseChar *pOwner, bool bSendPacket );

	bool CheckSpecialState( ioBaseChar *pOwner );
	void CheckWeaponFloatInfo( ioBaseChar *pOwner );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	void AddOwnerBuff( ioBaseChar *pOwner );
	void RemoveOwnerBuff( ioBaseChar *pOwner );
	void RemoveCircleMapEffect( ioBaseChar *pOwner );
	void AddCannonFullChargeEffect( ioBaseChar *pOwner );

public:
	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessCannonSpecialState( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	//Gauge
public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	virtual int GetNeedBullet();

protected:
	void DecreaseAllGauge();
public:
	ioCannonWeaponItem(void);
	ioCannonWeaponItem( const ioCannonWeaponItem &rhs );
	virtual ~ioCannonWeaponItem(void);
};

inline ioCannonWeaponItem* ToCannonWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CANNON_ITEM )
		return NULL;

	return dynamic_cast< ioCannonWeaponItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioCannonSpecialState : public ioSpecialStateBase
{
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_CANNON_SPECIAL"; }
	virtual const CharState GetState(){ return CS_CANNON_SPECIAL; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
public:
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon ) { return true; }
	virtual bool IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity ) const { return true; }
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

public:
	ioCannonSpecialState();
	virtual ~ioCannonSpecialState();
};

inline ioCannonSpecialState* ToCannonSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CANNON_SPECIAL )
		return NULL;

	return dynamic_cast< ioCannonSpecialState* >( pState );
}