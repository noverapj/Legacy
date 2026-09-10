#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;
class ioArrowWeapon;


typedef std::vector<float> LevelTimeRate;

class ioBowItem2 : public ioWeaponItem
{
protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_AIMING,
		FS_AIM_WAIT,
		FS_NORMAL_ATTACK,
		FS_AIMED,
		FS_NORMAL_FIRE,
		FS_AIMED_FIRE,
		FS_FULL_CHARGE,
		FS_BULLET_SYNC,
		FS_EFFECT_END_SYNC,
	};
	FireState m_FireState;

	enum AimJumpState
	{
		AJS_NONE,
		AJS_READY,
		AJS_CANCEL,
		AJS_JUMPPING,
	};
	AimJumpState m_AimJumpState;

	enum BulletCountType
	{
		BCT_NONE,
		BCT_COUNT,
		BCT_CHARGE,
	};

protected:
	//CEncrypt<int>	m_iMaxWeaponCnt;

	DWORD m_dwGatheringStartTime;

	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendZeroAimed;

	AttributeList m_vJumpExtendAttributeList;

	SecondAttributeList m_vSecondAttributeList;
	SecondAttributeList m_vSecondZeroAttributeList;

	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	ioHashString m_BranchDelayAni;

	int m_iMaxBullet;
	int m_iCurBullet;
	//float m_fIncBullet;

	float m_fZeroAimSpeedRate;
	CEncrypt<float> m_fReloadTimeRate;

	DWORD m_dwAimTime;
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;

	//float m_fMarkerRange;

	ioHashString m_AimUp;
	ioHashString m_AimCenter;
	ioHashString m_AimDown;

	ioHashString m_FireUp;
	ioHashString m_FireCenter;
	ioHashString m_FireDown;

	ioHashString m_ZeroAimUp;
	ioHashString m_ZeroAimCenter;
	ioHashString m_ZeroAimDown;

	ioHashString m_ZeroFireUp;
	ioHashString m_ZeroFireCenter;
	ioHashString m_ZeroFireDown;

protected:
	ioHashString m_AimMoveFront;
	ioHashString m_AimMoveBack;
	ioHashString m_AimMoveLeft;
	ioHashString m_AimMoveRight;

	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;

	float m_fAimMoveAniRate;

	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	float m_fAimRange_Add;
	int			m_nGuaugeDivideValue;
	//////////////////////////////////////////////////////////////////////////

protected:
	float m_fReloadTimeModifyRate;
	float m_fFireTimeModifyRate;

	LevelTimeRate m_LevelTimeRate;

	bool m_bSetReloadMoveAni;

	DWORD m_dwReloadStartTime;
	DWORD m_dwReloadEndTime;
	DWORD m_dwNextReloadTime;
	DWORD m_dwReloadGapTime;

protected:
	DWORD m_dwChargeStartTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	DWORD m_dwAimStartTime;
	DWORD m_dwZeroAimedCheckTime;
	DWORD m_dwZeroAimStartTime;

	bool m_bZeroAimState;
	bool m_bEnableZeroAiming;
	bool m_bEnableDownAim;

	bool m_bContactZeroAim;

	bool m_bAimEffect;
	ioHashString m_AimEffect;
	ioHashString m_ZeroAimedEffect;

	UniqueObjID m_ZeroAimedEffectID;

protected:
	DWORD m_dwJumpMarkerDuration;
	DWORD m_dwJumpMarkerEndTime;

protected:
	bool m_bSetSkillAimed;
	bool m_bSetAimedDir;

	float m_fCurAngle;
	bool m_bAimedState;
	bool m_bAimJumpAttack;
	D3DXVECTOR3 m_vStartDir;
	DWORD m_dwCheckAimCancelTime;
	DWORD m_dwTrackingEndTime;
	DWORD m_dwFireStateEndTime;

	ioHashString m_JumpAimUp;
	ioHashString m_JumpAimCenter;
	ioHashString m_JumpAimDown;

	ioHashString m_JumpFireUp;
	ioHashString m_JumpFireCenter;
	ioHashString m_JumpFireDown;

	ioHashString m_AimJumpReadyAnimation;
	ioHashString m_AimJumppingAnimation;

	int m_iCurCombo;

	D3DXVECTOR3 m_vAimedDir;

protected:
	BulletCountType m_BulletType;

	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<DWORD> m_dwNeedCoolTime;

	CEncrypt<DWORD> m_dwCurCoolTime;
	CEncrypt<DWORD> m_dwCheckCoolTime;

	ioHashString m_MissFire;
	float m_fMissFireRate;
	bool m_bUseBulletChangeDir;

protected:
	// 임시 : 원거리 파워 차징
	bool m_bEnablePowerCharge;

	float m_fMaxMarkerRangeRate;
	float m_fMaxAimRangeRate;
	float m_fMaxAimAngleRate;

	ioHashString m_FireEffect;
	bool m_bChangeDir;

protected:
	float m_fTargetAimRate;
	float m_fDownTargetAimRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool CheckAimState(bool bAimMove = false);
	virtual bool SetAimState( ioBaseChar *pChar );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void CheckJumpAttack( ioBaseChar *pOwner );

	virtual void SetNoneState( ioBaseChar *pOwner, int eNewState = 0 );

	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual float GetAttackSpeedRate() const;

public:
	void ModifyFireTimeRate( float fRate );
	void WasteBullet();

	inline float GetFireTimeModifyRate() const { return m_fFireTimeModifyRate; }

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAiming( ioBaseChar *pOwner );
	void OnAimWait( ioBaseChar *pOwner );
	void OnAimed( ioBaseChar *pOwner );
	void OnFire( ioBaseChar *pOwner );
	void OnJumpFire( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAiming( ioBaseChar *pOwner );
	void ChangeToNormalFire( ioBaseChar *pOwner );
	void ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void ChangeToAimedState( ioBaseChar *pOwner );
	void ChangeToAimedFire( ioBaseChar *pOwner );

	bool CheckZeroAim( ioBaseChar *pOwner );
	void SetZeroAim( ioBaseChar *pOwner, bool bSet );

	void EndGatheringEffect( ioBaseChar *pOwner, bool bGathering, bool bMaxEffect, bool bSendNet );

protected:
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void TrackingTarget( ioBaseChar *pOwner );

	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

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

	void CheckAimMoveAniState( ioBaseChar *pOwner );

public:
	void SetAimedSkillUse( bool bSkillUsed = false );
	void SetAimedDirSet( bool bAimedDirSet = false );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	bool IsAimedState();

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

public:
	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

	bool IsChargeBulletType();

public:
	virtual const ioHashString& GetJumppingAnimation() const;
	virtual void CheckAimJumpState( ioBaseChar *pChar );
	virtual bool CheckLanding( ioBaseChar *pChar );

protected:
	bool SetAimJumpState( ioBaseChar *pChar );

	void SetAimJumpReady( ioBaseChar *pChar );
	void SetAimJumpCancel( ioBaseChar *pChar );
	void SetAimJumpping( ioBaseChar *pChar, bool bFullTime=false );

	void UpdateAimDir( ioBaseChar *pOwner );

	bool CompareAimedTarget( ioBaseChar *pOwner );

	bool CheckDownAimState( ioBaseChar *pTarget );

	//
	void CheckChargeRate();
	void CheckTargetMarker( ioBaseChar *pOwner );

	float GetChargeAimRange();
	float GetChargeAimAngle();

	void SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir );

	void SetEffectSync( ioBaseChar *pOwner );

public:
	ioBowItem2();
	ioBowItem2( const ioBowItem2 &rhs );
	virtual ~ioBowItem2();
};

inline ioBowItem2* ToBowItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BOW2 )
		return NULL;

	return dynamic_cast< ioBowItem2* >( pWeapon );
}

