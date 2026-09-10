#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;
class ioArrowWeapon;


typedef std::vector<float> LevelTimeRate;

class ioPentjakSilatItem : public ioWeaponItem
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
		FS_EFFECT_FULL,
		FS_RETREAT,
	};
	FireState m_FireState;

	enum BulletCountType
	{
		BCT_NONE,
		BCT_COUNT,
		BCT_CHARGE,
	};

protected:
	CEncrypt<int>	m_iMaxWeaponCnt;

	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vGatheringAttributeList;
	//AttackAttribute m_ExtendZeroAimed;

	ioHashString m_BranchDelayAni;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

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

protected:
	ioHashString m_AimMoveFront;
	ioHashString m_AimMoveBack;
	ioHashString m_AimMoveLeft;
	ioHashString m_AimMoveRight;

	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;

	float m_fAimMoveAniRate;

	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

protected:
	float m_fFireTimeModifyRate;

protected:
	DWORD m_dwChargeStartTime;
	DWORD m_dwAimingStartTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	DWORD m_dwAimStartTime;
	bool m_bEnableDownAim;

	bool m_bAimEffect;
	ioHashString m_AimEffect;

protected:
	bool m_bSetSkillAimed;
	bool m_bSetAimedDir;

	float m_fCurAngle;
	bool m_bAimedState;
	
	D3DXVECTOR3 m_vStartDir;
	DWORD m_dwCheckAimCancelTime;
	DWORD m_dwTrackingEndTime;
	DWORD m_dwFireStateEndTime;

	int m_iCurCombo;

	D3DXVECTOR3 m_vAimedDir;

protected:
	BulletCountType m_BulletType;

	ioHashString m_MissFire;
	float m_fMissFireRate;
	bool m_bUseBulletChangeDir;

protected:
	// 임시 : 원거리 파워 차징
	CEncrypt<bool> m_bEnablePowerCharge;

	CEncrypt<float> m_fMaxMarkerRangeRate;
	CEncrypt<float> m_fMaxAimRangeRate;
	CEncrypt<float> m_fMaxAimAngleRate;

	ioHashString m_FireEffect;
	bool m_bChangeDir;

protected:
	float m_fTargetAimRate;
	float m_fDownTargetAimRate;

	//////////////////////////////////////////////////////////////////////////
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	//////////////////////////////////////////////////////////////////////////
	bool m_bDefenseEnableSet;
	DWORD m_dwDefenseStartTime;
	DWORD m_dwDefenseEndTime;

	bool m_bDefenseMotionSetted;
	bool m_bDefenseFirstMoveMotion;

	ioHashString m_AniDefenseFrontMove;
	ioHashString m_AniDefenseBackMove;
	ioHashString m_AniDefenseRightMove;
	ioHashString m_AniDefenseLeftMove;

	ioHashString m_szRetreatFr;
	ioHashString m_szRetreatBack;
	ioHashString m_szRetreatLt;
	ioHashString m_szRetreatRt;

	float m_fRetreatFrRate;
	float m_fRetreatBackRate;
	float m_fRetreatLtRate;
	float m_fRetreatRtRate;

	ioHashString m_szCurRetreatAni;
	float m_fCurRetreatAniRate;
	D3DXVECTOR3 m_vForceDir;

	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	DWORD m_dwRetreatEndTime;

	DWORD m_dwCounterAttackCheckTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool CheckAimState(bool bAimMove = false);
	//virtual bool SetAimState( ioBaseChar *pChar );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void CheckJumpAttack( ioBaseChar *pOwner );

	virtual void SetNoneState( ioBaseChar *pOwner, int eNewState = 0 );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual float GetAttackSpeedRate() const;

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual void CheckRetreatState( ioBaseChar *pOwner );
	void ClearState();

public:
	void ModifyFireTimeRate( float fRate );

	inline float GetFireTimeModifyRate() const { return m_fFireTimeModifyRate; }

	virtual int GetCurBullet();
	virtual int GetMaxBullet() { return m_iMaxBullet; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAiming( ioBaseChar *pOwner );
	void OnAimWait( ioBaseChar *pOwner );
	void OnAimed( ioBaseChar *pOwner );
	void OnFire( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAiming( ioBaseChar *pOwner, bool bFirst );
	void ChangeToNormalFire( ioBaseChar *pOwner );
	void ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void ChangeToAimedState( ioBaseChar *pOwner );
	void ChangeToAimedFire( ioBaseChar *pOwner );

	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

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
	void CheckChargeRateEffect( ioBaseChar *pOwner );

	void EndDefense( ioBaseChar *pOwner );

public:
	void SetAimedSkillUse( bool bSkillUsed = false );
	void SetAimedDirSet( bool bAimedDirSet = false );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	bool IsAimedState();

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

public:
	bool IsChargeBulletType();

protected:

	void UpdateAimDir( ioBaseChar *pOwner );

	bool CompareAimedTarget( ioBaseChar *pOwner );

	bool CheckDownAimState( ioBaseChar *pTarget );

	//
	void CheckChargeRate( ioBaseChar *pOwner );
	//void CheckTargetMarker( ioBaseChar *pOwner );

	float GetChargeAimRange();
	float GetChargeAimAngle();

	void SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir );
	void SetFullEffect( ioBaseChar *pOwner );

public:
	ioPentjakSilatItem();
	ioPentjakSilatItem( const ioPentjakSilatItem &rhs );
	virtual ~ioPentjakSilatItem();
};

inline ioPentjakSilatItem* ToPentjakSilatItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PENTJAK_SILAT_ITEM )
		return NULL;

	return dynamic_cast< ioPentjakSilatItem* >( pWeapon );
}

