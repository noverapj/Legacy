#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDestroyerItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum DestroyerFlyState
	{
		CFS_NONE,
		CFS_START,
		CFS_DELAY,
		CFS_MOVE,
		CFS_ATTACK,
		CFS_CHARGING,
		CFS_CHARGED,
		CFS_CHARGE_ATTACK,
		CFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
		SST_CHARGED_FIRE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DestroyerFlyState m_DestroyerFlyState;
	DestroyerFlyState m_OldDestroyerFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bFlyAttackEnable;

	// move
	DWORD m_dwDestroyerFlyStartTime;
	DWORD m_dwDestroyerFlyMoveTime;
	DWORD m_dwDestroyerFlyAttackMoveTime;
	DWORD m_dwDestroyerFlyAttackEndTime;
	DWORD m_dwDestroyerFlyChargeAttackEndTime;

	CEncrypt<float> m_fDestroyerFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	D3DXVECTOR3 m_vFlyForceDir;
	DWORD m_dwFlyForceTime;

	D3DXVECTOR3 m_vFlyAttackMoveDir;
	CEncrypt<float> m_fFlyAttackMoveSpeed;

	//
	ioHashString m_DestroyerTarget;
	ioHashString m_DestroyerTargetSkill;

	//
	DestroyerFlyInfo m_AttackDestroyerFly;
	DestroyerFlyInfo m_CurDestroyerFly;

	// mechanics gauge
	CEncrypt<float> m_fMaxDestroyerGauge;
	CEncrypt<float> m_fNeedDestroyerGauge;

	CEncrypt<float> m_fIncreaseDestroyerGauge;
	CEncrypt<float> m_fDecreaseDestroyerGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseDestroyerGauge;

	//CEncrypt<float> m_fShotDecreaseGauge;
	CEncrypt<float> m_fDefenceDecreaseGauge;

	CEncrypt<float> m_fCurDestroyerGauge;

	ioHashString m_EnableDestroyerEffect;
	UniqueObjID m_dwEnableDestroyerEffect;

	UniqueObjID m_dwFlyMoveEffect;

	//charge in charge
	DWORD m_dwStartChargingTime;

	//Tracking
	bool m_bNormalTracking;

	DWORD m_dwAimStartTime;
	DWORD m_dwTrackingEndTime;
	float m_fCurAngle;
	DWORD m_dwAimTime;

	ioHashString m_AimUp;
	ioHashString m_AimCenter;
	ioHashString m_AimDown;

	ioHashString m_FireUp;
	ioHashString m_FireCenter;
	ioHashString m_FireDown;

	ioHashString m_ChargeAimUp;
	ioHashString m_ChargeAimCenter;
	ioHashString m_ChargeAimDown;

	ioHashString m_ChargeFireUp;
	ioHashString m_ChargeFireCenter;
	ioHashString m_ChargeFireDown;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadDestroyerFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckDestroyerFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ReleaseBipe( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	void DecreaseNormalFireGauge();
	void DecreseChargeFireGauge();

public:
	void SetDestroyerFlyState( ioBaseChar *pOwner, const DestroyerFlyInfo &rkFlyInfo, bool bSendNet );
	DestroyerFlyState GetDestroyerFlyState() { return m_DestroyerFlyState; }
	
	void ChangeDestroyerFlyStart( ioBaseChar *pOwner );
	void ChangeDestroyerFlyDelay( ioBaseChar *pOwner );
	void ChangeDestroyerFlyMove( ioBaseChar *pOwner );
	void ChangeDestroyerFlyAttack( ioBaseChar *pOwner );
	void ChangeDestroyerFlyCharging( ioBaseChar*pOwner );
	void ChangeDestroyerFlyCharged( ioBaseChar *pOwner );
	void ChangeDestroyerFlyChargeAttack( ioBaseChar *pOwner );
	void ChangeDestroyerFlyEnd( ioBaseChar *pOwner );

	void ProcessDestroyerFlyState( ioBaseChar *pOwner );

	bool ProcessDestroyerFlyStart( ioBaseChar *pOwner );
	bool ProcessDestroyerFlyMove( ioBaseChar *pOwner );
	bool ProcessDestroyerFlyCharging( ioBaseChar *pOwner );
	void ProcessDestroyerFlyCharged( ioBaseChar *pOwner );

	bool IsDestroyerFlyCharCollisionSkipState();

	inline const int GetDestroyerFlyState() const { return (int)m_DestroyerFlyState; }
	inline const D3DXVECTOR3& GetDestroyerFlyAttMoveDir() const { return m_vFlyAttackMoveDir; }
	inline const float GetDestroyerFlyAttMoveSpeed() const { return m_fFlyAttackMoveSpeed; }

protected:
	void CheckDestroyerFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckFlyAttTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

	void SetDestroyerFlyMoveEffect( ioBaseChar *pChar );
	void UpdateDestroyerFlyMoveEffect( ioBaseChar *pChar );
	void ReleaseDestroyerFlyMoveEffect( ioBaseChar *pChar );

	void ChangeTrackingLR( ioBaseChar *pOwner );
	void ChangeTrackingUD( ioBaseChar *pOwner );
	void ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr );

	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bHold );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	float GetDecreaseShotGauge();

	void InitDestroyerGauge();
	void MaxDestroyerGauge();

	float GetCurDestroyerGauge();
	void SetCurDestroyerGauge( float fGauge );

	void SetEnableDestroyerEffect( ioBaseChar *pChar );
	void ReleaseEnableDestroyerEffect( ioBaseChar *pChar );

	void CheckEnableDestroyerGauge( ioBaseChar *pChar );

	bool IsEnableDestroyerGauge();
	bool IsDestroyerState();
	void ClearDestroyerState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	//D+이동중에는 방어가 되는데 방어시 gauge를 깍는다
	void SubtractionDefecseGauge();

public:
	const ioHashString& GetDestroyerTargetSkill();
	void ClearDestroyerTargetSkill();

	bool IsBallTarget();

public:
	ioDestroyerItem();
	ioDestroyerItem( const ioDestroyerItem &rhs );
	virtual ~ioDestroyerItem();
};

inline ioDestroyerItem* ToDestroyerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DESTROYER )
		return NULL;

	return dynamic_cast< ioDestroyerItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioDestroyerFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DESTROYER_FLY"; }
	virtual const CharState GetState(){ return CS_DESTROYER_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

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

public:
	ioDestroyerFlySpecialState();
	virtual ~ioDestroyerFlySpecialState();
};

inline ioDestroyerFlySpecialState* ToDestroyerFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DESTROYER_FLY )
		return NULL;

	return dynamic_cast< ioDestroyerFlySpecialState* >( pState );
}