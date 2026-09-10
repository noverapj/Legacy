#pragma once
#include "ioSpecialStateBase.h"

class ioUrielItem : public ioWeaponItem
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
		SS_ATTACK,
		SS_ATTACK_END_DELAY,
		SS_ADD_ATTACK,
		SS_WOUNDED_ATTACK,
	};

	enum WoundedState
	{
		WS_DELAY,
		WS_TIME_CONDITION_CHECK,
		WS_ACTION,
	};

	enum SyncType
	{
		ST_NORMAL_ATTACK,
		ST_CHARGE_ATTACK,
		ST_CHARGE_ATTACK_END_DELAY,
		ST_CHARGE_ADD_ATTACK,
		ST_CHARGE_ADD_ATTACK_FIRE,
		ST_SET_TARGET,
		ST_WOUNDED_ATTACK,
	};

	typedef struct _tagchargeattack
	{
		ioHashString	m_szChargeAttackAni;
		float			m_fChargeAttackAniRate;
		float			m_fChargeAttackAngle;
		float			m_fMaxChargeAttackRange;
		int				m_iChargeWeaponType;
		float			m_fChargeAttackEndPower;
		float			m_fChargeAttackSpeed;
		DWORD			m_dwEndDelay;
		AttackAttribute	m_AddAttack;
	}ChargeAttack;

	struct WoundedAttackInfo
	{
		float			fMinHeight;
		float			fMaxHeight;
		AttackAttribute	Attack;

		WoundedAttackInfo()
		{
			fMinHeight		= 0.0f;
			fMaxHeight		= 0.0f;
			Attack.Init();
		}
	};
	typedef std::vector<WoundedAttackInfo> WoundedAttackInfoList;

protected:
	//State
	ChargeState				m_ChargeState;
	SpecialState			m_SpecialState;
	WoundedState			m_WoundedState;

	//Ani
	DWORD					m_dwMotionStartTime;
	DWORD					m_dwMotionEndTime;;
	DWORD					m_dwAttackFireTime;
	//Normal Attack
	ioHashStringVec			m_AttackReadyAniList;
	int						m_iCurCombo;


	//D~ & Dash D~
	ChargeAttack			m_ChargeAttack;
	ChargeAttack			m_DashChargeAttack;
	
	ioHashString			m_szTargetName;
	ChargeAttack			m_CurChargeAttack;
	DWORD					m_dwChargeWeaponIndex;
	float					m_fCurChargeAttackRange;
	D3DXVECTOR3				m_vChargeAttackMoveDir;
	DWORD					m_dwChargeAttackEndDelayTime;

	//Gauge
	float					m_fMaxBullet;
	float					m_fCurBullet;
	float					m_fIncreseGaugeTic;
	float					m_fChargeAttackNeedBullet;
	float					m_fWoundedAttackNeedBullet;

	//피격중 공격
	WoundedAttackInfoList	m_WoundedAttackInfoList;
	DWORD					m_dwWoundedDelayTime;
	ioHashStringVec			m_vWoundedAttackOwnerBuffList;
	
	DWORD					m_dwWoundedStartTime;

protected:
	void ClearData();

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAddAttack( ioBaseChar *pOwner );
	void ChangeToChargeAttackEndDelayState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessSpecialAttack( ioBaseChar *pOwner );
	void ProcessSpecialAddAttack( ioBaseChar *pOwner );
	void ProcessChargeAttackEndDelay( ioBaseChar *pOwner );

	bool CheckChargeWeaponLive( ioBaseChar *pOwner );
	void ClearChargeWeapon( ioBaseChar *pOwner );

	bool IsEnableAddAttack( ioBaseChar *pOwner );

	void AddAttackFire( ioBaseChar *pOwner );

	//피격중 특별
	void CheckWoundExtendUseDelay( ioBaseChar* pOwner );
	bool CheckWoundedAttackGauge() const;
	bool ChangeToWoundedAttacFire( ioBaseChar *pOwner, const AttackAttribute& rkAttack );
	void ProcessWoundedAttackState( ioBaseChar* pOwner );
	void AddWoundedAttackOwnerBuff( ioBaseChar* pOwner );

public:
	void SetChargeAttack( ioBaseChar *pOwner, bool bDash );
	void ProcessSpecialState( ioBaseChar *pOwner );
	bool CheckChargeAttackGauge() const;
	bool IsProcessMove( ioBaseChar *pOwner );
	bool IsCollisionAvailableWeapon( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	void DecreaseChargeAttackGauge();
	void DecreaseWoundedAttackGauge();

public:
	virtual bool IsWoundAfterUseExtendState(){ return true; }
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );
	virtual void CallItemProcess( ioBaseChar* pOwner );
public:
	ioUrielItem(void);
	ioUrielItem( const ioUrielItem &rhs );
	virtual ~ioUrielItem(void);
};

inline ioUrielItem* ToUrielItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_URIEL_ITEM )
		return NULL;

	return dynamic_cast< ioUrielItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioUrielSpecialState : public ioSpecialStateBase
{
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_URIEL_STATE"; }
	virtual const CharState GetState(){ return CS_URIEL_STATE; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon );

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	//virtual bool IsStateEnd( ioBaseChar* pOwner );

public:
	ioUrielSpecialState();
	virtual ~ioUrielSpecialState();
};

inline ioUrielSpecialState* ToUrielSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_URIEL_STATE )
		return NULL;

	return dynamic_cast< ioUrielSpecialState* >( pState );
}
