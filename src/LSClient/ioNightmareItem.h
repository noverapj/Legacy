#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioNightmareItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};
	enum
	{
		e_DASH,
		e_Wound,
	};
	enum FlyState
	{
		LFS_NONE,
		LFS_START,
		LFS_DELAY,
		LFS_FIRE,
		LFS_FIRE_LOOP,
		LFS_FIRE_END,
	};
	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_FIRE,
	};
	enum
	{
		GAUGE_COUNT = 3,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	FlyState m_FlyState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	int m_iCurCombo;

	DWORD m_dwFlyStartTime;
	CEncrypt<float> m_fFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	DraculaFlyInfo m_AttackFly;
	DraculaFlyInfo m_CurFly;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge[GAUGE_COUNT];
	CEncrypt<float> m_fCurGauge;

	//지상 공중 판정 높이
	float			m_fCheckLandHeight;
	float			m_fAirJumpPower;

	//변신 시간
	DWORD			m_dwMaxSpecialTime;
	DWORD			m_dwSpecialStartTime;

	// Map Effect
	DWORD		m_dwMapEffectID;
	ioHashString	m_stMapEffect;

	AttackAttribute m_FireAttribute;
	ioHashString m_LoopAnimation;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	DWORD m_dwWeaponBaseIndex;
	DWORD m_dwWoundedChargeStartTime;

	bool m_bCollisionAvailable;
	DWORD m_dwCollisionSkipTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadFlyInfo( ioINILoader &rkLoader, DraculaFlyInfo& cInfo, const char *szAttack );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove = false );

	void DecreaseGauge( float fNeedGauge );
	void AddBuff( ioBaseChar *pOwner );
	void EraseBuff( ioBaseChar *pOwner );

	void ChangeLandNormalAttack( ioBaseChar *pOwner );
	void ChangeAirNornmalAttack( ioBaseChar *pOwner, bool bAttack );

public:
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void CheckFlyState( ioBaseChar *pOwner, bool bAttack );

	//Delay->move로 바뀔때 혹은 Up_Down일때
	void ChangeFlyMove( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner );

	bool IsFlyCharCollisionSkipState();
	
	void SetFireEndState( ioBaseChar *pOwner, bool bSend );
	void SetLoopState( ioBaseChar *pOwner, bool bSend );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();
	void SetCurGauge( float fGauge );

	bool IsEnableGauge( bool bDouble );
	bool IsDraculaState();
	void ClearState( ioBaseChar *pOwner, bool bEraseBuff = false );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	void SetFlyState( ioBaseChar *pOwner, const DraculaFlyInfo &rkFlyInfo, int iState, bool bSendNet );
	bool SetDashFlyState( ioBaseChar *pOwner, bool bSendNet );
	void SetWoundFlyState( ioBaseChar *pOwner, bool bSendNet );
	void SetAttackState( ioBaseChar *pOwner, bool bSendNet );

public:
	virtual bool IsWoundAfterUseExtendState();
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );

	bool IsCollisionAvailableWeaponBySpecialState();

public:
	ioNightmareItem();
	ioNightmareItem( const ioNightmareItem &rhs );
	virtual ~ioNightmareItem();
};

inline ioNightmareItem* ToNightmareItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NIGHTMARE_ITEM )
		return NULL;

	return dynamic_cast< ioNightmareItem* >( pItem );
}

class ioNightmareSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NIGHTMARE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_NIGHTMARE_SPECIAL; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
	virtual bool IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioNightmareSpecialState();
	virtual ~ioNightmareSpecialState();
};

inline ioNightmareSpecialState* ToNightmareSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NIGHTMARE_SPECIAL )
		return NULL;

	return dynamic_cast< ioNightmareSpecialState* >( pState );
}