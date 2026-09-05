#pragma once

#include "ioSpecialStateBase.h"

class ioBaseballItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_PITCHING,
		CS_SPECIAL_DELAY,
		CS_UPPER_SWING,
		CS_LEVEL_SWING,
		CS_SPECIAL_END,
		CS_NORMAL_ATTACK,
	};

protected:
	ChargeState m_ChargeState;

	//Normal Attack
	ioHashStringVec m_AttackReadyAniList;
	int m_iCurCombo;

	DWORD		m_dwMotionStartTime;
	DWORD		m_dwMotionEndTime;
	DWORD		m_dwFireTime;

	ioHashString m_szGatheringAni;
	float m_fGatheringAniRate;

	ioHashString m_szSpecialDelayAni;
	float m_fSpecialDelayAniRate;
	ioHashString m_szSpecialEndAni;
	float m_fSpecialEndAniRate;

	AttackAttribute m_LevelSwing;
	AttackAttribute m_UpperSwing;
	AttackAttribute m_PitchingAttack;

	ioHashString m_szBattingSound;
	DWORD m_dwBattingWeapon;	
	float m_fBattingMinAngle;
	float m_fBattingMaxAngle;
	float m_fBattingMinRange;
	float m_fBattingMaxRange;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fPitchingNeedGauge;
	CEncrypt<float> m_fStandNeedGauge;
	CEncrypt<float> m_fEndReturnGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

protected:
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwGatheringTime;
	DWORD m_dwSpecialDelayTime;
	DWORDVec m_vPitchingWeaponIndex;

protected:
	void ClearData();

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ProcessSpecialDelay( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeFlyComboAttackChargingState( ioBaseChar *pOwner );
	void ChangeGatheringState( ioBaseChar *pOwner );
	void ChangeSpecialDelayState( ioBaseChar *pOwner );
	void ChangeUpperSwingState( ioBaseChar *pOwner );
	void ChangeLevelSwingState( ioBaseChar *pOwner );
	void ChangeSpecialEndState( ioBaseChar *pOwner );
	void ChangePitchingState( ioBaseChar *pOwner );

	void SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr );
	void CheckPitchingWeapon( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual WeaponSubType GetSubType() const;

	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge( float fNeedGauge );
	void CheckDecreaseGauge( float fNeedGauge );
	void CheckIncreaseGauge( float fGauge );

	void CheckBattingWeapon( ioBaseChar *pOwner );

public:
	ioBaseballItem(void);
	ioBaseballItem( const ioBaseballItem &rhs );
	virtual ~ioBaseballItem(void);
};

inline ioBaseballItem* ToBaseballItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_BASEBALL_ITEM )
		return NULL;

	return dynamic_cast< ioBaseballItem* >( pItem );
}