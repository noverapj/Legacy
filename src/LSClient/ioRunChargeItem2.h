#pragma once


#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRunChargeItem2 : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_NORMAL_ATTACK,
		CS_CHARGING,
		CS_RUN,
		CS_FINISH_ATTACK,
		CS_RETREAT,
	};

	enum RunState
	{
		RS_NONE,
		RS_RUN,
		RS_ATTACK_KEY,
		RS_DEFENSE_KEY,
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

	// running
	ioHashString m_RunningAni;
	float m_fRunningAniRate;
	DWORD m_dwRunningDuration;
	
	DWORD m_dwRunningStartTime;
	DWORD m_dwRunningEndTime;

	// running attack key
	AttackAttribute m_ChargeAttackAttribute;
	DWORD m_dwRunningAttackEndTime;

	// running defense key
	AttackAttribute m_ChargeDefenseAttribute;
	DWORD m_dwRunningDefenseEndTime;

	// running finish
	AttackAttribute m_FinishAttribute;
	DWORD m_dwFinishAttackEndTime;

	// Rotate
	bool m_bTargetRot;
	bool m_bLeftRot;

	ioUserKeyInput::DirKeyInput m_ChargeStartDirKey;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<DWORD> m_dwRotateTime;

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

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnRunState( ioBaseChar *pOwner );
	void OnRetreatState( ioBaseChar *pOwner );
	void OnFinishAttackState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToRunState( ioBaseChar *pOwner );
	void ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void OnRunning( ioBaseChar *pOwner );
	void OnRunningAttackKey( ioBaseChar *pOwner );
	void OnRunningDefenseKey( ioBaseChar *pOwner );

	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunningAttack( ioBaseChar *pOwner );
	void ChangeToRunningDefense( ioBaseChar *pOwner );
	void ChangeToFinishAttack( ioBaseChar *pOwner );

	void CheckRunningMove( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );
	bool CheckRetreat( ioBaseChar *pOwner, bool bFront );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	ioRunChargeItem2();
	ioRunChargeItem2( const ioRunChargeItem2 &rhs );
	virtual ~ioRunChargeItem2();
};

inline ioRunChargeItem2* ToRunChargeItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RUN_CHARGE2 )
		return NULL;

	return dynamic_cast< ioRunChargeItem2* >( pItem );
}

