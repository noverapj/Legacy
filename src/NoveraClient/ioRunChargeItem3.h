#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRunChargeItem3 : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_NORMAL_ATTACK,
		CS_CHARGING,
		CS_RUNCHARGE,
		CS_FINISH_ATTACK,
		CS_RETREAT,
	};

	enum RunState
	{
		RS_NONE,
		RS_RUN,
		RS_AUTO,
		RS_ATTACK_KEY,
		RS_SPECIAL_RUN,
		RS_SPECIAL_AUTO,
		RS_SPECIAL_ATTACK_KEY,
	};

	enum SyncSubType
	{
		SST_ROTATE,
	};

protected:
	ChargeState m_ChargeState;
	RunState m_RunState;

	ioHashString m_AttackReadyAni;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// Run
	CEncrypt<float> m_fRunSpeed;
	CEncrypt<float> m_fSpecialRunSpeed;
	ioHashString m_RunningLoopSound;
	UniqueObjID m_RunningLoopSoundID;
	ioHashString m_SpecialRunningLoopSound;
	UniqueObjID m_SpecialRunningLoopSoundID;

	// running
	ioHashString m_RunningAni;
	float m_fRunningAniRate;
	DWORD m_dwRunningDuration;
	
	bool m_bEnableSpeicalRunning;
	ioHashString m_SpecialRunningAni;
	float m_fSpecialRunningAniRate;
	DWORD m_dwSpecialRunningDuration;

	DWORD m_dwRunningStartTime;
	DWORD m_dwRunningEndTime;

	// running auto attack
	AttackAttribute m_ChargeAutoAttribute;
	AttackAttribute m_SpecialChargeAutoAttribute;
	DWORD m_dwRunningAutoEndTime;

	// running key attack
	AttackAttribute m_ChargeKeyAttribute;
	AttackAttribute m_SpecialChargeKeyAttribute;
	DWORD m_dwRunningKeyEndTime;

	// running finish
	AttackAttribute m_FinishAttribute;
	AttackAttribute m_SpecialFinishAttribute;
	DWORD m_dwFinishAttackEndTime;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;

	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;
	CEncrypt<DWORD> m_dwSpecialRotateTime;

	// etc
	CEncrypt<bool> m_bOnlyTeamCharColSkip;
	DWORD m_dwFireDuration;

	// Retreat
	ioHashString m_szRetreatFr;
	float m_fRetreatFrRate;

	DWORD m_dwRetreatEndTime;

	D3DXVECTOR3 m_vForceDir;
	float m_fRetreatForceAmt;
	float m_fRetreatForceFric;
	DWORD m_dwRetreatForceTime;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

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
	virtual bool IsMoveAttackState( ioBaseChar *pOwner );

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

	bool IsEnableSpeicalRunning()	{ return m_bEnableSpeicalRunning; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void OnRunning( ioBaseChar *pOwner, bool bSpecial );
	void OnRunningAutoAttack( ioBaseChar *pOwner, bool bSpecial );
	void OnRunningKeyAttack( ioBaseChar *pOwner, bool bSpecial );

	void ChangeToRunning( ioBaseChar *pOwner, bool bSpecial );
	void ChangeToRunningAutoAttack( ioBaseChar *pOwner, bool bSpecial );
	void ChangeToRunningKeyAttack( ioBaseChar *pOwner, bool bSpecial );
	void ChangeToFinishAttack( ioBaseChar *pOwner, bool bSpecial );

	void CheckRunningMove( ioBaseChar *pOwner, bool bSpecial );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner, bool bSpecial );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	ioRunChargeItem3();
	ioRunChargeItem3( const ioRunChargeItem3 &rhs );
	virtual ~ioRunChargeItem3();
};

inline ioRunChargeItem3* ToRunChargeItem3( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RUN_CHARGE3 )
		return NULL;

	return dynamic_cast< ioRunChargeItem3* >( pItem );
}

