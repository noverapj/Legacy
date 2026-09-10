#pragma once


#include "ioWeaponItem.h"

class ioEntityGroup;
class ioMissileWeapon;

typedef std::vector<float> LevelTimeRate;

class ioLaserItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendZeroAimed;

	SecondAttributeList m_vSecondAttributeList;
	SecondAttributeList m_vSecondZeroAttributeList;

	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<DWORD> m_dwCurCoolTime;
	CEncrypt<DWORD> m_dwNeedCoolTime;

	CEncrypt<DWORD> m_dwCheckCoolTime;

	float m_fFireTimeModifyRate;

	DWORD m_dwAimTime;
	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;

	float m_fMarkerRange;

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

	ioHashString m_MissFire;
	float m_fMissFireRate;

protected:
	DWORD m_dwChargeStartTime;

	CEncrypt<DWORD> m_dwFireStartTime;
	CEncrypt<DWORD> m_dwFireEndTime;
	CEncrypt<DWORD> m_dwFireDelayTime;

	DWORD m_dwZeroAimedCheckTime;
	DWORD m_dwZeroAimStartTime;

	bool m_bZeroAimState;
	bool m_bEnableZeroAiming;
	bool m_bEnableDownAim;

	float m_fZeroAimSpeedRate;

	ioHashString m_ZeroAimedEffect;

protected:
	DWORD m_dwJumpMarkerDuration;
	DWORD m_dwJumpMarkerEndTime;
	
protected:
	bool m_bSetSkillAimed;
	bool m_bSetAimedDir;

protected:
	enum AimJumpState
	{
		AJS_NONE,
		AJS_READY,
		AJS_CANCEL,
		AJS_JUMPPING,
	};
	AimJumpState m_AimJumpState;

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
	};

	FireState m_FireState;
	DWORD m_dwAimStartTime;
	int	  m_iCurCombo;

	D3DXVECTOR3 m_vAimedDir;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
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

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual float GetAttackSpeedRate() const;

public:
	void ModifyFireTimeRate( float fRate );
	void SetFireStartTime( DWORD dwFireTime );

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

protected:
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void TrackingTarget( ioBaseChar *pOwner );

	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	bool CheckZeroAim( ioBaseChar *pOwner );
	void SetZeroAim( ioBaseChar *pOwner, bool bSet );
	
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
	void SetAimedSkillUse( bool bSkillUsed = false );
	void SetAimedDirSet( bool bAimedDirSet = false );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	bool IsAimedState();

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

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

public:
	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

public:
	ioLaserItem();
	ioLaserItem( const ioLaserItem &rhs );
	virtual ~ioLaserItem();
};

inline ioLaserItem* ToLaserItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LASER )
		return NULL;

	return dynamic_cast< ioLaserItem* >( pWeapon );
}

