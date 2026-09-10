#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

typedef struct tagChargeFlyInfo
{
	ioHashString m_WaitAni;
	DWORD m_dwWaitDuration;

	float m_fChargeFlyAngle;
	float m_fChargeFlyMaxRange;

	float m_fChargeFlyMaxSpeed;
	float m_fChargeFlyMinSpeed;
	float m_fChargeFlyAccelSpeed;

	DWORD m_dwChargeFlyMinSpeedDuration;

	AttackAttribute m_ChargeAttackAttribute;

	DWORD m_dwRotateTime;

	float m_fChargeFlyJumpAmt;

	tagChargeFlyInfo()
	{
		Init();
	}

	void Init()
	{
		m_WaitAni.Clear();
		m_dwWaitDuration = 0;

		m_fChargeFlyAngle = 0.0f;
		
		m_fChargeFlyMaxRange = 0.0f;

		m_fChargeFlyMaxSpeed = 0.0f;
		m_fChargeFlyMinSpeed = 0.0f;
		m_fChargeFlyAccelSpeed = 0.0f;

		m_dwChargeFlyMinSpeedDuration = 0;

		m_dwRotateTime = 0;

		m_fChargeFlyJumpAmt = 0.0f;
	}

} ChargeFlyInfo;

class ioGatotkacaItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};
	enum ChargeFlyState
	{
		CFS_WAIT,
		CFS_MOVE,
		CFS_END,
	};
	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_CANCEL_JUMP,
	};

protected:
	ioHashString m_AttackReadyAni;

	ChargeState m_ChargeState;
	ChargeFlyState m_ChargeFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// move
	DWORD m_dwChargeFlyWaitTime;
	DWORD m_dwChargeFlyMoveTime;

	CEncrypt<float> m_fChargeFlyCurRange;

	bool m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	ChargeFlyInfo m_AttackChargeFly;
	ChargeFlyInfo m_CurChargeFly;

	// test
	DWORD m_dwFirstMaxSpeed;

	bool m_bEnableCancelJump;
	DWORD m_dwCancelJumpTime;
	float m_fChargeFlyCancelJumpAmt;

	bool m_bEnableExtraGauge;
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;
	ioHashString m_szGaugeDecEffect;

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

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	void SetChargeFlyState( ioBaseChar *pOwner, const ChargeFlyInfo &rkChargeFlyInfo );
	void ChangeChargeFlyMove( ioBaseChar *pOwner );
	
	void ProcessChargeFlyState( ioBaseChar *pOwner );
	bool ProcessChargeFlyMove( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge( ioBaseChar *pOwner );
	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }
	void SetGaugeDecEffect( ioBaseChar *pOwner );

protected:
	void CheckChargeFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioGatotkacaItem();
	ioGatotkacaItem( const ioGatotkacaItem &rhs );
	virtual ~ioGatotkacaItem();
};

inline ioGatotkacaItem* ToGatotkacaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GATOTKACA )
		return NULL;

	return dynamic_cast< ioGatotkacaItem* >( pItem );
}