#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDonQuixoteItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		MCS_NONE,
		MCS_NORMAL_ATTACK,
		MCS_CHARGING,
		MCS_GATHERING,
		MCS_RUN,
		MCS_FINISH_ATTACK,
		MCS_RUN_END_ATTACK,
	};
	ChargeState m_ChargeState;

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_JUMP,
		RS_JUMP_LANDING,
		RS_END,
	};
	RunState m_RunState;

	enum SyncSubType
	{
		SST_NORMAL_ATTACK,
		SST_GATHERING_ROTATE,
		SST_ROTATE,
		SST_ATTACH,
		SST_GATHERING,
		SST_FULL_GATHERING,
		SST_START_WAIT,
		SST_START_RUN,
		SST_JUMP,
		SST_FINISH_ATTACK,
		SST_RUN_END_ATTACK,
	};

	enum DisableStopType
	{
		DST_NONE,		// 점프키(점프), 공격/방어(캔슬) 가능
		DST_ALL,		// 점프/공격/방어 모두 불가
		DST_ATTACK,		// 점프키(점프) 가능, 공격/방어(캔슬) 불가
		DST_JUMP,		// 점프키(점프) 불가, 공격/방어(캔슬) 가능
	};

protected:
	ioHashString m_AttackReadyAni;
	AttackAttribute m_ChargeAttackAttribute;
	AttackAttribute m_FinishAttribute;
	AttackAttribute m_RunEndAttribute;

	CEncrypt<int> m_iMaxBullet;
	int m_iCurBullet;
	int m_iNeedBullet;

	ioHashStringVec m_vAttackedBaseCharName;
	ioHashString	m_strRemoveBuffName;
	ioHashString	m_strAddBuffName;

	DWORD			m_dwSpecialWeaponIndex;
	DWORD			m_dwLandSpecialWeaponIndex;

	ioHashString	m_strChargeRunAni;
	float			m_fChargeRunAniRate;

protected:
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// Gathering
	ioHashString m_GatheringEffect;
	ioHashString m_GatheringMaxEffect;

	UniqueObjID m_GatheringEffectID;
	UniqueObjID m_GatheringMaxEffectID;

	ioHashString m_GatheringMaxSound;

	DWORD m_dwGatheringStartTime;

	// ChargeRate
	CEncrypt<float> m_fMaxRunTimeRate;
	CEncrypt<float> m_fMaxRunSpeedRate;

	// ChargeMotion
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;

	bool m_bFirstChargeMoveMotion;
	bool m_bSetChargingMoveAni;

	// Run
	float m_fRunSpeed;
	float m_fCurRunSpeed;

	float m_fRunJumpPower;
	float m_fStartRunJumpPower;

	ioHashString m_RunWaitAni;
	float m_fRunWaitAniRate;
	DWORD m_dwRunWaitEndTime;

	ioHashString m_RunningAni;
	float m_fRunningAniRate;
	DWORD m_dwRunningDuration;
	DWORD m_dwRunningEndTime;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunEndEnableTime;

	ioHashString m_RunEndAni;
	float m_fRunEndAniRate;
	DWORD m_dwRunEndTime;

	ioHashString m_RunJumpAni;
	float m_fRunJumpAniRate;
	DWORD m_dwRunJumpAniEndTime;

	DWORD m_dwFinishAttackEndTime;

	CEncrypt<bool> m_bDisableColCancel;
	CEncrypt<bool> m_bUseFinishAttack;
	CEncrypt<bool> m_bOnlyTeamCharColSkip;

	DisableStopType m_DisableRunStopType;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

	// etc
	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;

	DWORD m_dwFireDuration;

	//jump Attack
	DWORD m_dwJumpAttackStartTime;
	DWORD m_dwJumpAttackEndTime;
	CEncrypt<float> m_fJumpAttackSpeed;
	float   m_fJumpForceByJump;//공중 D~상태에서 변할때

	//Drift
	DWORD m_dwDriftStartTime;
	DWORD m_dwDriftEndTime;

	CEncrypt<DWORD> m_dwDriftRotateTime;
	CEncrypt<float> m_fDriftSpeed;

	DWORD	m_dwTestDriftTime;

	float	m_fTargetYaw;
	float	m_fDriftAngle;
	bool    m_bDriftRotNeed;
	float	m_fDriftForce;

	float	m_fJumpForce;
	float	m_fJumpLookForce;

	ioHashString	m_stEffectName;
	DWORD			m_dwEffectIndex;
	DWORD			m_dwPreTime;
	float			m_fCurRunningAniRate;
	bool			m_bZoneWeaponDeadByMap;
	float           m_fJumpAttack_JumpPower;
	bool			m_bWaitAferJump;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableChargeDefenseCheck();

	virtual bool IsMoveAttackState( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	void UpdateEffect( float fFrameGap );

	virtual void WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar);
	virtual void WeaponMapColDead( ioWeapon *pWeapon );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual bool IsCanSendControl() const;

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void OnJumpAttacking( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );
	void OnRunEndAttackState( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void SetFullGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

	void ChangeToRunState( ioBaseChar *pOwner );

	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnJumpRunning( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner, bool bJumpRun );
	void ChangeToRunningMotion( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner );
	void ChangeToFinishAttack( ioBaseChar *pOwner, bool bSend );
	void ChangeToRunEndAttack( ioBaseChar *pOwner, bool bAniPlay, bool bSend );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState( ioBaseChar *pOwner );
	virtual void CheckReserve( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CheckCreateEffect( ioBaseChar *pChar );

	void RemoveBuff( ioBaseChar *pOwner, const ioHashString& strTargetName );
	void AddBuff( ioBaseChar *pOwner, const ioHashString& strTargetName );

	void SetJumpRunning( ioBaseChar *pOwner, bool bFirstJump, bool bJumpAttack );

	void CheckKeyRot( ioBaseChar *pChar );

public:
	bool IsDonQuixoteState();

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }
	int GetNeedBullet() { return m_iNeedBullet; }

	void InitGauge();
	void MaxGauge();

	bool IsNoDropState( ioBaseChar *pOwner );

private:
	void AttachEraseInfo( ioBaseChar *pOwner, const ioHashString& strEraseName );
	bool CheckWeaponControl( ioBaseChar *pOwner );
	void BuffCheck( ioBaseChar *pOwner );
	bool IsEnableAddNewBuff( ioBaseChar *pTarget );

public:
	ioDonQuixoteItem();
	ioDonQuixoteItem( const ioDonQuixoteItem &rhs );
	virtual ~ioDonQuixoteItem();
};

inline ioDonQuixoteItem* ToDonQuixoteItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DON_QUIXOTE_ITEM )
		return NULL;

	return dynamic_cast< ioDonQuixoteItem* >( pItem );
}