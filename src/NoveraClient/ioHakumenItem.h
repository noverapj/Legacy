#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioHakumenItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum HakumenDefenseState
	{
		DS_NONE,
		DS_START,
		DS_DELAY,
		DS_DEFENSE_ATTACK,
		DS_END,
	};

	enum SyncSubType
	{
		SST_DEFENSE_STATE,
		SST_FLY_ATTACK_START,//UroborusAttack시전을 시작할때
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	HakumenDefenseState m_DefenseState;
	//LuciferFlyAttackState m_FlyAttackState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	//
	HakumenDefenseInfo m_DefenseAttack;
	HakumenDefenseInfo m_CurDefenseAttack;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;


	CEncrypt<float> m_fCurGauge;
	
	DWORD	m_dwMotionEndTime;

	bool	m_bJumpDefenseState;

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

	virtual void CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadDefenseInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void DefenseAttackFire( ioBaseChar *pOwner, bool bLandAttack = true );
	

public:
	void SetDefenseState( ioBaseChar *pOwner, const HakumenDefenseInfo &rkDefenseAttackInfo, bool bSendNet );
	HakumenDefenseState GetState() { return m_DefenseState; }
	void ChangeDefenseStart( ioBaseChar *pOwner );
	void ChangeDefenseDelay( ioBaseChar *pOwner );

	void ChangeDefenseEnd( ioBaseChar *pOwner );

	void ProcessDefenseState( ioBaseChar *pOwner, float fHeightGap );

	bool IsJumpDefenseState();

protected:
	void CheckDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckAniRotate( ioBaseChar *pChar );
	void DecreaseGauge();

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	bool IsEnableGauge();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );

public:
	ioHakumenItem();
	ioHakumenItem( const ioHakumenItem &rhs );
	virtual ~ioHakumenItem();
};

inline ioHakumenItem* ToHakumenItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HAKUMEN )
		return NULL;

	return dynamic_cast< ioHakumenItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioHakumenDefenseSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HAKUMEN_DEFENSE"; }
	virtual const CharState GetState(){ return CS_HAKUMEN_DEFENSE; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }		
	virtual bool IsLandingEnable(){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	
public:
	ioHakumenDefenseSpecialState();
	virtual ~ioHakumenDefenseSpecialState();
};

inline ioHakumenDefenseSpecialState* ToHakumenDefenseSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HAKUMEN_DEFENSE )
		return NULL;

	return dynamic_cast< ioHakumenDefenseSpecialState* >( pState );
}