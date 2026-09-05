#pragma once
#include "ioSpecialStateBase.h"

class ioStrangerItem : public ioWeaponItem
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
		SS_EFFECT_CHARGING,
		SS_CHARGING_ATTACK_START,
		SS_CHARGING_ATTACK_LOOP,
		SS_TELEPORT_START,
		SS_TELEPORT,
	};

	enum SyncType
	{
		ST_NONE,
		ST_NORMAL_ATTACK,
		ST_EFFECT_CHARGE,
		ST_CREATE_CHARGE_EFFECT,
		ST_EFFECT_ATTACK_START,
		ST_EFFECT_ATTACK_LOOP,
		ST_DIR,
		ST_TELEPORT_START,
		ST_TELEPORT,
		ST_EFFECT_DASH_ATTACK,
	};

	struct ChargeAttackAni
	{
		ioHashString	szChargeEffectAni;
		float			fChargeEffectAniRate;

		ioHashString	szChargeEffectAttackStartAni;
		float			fChargeEffectAttackStartAniRate;

		ioHashString	szChargeEffectAttackCenterAni;
		ioHashString	szChargeEffectAttackUpAni;
		ioHashString	szChargeEffectAttackDownAni;

		void Init()
		{
			szChargeEffectAni.Clear();
			fChargeEffectAniRate = FLOAT1;
			szChargeEffectAttackStartAni.Clear();
			fChargeEffectAttackStartAniRate = FLOAT1;
			szChargeEffectAttackCenterAni.Clear();
			szChargeEffectAttackUpAni.Clear();
			szChargeEffectAttackDownAni.Clear();
		}
	};

	struct TeleportStartAni
	{
		ioHashString szAni;
		float		 fAniRate;

		void Init()
		{
			szAni.Clear();
			fAniRate = FLOAT1;
		}
	};
	typedef std::vector< TeleportStartAni > TeleportStartAniList;

	struct TeleportAni
	{
		ioHashString szAni;
		float		 fAniRate;
		float		 fEndJumpPower;
		D3DXVECTOR3	 vOffset;
		void Init()
		{
			szAni.Clear();
			fAniRate = FLOAT1;
			fEndJumpPower = 0.0f;
			vOffset = ioMath::VEC3_ZERO;
		}
	};
	typedef std::vector< TeleportAni > TeleportAniList;
protected:
	ChargeState						m_ChargeState;
	SpecialState					m_SpecialState;

	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;
	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Charge
	int								m_iMaxEffectChargeCount;
	int								m_iCurEffectChargeCount;
	std::list< DWORD >				m_vEffectInfoList;
	Vector3Vec						m_vEffectOffsetList;
	ioHashString					m_szEffectName;
	WeaponInfo						m_EffectAttackInfo;
	WeaponInfoList					m_ChargeEffectAttackInfo;
	D3DXVECTOR3						m_vChargeEffectAttackOffset;
	DWORD							m_dwChargeEffectAttackWeaponIndex;
	ioHashString					m_szChargeEffectCreateSound;
	ioHashString					m_szBackGroundEffectName;
	
	DWORD							m_dwChargeEffectTicTime;
	DWORD							m_dwNextChargeEffectTime;
	float							m_fEffectChargeEndJumpPower;
	float							m_fEffectChargeAttackEndJumpPower;

	float							m_fVertUpLimitAngle;
	float							m_fVertDownLimitAngle;
	DWORD							m_dwVertRotateWeight;
	DWORD							m_dwHorzRotateWeight;
	D3DXVECTOR3						m_vAttackDir;
	float							m_fCurrMotionRate;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;

	ChargeAttackAni					m_CurChargeAttack;
	ChargeAttackAni					m_ChargeAttack;
	ChargeAttackAni					m_JumpChargeAttack;

	//Teleport
	TeleportStartAniList			m_vStarngerTeleportStartAniList;
	TeleportAniList					m_vStarngerTeleportAniList;
	int								m_iMaxStrangerTeleportCount;
	int								m_iCurStrangerTeleportCount;

	//Gauge
	float							m_fChargeAttackNeedGauge;
	float							m_fEffectDashAttackNeedGauge;
	float							m_fDashTeleportNeedGauge;

protected:
	void ClearData();
	void LoadChargeAttackAniProperty( ioINILoader &rkLoader );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToStrangerEffectCharge( ioBaseChar *pOwner, bool bJump );
	void ChangeToStrangerEffectAttackStart( ioBaseChar *pOwner, bool bJump );
	void ChangeToStrangerEffectAttackLoop( ioBaseChar *pOwner );
	void ChangeToTeleportStartState( ioBaseChar *pOwner );
	void ChangeToTeleportState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessEffectCharging( ioBaseChar *pOwner );
	void ProcessEffectChargeAttackStart( ioBaseChar *pOwner );
	void ProcessEffectChargeAttackLoop( ioBaseChar *pOwner );
	void ProcessTeleportStartState( ioBaseChar *pOwner );
	void ProcessTeleportState( ioBaseChar *pOwner );

	void ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, ioUserKeyInput::DirKeyInput eDir, bool bLeft );
	bool ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, bool bUp );
	void ProcessRotateAnimation( ioBaseChar *pOwner );
	void ProcessWeapon( ioBaseChar *pOwner );

	void CreateEffectChargeEffect( ioBaseChar *pOwner, int iIndex );
	void CreateEffectDashAttack( ioBaseChar *pOwner );
	bool CheckDirectionKey( ioBaseChar* pOwner );

	bool IsLimitAngle( float fLimitAngle, float fCurrAngle );
	bool IsLeftRotate( ioBaseChar* pOwner, ioUserKeyInput::DirKeyInput eNewDirKey );

	void RemoveEffectList( ioBaseChar *pOwner );
	void RemoveEffectInfoList( ioPlayStage *pStage );
	void RemoveSimpleBoxWeapon( ioBaseChar *pOwner );

public:
	void ClearSpecialState( ioBaseChar* pOwner );
	bool IsEnableStarngerSpecialState();
	bool IsEnableStrangerDashTeleportState();

	void SetStrangerSpecialState( ioBaseChar *pOwner, bool bJump );
	void SetStrangerTeleportState( ioBaseChar *pOwner );

	void ProcessStarngerSpecialState( ioBaseChar *pOwner );

	virtual int GetNeedBullet();
	void DecreaseGauge( float fNeedGauge );
public:
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

	virtual bool IsEnableItemDashCheck( ioBaseChar *pOwner, bool bCheck ) { return true; }
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	//Gauge
	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	//이펙트 이동시 사용할 프로세스
public:
	virtual void CallItemProcess( ioBaseChar* pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
public:
	ioStrangerItem(void);
	ioStrangerItem( const ioStrangerItem &rhs );
	~ioStrangerItem(void);
};

inline ioStrangerItem * ToStrangerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_STRANGER_ITEM )
		return NULL;

	return dynamic_cast< ioStrangerItem * >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioStrangerSpecialState : public ioSpecialStateBase
{
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_STRANGER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_STRANGER_SPECIAL; }	
	//virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );
public:
	virtual bool IsEnableDashState( ioBaseChar* pOwner ) { return true; }
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
public:
	ioStrangerSpecialState();
	virtual ~ioStrangerSpecialState();
};