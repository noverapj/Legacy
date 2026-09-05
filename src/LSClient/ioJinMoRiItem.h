#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioJinMoRiItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_FULL_GATHER,
		CS_ATTACK_FIRE,
		CS_COMBO,
		CS_RETREAT,
		CS_DASH_JUMP,
		CS_DASH_EXTEND_ATTACK,
		CS_END,
	};

	enum SyncType
	{
		//ST_SET_GP,
		//ST_RELEASE_GP,
		ST_SET_GP_EFFECT,
		//ST_RELEASE_GP_EFFECT,
		ST_SET_NEXT_COMBO,
		ST_RETREAT_ATTACK,
		ST_END,
	};

	enum RetreatAttackType
	{
		RET_ATTACK_TYPE_NONE,
		RET_ATTACK_TYPE1,
		RET_ATTACK_TYPE2,
		RET_ATTACK_TYPE3,
		RET_ATTACK_TYPE4,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	//bool m_bCharged;

	// extra gauge : gp
	CEncrypt<bool> m_bUsedGPState;

	CEncrypt<float> m_fNeedGPGauge;

	ioHashString m_GPStateEffect;
	UniqueObjID m_dwGPStateEffect;

	//
	AttackAttribute m_ChangeGPStateAttack;
	AttackAttribute m_ReleaseGPStateAttack;

	ioHashStringVec m_vGPStateBuffList;
	ioHashStringVec m_vReleaseGPStateBuffList;

	CEncrypt<DWORD> m_dwGPStateObjectItem;

	//
	DWORD			m_dwMotionEndTime;

	CEncrypt<DWORD> m_dwGatheringTime;
	float			m_fGatherAniRate;
	ioHashString	m_GatherAnimation;

	CEncrypt<int> m_iChargeCombo;
	CEncrypt<DWORD> m_dwMaxComboDuration;
	CEncrypt<DWORD> m_dwStartComboTime;

	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	AttributeList m_vChargeComboAttList;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	// retreat attack
	AttackAttribute m_RetreatAttribute;
	DWORD m_dwRetreatEndTime;

	float			m_DashAttackEndJumpAmt;
	D3DXVECTOR3		m_vDashMoveDir;
	ioHashString	m_stDashTarget;

	float			m_fExtendTargetAngle;
	float			m_fExtendTargetRange;
	float			m_fExtendTargetUpHeight;
	float			m_fExtendTargetDownHeight;

	float			m_fTargetAngle;
	float			m_fTargetRange;
	float			m_fTargetUpHeight;
	float			m_fTargetDownHeight;

	AttackAttribute m_DashAttackMove;
	AttackAttribute m_DashExtraAttack_left;
	AttackAttribute m_DashExtraAttack_right;
	AttackAttribute m_DashExtraAttack_up;
	AttackAttribute m_DashExtraAttack_down;

	//int				m_nDashAttackType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );
	
	void ReleaseGPState( ioBaseChar *pOwner, bool bObjReleased = false );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );

	void ChangeToGatheringState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToComboState( ioBaseChar *pOwner );
	bool ChangeToRetreatAttack( ioBaseChar *pOwner, int nAttackType  );
	void ChangeEnd( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void SetNextComboState( ioBaseChar *pOwner );

	void FindExtendAttackTarget( ioBaseChar *pOwner );
	void FindDashTarget( ioBaseChar *pOwner );

	void CheckDashMoveDir( ioBaseChar *pOwner );

	bool IsEnableTargetState( ioBaseChar *pTarget );

public:
	bool IsGPState();

	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	bool IsEnableGPState( ioBaseChar *pOwner );
	void SetUseGPState( ioBaseChar *pOwner );
	void ApplyUseGPState( ioBaseChar *pOwner );

	void SetEnableGPStateEffect( ioBaseChar *pChar );
	void ReleaseEnableGPStateEffect( ioBaseChar *pChar );

	bool CheckReleaseGPState( ioBaseChar *pOwner );

public:
	ioJinMoRiItem();
	ioJinMoRiItem( const ioJinMoRiItem &rhs );
	virtual ~ioJinMoRiItem();
};

inline ioJinMoRiItem* ToJinMoRiItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JINMORI_ITEM )
		return NULL;

	return dynamic_cast< ioJinMoRiItem* >( pItem );
}

