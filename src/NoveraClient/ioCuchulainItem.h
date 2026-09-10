#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioCuchulainItem : public ioWeaponItem
{
public:
	enum CuchulainFlyState
	{
		CFS_NONE,
		CFS_START,
		CFS_MOVE,
		CFS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	AttackAttribute m_MiddleAttack;

	CuchulainSearchInfo m_SearchInfo;
	CuchulainFlyInfo    m_FlyInfo;
	CuchulainFlyInfo    m_CurFlyInfo;

	ioHashString	m_stAimingMaker;
	ioHashString	m_stAimedMaker;

	DWORD		    m_dwFlyStartTime;
	float			m_fFlyMoveCurRange;
	DWORD			m_dwFlyKeyReservTime;

	bool			m_bSpecialChagerKey;
	DWORD			m_dwSpecialChargeStartTime;
	DWORD			m_dwSpecialChargeTime;

	CuchulainFlyState m_SpecialMoveState;

	float			m_fChargeBuffSearchRange;
	ioHashString    m_ChargeSearchBuff1;
	ioHashString    m_ChargeSearchAddBuff1;
	ioHashString    m_ChargeSearchBuff2;
	ioHashString    m_ChargeSearchAddBuff2;
	ioHashString    m_ChargeSearchBuff3;
	ioHashString    m_ChargeSearchAddBuff3;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_MIDDLE_ATTACK_FIRE,
	};

	enum NetSyncType
	{
		NST_NONE,
		NST_TARGET_INIT,
		NST_TARGETING,
		NST_RE_TARGETING,
		NST_MIDDLE_ATTACK,
		NST_FULL_BUFF_SEARCH_ATTACK,
		NST_SPECIAL_S_MOVE,
		NST_SPECIAL_S_END_JUMP,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	DWORD m_dwMiddleChargeTime;
	DWORD m_dwFullChargeTime;

	bool  m_bMiddleCharged;
	bool  m_bFindMiddleCharge;

	ioHashString m_AimedTarget;
	DWORD m_dwMiddleAttackEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual int GetNeedBullet();
	void DecreseBullet();

public:

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual WeaponSubType GetSubType() const;

	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

	void ProcessSpecialMove(ioBaseChar *pOwner );

	//////////////////////////////////////////////////////////////////////////
	void SetSpecialMoveState( ioBaseChar *pOwner, const CuchulainFlyInfo& info, bool SendNet );

	void ChargeBuffSearch( ioBaseChar *pOwner, bool bSend );

protected:
	void ChangeSpecialMove_Start( ioBaseChar *pOwner );
	void ChangeSpecialMove_Move( ioBaseChar *pOwner );
	bool ProcessSpecialMove_Move( ioBaseChar *pOwner );
	void ChangeSpecialMove_End( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindMiddleSearchTarget( ioBaseChar *pOwner, const CuchulainSearchInfo &info );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	
	bool CheckTargetValidate( ioBaseChar *pOwner, const CuchulainSearchInfo &info );

	void SetMiddleAttack( ioBaseChar *pOwner,const AttackAttribute& att );

public:
	void CheckMiddleSearch( ioBaseChar *pOwner, bool bTargetEffect = true );
	void AimTargetInit( ioBaseChar *pOwner, bool bSend );
	void ClearAimInfo( ioBaseChar *pOwner );
	const ioHashString& GetAimedTarget();
	void SetAimedTarget( const ioHashString& stTarget);


public:
	ioCuchulainItem();
	ioCuchulainItem( const ioCuchulainItem &rhs );
	virtual ~ioCuchulainItem();
};

inline ioCuchulainItem* ToCuchulainWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CUCHULAIN_ITEM )
		return NULL;

	return dynamic_cast< ioCuchulainItem* >( pItem );
}


class ioCuchulainItemSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_CUCHULAIN_SPECIAL_STATE"; }
	virtual const CharState GetState(){ return CS_CUCHULAIN_SPECIAL_STATE; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

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
	ioCuchulainItemSpecialState();
	virtual ~ioCuchulainItemSpecialState();
};

inline ioCuchulainItemSpecialState* ToCuchulainSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CUCHULAIN_SPECIAL_STATE )
		return NULL;

	return dynamic_cast< ioCuchulainItemSpecialState* >( pState );
}

