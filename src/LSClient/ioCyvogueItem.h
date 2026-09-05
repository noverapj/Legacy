#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioNormalSkill.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioCyvogueItem : public ioWeaponItem
{
	struct CyvogueAttackAniInfo
	{
		ioHashString m_CenterAttack;
		ioHashString m_LeftAttack;
		ioHashString m_RightAttack;
		ioHashString m_FrontAttack;
		ioHashString m_BackAttack;
	};

public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum CyvogueState
	{
		CFS_NONE,
		CFS_START,
		CFS_DELAY,
		CFS_A_MODE,
		CFS_S_MODE,
		CFS_FIRE_D_MODE,
		CFS_FIRE_A_MODE,
		CFS_FIRE_S_MODE,
		CFS_FAIL_MOTION,
		CFS_END,
	};

	enum AttackAniDirection
	{
		AAD_CENTER,
		AAD_FRONT,
		AAD_BACK,
		AAD_LEFT,
		AAD_RIGHT,
	};

	enum CyvogueModeChargeState
	{
		CMC_NONE,
		CMC_A,
		CMC_S,
		CMC_D,
		CMC_CANCEL,
	};


	enum SyncSubType
	{
		SST_CHANGE_STATE,
		//SST_ROTATE,
		SST_EFFECT,
		//SST_MARKER,
		//SST_CHARGED_FIRE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	CyvogueState m_CyvogueState;
	//CyvogueState m_OldCyvogueFlyState;

	CyvogueModeChargeState m_CuvogueModeChargeState;

	DWORD m_dwModeChargeStartTime;
	DWORD m_dwModeCancleTime;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	//
	ioHashString m_CyvogueTarget;
	ioHashString m_CyvogueTargetSkill;

	//
	CyvogueFlyInfo m_AttackCyvogueInfo;
	CyvogueFlyInfo m_CurCyvogueInfo;

	// mechanics gauge
	CEncrypt<float> m_fMaxCyvogueGauge;
	CEncrypt<float> m_fNeedCyvogueGauge;

	CEncrypt<float> m_fIncreaseCyvogueGauge;
	CEncrypt<float> m_fDecreaseCyvogueGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseCyvogueGauge;

	//CEncrypt<float> m_fShotDecreaseGauge;

	CEncrypt<float> m_fCurCyvogueGauge;

	//charge in charge
	DWORD m_dwStartChargingTime;

	float m_fCurAngle;
	DWORD m_dwAimTime;

	ioHashString m_AimUp_A;
	ioHashString m_AimCenter_A;
	ioHashString m_AimDown_A;

	ioHashString m_AimUp_S;
	ioHashString m_AimCenter_S;
	ioHashString m_AimDown_S;

	ioHashString m_FailMotion;
	float		 m_fFailMotionRate;
	//////////////////////////////////////////////////////////////////////////
	DWORD m_dwChargeCancelTime;
	DWORD m_dwCharge_a_mode_time;
	DWORD m_dwCharge_s_mode_time;

	bool m_bKeyFlag;
	float m_fACircleRangeMin;
	float m_fACircleRangeMax;
	float m_fACircleMoveSpeed;
	float m_fACircleHeightGap;
	float m_fACircleOffSet;

	float m_fSCircleRangeMin;
	float m_fSCircleRangeMax;
	float m_fSCircleMoveSpeed;
	float m_fSCircleHeightGap;
	float m_fSCircleOffSet;

	D3DXVECTOR3 m_vCirclePos;

	ioHashString m_AmodeReadyCircle;
	ioHashString m_SmodeReadyCircle;

	ioHashString m_RedEffect;
	ioHashString m_BlueEffect;

	ioHashString m_stAmodeFireCenter;
	ioHashString m_stAmodeFireUp;
	ioHashString m_stAmodeFireDown;

	ioHashString m_stSmodeFireCenter;
	ioHashString m_stSmodeFireUp;
	ioHashString m_stSmodeFireDown;
	//////////////////////////////////////////////////////////////////////////
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	DWORD	m_dwMotionEndTime;

	CyvogueAttackAniInfo m_DmodeAttackAniInfo;

	float	m_fAimAniWeightRate;
	float	m_fFireAniWeightRate;

	ioHashStringVec m_vAddMeshList;
	ioHashStringVec m_vAddBuffList;
	bool m_bAddEntityAndBuff;

	//////////////////////////////////////////////////////////////////////////
	int		m_iCurFireCount;
	int		m_iRandomRange;
	float	m_fWeaponGapDistance;
	float   m_fCircleRange;
	float   m_fWeaponFloatPower;
	float   m_fWeaponFloatMinPower;
	float   m_fWeaponGravityRate;
	float   m_fRevisionRate;
	float   m_fWeaponMinSpeed;
	float	m_fUpLimitHeight;
	float	m_fUpLimitRange;
	WeaponMoveInfo m_vWeaponMoveInfo;

	float	m_fDecreaseGauge_Amode;
	float   m_fDecreaseGauge_Smode;
	float   m_fDecreaseGauge_Dmode;

	DWORD	m_dwModeStartTime;
	DWORD	m_dwMaxAModeDelayTime;
	DWORD	m_dwMaxSModeDelayTime;
	//////////////////////////////////////////////////////////////////////////
	float	m_fGravity;
	//////////////////////////////////////////////////////////////////////////
	ioHashString m_stTargetCircleA;
	ioHashString m_stTargetCircleS;

	DWORD   m_dwTargetCircleStartTime;
	DWORD	m_dwTargetCircleMaxTime;
	UniqueObjID m_dwCircleEffectID;
	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual float GetAttackSpeedRate() const;

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadCyvogueInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckCyvogueFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ReleaseBipe( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	void DecreaseNormalFireGauge();
	void DecreseChargeFireGauge();

public:
	void SetCyvogueState( ioBaseChar *pOwner, const CyvogueFlyInfo &rkFlyInfo, bool bSendNet );
	CyvogueState GetCyvogueFlyState() { return m_CyvogueState; }

	void ChangeCyvogueMoveAni( ioBaseChar *pOwner );

	void ChangeStartState( ioBaseChar *pOwner, bool bSendNet );
	void ChangeDelayState( ioBaseChar *pOwner, bool bSendNet );
	void ChangeAmodeState( ioBaseChar *pOwner );
	void ChangeSmodeState( ioBaseChar *pOwner );

	void ChangeAFireState( ioBaseChar *pOwner );
	void ChangeSFireState( ioBaseChar *pOwner );
	void ChangeDFireState( ioBaseChar *pOwner );
	void ChangeEndState( ioBaseChar *pOwner );

	void ChangeFailMotionState( ioBaseChar *pOwner );

	void ProcessCyvogueState( ioBaseChar *pOwner );

	inline const int GetCyvogueFlyState() const { return (int)m_CyvogueState; }

protected:
	void CheckCyvogueState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void ChangeTrackingUD( ioBaseChar *pOwner );
	void ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr );

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
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	float GetDecreaseShotGauge();

	void InitCyvogueGauge();
	void MaxCyvogueGauge();

	float GetCurCyvogueGauge();
	void SetCurCyvogueGauge( float fGauge );

	void CheckEnableCyvogueGauge( ioBaseChar *pChar );

	bool IsEnableCyvogueGauge();
	bool IsCyvogueState();
	void ClearCyvogueState( ioBaseChar *pOwner );

	float GetGravity();

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetCyvogueTargetSkill();
	void ClearCyvogueTargetSkill();
	bool IsEnableMoveState();

	bool IsBallTarget();

public:
	ioCyvogueItem();
	ioCyvogueItem( const ioCyvogueItem &rhs );
	virtual ~ioCyvogueItem();

private:
	bool CheckChargingMode( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	AttackAniDirection GetAttackAniDirection( ioBaseChar *pOwner );
	void AddEntityAndBuff( ioBaseChar *pOwner );
	void EraseEntityAndBuff( ioBaseChar *pOwner );

	//ÃÑ¾Ë Æ÷¹°¼± ±«Àû °è»ê
	void CheckWeaponFloatInfo( ioBaseChar *pOwner, const D3DXVECTOR3& vStartPos );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );

	void ReleaseEffect( ioBaseChar *pOwner );
	void CreateEffect( ioBaseChar *pOwner, bool bAmode,	const D3DXVECTOR3& vPosTarget );
};

inline ioCyvogueItem* ToCyvogueItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CYVOGUE )
		return NULL;

	return dynamic_cast< ioCyvogueItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioCyvogueSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_CYVOGUE_STATE"; }
	virtual const CharState GetState(){ return CS_CYVOGUE_STATE; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	
public:
	ioCyvogueSpecialState();
	virtual ~ioCyvogueSpecialState();
};

inline ioCyvogueSpecialState* ToCyvogueSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CYVOGUE_STATE )
		return NULL;

	return dynamic_cast< ioCyvogueSpecialState* >( pState );
}