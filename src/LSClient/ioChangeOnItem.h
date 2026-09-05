#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;
class ioDruidEagleDummy;

class ioChangeOnItem : public ioWeaponItem
{
public:
	enum WerewolfChangeState
	{
		WSC_DELAY,
		WSC_DASH,
		WSC_GRIFFIN,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
	};
	enum SpecialFlyState
	{
		SFS_TARGET_FIND,
		SFS_TARGET_ATTACK_PRE,
		SFS_TARGET_ATTACK,
		SFS_UP,
		SFS_GRAP_DELAY,
		SFS_GRAP_CHARGING,
		SFS_GRAP_ATTACK,
		SFS_END,
	};
	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_COLL,	//지형과 충돌
		MS_LANDING,	//바닥과 충돌
		MS_END,		//이동 완료
	};
	enum AttackKeyType
	{
		AKT_NONE,	
		AKT_PRESS,
	};
	enum UpDownKey
	{
		UDK_NONE,
		UDK_UP,
		UDK_DOWN,
	};	
	enum SyncState
	{
		SS_TARGET_FIND,
		SS_TARGET_ATTACK_PRE,
		SS_TARGET_ATTACK,
		SS_UPDATE_TARGET,
		SS_REMOVE_TARGET,
		SS_UP,
		SS_END,
		SS_CHANGE_BEAR,
		SS_UPDOWN_KEY,
		SS_GRAP_DELAY,
		SS_GRAP_ATTACK,
	};	

	struct EagleFlyInfo
	{		
		float	m_fFlyAccelSpeed;
		float	m_fFlyMinSpeed;
		float	m_fFlyMaxSpeed;
		float	m_fMaxRange;
		DWORD	m_dwFlyMinSpeedDuration;

		EagleFlyInfo()
		{			
			m_fFlyAccelSpeed		= 0.0f;
			m_fFlyMinSpeed			= 0.0f;
			m_fFlyMaxSpeed			= 0.0f;
			m_fMaxRange				= 0.0f;

			m_dwFlyMinSpeedDuration	= 0;
		}
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	WerewolfChangeState m_ChangeState;
	int m_iCurCombo;

	CEncrypt<int> m_iMaxSpecialAttCnt;
	CEncrypt<int> m_iNeedSpecialAttCnt;
	CEncrypt<int> m_iCurSpecialAttCnt;

	CEncrypt<float> m_fCurChangeDamage;
	CEncrypt<float> m_fMaxChangeDamage;
	CEncrypt<float> m_fDecreaseChangeDamage;
	CEncrypt<float> m_fFlyDecreaseChangeDamage;
	CEncrypt<float> m_fDownDecreaseChangeDamage;
	CEncrypt<float> m_fUseGaugeDecreaseChangeDamage;
	float m_fChangeWerewolfEndJumpPower;

	ioHashString m_szReleaseWereWolfAni;
	float m_fReleaseWereWolfAniRate;
	bool m_bChangeToGriffin;

protected:
	ioHashString m_GrapplingTargetName;
	GrapplingSkillWoundedInfo m_GrapplingInfo;

	SpecialFlyState	m_SpecialFlyState;
	AttackKeyType	m_AttackKeyType;
	UpDownKey		m_UpDownKey;

	AttackAttribute m_GrapAttackAttribute;
	EagleFlyInfo	m_FindTargetEagleFlyInfo;
	EagleFlyInfo	m_TargetAttackEagleFlyInfo;
	EagleFlyInfo	m_UpFlyInfo;

	ioHashString	m_TargetName;
	float			m_fTargetMaxRange;
	float			m_fTargetMinAngle;
	float			m_fTargetMaxAngle;
	float			m_fReadyMarkerOffset;
	float			m_fUpAngle;
	float			m_fCheckHeightMin;
	float			m_fCurrFlyRange;
	float			m_fFlyEndJumpPower;

	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;

	ioUserKeyInput::DirKeyInput	m_eDirKey;
	D3DXVECTOR3		m_vMoveDir;
	D3DXVECTOR3		m_vPreDir;

	DWORD			m_dwStartTime;
	DWORD			m_dwMotionEndTime;
	DWORD			m_dwCharingStartTime;
	DWORD			m_dwTargetingTime;
	DWORD			m_dwTargetingStartTime;
	DWORD			m_dwCurrWeaponIdx;	
	DWORD			m_dwWeaponIdx;
	DWORD			m_dwFlyChargeTime;
	DWORD			m_dwGrapChargeMaxTime;

	ioHashString	m_GriffinDummyCharName;
	int				m_GriffinDummyCharIndex;

protected:
	ioHashString m_WerewolfDummyCharName;
	int m_iWerewolfDummyCharIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadTargetMarker( ioINILoader &rkLoader );
	void LoadFindFly( ioINILoader &rkLoader );
	void LoadTargetFly( ioINILoader &rkLoader );
	void LoadUpFly( ioINILoader &rkLoader );

	virtual ioItem* Clone();

	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );	

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

	bool CheckOwnerDownState( ioBaseChar *pOwner );
	bool CheckGrapplingTargetValidate( ioBaseChar *pOwner );

public:
	void ChangeToRiding( ioBaseChar *pOwner, WerewolfChangeState eState );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	float GetMaxChangeDamage()		{ return m_fMaxChangeDamage; }
	float GetCurChangeDamage()		{ return m_fCurChangeDamage; }
	float GetWereWolfEndJumpPower()		{ return m_fChangeWerewolfEndJumpPower; }
	WerewolfChangeState GetWolfChangeState()	{ return m_ChangeState; }

	void DecreaseExtraNeedGauge();
	bool IsEnableGauge();

	void ProcessFlyState( ioBaseChar *pOwner );
	void InitializeFlyState( ioBaseChar* pOwner );
	void FinalizeFlyState( ioBaseChar* pOwner );
	bool IsFlyStateEnd();
	void CheckFallStateChange( ioBaseChar* pOwner, float fBottomHeight );

public:
	void ApplyChangeWereWolfState( ioBaseChar *pChar );

	virtual bool CheckChangeWereWolfState( ioBaseChar *pChar );
	virtual void SetChangeWereWolfState( ioBaseChar *pChar, WerewolfChangeState eState );

	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );
	virtual void DecreaseChangeDamage( ioBaseChar *pOwner );

	virtual bool CheckReleaseWereWolfState( ioBaseChar *pChar );

	virtual void ReleseWereWolfState( ioBaseChar *pChar );
	virtual void ReleseWereWolfStateByOwner( ioBaseChar *pChar );
	void ReleseWereWolfStateByGauge( ioBaseChar *pChar );

	virtual bool IsEnableActionStopDelay();

	bool IsRidingState();
	bool IsGriffinState();

	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );
	void ChangeToWereWolfState( ioBaseChar *pChar, WerewolfChangeState eState );
	void SetChangeToGriffinState( bool bChange );

	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	bool CreateWerewolfDummy( ioBaseChar *pOwner, int iIndex, WerewolfChangeState eState );
	void SetWerewolfDummyCharIndex( ioBaseChar *pOwner, int iCurIndex );

protected:
	void OnFindTarget( ioBaseChar* pOwner );
	void OnTargetAttackPre( ioBaseChar* pOwner );
	void OnTargetAttack( ioBaseChar* pOwner );
	void OnUp( ioBaseChar* pOwner );

	void ProcessKeyCheck( ioBaseChar *pOwner, bool bSync = true );
	int ProcessFlyMove( ioBaseChar *pOwner, const EagleFlyInfo& FlyInfo, DWORD dwStartTime );

	bool CheckItemGauge( ioBaseChar* pOwner );

	void UpdateTarget( ioBaseChar* pOwner );
	void ChangeTarget( ioBaseChar* pOwner, ioBaseChar* pTarget );
	void RemoveTarget( ioBaseChar* pOwner );

	void ChangeToTargetAttackPre( ioBaseChar* pOwner, ioPlayStage* pStage, const ioHashString& szTargetName, ioUserKeyInput::DirKeyInput eKey );
	void ChangeToTargetAttack( ioBaseChar* pOwner );
	void ChangeToEnd( ioBaseChar* pOwner );
	void ChangeToObjectEquip( ioBaseChar* pOwner );
	void ChangeToUp( ioBaseChar* pOwner );		
	void ChangeToFindTarget( ioBaseChar* pOwner );	
	void ChangeFlyGrapDelay( ioBaseChar *pOwner );
	void ChangeFlyGrapAttack( ioBaseChar *pOwner );

	void ClearWeapon( ioBaseChar* pOwner );

	void CreateGriffinDummy( ioBaseChar* pOwner );
	void DestroyGriffinDummy( ioBaseChar* pOwner );
	ioDruidEagleDummy* GetGriffinDummy( ioBaseChar* pOwner );	

public:
	ioChangeOnItem();
	ioChangeOnItem( const ioChangeOnItem &rhs );
	virtual ~ioChangeOnItem();
};

inline ioChangeOnItem* ToChangeOnItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHANGE_ON_ITEM )
		return NULL;

	return dynamic_cast< ioChangeOnItem* >( pItem );
}

class ioChangeOnGriffinFlyState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
	
public:
	virtual const char* GetStateName(){ return "CS_CHANGE_ON_GRIFFIN_FLY"; }
	virtual const CharState GetState(){ return CS_CHANGE_ON_GRIFFIN_FLY; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsStateEnd( ioBaseChar* pOwner );

public:
	// 중간 입장 발생시 동기화 선택
	virtual bool IsJoinSync(){ return true; }

public:
	ioChangeOnGriffinFlyState();
	virtual ~ioChangeOnGriffinFlyState();
};

inline ioChangeOnGriffinFlyState* ToChangeOnGriffinFlyState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CHANGE_ON_GRIFFIN_FLY )
		return NULL;

	return dynamic_cast< ioChangeOnGriffinFlyState* >( pState );
}