#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGhostStealer : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_ADD_EXTEND_ATTACK,
		CS_NORMAL_ATTACK,
		CS_RETREAT,
		CS_NONE_CHARGE_NORAML,
	};

	enum UroborusSync
	{
		UC_SET,
		UC_ADD_SET,
	};

protected:
	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;


protected:
	GhostStealerInfo m_AttackInfo;
	
	ioHashStringVec m_FakeAniList;
	//ioHashString	 m_FakeAni;
	std::vector< float > m_fFakeAniRateList;
	//float			 m_fFakeAniRate;

	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

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

	CEncrypt<bool> m_bSetUroborusSkill;

	// uroborus bullet
	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<DWORD> m_dwNeedCoolTime;

	CEncrypt<float> m_fCurCoolTime;
	CEncrypt<float> m_fRecoveryTime;


	//¾Ç·É µµµÏ Ãß°¡
	DWORD	m_dwFireEndTime;
	DWORD	m_dwFullAnimationTime;
	DWORD	m_dwFakeAttackCnt;

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

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );

	bool IsEnableFakeAttackState();

	void CheckExtendAttack( ioBaseChar *pOwner );
	bool IsFakeAttackTiming( ioBaseChar *pOwner );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();

	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

	inline float GetUroborusCurAngle() const { return m_AttackInfo.m_fCurAngle; }

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAttackFire( ioBaseChar *pOwner );
	void OnAddExtendAttackFire( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ChangeToAddExtandAttackFire( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	void UpdateGuidEffect( ioBaseChar *pOwner );

public:
	ioGhostStealer();
	ioGhostStealer( const ioGhostStealer &rhs );
	virtual ~ioGhostStealer();
};

inline ioGhostStealer* ToGhostStealerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GHOST_STEALER )
		return NULL;

	return dynamic_cast< ioGhostStealer* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioGhostStealerSpecialState : public ioSpecialStateBase
{
protected:
	D3DXVECTOR3 m_GhostStealerDir;

	float m_fGhostStealerMoveSpeed;
	float m_fGhostStealerMaxRange;
	float m_fGhostStealerCurRange;

	float m_fGhostStealerEndJumpAmt;
	float m_fGhostStealerVolumeRate;

	float m_fPreGhostStealerAddSineValue;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GHOST_STEALER_STATE"; }
	virtual const CharState GetState(){ return CS_GHOST_STEALER_STATE; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }		

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	void SetGhostStealerInfo( D3DXVECTOR3 vMoveDir, float fMoveSpeed, float fMoveRange, float fEndJumpAmt, float fVolumeRate );

public:
	ioGhostStealerSpecialState();
	virtual ~ioGhostStealerSpecialState();
};

inline ioGhostStealerSpecialState* ToGhostStealerSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GHOST_STEALER_STATE )
		return NULL;

	return dynamic_cast< ioGhostStealerSpecialState* >( pState );
}