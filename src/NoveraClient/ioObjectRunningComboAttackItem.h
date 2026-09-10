#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectRunningComboAttackItem : public ioObjectItem
{
friend class ioObjectRunningComboSpecialState;
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
		PS_CHARGE_END,
		PS_END,
	};
	
	typedef struct tagRunInfo
	{
		AttackAttribute m_RunReady;
		AttackAttribute m_RunEnd;

		ioHashString m_szRunLoopAni;
		float m_fRunLoopAniRate;

		float m_fJumpPower;
		float m_fReadyRunSpeed;
		float m_fRunSpeed;
		DWORD m_dwRunDuration;
		DWORD m_dwRunRotateSpeed;
		int m_nRunWeaponNum;

		void Init()
		{
			m_RunReady.Init();
			m_RunEnd.Init();

			m_szRunLoopAni.Clear();
			m_fRunLoopAniRate = FLOAT1;

			m_fJumpPower = 0.0f;
			m_fReadyRunSpeed = 0.0f;
			m_fRunSpeed = 0.0f;
			m_dwRunDuration = 0;
			m_dwRunRotateSpeed = 0;
			m_nRunWeaponNum = 0;
		}

		tagRunInfo()
		{
			Init();
		}
	}RunInfo;

	typedef std::vector<RunInfo> RunInfoList;

protected:
	DWORD m_dwProtectTime;
	bool m_bEnableObjectDefense;

	RunInfo m_NormalRunInfo;
	RunInfo m_JumpRunInfo;
	RunInfoList m_ChargeRunInfo;
	int m_nMaxChargeRunCnt;
	
	float m_fEndJumpPower;
	float m_fJumpHeightGap;

protected:
	WeaponItemState m_WeaponState;
	RunningAttackState m_RunningAttackState;

	DWORD m_dwAttackStartTime;
	DWORD m_dwCheckProtectTime;
	
	RunInfo m_CurInfo;
	
	bool m_bSetNormalAttack;
	bool m_bSetJumpAttack;
	bool m_bSetChargeAttack;
	int m_nCurChargeRun;

	DWORD m_dwMotionEndTime;
	DWORD m_dwCheckLoopTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	
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

	virtual bool IsAutoTarget() const;

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	bool IsCharCollisionSkipState();
	
public:
	void ProcessRunningAttackState( ioBaseChar *pOwner );

protected:
	void LoadRunInfo( ioINILoader &rkLoader, char* szType, RunInfo &sRunInfo );
	void SetCurRunInfo( bool bNormal, bool bJump, bool bCombo );

protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetRunningAttackState( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );
	void SetChargeAttackState( ioBaseChar *pOwner );

protected:
	void SetRunningReadyState( ioBaseChar *pOwner );
	void SetRunningLoopState( ioBaseChar *pOwner );
	void SetRunningEndState( ioBaseChar *pOwner );
	void SetRunningAttackEndState( ioBaseChar *pOwner );
	void CrateWeapon( ioBaseChar *pOwner );

protected:
	void ProcessLoopState( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckRotate( ioBaseChar *pOwner );
	bool IsFloatState( ioBaseChar *pOwner );

public:
	ioObjectRunningComboAttackItem();
	ioObjectRunningComboAttackItem( const ioObjectRunningComboAttackItem &rhs );
	virtual ~ioObjectRunningComboAttackItem();
};

inline ioObjectRunningComboAttackItem* ToObjectRunningComboAttackItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_RUNNING_COMBO_ATTACK )
		return NULL;

	return dynamic_cast< ioObjectRunningComboAttackItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioObjectRunningComboSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_OBJECT_RUNNING_COMBO_ATTACK"; }
	virtual const CharState GetState(){ return CS_OBJECT_RUNNING_COMBO_ATTACK; }
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

public:
	ioObjectRunningComboSpecialState();
	virtual ~ioObjectRunningComboSpecialState();
};

inline ioObjectRunningComboSpecialState* ToObjectRunningComboSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_OBJECT_RUNNING_COMBO_ATTACK )
		return NULL;

	return dynamic_cast< ioObjectRunningComboSpecialState* >( pState );
}