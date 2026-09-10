#pragma once
#include "ioweaponitem.h"
class ioOutBoxerItem :	public ioWeaponItem
{
public:
	enum OutBoxerChargeState
	{
		OBCS_NONE,
		OBCS_CHARGING,
		OBCS_NORMAL_ATTACK,
		OBCS_FIRST_STEP,
		OBCS_SECOND_STEP,
		OBCS_MOVE,
		OBCS_BOXER_ATTACK,
		OBCS_SECOND_STEP_CHARGING,
		OBCS_SECOND_STEP_ATTACK,
		OBCS_SECOND_STEP_DEFENSE,
		OBCS_END,
	};
protected:
	enum BoxerNextStepState
	{
		BNSS_NONE,
		BNSS_FIRST_STEP,
		BNSS_SECOND_STEP,
	};

protected:
	OutBoxerChargeState			m_OutBoxerChargeState;
	BoxerNextStepState			m_BoxerNextStepState;

	//Normal Attack
	ioHashStringVec				m_AttackReadyAniList;
	int							m_iCurCombo;

	//First Step
	ioHashString				m_FirstStepAni;
	float						m_fFirstStepAniRate;

	//Move
	int							m_iCurMovingCount;
	ioHashString				m_CurMoveAni;
	float						m_fCurMoveAniRate;
	ioHashString				m_MoveAniFr;
	float						m_fMoveAniFrRate;
	ioHashString				m_MoveAniBk;
	float						m_fMoveAniBkRate;
	ioHashString				m_MoveAniRt;
	float						m_fMoveAniRtRate;
	ioHashString				m_MoveAniLt;
	float						m_fMoveAniLtRate;

	D3DXVECTOR3					m_vMoveDir;
	DWORD						m_dwMoveTime;

	float						m_fMoveForce;
	float						m_fMoveForceFriction;

	//Boxer Attack
	AttackAttribute				m_BoxerAttack;

	//Second Step
	ioHashString				m_SecondStepAni;
	float						m_fSecondStepAniRate;
	DWORD						m_dwSecondStepStartTime;
	DWORD						m_dwEnableSecondStepJumpTime;

	//Second Step Charging
	DWORD						m_dwSecondStepChargeTime;
	ioHashString				m_SecondStepChargeTimeAni;

	//Second Step Attack
	AttackAttribute				m_SecondStepAttack;

	//Second Step Defecse
	ioHashString				m_SecondStepDefenseAni;
	float						m_fSecondStepDefenseAniRate;
	int							m_iCurSecondStepDefenseCount;

	DWORD						m_dwMotionStartTime;
	DWORD						m_dwMotionEndTime;

	//Gauge
	float						m_fMaxBullet;
	float						m_fCurBullet;

	float						m_fIncreseGaugeDelayTic;
	float						m_fIncreseGaugeRunTic;
	float						m_fIncreseGaugeDefaultTic;

	float						m_fDecreaseGaugeMoveState;
	float						m_fDecreaseGaugeBoxerAttackState;
	float						m_fDecreaseGaugeSecondStepAttackState;
	float						m_fDecreaseGaugeSecondStepDefenceState;
	float						m_fDecreaseGaugeSecondStepDefenceWeapon;

	int							m_iNeedBullet;

protected:
	void ClearData();

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessSecondStepCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToFirstStepState( ioBaseChar *pOwner );
	void ChangeToMoveState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey );
	void ChangeToNextStep( ioBaseChar *pOwner );
	void ChangeToBoxerAttackState( ioBaseChar *pOwner, bool bReduceGauge );
	void ChangeToSecondStepState( ioBaseChar *pOwner );
	void ChangeToSecondStepChargeState( ioBaseChar *pOwner );
	void ChangeToSecondStepAttackState( ioBaseChar *pOwner );
	void ChangeToSecondStepDefenseState( ioBaseChar *pOwner );
	void ChangeToEndState( ioBaseChar *pOwner );

	void ProcessFirstStepState( ioBaseChar *pOwner );
	void ProcessMoveState( ioBaseChar *pOwner );
	void ProcessBoxerAttackState( ioBaseChar *pOwner );
	bool ProcessBoxerAttackReserveKeyInput( ioBaseChar *pOwner );
	void ProcessSecondStepState( ioBaseChar *pOwner );
	void ProcessSecondStepAttackState( ioBaseChar *pOwner );
	void ProcessSecondStepDefenseState( ioBaseChar *pOwner );
	
	void CheckMoveAni( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey );
	
	void SetMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void DecreaseGauge( float fNeedGauge );
public:
	virtual int GetNeedBullet() { return m_iNeedBullet; }
	inline const int& GetNormalComboCount( ) { return m_iCurCombo; }
	inline const int& GetSecondStepDefenseCount( ) { return m_iCurSecondStepDefenseCount; }
	inline const int& GetMoveingCount() { return m_iCurMovingCount; }
	inline const OutBoxerChargeState& GetOutBoxerChargeState() { return m_OutBoxerChargeState; }
	void GetAnimationNameByState( OutBoxerChargeState state, ioHashString& szAni, float& fRate );
	
	bool IsNoDropState( ioBaseChar *pOwner );

protected:
	virtual void CheckReserve( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	
	virtual ioItem* Clone();

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual WeaponSubType GetSubType() const;
	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle, AutoTargetType eType );
	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int)m_fCurBullet; }

	virtual bool IsEnableChargeDefenseCheck();

public:
	ioOutBoxerItem(void);
	ioOutBoxerItem( const ioOutBoxerItem &rhs );
	~ioOutBoxerItem(void);
};

inline ioOutBoxerItem* ToOutBoxerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_OUTBOXER_ITEM )
		return NULL;

	return dynamic_cast< ioOutBoxerItem* >( pItem );
}