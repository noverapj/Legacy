#pragma once

#include "ioSpecialStateBase.h"
#include "ioWeaponItem.h"

class ioEntityGroup;
class ioMissileWeapon;

typedef std::vector<float> LevelTimeRate;

class ioFireWeaponItem : public ioWeaponItem
{
public:
	struct TargetInfo
	{
		ioHashString	m_TargetName;
		float			m_fLenth;

		TargetInfo()
		{
			m_TargetName.Clear();
			m_fLenth = 0.0f;
		}
	};
	typedef std::vector<TargetInfo> TargetInfoList;

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;
	AttackAttribute m_ExtendZeroAimed;

	SecondAttributeList m_vSecondAttributeList;
	SecondAttributeList m_vSecondZeroAttributeList;

	ioHashString m_ReloadAnimation;
	ioHashString m_ReloadMoveAnimation;
	ioHashString m_ReloadEffect;

	//CEncrypt<int> m_iMaxBullet;
	//CEncrypt<int> m_iCurBullet;
	int m_iMaxBullet;
	int m_iCurBullet;
	CEncrypt<int> m_iNeedBullet;

	float m_fZeroAimSpeedRate;
	CEncrypt<float> m_fReloadTimeRate;

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

	ioHashString m_CurAimUp;
	ioHashString m_CurAimCenter;
	ioHashString m_CurAimDown;
	
protected:
	float m_fReloadTimeModifyRate;
	float m_fFireTimeModifyRate;
	
	LevelTimeRate m_LevelTimeRate;

	CEncrypt<DWORD> m_dwReloadStartTime;
	CEncrypt<DWORD> m_dwReloadEndTime;
	CEncrypt<DWORD> m_dwNextReloadTime;
	CEncrypt<DWORD> m_dwReloadGapTime;

protected:
	DWORD m_dwChargeStartTime;

	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	DWORD m_dwZeroAimedCheckTime;
	DWORD m_dwZeroAimStartTime;

	bool m_bZeroAimState;
	bool m_bEnableZeroAiming;
	bool m_bEnableDownAim;

	DWORD m_dwFireStateEndTime;
	float m_fCurAngle;
	bool m_bAimJumpAttack;

	ioHashString m_ZeroAimedEffect;
	
protected:
	bool m_bSetSkillAimed;
	bool m_bSetAimedDir;
	bool m_bSetReloadMoveAni;

protected:
	bool m_bAimedState;
	D3DXVECTOR3 m_vStartDir;
	DWORD m_dwCheckAimCancelTime;
	DWORD m_dwTrackingEndTime;

	ioHashString m_JumpAimUp;
	ioHashString m_JumpAimCenter;
	ioHashString m_JumpAimDown;

	ioHashString m_JumpFireUp;
	ioHashString m_JumpFireCenter;
	ioHashString m_JumpFireDown;

	ioHashString m_AimJumpReadyAnimation;
	ioHashString m_AimJumppingAnimation;

	CEncrypt<int>	m_iMaxWeaponCnt;

protected:
	float m_fTargetAimRate;
	float m_fDownTargetAimRate;

	bool m_bAimMove;
	bool m_bFirstAimMoveMotion;
	bool m_bAimMotionSetted;
	bool m_bChangeDir;
	float m_fAimMoveAniRate;
	float m_fAimMoveSpeedRate;
	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

	ioHashString m_AimMoveFront;
	ioHashString m_AimMoveBack;
	ioHashString m_AimMoveLeft;
	ioHashString m_AimMoveRight;

	bool m_bDefenseDash;
	bool m_bDefenseKey;
	float m_fProneStartAniRate;
	float m_fProneEndAniRate;
	float m_fProneAimAngle;
	float m_fProneAimRange;
	float m_fCurProneMotionRate;
	DWORD m_dwMotionEndTime;
	ioHashString m_ProneLoopAni;
	ioHashString m_ProneUpLoopAni;
	ioHashString m_ProneDownLoopAni;
	ioHashString m_ProneUpFireAni;
	ioHashString m_ProneDownFireAni;
	ioHashString m_ProneStartAni;
	ioHashString m_ProneEndAni;
	ioHashString m_szProneTargetName;
	ioHashString m_szProneAimMark;
	ioHashStringVec m_vProneFireBufList;
	ioHashStringVec m_vProneStateBufList;

	TargetInfoList m_vProneTargetInfoList;	
	AttackAttribute	m_ProneAttack;
	D3DXVECTOR3 m_vProneStartDir;
	D3DXVECTOR3 m_vProneAttackDir;
	D3DXVECTOR3 m_vProneAttackOffset;

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
		FS_PRONE_START,
		FS_PRONE,
		FS_PRONE_FIRE,
		FS_PRONE_END,
		FS_END,

		CSS_CHNAGE_CHAR,
	};

	enum AimJumpState
	{
		AJS_NONE,
		AJS_READY,
		AJS_CANCEL,
		AJS_JUMPPING,
	};

	FireState m_FireState;
	AimJumpState m_AimJumpState;
	DWORD m_dwAimStartTime;
	int	  m_iCurCombo;

	D3DXVECTOR3 m_vAimedDir;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void CheckJumpAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

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
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	void ModifyFireTimeRate( float fRate );
	void WasteBullet();

	inline float GetFireTimeModifyRate() const { return m_fFireTimeModifyRate; }
	
	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	virtual void SetCurBullet( int iCurBullet );

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

	bool IsDefenseDashEnable()			{ return m_bDefenseDash; }
	bool CheckDefenseSpecialState( ioBaseChar *pOwner );

	void SetProne( ioBaseChar *pOwner, bool bSendNet );
	void SetProneStart( ioBaseChar *pOwner, bool bSendNet );
	void AttackProneFire( ioBaseChar *pChar );
	void SetProneFireState( ioBaseChar *pOwner, bool bSendNet );
	void SetProneEndState( ioBaseChar *pOwner, bool bSendNet );
	void SetEndState( ioBaseChar *pOwner, bool bSendNet );
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckVDir( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	bool SetAimJumpState( ioBaseChar *pChar );

	void SetAimJumpReady( ioBaseChar *pChar );
	void SetAimJumpCancel( ioBaseChar *pChar );
	void SetAimJumpping( ioBaseChar *pChar, bool bFullTime=false );

	void UpdateAimDir( ioBaseChar *pOwner );

	bool CompareAimedTarget( ioBaseChar *pOwner );

	bool CheckDownAimState( ioBaseChar *pTarget );
	void CheckAimMoveAniState( ioBaseChar *pOwner );

public:
	ioFireWeaponItem();
	ioFireWeaponItem( const ioFireWeaponItem &rhs );
	virtual ~ioFireWeaponItem();
};

inline ioFireWeaponItem* ToFireWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_FIRE )
		return NULL;

	return dynamic_cast< ioFireWeaponItem* >( pWeapon );
}

class ioFireSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_FIRE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_FIRE_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioFireSpecialState();
	virtual ~ioFireSpecialState();
};

inline ioFireSpecialState* ToFireSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_FIRE_SPECIAL )
		return NULL;

	return dynamic_cast< ioFireSpecialState* >( pState );
}