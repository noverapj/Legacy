#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioUroborusItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
	};

	enum UroborusSync
	{
		UC_SET,
	};

protected:
	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

protected:
	ioHashString m_AttackReadyAni;
	AttackAttribute m_ExtendAttackAttribute;
	int m_iCreateWeaponIndex;

	// Retreat
	ioHashString m_szRetreatFr;
	float m_fRetreatFrRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	// uroborus
	DWORD m_dwFireStartTime;

	ioHashString m_UroborusGuidEffect;
	UniqueObjID m_GuidEffectID;

	ioHashString m_UpUroborusAni;
	ioHashString m_DownUroborusAni;
	float m_fUpUroborusAniRate;
	float m_fDownUroborusAniRate;

	CEncrypt<float> m_fUroborusMoveSpeed;
	CEncrypt<float> m_fUroborusMoveRate;
	CEncrypt<float> m_fUroborusMoveAngle;

	CEncrypt<float> m_fUroborusMinAngle;
	CEncrypt<float> m_fUroborusMaxAngle;
	CEncrypt<float> m_fUroborusCurAngle;

	CEncrypt<float> m_fUroborusEndJumpAmt;

	CEncrypt<DWORD> m_dwUroborusAngleTime;

	CEncrypt<bool> m_bSetUroborusSkill;

	// uroborus bullet
	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<DWORD> m_dwNeedCoolTime;

	CEncrypt<float> m_fCurCoolTime;
	CEncrypt<float> m_fRecoveryTime;

	//
	ioHashString m_GatherAnimation;
	UniqueObjID m_GatheringEffectID;
	DWORD m_dwGatheringStartTime;

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
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );
	
	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();

	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

	D3DXVECTOR3 CheckUroborusDir( ioBaseChar *pOwner );
	D3DXVECTOR3 CheckUroborusDirForSkill( ioBaseChar *pOwner );

	inline float GetUroborusMinAngle() const { return m_fUroborusMinAngle; }
	inline float GetUroborusCurAngle() const { return m_fUroborusCurAngle; }

	inline bool IsUroborusForSkill() const { return m_bSetUroborusSkill; }
	inline void InitUroborusForSkill() { m_bSetUroborusSkill = false; }

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnAttackFire( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void CheckCurUroborusAngle( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	void UpdateGuidEffect( ioBaseChar *pOwner );

public:
	ioUroborusItem();
	ioUroborusItem( const ioUroborusItem &rhs );
	virtual ~ioUroborusItem();
};

inline ioUroborusItem* ToUroborusItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_UROBORUS )
		return NULL;

	return dynamic_cast< ioUroborusItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioUroborusSpecialState : public ioSpecialStateBase
{
protected:
	D3DXVECTOR3 m_UroborusTargetPos;
	D3DXVECTOR3 m_UroborusDir;

	float m_fUroborusMoveSpeed;
	float m_fUroborusMaxRange;
	float m_fUroborusCurRange;
	float m_fUroborusEndJumpAmt;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_UROBORUS"; }
	virtual const CharState GetState(){ return CS_UROBORUS; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	
public:
	void SetUroborusInfo( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange, float fEndJumpAmt );

public:
	ioUroborusSpecialState();
	virtual ~ioUroborusSpecialState();
};

inline ioUroborusSpecialState* ToUroborusSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_UROBORUS )
		return NULL;

	return dynamic_cast< ioUroborusSpecialState* >( pState );
}