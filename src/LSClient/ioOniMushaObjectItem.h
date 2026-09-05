#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioOniMushaObjectItem : public ioObjectItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_RUN,
		WS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_RUN_START,
		ST_RUN_END,
		ST_INPUT_JUMP,
		ST_INPUT_ATTACK,
		ST_ROTATE,
		ST_RUN_EXTRA,
		ST_RUN_EXTRA_ATTACK,
	};

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_RUN_ATTACK,
		RS_END,
	};

	enum RunExtraState
	{
		RES_Start,
		RES_Attack,
	};

protected:
	DWORD m_dwProtectTime;
	ioHashStringVec m_AttackReadyAniList;

	ioHashStringVec m_EquipMaleMeshList;
	ioHashStringVec m_EquipFemaleMeshList;
	ioHashString m_szEquipEffect;

	float m_fRunSpeed;
	DWORD m_dwRunRotateTime;

	ioHashString m_RunWaitAni;
	float m_fRunWaitAniRate;

	ioHashString m_RunLoopAni;
	float m_fRunLoopAniRate;
	DWORD m_dwRunLoopTime;
	
	ioHashString m_RunExtraAni;
	float m_fRunExtraAniRate;

	float m_fRunExtraEndJumpPower;
	float m_fRunExtraAttackEndJumpPower;

	AttackAttribute	m_RunAttack;
	AttackAttribute	m_RunEndAttack;
	AttackAttribute	m_RunExtraAttack;
	ioHashStringVec m_RunBuffList;

	bool m_bOnlyTeamCharColSkip;

	float m_fChargeUseGauge;

	float m_fRunExtraJumpAngle;
	float m_fRunExtraJumpSpeed;
	float m_fRunExtraJumpMaxRange;

protected:
	WeaponItemState m_WeaponState;
	RunState m_RunState;
	RunExtraState m_RunExtraState;

	DWORD m_dwAttackStartTime;

	DWORD m_dwCheckProtectTime;
	int m_nNormalCombo;
	DWORD m_dwMotionEndTime;	

	DWORD m_dwRunLoopEndTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

	float m_fCurRunExtraJumpRange;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual ObjectSubType GetObjectSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsMoveAttackState( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

public:
	virtual bool IsProtectState();
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	
protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void DecreaseGauge( ioBaseChar *pOwner, float fUseGauge );
		
protected:
	void EquipChangeMesh( ioBaseChar *pOwner );
	void ReleaseChangeMesh( ioBaseChar *pOwner );
	
protected:
	void ChangeToRunState( ioBaseChar *pOwner );
	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunAttack( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner );

protected:
	void OnRunState( ioBaseChar *pOwner );
	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnRunAttack( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );

protected:
	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckDirKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	bool IsNoDropState( ioBaseChar *pOwner );

protected:
	void LoadRunBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList );
	void AddRunBuff( ioBaseChar *pOwner );
	void RemoveRunBuff( ioBaseChar *pOwner );

protected:
	void ChangeToRunExtra( ioBaseChar *pOwner );
	void ChangeToRunExtraAttack( ioBaseChar *pOwner );
	void ProcessRunExtraJump( ioBaseChar *pOwner );

	void OnRunExtra( ioBaseChar *pOwner );
	void OnRunExtraAttack( ioBaseChar *pOwner );

public:
	void OnProcessRunExtraState( ioBaseChar *pOwner );
	
public:
	bool IsReleaseItem( const ioBaseChar* pOwner ) const;

public:
	ioOniMushaObjectItem();
	ioOniMushaObjectItem( const ioOniMushaObjectItem &rhs );
	virtual ~ioOniMushaObjectItem();
};

inline ioOniMushaObjectItem* ToOniMushaObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_ONIMUSHA )
		return NULL;

	return dynamic_cast< ioOniMushaObjectItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioOnimushaRunExtraState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_ONIMUSHA_RUN_EXTRA_STATE"; }
	virtual const CharState GetState(){ return CS_ONIMUSHA_RUN_EXTRA_STATE; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioOnimushaRunExtraState();
	virtual ~ioOnimushaRunExtraState();
};

inline ioOnimushaRunExtraState* ToOnimushaRunExtraState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_ONIMUSHA_RUN_EXTRA_STATE )
		return NULL;

	return dynamic_cast< ioOnimushaRunExtraState* >( pState );
}