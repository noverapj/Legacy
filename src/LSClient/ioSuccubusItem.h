#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioSuccubusItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum SuccubusAttackState
	{
		MFS_NONE,
		MFS_START,
		//MFS_ATTACK_WAIT,
		MFS_ATTACK_HITED,
		MFS_ATTACK_ADD,
		//MFS_ATTACK_ADD_AND_WAIT_END,
		MFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	int m_nAttackCnt;

protected:
	SuccubusAttackState m_SuccubusAttackState;

	CEncrypt<float> m_fGoomihoFlyCurRange;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	SuccubusAttackInfo m_AttackInfo;
	SuccubusAttackInfo m_CurAttackInfo;

	DWORD m_dwFireMotionTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwWeaponIndex;

	// attack move
	//DWORD m_dwGoomihoFlyAttackMoveTime;

	// grappling loop
	DWORD m_dwGoomihoFlyGrapplingLoopTime;
	DWORD m_dwGoomihoFlyGrapplingLoopDuration;
	DWORD m_dwGoomihoFlyGrapplingEnableMoveTime;

	// grappling move
	bool m_bGrapplingDirMove;
	DWORD m_dwGoomihoFlyGrapplingMoveTime;

	// end attack
	DWORD m_dwGoomihoFlyEndAttackTime;

	DWORD m_dwGoomihoFlyEndTime;

	D3DXVECTOR3 m_vFlyMoveDir;
	CEncrypt<float> m_fFlyMoveSpeed;

	//
	ioHashString m_Target;

	DWORD m_dwHitedTime;

protected:
	bool m_bInitGaugeDown;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

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
	void LoadSuccubusInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	void SetSuccubusAttackState( ioBaseChar *pOwner, const SuccubusAttackInfo &rkFlyInfo, bool bSendNet );
	void SetSuccubusStateByDash( ioBaseChar *pOwner, const SuccubusAttackInfo &rkFlyInfo );

	void ProcessSuccubusState( ioBaseChar *pOwner );

	bool IsGoomihoFlyCharCollisionSkipState();

	inline const int GetGoomihoFlyState() const { return (int)m_SuccubusAttackState; }
	inline const D3DXVECTOR3& GetGoomihoFlyMoveDir() const { return m_vFlyMoveDir; }
	inline const float GetGoomihoFlyMoveSpeed() const { return m_fFlyMoveSpeed; }

protected:
	void CheckSuccubusState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pOwner );

	void ChangeAttackStart( ioBaseChar *pOwner );
	//void ChangeAttackFire( ioBaseChar *pOwner );
	void ChangeAttackHited( ioBaseChar *pOwner );
	void ChangeAttackAdd( ioBaseChar *pOwner );

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

protected:
	void SetChangeEndState( ioBaseChar *pOwner );
	void CheckAttackHited( ioBaseChar *pOwner );

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
	bool IsSuccubusState();

	void ClearAttackState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	bool IsSuccubusLockState( ioBaseChar *pOwner );

public:
	bool IsBallTarget();

	bool SetTarget( const ioHashString &szTargetName, ioBaseChar* pOwner );
	void RemoveWeapon( ioBaseChar *pOwner );

	ioUserKeyInput::DirKeyInput GetCurKey();

public:
	ioSuccubusItem();
	ioSuccubusItem( const ioSuccubusItem &rhs );
	virtual ~ioSuccubusItem();
};

inline ioSuccubusItem* ToSuccubusItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SUCCUBUS )
		return NULL;

	return dynamic_cast< ioSuccubusItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioSuccubusSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SUCCUBUS_ATTACK"; }
	virtual const CharState GetState(){ return CS_SUCCUBUS_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

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
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	
public:
	ioSuccubusSpecialState();
	virtual ~ioSuccubusSpecialState();
};

inline ioSuccubusSpecialState* ToSuccubusSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SUCCUBUS_ATTACK )
		return NULL;

	return dynamic_cast< ioSuccubusSpecialState* >( pState );
}