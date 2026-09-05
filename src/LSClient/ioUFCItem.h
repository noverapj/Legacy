#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioUFCItem : public ioWeaponItem
{
public:
	enum SpecialState
	{
		SS_NORMAL_S,
		SS_NORMAL_A,
		SS_CHARGE_S,
		SS_CHARGE_A,
		SS_SKILL_ASD,
		SS_SKILL_AD,
		SS_SKILL_AS,
		SS_SKILL_ASD_EXTRA,
		SS_SKILL_AD_EXTRA,
		SS_SKILL_AS_EXTRA,
		SS_RETREAT_D,

		SS_EXTRA = 3,
	};

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_SPECIAL_ATTACK_FIRE,
		CS_RETREAT_MOVE,
		CS_POSE_CHANGE_PRE,
		CS_POSE_CHANGE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	
	ChargeState			m_ChargeState;
	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	bool				m_bCharged;

protected:
	DWORD				m_dwMotionEndTime;

	AttackAttribute		m_NormalSAttribute;
	AttackAttribute		m_NormalAAttribute;
	AttackAttribute		m_ChargeSAttribute;
	AttackAttribute		m_ChargeAAttribute;
	AttackAttribute		m_RetreatDAttribute;
	AttackAttribute		m_SkillASDAttribute;
	AttackAttribute		m_SkillADAttribute;
	AttackAttribute		m_SkillASAttribute;
	AttackAttribute		m_SkillASDExtraAttribute;
	AttackAttribute		m_SkillADExtraAttribute;
	AttackAttribute		m_SkillASExtraAttribute;

	ioHashString					m_szRetreatMoveFr;
	ioHashString					m_szRetreatMoveBk;
	ioHashString					m_szRetreatMoveRt;
	ioHashString					m_szRetreatMoveLt;
	float							m_fRetreatMoveRateFr;
	float							m_fRetreatMoveRateBk;
	float							m_fRetreatMoveRateRt;
	float							m_fRetreatMoveRateLt;
	float							m_fRetreatMoveForce;
	float							m_fRetreatMoveFriction;

	DWORD							m_dwMoveForceTime;
	D3DXVECTOR3						m_vMoveForceDir;

	ioHashString					m_szPoseSetAni;
	ioHashString					m_szPoseReleaseAni;
	float							m_fPoseSetAniRate;
	float							m_fPoseReleaseAniRate;

	ioHashString					m_szPoseEffect;
	DWORD m_dwPoseEffectIndex;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fCurGauge;
	ioHashStringVec m_vGaugeMaxBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	void ChangeToSpecialAttackFire( ioBaseChar *pOwner, int iSpecialState );

public:
	virtual WeaponSubType GetSubType() const;
	virtual bool CheckOnHand();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

protected:
	//대쉬 공격
	virtual void CheckExtendDashState( ioBaseChar *pOwner );	
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );	

	void ChangeToRetreatMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir );
	void CheckFireAndMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void SetPoseChangeState( ioBaseChar *pOwner );
	void SetPoseChangePreState( ioBaseChar *pOwner );

	void CreatePoseEffect( ioBaseChar *pChar );
	void DestroyPoseEffect( ioBaseChar *pChar );

protected:
	void ProcessRetreatMoveState( ioBaseChar *pOwner );

	void SetGaugeMaxBuff( ioBaseChar *pChar );
	void ReleaseGaugeMaxBuff( ioBaseChar *pChar );

public:
	ioUFCItem();
	ioUFCItem( const ioUFCItem &rhs );
	virtual ~ioUFCItem();
};

inline ioUFCItem* ToUFCItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_UFC_ITEM )
		return NULL;

	return dynamic_cast< ioUFCItem* >( pItem );
}