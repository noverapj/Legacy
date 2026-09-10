#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioWitchItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum WitchFlyState
	{
		WFS_NONE,
		WFS_WAIT,
		WFS_START,
		WFS_MOVE,
		WFS_CHARGE,
		WFS_ATTACK,
		WFS_DASH,
		WFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	WitchFlyState m_WitchFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// move
	DWORD m_dwWitchFlyWaitTime;
	DWORD m_dwWitchFlyStartTime;
	DWORD m_dwWitchFlyChargeTime;
	DWORD m_dwWitchFlyAttackEndTime;

	CEncrypt<float> m_fWitchFlyCurRange;

	bool m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	bool m_bUseSkillWitchFly;
	bool m_bWitchFly;
	WitchFlyInfo m_AttackWitchFly;
	WitchFlyInfo m_AttackWitchJumpFly;
	WitchFlyInfo m_CurWitchFly;

	ioHashString m_CurMoveAni;

	// witch gauge
	CEncrypt<float> m_fMaxWitchGauge;
	CEncrypt<float> m_fNeedWitchGauge;

	CEncrypt<float> m_fIncreaseWitchGauge;
	CEncrypt<float> m_fDecreaseWitchGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseWitchGauge;

	CEncrypt<float> m_fCurWitchGauge;

	ioHashString m_EnableWitchEffect;
	UniqueObjID m_dwEnableWitchEffect;

	AttackAttribute m_DashAttack;

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

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	bool IsCanDash( ioBaseChar *pOwner );

	void CheckWitchFlyMoveAni( ioBaseChar *pOwner );

public:
	void SetWitchFlyState( ioBaseChar *pOwner, const WitchFlyInfo &rkWitchFlyInfo, bool bUseSkill );
	void SetWitchJumpFlyState( ioBaseChar *pOwner );
	WitchFlyState GetWitchFlyState() { return m_WitchFlyState; }
	void ChangeWitchFlyStart( ioBaseChar *pOwner );
	void ChangeWitchFlyMove( ioBaseChar *pOwner );
	void ChangeWitchFlyCharge( ioBaseChar *pOwner );
	void ChangeWitchFlyAttack( ioBaseChar *pOwner );
	void ChangeWitchFlyEnd( ioBaseChar *pOwner, float fJumpAmt = 0.f );
	void ChangeWitchFlyDash( ioBaseChar *pOwner );

	void ProcessWitchFlyState( ioBaseChar *pOwner );
	bool ProcessWitchFlyStart( ioBaseChar *pOwner );
	bool ProcessWitchFlyMove( ioBaseChar *pOwner );

	bool IsWitchFlyCharCollisionSkipState();

protected:
	void CheckWitchFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ApplyFlyDashAttackMove( ioBaseChar *pOwner );

// witch Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitWitchGauge();
	void MaxWitchGauge();

	float GetCurWitchGauge();
	void SetCurWitchGauge( float fGauge );

	void SetEnableWitchEffect( ioBaseChar *pChar );
	void ReleaseEnableWitchEffect( ioBaseChar *pChar );

	void CheckEnableWitchGauge( ioBaseChar *pChar );
	
	bool IsEnableWitchGauge();
	bool IsWitchState();
	bool IsSkillWitchState();
	void ClearWitchState( ioBaseChar *pOwner, CharState eNewState );
	void ClearWitchFly()	{ m_bWitchFly = false; }

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	ioWitchItem();
	ioWitchItem( const ioWitchItem &rhs );
	virtual ~ioWitchItem();
};

inline ioWitchItem* ToWitchItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_WITCH )
		return NULL;

	return dynamic_cast< ioWitchItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioWitchFlySpecialState : public ioSpecialStateBase
{
protected:
	ioHashString m_WitchFlyBuffName;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_WITCH_FLY"; }
	virtual const CharState GetState(){ return CS_WITCH_FLY; }	

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
	virtual bool IsCanChangeJumppingState( ioBaseChar* pOwner );

public:
	void SetWitchFlyBuffName( ioHashString szWitchFlyBuffName );

public:
	ioWitchFlySpecialState();
	virtual ~ioWitchFlySpecialState();
};

inline ioWitchFlySpecialState* ToWitchFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_WITCH_FLY )
		return NULL;

	return dynamic_cast< ioWitchFlySpecialState* >( pState );
}