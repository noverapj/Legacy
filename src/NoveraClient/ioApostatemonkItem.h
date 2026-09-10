#pragma once

#include "ioSpecialStateBase.h"

class ioApostatemonkItem : public ioWeaponItem
{
protected:
	typedef struct _tagApostateReady
	{
		ioHashString		m_szReayAni;
		float				m_fReayAniRate;
		ioHashString		m_szCheckBuff;
		ioHashStringVec		m_szStartBuffList;
		ioHashStringVec		m_szEndBuffList;
	}ApostateReady;

	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,

		CS_APOSTATE_READY,

		CS_RELAX_ATTACK,
	};

	enum MonkState
	{
		MS_NORMAL,
		MS_APOSTATE,
		MS_RELAX,
	};
	
	enum ApostateSpecialState
	{
		ASS_NONE,
		ASS_APOSTATE_ATTACK,
		ASS_APOSTATE_ADD_ATTACK,
		ASS_APOSTATE_ATTACK_END,

		ASS_EXTEND_DASH,
		ASS_APOSTATE_EXTEND_DASH,

		ASS_JUMP_DASH,
		ASS_LAND_ATTACK,
	};

	enum SyncState
	{
		SS_CHECK_MONK_STATE,
		SS_NORMAL_ATTACK,
		SS_APOSTATE_READY,
		SS_APOSTATE_READY_END,
		SS_APOSTATE_ATTACK,
		SS_APOSATTE_ATTACK_CHANGE_DIR_KEY,
		SS_APOSTATE_ADD_ATTACK,
		SS_APOSTATE_ATTACK_END,
		SS_RELAX_ATTACK,
		SS_EXTEND_DASH,
		SS_APOSTATE_EXTEND_DASH,
		SS_JUMP_DASH,
		SS_LAND_ATTACK,
		SS_DIRECTION_ROTATION,
	};
	
protected:
	//State
	ChargeState						m_ChargeState;
	MonkState						m_MonkState;
	ApostateSpecialState			m_ApostateSpecialState;
	ioUserKeyInput::DirKeyInput		m_CurDirKey;
	
	//Ani
	DWORD							m_dwMotionStartTime;
	DWORD							m_dwMotionEndTime;

	//Normal Attack
	ioHashStringVec					m_AttackReadyAniList;
	int								m_iCurCombo;

	//Apostate Buff
	int								m_iMaxApostateApostateReadyCount;
	int								m_iCurApostateApostateReadyCount;

	//Apostate Ready
	std::vector<ApostateReady>		m_vApostateReadyList;

	//Check Apostate State Buff
	ioHashString					m_szCheckApostateBuff;
	ioHashString					m_szCheckRelaxBuff;

	//Apostate Attack
	AttackAttribute					m_ApostateAttack;
	DWORD							m_dwApostateAttackRotateSpeed;
	float							m_fApostateAttackRunSpeed;
	AttackAttribute					m_ApostateAddAttack;
	ioHashString					m_szApostateAttackEndAni;
	float							m_fApostateAttackEndAniRate;

	//Relax
	AttackAttribute					m_RelaxAttack;

	//Dash
	int								m_iCurApostateExtendCombo;
	AttackAttribute					m_ExtendDash;				//dash D~
	AttributeList					m_ApostateExtendDashList;		//D~
	DWORD							m_dwApostateExtendDashChargeStartTime;
	DWORD							m_dwApostateExtendDashChargeTime;

	//Jump Dash
	AttackAttribute					m_JumpDashAttack;
	AttackAttribute					m_JumpDashLandAttack;
	DWORD							m_dwLandAttackChargeStartTime;
	DWORD							m_dwLandAttackChargeTime;
	bool							m_bReserveApostateAttack;
	float							m_fJumpDashEnableHeightGap;
	float							m_fJumpDashAttackGravity;

	//Gauge
	float							m_fMaxBullet;
	float							m_fCurBullet;

	float							m_fIncreaseApostateReadyGauge;
	float							m_fDecreaseRelaxStateGaugeTic;

	//Remove Buff
	ioHashStringVec					m_szRemoveBuffList;
	//Add Buff ( Release Time )
	ioHashString					m_szAfterEffectBuff;
	
protected:
	void ClearData();
	void LoadApostateReadyState( ioINILoader &rkLoader );

protected:
	void CheckMonkState( ioBaseChar *pOwner );

	//Normal Attack
	void ChangeNormalAttackState( ioBaseChar *pOwner );
	void ProcessNormalAttackState( ioBaseChar *pOwner );

	//Normal
	void CheckApostateReadyCount( ioBaseChar *pOwner );
	void ChangeNormalApostateReadyState( ioBaseChar *pOwner );
	void SetApostateReadyEndState( ioBaseChar *pOwner );
	void ProcessNormalState( ioBaseChar *pOwner );
	void ProcessNormalChargingState( ioBaseChar *pOwner );
	void ProcessNormalApostateReadyState( ioBaseChar *pOwner );

	int SetAttackAni( ioBaseChar *pOwner, const AttackAttribute& rkAtt, bool bUseTracking );

	//Apostate
	bool IsApostateState( ioBaseChar *pOwner );
	void ChangeApostateAttackState( ioBaseChar *pOwner );
	void ChangeApostateAddAttackState( ioBaseChar *pOwner );
	void ChangeApostateAttackEndState( ioBaseChar *pOwner );
	void CheckApostateAttackKeyInput( ioBaseChar *pOwner );
	void ProcessApostateState( ioBaseChar *pOwner );
	void ProcessApostateChargingState( ioBaseChar *pOwner );
	void ProcessApostateAttackState( ioBaseChar *pOwner );
	void ProcessApostateAttackRotate( ioBaseChar *pOwner );
	void ProcessApostateAddAttackState( ioBaseChar *pOwner );
	void ProcessApostateAttackEndState( ioBaseChar *pOwner );

	//Relax
	void ChangeRelaxAttackState( ioBaseChar *pOwner );
	void ProcessRelaxState( ioBaseChar *pOwner );
	void ProcessRelaxChargingState( ioBaseChar *pOwner );
	void ProcessRelaxAttackState( ioBaseChar *pOwner );

	//Buff
	void AddBuff( ioBaseChar *pOwner, const ioHashString& szBuffName );
	void AddBuffList( ioBaseChar *pOwner, const ioHashStringVec& vBuffList );
	void RemoveBuff( ioBaseChar *pOwner, const ioHashString& szBuffName );
	void RemoveBuffList( ioBaseChar *pOwner, const ioHashStringVec& vBuffList );

	//Extend Dash
	void ProcessExtendDash( ioBaseChar *pOwner );
	void ProcessApostateExtendDash( ioBaseChar *pOwner );
	void ChangeApostateExtendDashState( ioBaseChar *pOwner, int iCombo );

	//Jump Dash
	void ProcessJumpDashState( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pOwner );
	void ChangeLandAttackState( ioBaseChar *pOwner );
	void ProcessLandAttackState( ioBaseChar *pOwner );

	void IncreaseGauge( float fNeedGauge );

	//방향키 회전
	void DirectionRotationOwner( ioBaseChar *pOwner );
public:
	//Extend Dash
	void SetExtendDash( ioBaseChar *pOwner );
	void ProcessApostateMonkSpecialState( ioBaseChar *pOwner );

	//Jump Dash
	bool IsEnableJumpDashAttack( ioBaseChar *pOwner );
	void SetJumpDashAttackState( ioBaseChar *pOwner );

	bool IsEnableBlowWoundedBySpecialState();
	bool IsNoDropSpecialState();
	void SetSpecialStateGravity( float &fCurGravity );

	virtual int GetNeedBullet();
	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

protected:
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet() { return (int)m_fMaxBullet; }
	virtual int GetCurBullet() { return (int) m_fCurBullet; }
	virtual void SetCurBullet( int iCurBullet );

public:
	ioApostatemonkItem(void);
	ioApostatemonkItem( const ioApostatemonkItem &rhs );
	virtual ~ioApostatemonkItem(void);
};

inline ioApostatemonkItem* ToApostatemonkItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_APOSTATE_MONK_ITEM )
		return NULL;

	return dynamic_cast< ioApostatemonkItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------
class ioApostateMonkSpecialState : public ioSpecialStateBase
{
public:
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_APOSTATE_MONK_SPECIAL"; }
	virtual const CharState GetState(){ return CS_APOSTATE_MONK_SPECIAL; }

	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void CheckSpecialState( ioBaseChar* pOwner );

	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
public:
	ioApostateMonkSpecialState();
	virtual ~ioApostateMonkSpecialState();
};