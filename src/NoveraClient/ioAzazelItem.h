#pragma once

#include "ioSpecialStateBase.h"

class ioAzazelItem : public ioWeaponItem
{
private:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_FIND_TARGET,
		CS_TARGET_ATTACK,
		CS_FAIL,
		CS_TELEPORT,
		CS_TELEPORT_END,
		CS_PUSH,

		//Sync
		CS_UPDATE_TARGET,
		CS_EXTEND_ATTACK_FIRE,
		CS_CREATE_DUMMY,
		CS_PUSH_ATTACK,
	};

private:
	//State
	ChargeState		m_ChargeState;

	//Normal Attack
	ioHashStringVec	m_AttackReadyAniList;
	int				m_iCurCombo;

	//Ani
	DWORD			m_dwMotionStartTime;
	DWORD			m_dwMotionEndTime;
	DWORD			m_dwFireStartTime;

	//D~
	//Speed Rate
	float			m_fGatherMoveSpeedRate;
	//Ani
	ioHashString	m_szGatherLoopMotion;
	ioHashString	m_szGatherMoveLoopMotion;
	float			m_fGatherAniRate;
	bool			m_bGatherMoveAni;

	//Effect
	ioHashString	m_GatherOwnerAuraEffect;
	ioHashString	m_GatherAuraEffect;
	float			m_fHeightOffSet;
	float			m_fGatheringScaleRate;
	float			m_fGatheringScaleMaxRate;
	ioHashString	m_AimEffect;
	DWORD			m_dwAimEffectID;

	UniqueObjID		m_dwGatherAuraEffectID;
	float			m_fCurrScaleRate;

	//Target
	float			m_fTargetRange;
	float			m_fTargetCurrRange;
	float			m_fTargetRangeRate;
	DWORD			m_dwFindTargetStartTime;
	DWORD			m_dwFindTargetTime;
	
	AttackAttribute	m_ExtendAttack;
	D3DXVECTOR3 m_vExtendAttackOffset;

	//Fail
	ioHashString	m_szFailAni;
	float			m_fFailAniRate;

	float m_fDummyCharOffset;
	ioHashString m_szDummyCharName;

	ioHashString m_szExtendAttackBuff;

	ioHashString m_szTeleportTarget;
	ioHashString m_szTeleportStartMotion;
	ioHashString m_szTeleportEndMotion;
	float m_fTeleportStartMotionRate;
	float m_fTeleportEndMotionRate;
	float m_fTeleportRange;
	float m_fTeleportEndJumpAmt;
	DWORD m_dwTeleportStartWeapon;
	DWORD m_dwTeleportEndWeapon;
	bool m_bTeleportEnable;

	ioHashString m_szPushMotion;
	ioHashString m_szPushWeaponBuff;
	float m_fPushMotionRate;
	float m_fPushEndJumpAmt;
	DWORD m_dwPushWeapon;
	D3DXVECTOR3 m_vPushDir;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

protected:
	void ClearData();
	void ClearState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessTargetFind( ioBaseChar *pOwner );
	void ProcessTargeting( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessExtendAttack( ioBaseChar *pOwner );
	void ProcessFailState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar* pOwner );
	void ChangeToTargetFind( ioBaseChar* pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );
	void ChangeToExtendFail( ioBaseChar* pOwner );

	void CheckGatherMoveState( ioBaseChar *pOwner );

	void ExtendAttackFire( ioBaseChar *pOwner );

	void RemoveAimEffect( ioBaseChar *pOwner );
	void UpdateTargetMaker( ioBaseChar *pOwner );
	void UpdateGatheringEffect( ioBaseChar *pOwner );

	void RemoveAuraEffect( ioBaseChar *pOwner );

	void SetTeleportEndState( ioBaseChar *pOwner, bool bSendPacket, D3DXVECTOR3 vOwnerPos = D3DXVECTOR3(), D3DXVECTOR3 vTargetPos = D3DXVECTOR3() );
	void SetEndState( ioBaseChar *pOwner, float fJumpAmt );
	void SpecialAttackFire( ioBaseChar *pOwner, DWORD dwWeapon );
	void SetPushBuffState( ioBaseChar *pOwner, ioBaseChar *pWoundChar );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual float GetAttackSpeedRate() const;

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	void CreateDummyCharState( IN ioBaseChar *pOwner, IN D3DXVECTOR3 vTargetPos );
	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

	void SetTeleportState( ioBaseChar *pOwner );
	void SetPushState( ioBaseChar *pOwner, bool bSendPacket );
	bool CheckTeleportTarget( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	ioAzazelItem(void);
	ioAzazelItem( const ioAzazelItem &rhs );
	~ioAzazelItem(void);
};

inline ioAzazelItem* ToAzazelItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_AZAZEL_ITEM )
		return NULL;

	return dynamic_cast< ioAzazelItem* >( pItem );
}

class ioAzazelSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_AZAZEL_SPECIAL"; }
	virtual const CharState GetState(){ return CS_AZAZEL_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioAzazelSpecialState();
	virtual ~ioAzazelSpecialState();
};

inline ioAzazelSpecialState* ToAzazelSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_AZAZEL_SPECIAL )
		return NULL;

	return dynamic_cast< ioAzazelSpecialState* >( pState );
}