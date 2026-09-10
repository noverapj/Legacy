#pragma once

#include "ioChargeCommandItem.h"
#include "ioSpecialStateBase.h"

class ioNakoruruItem : public ioChargeCommandItem
{
public:
	enum WitchFlyState
	{
		WFS_NONE,
		WFS_WAIT,
		WFS_START,
		WFS_MOVE,
		WFS_CHARGE,
		WFS_ATTACK,
		WFS_END,
		WFS_MOVE2,
		WFS_CANCEL,
		WFS_JUMP_GRAP,
		WFS_LAND_ATTACK,
	};

protected:
	WitchFlyState m_WitchFlyState;

	int m_iCurWitchCombo;

	// move
	DWORD m_dwWitchFlyWaitTime;
	DWORD m_dwWitchFlyStartTime;
	DWORD m_dwWitchFlyChargeTime;
	DWORD m_dwWitchFlyAttackEndTime;

	DWORD m_dwWitchFlyKeyReserveTime;
	DWORD m_dwWitchFlyCancelEndTime;

	CEncrypt<float> m_fWitchFlyCurRange;

	bool m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	WitchFlyInfo m_AttackWitchFly;
	WitchFlyInfo m_CurWitchFly;

	AttackAttribute m_WitchFlyCancelAttack;
	CEncrypt<float> m_fWitchFlyCancelGravityRate;

	ioHashString m_CurMoveAni;

	// gauge
	CEncrypt<float> m_fMaxWitchGauge;
	CEncrypt<float> m_fNeedWitchGauge;

	CEncrypt<float> m_fIncreaseWitchGauge;
	CEncrypt<float> m_fDecreaseWitchGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseWitchGauge;

	CEncrypt<float> m_fCurWitchGauge;

	ioHashString m_EnableWitchEffect;
	UniqueObjID m_dwEnableWitchEffect;

protected:
	ioHashString m_EquipDummyCharName;
	int m_iEquipDummyCharIndex;

protected:
	ItemJumpAttackInfo m_CurJumpAttackInfo;
	ioHashString m_GrapplingTargetName;
	DWORD m_dwMotionEndTime;

	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void CheckWitchFlyMoveAni( ioBaseChar *pOwner );
	void CheckWitchFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	void SetWitchFlyState( ioBaseChar *pOwner, const WitchFlyInfo &rkWitchFlyInfo, bool bUseSkill );
	WitchFlyState GetWitchFlyState() { return m_WitchFlyState; }

	void ChangeWitchFlyStart( ioBaseChar *pOwner );
	void ChangeWitchFlyMove( ioBaseChar *pOwner, bool bSendNet );
	void ChangeWitchFlyCharge( ioBaseChar *pOwner );
	void ChangeWitchFlyAttack( ioBaseChar *pOwner );
	void ChangeWitchFlyCancel( ioBaseChar *pOwner );
	void ChangeWitchFlyEnd( ioBaseChar *pOwner );
	
	void ProcessWitchFlyState( ioBaseChar *pOwner );
	bool ProcessWitchFlyStart( ioBaseChar *pOwner );
	bool ProcessWitchFlyMove( ioBaseChar *pOwner );
	bool ProcessWitchFlyCancel( ioBaseChar *pOwner );

	void ProcessWitchFlyMoving( ioBaseChar *pOwner );

	void ProcessWitchJumpGrapMove( ioBaseChar *pOwner );
	bool ProcessWitchFlyLandAttack( ioBaseChar *pOwner );

	bool IsWitchFlyCharCollisionSkipState();

// witch Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitWitchGauge();
	void MaxWitchGauge();

	float GetCurWitchGauge();
	void SetCurWitchGauge( float fGauge );

	void SetEnableWitchEffect( ioBaseChar *pChar );
	void ReleaseEnableWitchEffect( ioBaseChar *pChar );

	void CheckEnableWitchGauge( ioBaseChar *pChar );
	
	bool IsEnableWitchGauge();
	bool IsWitchState();

	void ClearWitchState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void ProcessReserveDummyCreate( ioBaseChar *pOwner, const ioPlayMode* const pMode );

protected:
	void CreateEquipDummyChar( ioBaseChar *pOwner );
	void DestroyEquipDummyChar( ioBaseChar *pOwner );

	virtual bool CheckEnableMamaHahaCmd( ioBaseChar *pOwner, CmdUseType eUseType );

	bool CheckEnableOwnerAttach( ioBaseChar *pOwner );

//////////////////////////////////////////////////////////////////////////
//Aa¡¾a
public:
	void SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info );
	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	float GetGrapGravity();
	bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

protected:
	bool CheckLanding( ioBaseChar *pChar );
	void StartLandAttack( ioBaseChar *pOwner );
//////////////////////////////////////////////////////////////////////////

public:
	ioNakoruruItem();
	ioNakoruruItem( const ioNakoruruItem &rhs );
	virtual ~ioNakoruruItem();
};

inline ioNakoruruItem* ToNakoruruItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NAKORURU )
		return NULL;

	return dynamic_cast< ioNakoruruItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioNakoruruFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NAKORURU_FLY"; }
	virtual const CharState GetState(){ return CS_NAKORURU_FLY; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	
public:
	ioNakoruruFlySpecialState();
	virtual ~ioNakoruruFlySpecialState();
};

inline ioNakoruruFlySpecialState* ToNakoruruFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NAKORURU_FLY )
		return NULL;

	return dynamic_cast< ioNakoruruFlySpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioNakoruruJumpAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NAKORURU_JUMP_ATTACK"; }
	virtual const CharState GetState(){ return CS_NAKORURU_JUMP_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioNakoruruJumpAttackSpecialState();
	virtual ~ioNakoruruJumpAttackSpecialState();
};

inline ioNakoruruJumpAttackSpecialState* ToNakoruruJumpAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NAKORURU_JUMP_ATTACK )
		return NULL;

	return dynamic_cast< ioNakoruruJumpAttackSpecialState* >( pState );
}