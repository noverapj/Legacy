#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioHadesItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
	};

	enum HadesAttackState
	{
		MFS_NONE,
		MFS_START,
		MFS_HALF_STATE,
		MFS_FULL_STATE,
		MFS_ATTACK_HITED,
		MFS_ATTACK_ADD,
		MFS_ATTACK_LOOP,
		MFS_ATTACK_MOVE,
		MFS_ATTACK_MOVE_AUTO,
		MFS_ATTACK_END,
		MFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
		SST_ENDJUMP,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwGatheringStartTime;
	int m_iCurCombo;
	int m_nAttackCnt;

	ioHashString m_stGatheringAni;
	float		 m_fGatheringAniRate;

	bool		m_bMoveIsDone;


protected:
	HadesAttackState m_HadesAttackState;

	CEncrypt<float> m_fFlyCurRange;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	HadesAttackInfo m_AttackInfo;
	HadesAttackInfo m_HalfAttackInfo;
	HadesAttackInfo m_CurAttackInfo;

	//점프쪽
	HadesAttackInfo		m_JumpHalfChargeInfo;
	HadesAttackInfo		m_JumpFullChargeInfo;

	HadesAttackInfo		m_DashHalfChargeInfo;
	HadesAttackInfo		m_DashFullChargeInfo;

	DWORD m_dwFireMotionTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwWeaponIndex;

	D3DXVECTOR3 m_vFlyMoveDir;

	//
	ioHashString m_Target;

	DWORD m_dwHitedTime;
	DWORD m_dwAddAttackTime;
	DWORD m_dwAttackWaitLoopEndTime;

	bool m_bAirAddAttckType;
	float m_fAirAttackTypeCheckHeight;

	//A키 이동관련
	//float			m_fMaxMoveRange;
	float			m_fCurMaxMoveRange;
	//float			m_fMoveSpeed;
	//AttackAttribute m_MoveAttackInfo;
	float			m_fCurMoveRange;
	float			m_fNeedMoveRange;
	D3DXVECTOR3		m_vMoveAttackDir;
	bool			m_bAutoMove;

	ioHashString	m_stLockControlBuff;

	bool			m_bCurMaxMoveSeted;
	D3DXVECTOR3		m_vMoveAttackStartPos;

	float			m_fGoalOffSetLength;

protected:
	bool m_bInitGaugeDown;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

	float			m_fCheckWeaponLength;

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

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadHadesInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void ChangeGatheringState( ioBaseChar *pOwner );

	void SetAirFloatStateChange( ioBaseChar *pOwner );

	bool Process_A_AttackMove( ioBaseChar *pOwner );
	void WeaponCheck( ioBaseChar *pOwner );

	void SetMoveAttackEndJump( ioBaseChar *pOwner );

	bool CheckWeaponLength( ioBaseChar *pOwner );

public:
	void SetHadesAttackState( ioBaseChar *pOwner, const HadesAttackInfo &rkFlyInfo, bool bAuto, bool bSendNet, bool bDash = false );
	void SetHadesHalfAttackState( ioBaseChar *pOwner, const HadesAttackInfo &rkFlyInfo, bool bAuto, bool bSendNet, bool bJump = false, bool bJumpHalf = false, bool bDash = false );
	//void SetHadesStateByDash( ioBaseChar *pOwner, const HadesAttackInfo &rkFlyInfo );

	void ProcessHadesState( ioBaseChar *pOwner );

	bool IsFlyCharCollisionSkipState();

	inline const int GetFlyState() const { return (int)m_HadesAttackState; }
	inline const D3DXVECTOR3& GetFlyMoveDir() const { return m_vFlyMoveDir; }

protected:
	void CheckHadesState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pOwner );

	void ProcessLookRotate( ioBaseChar *pOwner );

	void ChangeAttackStart( ioBaseChar *pOwner );
	void ChangeAttackHited( ioBaseChar *pOwner );
	void ChangeAttackAdd( ioBaseChar *pOwner );
	void ChangeAttackMove( ioBaseChar *pOwner );

	void ProcessWeaponLine( ioBaseChar *pOwner );

	void ChangeTrackingUD_Fire( ioBaseChar *pOwner );
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

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

	int   m_nMaxAddAttackCnt;
	int	  m_nCurAddAttackCnt;

protected:
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeAttackLoopState( ioBaseChar *pOwner );
	void SetChangeAttackEndState( ioBaseChar *pOwner );
	void CheckAttackHited( ioBaseChar *pOwner );

	void CheckDummyCreate( ioBaseChar *pOwner );
	void BuffStateChange( ioBaseChar *pCreator, ioBaseChar *pTarget );
	void TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp = "", const ioHashString& stCenter = "", const ioHashString& stDown = "" );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );

	void CheckDecreaseGauge();

	//
	void CheckEnableGauge( ioBaseChar *pChar );

	bool IsEnableGauge();
	bool IsHadesState();

	void ClearAttackState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	bool IsLockState( ioBaseChar *pCreator, ioBaseChar *pTarget );

public:
	bool IsBallTarget();

	bool SetTarget( const ioHashString &szTargetName, ioBaseChar* pOwner );
	void RemoveWeapon( ioBaseChar *pOwner );

	ioUserKeyInput::DirKeyInput GetCurKey();

public:
	ioHadesItem();
	ioHadesItem( const ioHadesItem &rhs );
	virtual ~ioHadesItem();
};

inline ioHadesItem* ToHadesItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HADES_ITEM )
		return NULL;

	return dynamic_cast< ioHadesItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioHadesSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HADES_ATTACK"; }
	virtual const CharState GetState(){ return CS_HADES_ATTACK; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	
public:
	ioHadesSpecialState();
	virtual ~ioHadesSpecialState();
};

inline ioHadesSpecialState* ToHadesSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HADES_ATTACK )
		return NULL;

	return dynamic_cast< ioHadesSpecialState* >( pState );
}