#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSantaItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};
	enum FlyState
	{
		LFS_NONE,
		LFS_PRE_START,
		LFS_START,
		LFS_DELAY,
		LFS_CHARGING,
		LFS_ATTACK,
		LFS_GRAP_DELAY,
		LFS_GRAP_CHARGING,
		LFS_GRAP_ATTACK,
		LFS_GRAP_DASH,
		LFS_GRAP_PUSH,
		LFS_END,
	};
	enum FlyAttackState
	{
		LFAS_NONE,
		LFAS_DELAY,
		LFAS_ATTACK,
		LFAS_ATTACKED,
	};
	enum SyncSubType
	{
		SST_DELAY,
		SST_FLY_PRESTATE,
		SST_ROTATE,
		SST_MARKER,
		SST_ATTACK_START,
		SST_ATTACK_GRAP,
		SST_GRAP_DELAY,
		SST_GRAP_ATTACK,
		SST_GRAP_DASH,
		SST_GRAP_PUSH,
		SST_GRAP_PUSH_BUFF,
		SST_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	FlyState m_FlyState;
	FlyAttackState m_FlyAttackState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<float> m_fFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	BladeFlyInfo m_AttackFly;
	BladeFlyInfo m_JumpAttackFly;
	bool m_bJump;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fFlyDecreaseGauge;
	CEncrypt<float> m_fGrapDecreaseGauge;
	CEncrypt<float> m_fHighDecreaseGauge;
	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fGrapIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

	ioHashString m_szStartDelayAni;
	ioHashString m_szJumpStartDelayAni;
	float m_fStartDelayAniRate;
	float m_fJumpStartDelayAniRate;
	AttackAttribute m_GrapAttackAttribute;
	AttackAttribute m_UroborusAttack;
	AttackAttribute m_DashAttack;

	DWORD m_dwMotionEndTime;
	DWORD m_dwFireStartTime;

	float m_fFlyEndJumpAmt;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;

	DWORD m_dwRotateTime;
	float m_fMoveSpeed;
	float m_fUpDownSpeed;

	DWORD m_dwGrapRotateTime;
	float m_fGrapMoveSpeed;
	float m_fGrapDashSpeed;
	float m_fGrapUpDownSpeed;

	ioHashString m_stBoardEffectName;
	DWORD			m_dwEffectIndex;

	DWORD	m_dwD_Key_StartTime;
	DWORD   m_dwGrapChargeMaxTime;
	DWORD   m_dwFlyChargeMaxTime;

	//한화면에 해당 웨폰이 해당 숫자 이상 존재 못하게
	int     m_iGrapAttackMax;
	int		m_iGrapAttackType;

	DWORD	m_dwAttackKeyReservTime;
	DWORD   m_dwMaxDropZoneWaitTime;
	DWORD   m_dwCurDropZoneStartTime;
	
	ioHashString m_Target;
	float m_fTargetMarkerRange;
	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetDownHeight;
	TargetWoundType m_TargetWoundType;

	ioHashString m_GrapplingTargetName;
	ioHashString m_GrapplingAniName;
	ioHashString m_GrapplingTargetRemoveBuff;
	float m_fGrapAniTimeRate;
	float m_fGrapReturnCheckLength;
	float m_fGrapReturnDirRate;
	float m_fGrapReturnPosAddY;	
	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;
	DWORD m_dwGrapWeaponIdx;
	GrapplingSkillWoundedInfo m_GrapplingInfo;

	ioHashString m_szPushMotion;
	ioHashString m_szPushWeaponBuff;
	float m_fPushMotionRate;
	DWORD m_dwPushWeapon;

	bool m_bSetUpKey;
	bool m_bSetDownKey;

protected:
	ioHashString	m_szRidingDummyCharName;
	int				m_iRidingDummyCharIndex;

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
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	D3DXVECTOR3 CalcReturnUroborusPosition(ioWeapon *pWeapon);
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	bool SetTarget( const ioHashString &szTargetName );
	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	bool CheckGrapplingTargetValidate( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void CheckCreateEffect( ioBaseChar *pChar );
	bool Check_D_Attack_Max_Weapon_Cnt( ioBaseChar *pOwner );
	void CheckFlyAttTarget( ioBaseChar *pOwner );

	bool IsDropZone( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );
	void FlyAttackFire( ioBaseChar *pOwner );
	void WaitTargetReturn( ioBaseChar *pOwner );
	void StartGrappAttack( ioBaseChar *pOwner );

public:
	void SetFlyState( ioBaseChar *pOwner, bool bJump, bool bSendNet );
	FlyState GetFlyState() { return m_FlyState; }
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyGrapDelay( ioBaseChar *pOwner );
	void ChangeFlyGrapAttack( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );
	void ChangeFlyAttack( ioBaseChar *pOwner );
	void ChangeFlyDashAttackMove( ioBaseChar *pOwner );
	void SetPushState( ioBaseChar *pOwner, bool bSendPacket );
	void SpecialAttackFire( ioBaseChar *pOwner, DWORD dwWeapon );
	void SetPushBuffState( ioBaseChar *pOwner, ioBaseChar *pWoundChar );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();
	bool IsCanDash( ioBaseChar *pOwner );

	inline const int GetFlyState() const { return (int)m_FlyState; }

protected:
	void CheckDecreaseGauge();

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	bool IsEnableGauge();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	void InitBladeGauge();

protected:
	void CreateDummy( ioBaseChar* pOwner );

public:
	void DestroyDummy( ioBaseChar* pOwner );

public:
	ioSantaItem();
	ioSantaItem( const ioSantaItem &rhs );
	virtual ~ioSantaItem();
};

inline ioSantaItem* ToSantaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SANTA_ITEM )
		return NULL;

	return dynamic_cast< ioSantaItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioSantaFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SANTA_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SANTA_SPECIAL; }	
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
	virtual bool IsNoDropState() { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioSantaFlySpecialState();
	virtual ~ioSantaFlySpecialState();
};

inline ioSantaFlySpecialState* ToSantaFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SANTA_SPECIAL )
		return NULL;

	return dynamic_cast< ioSantaFlySpecialState* >( pState );
}