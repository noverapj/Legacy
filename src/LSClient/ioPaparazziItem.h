#pragma once

#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPaparazziItem : public ioWeaponItem
{
public:
	enum ChargeShotState
	{
		CSS_NONE,
		CSS_NORMAL_ATTACK,
		CSS_CHARGING,
		CSS_HALF_CHARGE_ATTACK,
		CSS_AUTO_SHOT,
		CSS_AUTO_SHOT_STOP,
		CSS_AUTO_SHOT_BRANCH,
		CSS_AUTO_SHOT_END,
		CSS_CANCEL_DASH,
	};

protected:
	//AttributeList m_vExtendAttributeList;
	//AttackAttribute m_ExtendMaxAttribute;

	ChargeShotState m_ChargeShotState;
	//ioHashString m_BranchDelayAni;
	ioHashStringVec m_AttackReadyAniList;

	// D꾹 차징 - 연속 공격의 첫타 준비 동작 애니
	ioHashString m_linearFirstAttackReadyAni;

	WeaponInfo m_WeaponInfo;
	WeaponFireInfoList m_vWeaponFireInfoList;
	int m_iWeaponFireCnt;
	ioHashString m_stChargeAnimation;
	float m_fChargeAniRate;


	int m_iCurCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwWeaponBaseIndex;
	DWORD m_dwAttackFullTime;
	DWORD m_dwAttackRateLimitTime;

	float m_fWeaponRangeRate;


protected:

	bool m_bSetChargingMoveAni;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	float m_fFireEndAnimation;
	DWORD m_FireEndAnimationTime;

	// Key Reserve
	DWORD m_dwExtendCancelTime;
	bool m_bAttackCancel;

	// 용병 변경 시 게이지 초기화 on/off

	
	int		m_iCurBullet;
	int		m_iMaxBullet;
	int		m_iSpecialAttackNeedBullet;
	float	m_fTempBullet;
	float	m_fBulletIncreaseRate;

	AttributeList m_vSpecialComboList;
	int m_iCurLinearCombo;
	int m_iMaxLinearCombo;
	typedef std::vector<ioHashString> vSpecialComboEndAniList;
	vSpecialComboEndAniList m_vSpecialComboBranchAniList;
	vSpecialComboEndAniList m_vSpecialComboEndAniList;

	// 약 D~ 
	DWORD	m_dwHalfChargeTime;		// 차징 타임
	AttackAttribute	m_HalfAttack;

	// 용병 교체 시 불렛 게이지 초기화 사용 유무 
	bool m_bBulletGaugeInit;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void InitExtraGauge();

	int GetCurBullet();
	int GetMaxBullet();
	int GetNeedBullet();
	void SetCurBullet( int iCurBullet );
	void DecreaseBulletsToDoSpecialAttack();

	void CheckComboState( ioBaseChar *pChar );

	void CheckPaparazziState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

	virtual bool SetExtendDashState( ioBaseChar *pOwner );

public:
	virtual bool IsAutoTarget() const { return false; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAutoShot( ioBaseChar *pOwner );
	void OnAutoShotEnd( ioBaseChar *pOwner );
	void OnHalfChargeAttack( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ApplyChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAutoShot( ioBaseChar *pOwner );
	void ChangeToAutoShotStop( ioBaseChar *pOwner );
	void ChangeToAutoShotBranch( ioBaseChar *pOwner );
	void ChangeToAutoShotEnd( ioBaseChar *pOwner );

	// 약 D~
	void ChangeToHalfChargeAttack( ioBaseChar *pOwner );

	bool SetChangeFireMotion( ioBaseChar *pOwner );

	void ApplyChangeAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeFireMotion( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual void CheckCancelReserve( ioBaseChar *pOwner );

	void ProcessChargeAttack( ioBaseChar *pOwner);

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void CheckLinearCombo( ioBaseChar *pChar, bool bFirst );
	void SetNextLinearCombo( ioBaseChar *pChar );

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:
	void CreateWeapon( ioBaseChar *pOwner );

public:
	ioPaparazziItem();
	ioPaparazziItem( const ioPaparazziItem &rhs );
	virtual ~ioPaparazziItem();
};

inline ioPaparazziItem* ToPaparazziItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PAPARAZZI_ITEM )
		return NULL;

	return dynamic_cast< ioPaparazziItem* >( pWeapon );
}

inline ioPaparazziItem* ToPaparazziItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PAPARAZZI_ITEM )
		return NULL;

	return dynamic_cast< ioPaparazziItem* >( pWeapon );
}

inline const ioPaparazziItem* ToPaparazziItem( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PAPARAZZI_ITEM )
		return NULL;

	return dynamic_cast< const ioPaparazziItem* >( pWeapon );
}
