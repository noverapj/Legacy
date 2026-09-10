#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectRunningAttackItem : public ioObjectItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_END,
	};

	enum RunningAttackState
	{
		RAS_NONE,
		RAS_READY,
		RAS_READY_END,
		RAS_LOOP,
		RAS_LOOP_END,
		RAS_END,
	};

	enum PacketState
	{
		PS_RUN_READY,
		PS_CREATE_WEAPON,
		PS_ROTATE,
		PS_END,
	};

protected:
	WeaponItemState m_WeaponState;
	RunningAttackState m_RunningAttackState;

	DWORD m_dwAttackStartTime;

	DWORD m_dwProtectTime;
	DWORD m_dwCheckProtectTime;
	
	bool m_bEnableObjectDefense;

	//애니메이션 설정
	//일반
	ioHashString m_szNormalRunReadyAni;
	float m_fNormalRunReadyAniRate;
	ioHashString m_szNormalRunLoopAni;
	ioHashString m_szNormalRunEndAni;
	float m_fNormalRunEndAniRate;

	//차지
	ioHashString m_szChargeRunReadyAni;
	float m_fChargeRunReadyAniRate;
	ioHashString m_szChargeRunLoopAni;
	ioHashString m_szChargeRunEndAni;
	float m_fChargeRunEndAniRate;

	//점프
	ioHashString m_szJumpRunReadyAni;
	float m_fJumpRunReadyAniRate;
	ioHashString m_szJumpRunLoopAni;
	ioHashString m_szJumpRunEndAni;
	float m_fJumpRunEndAniRate;

	//슬라이드 옵션 설정
	float m_fNormalRunSpeed;
	float m_fNormalReadyRunSpeed;
	DWORD m_dwNormalRunDuration;
	DWORD m_dwNormalRunRotateSpeed;
	int m_nNormalRunWeaponNum;

	float m_fChargeRunSpeed;
	float m_fChargeReadyRunSpeed;
	DWORD m_dwChargeRunDuration;
	DWORD m_dwChargeRunRotateSpeed;
	int m_nChargeRunWeaponNum;

	float m_fJumpRunSpeed;
	float m_fJumpReadyRunSpeed;
	DWORD m_dwJumpRunDuration;
	DWORD m_dwJumpRunRotateSpeed;
	int m_nJumpRunWeaponNum;

	float m_fCurRunSpeed;
	float m_fCurReadyRunSpeed;
	DWORD m_dwCurRunDuration;
	int m_nCurRunRotateSpeed;
	int m_nCurRunWeaponNum;

	bool m_bSetNormalAttack;
	bool m_bSetChargeAttack;
	bool m_bSetJumpAttack;

	DWORD m_dwRunReadyEndTime;
	DWORD m_dwCheckLoopTime;
	DWORD m_dwRunLoopEndTime;
	
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	ForceInfoList m_vEndForceInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual bool IsProtectState();

	virtual bool IsAutoTarget() const { return false; }

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	
public:
	void ProcessRunningAttackState( ioBaseChar *pOwner );
	bool IsCharCollisionSkipState();

protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetRunningAttackState( ioBaseChar *pOwner, float fReadySpeed, float fLoopSpeed, DWORD dwDuration, DWORD dwRotateSpeed, int nAttackType );
	void SetNormalAttackState( ioBaseChar *pOwner );

protected:
	void SetRunningReadyState( ioBaseChar *pOwner );
	void SetRunningLoopState( ioBaseChar *pOwner );
	void CrateWeapon( ioBaseChar *pOwner );
	void ProcessLoopState( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckRotate( ioBaseChar *pOwner );
	void SetRunningEndState( ioBaseChar *pOwner );
	void SetRunningAttackEndState( ioBaseChar *pOwner );

public:
	ioObjectRunningAttackItem();
	ioObjectRunningAttackItem( const ioObjectRunningAttackItem &rhs );
	virtual ~ioObjectRunningAttackItem();
};

inline ioObjectRunningAttackItem* ToObjectRunningAttackItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_RUNNING_ATTACK )
		return NULL;

	return dynamic_cast< ioObjectRunningAttackItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioObjectRunningAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_OBJECT_RUNNING_ATTACK"; }
	virtual const CharState GetState(){ return CS_OBJECT_RUNNING_ATTACK; }	
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

public:
	ioObjectRunningAttackSpecialState();
	virtual ~ioObjectRunningAttackSpecialState();
};

inline ioObjectRunningAttackSpecialState* ToObjectRunningAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_OBJECT_RUNNING_ATTACK )
		return NULL;

	return dynamic_cast< ioObjectRunningAttackSpecialState* >( pState );
}