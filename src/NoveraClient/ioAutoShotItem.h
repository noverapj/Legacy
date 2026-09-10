#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"

class ioEntityGroup;
class ioArrowWeapon;


typedef std::vector<float> LevelTimeRate;

class ioAutoShotItem : public ioWeaponItem
{
protected:
	enum FireState
	{
		FS_NONE,
		FS_NORMAL_ATTACK,
		FS_CHARGING,
		FS_AUTO_SHOT,
		FS_AUTO_SHOT_END,
	};
	FireState m_FireState;

	enum JumpAutoShotState
	{
		JAS_NONE,
		JAS_READY,
		JAS_AUTO_SHOT,
		JAS_NORMAL,
	};
	JumpAutoShotState m_JumpAutoShotState;

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttributeList m_vJumpAttributeList;

	CEncrypt<int> m_iMaxBullet;
	CEncrypt<int> m_iCurBullet;

	float m_fAimAngle;
	float m_fAimRange;
	float m_fMinRange;

	float m_fMarkerRange;

protected:
	struct MotionInfo
	{
		ioHashString m_FireUp;
		ioHashString m_FireCenter;
		ioHashString m_FireDown;

		ioHashString m_AimMoveFront;
		ioHashString m_AimMoveBack;
		ioHashString m_AimMoveLeft;
		ioHashString m_AimMoveRight;
	};

	typedef std::vector< MotionInfo > MotionInfoList;
	MotionInfoList m_MotionInfoList;

	float m_fAimMoveAniRate;

	struct JumpMotionInfo
	{
		ioHashString m_FireUp;
		ioHashString m_FireCenter;
		ioHashString m_FireDown;
	};

	typedef std::vector< JumpMotionInfo > JumpMotionInfoList;
	JumpMotionInfoList m_JumpMotionInfoList;

	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;

	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

protected:
	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	CEncrypt<float> m_fReloadTimeRate;

	float m_fReloadTimeModifyRate;
	
	LevelTimeRate m_LevelTimeRate;

	bool m_bSetReloadMoveAni;

	DWORD m_dwReloadStartTime;
	DWORD m_dwReloadEndTime;
	DWORD m_dwNextReloadTime;
	DWORD m_dwReloadGapTime;

protected:
	DWORD m_dwChargeStartTime;

	DWORD m_dwFireStartTime;
	CEncrypt<DWORD> m_dwFireEndTime;
	DWORD m_dwFireDelayTime;
	
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

	CEncrypt<int> m_iCurCombo;
	D3DXVECTOR3 m_vAimedDir;

protected:
	ioHashString m_AutoShotEndAni;
	float m_fAutoShotEndAniRate;

	DWORD m_dwRotateStartTime;
	DWORD m_dwRotateEndTime;

	DWORD m_dwRotateReturnStartTime;
	DWORD m_dwRotateReturnEndTime;

	DWORD m_dwJumpAutoShotReadyStartTime;
	DWORD m_dwJumpAutoShotReadyDuration;

	ioHashStringVec m_FireEffectList;


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

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual float GetAttackSpeedRate() const;

public:
	void WasteBullet();

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAutoShot( ioBaseChar *pOwner );
	void OnAutoShotEnd( ioBaseChar *pOwner );

	bool OnJumpAutoShotReady( ioBaseChar *pOwner );
	bool OnJumpNormalShot( ioBaseChar *pOwner );
	bool OnJumpAutoShot( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToAutoShot( ioBaseChar *pOwner );
	void ChangeToAutoShotEnd( ioBaseChar *pOwner );

	bool ChangeToJumpNormalShot( ioBaseChar *pOwner );
	void ChangeToJumpAutoShot( ioBaseChar *pOwner );

	bool SetChangeFireMotion( ioBaseChar *pOwner );
	bool SetChangeJumpFireMotion( ioBaseChar *pOwner );

	void ApplyChangeAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeFireMotion( ioBaseChar *pOwner );

	void ApplyChangeJumpAutoShot( ioBaseChar *pOwner );
	void ApplySetChangeJumpFireMotion( ioBaseChar *pOwner );

protected:
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	
	void AimedFireToUp( ioEntityGroup *pGrp,
						float fWeight,
						float fTimeRate,
						DWORD dwPreDelay,
						bool bJump,
						bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
						  float fWeight,
						  float fTimeRate,
						  DWORD dwPreDelay,
						  bool bJump,
						  bool bHold );

	void CheckAimMoveAniState( ioBaseChar *pOwner );

public:
	void SetAimedSkillUse( bool bSkillUsed = false );
	void SetAimedDirSet( bool bAimedDirSet = false );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	bool IsAimedState();

	void SetJumpAutoShotState( ioBaseChar *pOwner );
	bool CheckJumpAutoShotState( ioBaseChar *pOwner );

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

public:
	virtual const ioHashString& GetJumppingAnimation() const;
	virtual bool CheckLanding( ioBaseChar *pChar );

protected:
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void UpdateAimDir( ioBaseChar *pOwner );

	void SetFireEffect( ioBaseChar *pOwner, const D3DXVECTOR3 &vDir );

	bool CompareAimedTarget( ioBaseChar *pOwner );

public:
	ioAutoShotItem();
	ioAutoShotItem( const ioAutoShotItem &rhs );
	virtual ~ioAutoShotItem();
};

inline ioAutoShotItem* ToAutoShotItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_AUTO_SHOT )
		return NULL;

	return dynamic_cast< ioAutoShotItem* >( pWeapon );
}

