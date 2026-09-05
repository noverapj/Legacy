#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioTrinityItem : public ioWeaponItem
{
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum TrinityState
	{
		TS_NONE,
		TS_GATHERING,
		TS_SPECIAL_NONE_CHARGE,
		TS_SPECIAL_FULL_CHARGE,
		TS_RELOAD,
		TS_END,
	};


	enum SyncSubType
	{
		SST_CHANGE_STATE,
		SST_GATHERING,
		SST_RELOAD,
		SST_NONE_FIRE,
		SST_CHARGE_FIRE,
		SST_RELOADED,
		SST_END,
	};

	bool m_bVisibleMesh;

protected:
	ioHashStringVec		m_AttackReadyAniList;
	ioHashString		m_GatherAnimation;
	float				m_fGatherAniRate;
	DWORD				m_dwGatheringStartTime;

	ChargeState			m_ChargeState;
	TrinityState		m_State;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	//
	TrinityInfo			m_AttackInfo;

	CEncrypt<float>		m_fMaxGauge;
	CEncrypt<float>		m_fCurGauge;
	CEncrypt<float>		m_fIncreaseGauge;

	//////////////////////////////////////////////////////////////////////////
	DWORD				m_dwMotionEndTime;
	//////////////////////////////////////////////////////////////////////////
	int					m_nWeaponIndex;
	int					m_nMaxWeaponCnt;
	//////////////////////////////////////////////////////////////////////////
	UniqueObjID			m_dwEffect;
	//////////////////////////////////////////////////////////////////////////

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

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

	virtual float GetAttackSpeedRate() const;

	virtual void SetVisible( ioBaseChar *pOwner, bool bVisible );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadTrinityInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeFullChargeAttack( ioBaseChar *pOwner );
	void ChangeNoneChargeAttack( ioBaseChar *pOwner );
	void ChangeReloadState( ioBaseChar *pOwner );
	void ChangeReloadedState( ioBaseChar *pOwner );
	void ChangeEndState( ioBaseChar *pOwner );

	void OnCharging( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckCyvogueFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ReleaseBipe( ioBaseChar *pOwner );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	void DecreaseNormalFireGauge();
	void DecreseChargeFireGauge();

public:
	void SetTrinityState( ioBaseChar *pOwner, const TrinityInfo &rkInfo, bool bSendNet );
	TrinityState GetState() { return m_State; }

	void ProcessTrinityState( ioBaseChar *pOwner );

	inline const int GetState() const { return (int)m_State; }

protected:
	void CheckState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void ChangeGatheringState(  ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	void DecreaseGauge();

	int GetCurWeapon();
	void NextWeapon();

	bool IsTrinityState();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void AddEntityAndEffect( ioBaseChar *pOwner, bool bJumpAttack = false );
	void EraseEntityAndEffect( ioBaseChar *pOwner );
	void AddNormalEntityAndEffect( ioBaseChar *pOwner );

	void SetCurWeapon( int nCurWeapon );

	void ShowWeapon( ioBaseChar *pOwner );
	void HideWeapon( ioBaseChar *pOwner );

public:
	ioTrinityItem();
	ioTrinityItem( const ioTrinityItem &rhs );
	virtual ~ioTrinityItem();

private:
	bool CheckChargingMode( ioBaseChar *pOwner );
	void CheckNetWork( ioBaseChar *pOwner, SP2Packet &rkPacket );
};

inline ioTrinityItem* ToTrinityItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_TRINITY )
		return NULL;

	return dynamic_cast< ioTrinityItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioTrinitySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_TRINITY"; }
	virtual const CharState GetState(){ return CS_TRINITY; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	
public:
	ioTrinitySpecialState();
	virtual ~ioTrinitySpecialState();
};

inline ioTrinitySpecialState* ToTrinitySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_TRINITY )
		return NULL;

	return dynamic_cast< ioTrinitySpecialState* >( pState );
}