#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioRedHoodItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		ES_NONE,
		ES_CHARGING,
		ES_SPECIAL_ATTACK,
		ES_NORMAL_ATTACK,
		ES_SPECIAL_MOVE,
		ES_END,
	};

	enum SpecialMoveState
	{
		SMS_NONE,
		SMS_MOVE,
		SMS_JUMP,
		SMS_END,
	};

	enum SyncSubType
	{
		SST_BLINK_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
	};

	enum DefenceCounterState
	{
		DCS_NONE,
		DCS_DELAY,
		DCS_COMBO,
		DCS_CHARGE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;
	int m_nAttackCnt;

	float			m_fDashMovedLenth;
	D3DXVECTOR3		m_vDashMoveDir;

	bool			m_bKeyReserved;
	bool			m_bAttackKeyReserved;
	bool			m_bDefenseKeyReserved;
	bool			m_bJumpKeyReserved;

	void			ClearReserveKey();

	AttributeList	m_ChargeAttackList;

	bool			m_bThirdChargeAttack;//3Å¸ÈÄ d²Ú¶¼±â½Ã ÆøÆÄ¿ë ÇÃ·¹±×

	bool			m_bSDSkillUsed;

	RedHoodFlyInfo	m_SpecialMoveInfo;

	//////////////////////////////////////////////////////
	ioHashString	m_stSD_Blow_Motion;
	float			m_fSD_BlowMotionRate;

	bool			m_bEndAttackJump;

	//////////////////////////////////////////////////////////////////////////
	DWORD			m_dwSpecialMoveMacroTime_S;
	DWORD			m_dwSpecialMoveMacroTime_E;		
	float			m_fFlyCurRange;

	//////////////////////////////////////////////////////////////////////////
	DWORD			m_dwEnableReserveKeyTime;
	AttackAttribute m_CounterAttack;
	AttributeList   m_CounterAttackList;
	AttributeList	m_CounterChargeAttackList;

	DefenceCounterState m_DefenceCounterState;
	DWORD			m_dwCounterChargeTime;
	int				m_nDefenceCounterComboIndex;

protected:
	SpecialMoveState		m_SpecialMoveState;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	EvyBlinkInfo	m_NormalBlinkInfo;

	DWORD			m_dwMotionEndTime;

	DWORD m_dwSpecialMoveStartTime;
	DWORD m_dwSpecialMoveEndTime;

protected:

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

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

	/*virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );*/

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadInfo( ioINILoader &rkLoader );
	//void CheckCounterCharging( ioBaseChar *pOwner );
	//void SetDefenceCounterChargeAttack( ioBaseChar *pOwner );
	//void SetCounterComboState( ioBaseChar *pOwner, int nComboIndex );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner, int iCurCombo );
	void ChangeDefenseState( ioBaseChar *pOwner );
	void ChangeSpecialMoveEndJump( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void KeyReserve( ioBaseChar *pOwner );

	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	//void CheckCounterReserveState( ioBaseChar *pOwner );
	//void CheckCounterReserveKey( ioBaseChar *pOwner );
public:
	void SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet );
	void ProcessSpecialMoveState( ioBaseChar *pOwner );

/*public:
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );*/

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void  SetCurExtraGauge( float fGauge );

	void DecreaseGauge();

	bool IsEnableGauge();
	void ClearSpecialState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	bool IsBallTarget();

public:
	ioRedHoodItem();
	ioRedHoodItem( const ioRedHoodItem &rhs );
	virtual ~ioRedHoodItem();
};

inline ioRedHoodItem* ToRedHoodItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RED_HOOD_ITEM )
		return NULL;

	return dynamic_cast< ioRedHoodItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioRedHoodItemMoveSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HOOD_MOVE"; }
	virtual const CharState GetState(){ return CS_RED_HOOD_MOVE; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	
public:
	ioRedHoodItemMoveSpecialState();
	virtual ~ioRedHoodItemMoveSpecialState();
};

inline ioRedHoodItemMoveSpecialState* ToRedHoodMoveSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_RED_HOOD_MOVE )
		return NULL;

	return dynamic_cast< ioRedHoodItemMoveSpecialState* >( pState );
}


