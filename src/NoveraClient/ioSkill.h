#pragma once

#include "ioComplexStringPrinter.h"
#include "ScreenBlindSetting.h"
#include "EncryptValue.h"

#include "WeaponAttribute.h"
#include "SkillTypeID.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class ioWeapon;
class SP2Packet;
class ioPlayStage;
class ioGrowthUpInfo;

class ioSkill
{
public:
	enum ObjEnableType
	{
		OET_NONE,
		OET_TYPE1,
	};

protected:
	ioHashString m_GroupName;
	ioHashString m_TitleName;

	ioHashString m_Name;
	ioHashString m_DescName;
	ioHashString m_DescFullName;
	ioHashString m_Desc;
	ioHashString m_DescBubble;

	SkillUseType m_SkillUseType;

	DWORD m_dwWeaponBaseIndex;
	DWORD m_dwDescBubbleDuration;

	// camera
	ioHashString m_CameraBuff;

	// Normal
	ioHashString m_SkillMotionName;
	float m_fAnimationTimeRate;

	int  m_ProtectCancelType;
	CEncrypt<bool> m_bDisableSkillProtect;
	bool m_bPreDelayProtect;
	ioHashString m_PreDelayAnimation;
	DWORD m_dwPreDelayTime;
	DWORD m_dwAnimationEndDelay;

	ioHashString m_PreDelayOwnerEffect;
	ioHashString m_PreDelayOtherEffect;

	// Down
	ioHashString m_DownSkillMotionName;
	float m_fDownAnimationTimeRate;

	bool m_bDownPreDelayProtect;
	ioHashString m_DownPreDelayAnimation;
	DWORD m_dwDownPreDelayTime;

	ioHashString m_DownPreDelayOwnerEffect;
	ioHashString m_DownPreDelayOtherEffect;

	// Jump
	ioHashString m_JumpSkillMotionName;
	float m_fJumpAnimationTimeRate;

	bool m_bJumpPreDelayProtect;
	ioHashString m_JumpPreDelayAnimation;
	DWORD m_dwJumpPreDelayTime;

	ioHashString m_JumpPreDelayOwnerEffect;
	ioHashString m_JumpPreDelayOtherEffect;

	ioHashString m_PreDelayEffect;
	ioHashString m_SkillEffectName;
	ioHashString m_IconName;
	ioHashString m_SkillTextImage;
	ioHashString m_SkillEndEffectName;
	ioHashString m_SkillSoundName;
	UniqueObjID m_SkillSoundID;

	ioHashString m_SkillMapEffectName;
	UniqueObjID m_dwSkillMapEffect;

	DWORD m_dwPreDelayStartTime;
	DWORD m_dwFireMotionEndTime;

	ScreenBlindTime m_ScreenBlindTime;
	
	bool m_bUseCustomBlind;
	ScreenBlindSetting m_SkillBlind;

	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fFireDecreaseGauge;

	DWORD m_dwCameraEvent;

	ioItem *m_pOwnerItem;
	CEncrypt<DWORD> m_dwMotionStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwEnableReserveTime;
	
	bool m_bUseAutoTargetValue;
	bool m_bAutoTargetDownState;
	float m_fAutoTargetMinAngle;
	float m_fAutoTargetMaxAngle;
	float m_fAutoTargetRange;

	CEncrypt<int> m_iSkillType;
	CEncrypt<int> m_iSubSkillType;

	bool m_bReduceGauge;
	SkillCharColType m_EnableCharCollisionSkip;

	DWORD m_dwSwitchSkill;
	DWORD m_dwEnableSwitchTime;

	DWORD m_dwSemiSwitchSkill;
	DWORD m_dwEnableSemiSwitchTime;

	// For Growth
	float m_fExtraAniRate;

	// For JumpingSkill
	CEncrypt<bool> m_bUseOnlyJumpping;
	CEncrypt<bool> m_bEnableJumpingSkill;
	CEncrypt<bool> m_bEnableJumpingSkillOnBlowWound;
	CEncrypt<float> m_fEnableJumpingSkillMaxHeight;

	// For DownSkill
	CEncrypt<bool> m_bUseOnlyDown;
	CEncrypt<bool> m_bEnableDownSkill;

	// For BlowWoundSkill
	CEncrypt<bool> m_bEnableBlowWoundSkill;
	CEncrypt<bool> m_bNoCheckDropSpeed;
	CEncrypt<float> m_fEnableBlowWoundSkillHeight;
	CEncrypt<float> m_fEnableBlowWoundSkillMaxHeight;

	// For Skill End Next Jump
	float m_fSkillEndNextJumpAmt;
	float m_fSkillEndNextLandRate;

	// For Wound Skill
	CEncrypt<bool> m_bEnableWoundSkill;

	// For IceState Skill
	CEncrypt<bool> m_bEnableIceStateSkill;

	// for flamestate(buff) skill
	CEncrypt<bool> m_bEnableIFlameStateBuff;
	ioHashStringVec m_bEnableFlameStateBuffList;

	CEncrypt<bool> m_bInitJumpPower;
	CEncrypt<bool> m_bCircleNoChangeY;

	// For SKill Move
	CEncrypt<float> m_fSkillMoveRate;

	bool m_bNoChangeFallState;
	bool m_bHideEquipItemByRotationWeapon;

	// For Gravity
	CEncrypt<float> m_fSkillGravityRate;
	CEncrypt<float> m_fSkillGravityDownRate;
	CEncrypt<float> m_fSkillGravityJumpRate;

	// For Ani Rotate
	vAniRotateInfoList m_vAniRotateInfoList;
	
	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

	ioHashString m_OwnerSkillStartBuff;
	ioHashString m_OwnerSkillEndBuff;
	ioHashString m_AutoStartBuff;

	// ZeroHP, Boss, King
	CEncrypt<bool> m_bDisableZeroHPSkill;
	CEncrypt<bool> m_bDisableBossSkill;
	CEncrypt<bool> m_bDisableKingSkill;
	CEncrypt<bool> m_bDisableFootballSkill;
	CEncrypt<bool> m_bDisableGangSiModeSkill;
	CEncrypt<int> m_iDisableWeaponTypeSkill;

	// limit hp rate
	CEncrypt<bool> m_bUseLimitHP;
	CEncrypt<float> m_fLimitHPRate;

	// Grappling
	GrapplingSkillWoundedInfo	m_SkillGrapplingInfo;
	bool						m_bChangeGrapplingTargetPos;

	// Hide skill desc
	CEncrypt<bool> m_bHideSkillDesc;
	CEncrypt<float> m_fPassiveSkillGaugeRate;

	// 다시 재점핑 ( 총 횟수 ) 2020-05-09
	int m_iSkywalkerJumpCount;
	int m_iCurSkywalkerJumpCount;

protected:
	CEncrypt<float> m_fCurChargeRate;

	CEncrypt<float> m_fMaxDamageRate;
	CEncrypt<float> m_fMaxForceRate;
	CEncrypt<float> m_fMaxBlowRate;

	CEncrypt<float> m_fMaxRangeRate;
	CEncrypt<float> m_fMaxSpeedRate;
	CEncrypt<float> m_fMaxChargeAniRate;
	CEncrypt<float> m_fMaxForceSlidingRate;

	CEncrypt<float> m_fMaxTargetRangeRate;
	CEncrypt<float> m_fMaxTargetAngleRate;

	CEncrypt<float> m_fMaxFloatRate;

	CEncrypt<float> m_fMaxScaleRate;
	CEncrypt<float> m_fMaxExplosionRate;

	CEncrypt<float> m_fMaxLiveTimeRate;
	CEncrypt<float> m_fMaxWoundedTimeRate;

	CEncrypt<float> m_fMaxBuffLiveTimeRate;

	//윤태철
	ObjEnableType   m_nEnableObjitemSkillType;

	BuffInfoList	m_OwnerPassiveBuffList;

// For Air Jump
protected:
	float m_fAirJumpPower;
	float m_fAirForcePower;
	float m_fAirJumpPowerRate;

	std::list<float> m_vAirJumpTimeList;
	int m_iCurAirJump;

// For ExtraAniJump
	DWORD m_dwExtraAniJumpTime;
	float m_fExtraAniJumpPower;

	int m_iCurExtraAniJump;
	vExtraAniJumpList m_vExtraAniJumpList;

// For Dir Skill
	CEncrypt<bool> m_bChangeDirRot;
	CEncrypt<bool> m_bEnableDirRotNormalSkill;
	CEncrypt<bool> m_bEnableDirRotJumpSkill;
	CEncrypt<bool> m_bEnableDirRotDownSkill;

// For Speed Skill
	CEncrypt<bool> m_bInitMoveSpeedNormalSkill;
	CEncrypt<bool> m_bInitMoveSpeedJumpSkill;
	CEncrypt<bool> m_bInitMoveSpeedDownSkill;

	DefenseBreakType m_eDefenseSkillBreakType;

// For Skill Cancel Skill
	CEncrypt<bool> m_bEnableCancelBySkill;
	CEncrypt<DWORD> m_dwEnableCancelBySkillTime;
	CEncrypt<DWORD> m_dwSkillStartTime;

// For ActiveCount
	CEncrypt<int>  m_iMaxActiveCount;
	int            m_iCurActiveCount;
protected:
	ioHashString m_HandMesh;
	bool m_bUseItemMesh;
	bool m_bVisibleEquipMesh;

	bool m_bSetHandMesh;
protected:
	ioHashStringVec	m_ExBuffList;
	CEncrypt<bool> m_bExBuffHPZero;

protected:
	bool m_bPetForceHide;

protected:
	bool m_bEnableThunderBirdHoldUseSkill;

	bool m_bNoCheckFallState;
	bool m_bSwitchSkillEndJump;
	bool m_bNoCheckSkillmotion;

	int m_iAccessoryRandomNum;

	bool m_bAutomaticBuffOn;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
	virtual int GetTypeID() const;

	void SetGroupName( const ioHashString& szGroupName );
	void SetTitleName( const ioHashString& szTitleName );

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanSwitchSkill() const;
	virtual bool IsCanSemiSwitchSkill() const;

	virtual bool IsCurSkywalkerJumpCount() const;	// 2020-05-09

	virtual bool IsCanCancelBySkill( ioBaseChar *pOwner, OUT int &iSkillNum ) const;

	virtual bool IsCanUseZeroHP( ioBaseChar *pChar ) const;
	virtual bool IsCanUseSkillWithCrown( ioBaseChar *pChar );
	virtual bool IsCanUseBoss( ioBaseChar *pChar ) const;
	virtual bool IsCanUseFootball() const;
	virtual bool IsCanUseGangSiMode() const;
	virtual bool IsCanUseByWeaponType( ioBaseChar *pChar ) const;
	virtual bool IsCanUseThunderBirdHold( ioBaseChar *pChar ) const;

	virtual bool IsCanUseLimitHP( ioBaseChar *pChar ) const;
	
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsOnOffEnable();

	virtual bool IsCanDefenseStateSkill();
	virtual void SetDefenseStateSkill( ioBaseChar *pChar );
	virtual DefenseBreakType GetDefenseSkillBreakeType() const { return m_eDefenseSkillBreakType; }

	virtual bool HasBuff( int iBuffType ) const;
	virtual int	 GetBuffType( int iBuffType = 0 );
	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual SkillCharColType IsCharSkipState() const;
	virtual bool IsEntityCollisionSkipState() const;
	virtual float GetSkillEndNextJumpAmt() const { return m_fSkillEndNextJumpAmt; }
	virtual float GetSkillEndNextLandRate() const { return m_fSkillEndNextLandRate; }
	virtual SkillEndJumpType GetSkillEndNextJumpMustEnable(){ return SEJT_DEFAULT; }
	virtual bool IsEndState() const;
	virtual bool IsAttackEndState() const;

	virtual bool IsJumpState() const;
	virtual bool IsDownState() const;

	virtual bool IsNoDropState() const;
	virtual bool IsCanSendControl() const;

	virtual bool IsSkipState() const;

	virtual float GetSkillMoveRate() const;

	// grappling
	virtual bool SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pAttacker, ioBaseChar *pTarget );
	virtual bool EnableChangeGrapplingTargetPos() const { return m_bChangeGrapplingTargetPos; }

	virtual GrapplingSkillWoundedInfo GetSkillWoundGrapplingInfo();

	// etc
	virtual void SetCreateWeaponDependency( ioBaseChar *pOwner, ioPlayStage *pStage );

	virtual void SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon = NULL );
	
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEndByWounded( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual void OnProcessNotUseSkill( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillItemRelease( ioBaseChar *pChar );
	
public:
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyEquipSlotSkillExtraInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyExtraPassiveSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );
	virtual bool IsUseAutoTargetValue();
	virtual bool IsAutoTargetDownState();

	virtual bool CheckUseBuff( ioBaseChar *pChar );
	virtual bool CheckUseItemMesh();

	virtual bool CheckUseActiveCount();

	virtual void CheckAniRotate( ioBaseChar *pChar );

	virtual bool IsEnableDashCheck( ioBaseChar *pChar );

	virtual bool IsEnableMultiActive( ioBaseChar *pChar );
	virtual bool IsInvisibleBottomCircle( ioBaseChar *pChar ) { return false; }

	virtual bool IsCheckEnableSkill( int iSlot, ioBaseChar *pChar ) { return true;	}	// 스킬 사용 여부 2020-11-13

public:
	inline void SetOwnerItem( ioItem *pItem ) { m_pOwnerItem = pItem; }
	inline ioItem *GetOwnerItem() const { return m_pOwnerItem; }
	virtual void SetSpecialValue(){ m_iCurActiveCount = m_iMaxActiveCount; }
	inline int GetSubSkillType() const { return m_iSubSkillType; }
	virtual void SetOwnerAutomaticBuff( ioBaseChar *pOwner, ioPlayStage* pStage );

	// 배틀모드 전장으로 진입할때 CurActiveCount 0으로 초기화
	void ResetCurActiveCount(){ m_iCurActiveCount = 0; }
public:
	void SetMotionTime( DWORD dwStartTime, DWORD dwEndTime );
	void SetKeyReserveTime( DWORD dwEnableTime );
	
	virtual void GetAutoTargetValue( float &fRange, float &fMinAngle, float &fMaxAngle );

	bool CheckGaugeMax( const ioHashString &szName );

public:
	float GetNeedGauge( const ioHashString &szName );
	float GetFireNeedGauge( const ioHashString &szName );
	const ScreenBlindSetting* GetScreenBlindSetting() const;

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetDescName() const { return m_DescName; }
	inline const ioHashString& GetDescFullName() const { return m_DescFullName; }
	inline const ioHashString& GetDesc() const { return m_Desc; }
	inline const ioHashString& GetSkillEffectName() const { return m_SkillEffectName; }
	inline const ioHashString& GetIconName() const { return m_IconName; }
	inline const ioHashString& GetSkillTextImage() const { return m_SkillTextImage; }

	inline const ioHashString& GetOwnerSkillStartBuff() const { return m_OwnerSkillStartBuff; }
	inline const ioHashString& GetOwnerSkillEndBuff() const { return m_OwnerSkillEndBuff; }
	inline const ioHashString& GetAutoStartBuff() const { return m_AutoStartBuff; }
	
	inline DWORD GetSkillMotionTime() const { return m_dwMotionEndTime - m_dwMotionStartTime; }

	inline DWORD GetCameraEventCode() const { return m_dwCameraEvent; }
	inline const ScreenBlindTime& GetScreenBlindTime() const { return m_ScreenBlindTime; }

	virtual bool IsCanJumpingSkill() const { return m_bEnableJumpingSkill; }
	virtual bool IsCanJumpingSkillOnBlowWound() const { return m_bEnableJumpingSkillOnBlowWound; }
	inline bool IsOnlyJumppingSkill() const { return m_bUseOnlyJumpping; }

	inline float GetJumpingSkillEnableMaxHeight() const { return m_fEnableJumpingSkillMaxHeight; }

	virtual bool IsCanDownSkill() const { return m_bEnableDownSkill; }
	inline bool IsOnlyDownSkill() const { return m_bUseOnlyDown; }

	virtual bool IsCanBlowWoundSkill() const { return m_bEnableBlowWoundSkill; }

	inline bool IsNoCheckDropSpeed() const { return m_bNoCheckDropSpeed; }
	inline float GetBlowWoundSkillEnableHeight() const { return m_fEnableBlowWoundSkillHeight; }
	inline float GetBlowWoundSkillEnableMaxHeight() const { return m_fEnableBlowWoundSkillMaxHeight; }

	virtual bool IsCanWoundSkill() const { return m_bEnableWoundSkill; }
	inline bool IsCanIceStateSkill() const { return m_bEnableIceStateSkill; }
	inline bool isCanFlameStateBuff() const { return m_bEnableIFlameStateBuff; }

	inline bool IsNoChangeFallState() const { return m_bNoChangeFallState; }
	inline bool IsHideEquipItemByRotationWeapon() const { return m_bHideEquipItemByRotationWeapon; }

	inline void SetSkillUseType( SkillUseType eType ) { m_SkillUseType = eType; }
	inline SkillUseType GetSkillUseType() const { return m_SkillUseType; }

	inline bool IsChangeDirRot() const { return m_bChangeDirRot; }

	bool NoCheckFallState() const{ return m_bNoCheckFallState; }

public:
	virtual float GetSkillGravityRate();

	virtual const ioHashString& GetSkillMotionName() const;
	virtual bool HasSkillMotion() const;

	virtual float GetSkillMotionRate() const;
	virtual bool IsPreDelayProtect() const;

	virtual const ioHashString& GetPreDelayAniName() const;
	virtual DWORD GetPreDelayTime() const;

	virtual const ioHashString& GetPreDelayOwnerEffect() const;
	virtual const ioHashString& GetPreDelayOtherEffect() const;

	virtual void SetCancelSkill( ioBaseChar *pChar );

public:
	int GetGrowthLevel( const ioHashString &szName );
	int GetGrowthExtraLevel();
	ioGrowthUpInfo* GetGrowthUpInfoByType( int iType, const ioHashString &szName );

protected:
	void SetExtraAniRate( float fRate );

	void InitExtraAniJump();
	void SetExtraAniJump( ioBaseChar *pChar, int iAniID, float fTimeRate );
	void CheckExtraAniJump( ioBaseChar *pChar );

	virtual void ClearAirJump();
	virtual void SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	virtual void CheckAirJump( ioBaseChar *pChar );

protected:
	void CheckSkillDesc( ioBaseChar *pOwner );
	bool CheckNeedGauge( ioBaseChar *pOwner, bool bHelpMsg );

	void SetSkillNameChatBubble( ioBaseChar *pOwner );

	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );

public:
	void CheckChangeDirRot( ioBaseChar *pOwner );
	void CheckInitMoveSpeed( ioBaseChar *pOwner );

public:
	virtual bool UseActiveCount( ioBaseChar *pOwner );
	
	virtual int GetMaxActiveCount( ioBaseChar *pOwner );
	virtual int GetCurActiveCount( ioBaseChar *pOwner );

	virtual bool HideHPGauge();

public:
	virtual void SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock );
	virtual void SetCollisionByApplySkill();

	virtual void SetChargingRateForWeapon( ioWeapon *pWeapon );

public:
	inline float GetBaseNeedGauge() const { return m_fNeedGauge; }
	inline float GetBaseFireDecreaseGauge() const { return m_fFireDecreaseGauge; }
	void SetAccessoryRandomNum( int iNum ) { m_iAccessoryRandomNum = iNum; }
	int GetAccessoryRandomNum() const { return m_iAccessoryRandomNum; }

public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );
	virtual bool IsFixedExceptionCamera();
	inline ioHashStringVec GetExBuffList() const { return m_ExBuffList; }
	inline bool EnableExBuffOnHPZero(){ return m_bExBuffHPZero; }

public:
	ioHashStringVec& GetEnableFlameStateBuffList();

public:
	float GetIncSkillGaugeRate();
	int GetObjEnableSkillType();

public:
	bool IsCanSwitchSkillEndJump() { return m_bSwitchSkillEndJump; }

public:
	virtual bool IsUseActiveCnt() const { return false; }
	virtual int GetMaxActiveCnt( ioBaseChar *pChar ) const { return 0; }
	virtual int GetCurActiveCnt( ioBaseChar *pChar ) const { return 0; }

public:
	ioSkill();
	ioSkill( const ioSkill &rhs );
	virtual ~ioSkill();
};