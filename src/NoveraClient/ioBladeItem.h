#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioBladeItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum BladeFlyState
	{
		LFS_NONE,
		LFS_START,
		LFS_DELAY,
		LFS_FLY_D_ATTACK,
		LFS_FLY_D_CHARGING,
		LFS_FLY_S_ATTACK,
		LFS_READY_JUMP,
		LFS_JUMP,
		LFS_LANDING,
		LFS_END,
	};

	//LFS_FLY_ATTACK상황 안의 상황
	enum BladeFlyAttackState
	{
		LFAS_NONE,
		LFAS_DELAY,
		LFAS_ATTACK,
		LFAS_ATTACKED,
	};

	enum SyncSubType
	{
		SST_DELAY,
		SST_FLY_STATE,
		SST_ROTATE,//플라이 상태일때 상승 하강, 회전등을 처리할때
		SST_FLY_A_ATTACK,
		SST_FLY_S_ATTACK,
		SST_FLY_D_ATTACK,
		SST_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	BladeFlyState m_FlyState;
	BladeFlyAttackState m_FlyAttackState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// move
	DWORD m_dwFlyStartTime;
	DWORD m_dwFlyMoveTime;
	DWORD m_dwFlyAttackMoveStartTime;
	DWORD m_dwFlyAttackMoveEndTime;
	DWORD m_dwFlyAttackEndTime;
	DWORD m_dwFlyAttackTime;

	CEncrypt<float> m_fFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	BladeFlyInfo m_AttackFly;
	BladeFlyInfo m_CurFly;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

	AttackAttribute m_StartAttribute;
	AttackAttribute m_A_AttackAttribute;
	AttackAttribute m_A_AttackAttributeDisableJump;
	AttackAttribute m_D_AttackAttribute;
	AttackAttribute m_S_AttackAttribute;

	//잡기 성공이후 정보들(점프 ,중력, 랜드어택, 애니)
	float m_f_S_Attack_Move_Angle;
	float m_f_S_Attack_Move_Angle_Max_Range;
	float m_f_Cur_S_Attack_Move_Range;
	
	DWORD m_dwMotionStartTime;
	DWORD m_dwPreDelayStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwSpecialStartTime;

	float m_fJumpRegHeight;
	float m_fFlyJumpAmt;
	float m_fFlyEndJumpAmt;
	float m_fFlyJumpLookForce;

	DWORD m_dwMaxLifeTime;

	ioHashString m_stLandingAni;
	float		 m_fLandingAniRate;

	ioHashString m_DelayAni;
	float m_fDelayAniRate;
	DWORD m_dwRotateTime;
	float m_fMoveSpeed;
	float m_f_S_AttackMoveSpeed;
	float m_f_A_AttackMoveSpeed;

	ioHashString m_stBoardEffectName;
	DWORD			m_dwEffectIndex;

	DWORD	m_dwD_Key_StartTime;
	DWORD   m_dwD_Key_MaxTime;

	//한화면에 해당 웨폰이 해당 숫자 이상 존재 못하게
	int     m_n_D_Attack_Max_Weapon_Cnt;
	int		m_n_D_Attack_Type;

	DWORD	m_dwEnableJumpTime;
	DWORD	m_dwAttackKeyReservTime;
	DWORD   m_dwMaxDropZoneWaitTime;
	DWORD   m_dwCurDropZoneStartTime;

	float   m_f_S_Min_Land_Height;
	float   m_f_JumpMaxHeight;
	bool    m_bEnableMaxJump;

protected:
	ioHashString	m_szRidingDummyCharName;
	int				m_iRidingDummyCharIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	bool IsEnableMaxJump();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

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
	bool CheckAttackKey( ioBaseChar *pOwner );
	bool Check_D_Attack_Max_Weapon_Cnt( ioBaseChar *pOwner );

	bool IsDropZone( ioBaseChar *pOwner );

public:
	void SetFlyState( ioBaseChar *pOwner, const BladeFlyInfo &rkFlyInfo, bool bSendNet );
	BladeFlyState GetFlyState() { return m_FlyState; }
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFly_D_Attack( ioBaseChar *pOwner );
	void ChangeFly_S_Attack( ioBaseChar *pOwner );
	void ChangeFly_Jump_State( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );
	void ChangeFlyLanding( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();

	inline const int GetFlyState() const { return (int)m_FlyState; }

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool CheckLanding( ioBaseChar *pChar );

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
	bool IsBladeState();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	void InitBladeGauge();

protected:
	void CreateDummy( ioBaseChar* pOwner );
public:
	void DestroyDummy( ioBaseChar* pOwner );

public:
	ioBladeItem();
	ioBladeItem( const ioBladeItem &rhs );
	virtual ~ioBladeItem();
};

inline ioBladeItem* ToBladeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BLADE_ITEM )
		return NULL;

	return dynamic_cast< ioBladeItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioBladeFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_BLADE_FLY"; }
	virtual const CharState GetState(){ return CS_BLADE_FLY; }	
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
	ioBladeFlySpecialState();
	virtual ~ioBladeFlySpecialState();
};

inline ioBladeFlySpecialState* ToBladeFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_BLADE_FLY )
		return NULL;

	return dynamic_cast< ioBladeFlySpecialState* >( pState );
}