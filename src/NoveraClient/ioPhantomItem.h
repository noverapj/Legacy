#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPhantomItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum PhantomFlyState
	{
		PFS_NONE,
		PFS_WAIT,
		PFS_START,
		PFS_MOVE,
		PFS_TRACE_MOVE,
		PFS_POSSESSION,
		PFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_MOVE_KEY,
		SST_EFFECT,
		SST_MARKER,
		SST_POSSESSION_KEY,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	PhantomFlyState m_PhantomFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// phantomfly
	PhantomFlyInfo m_AttackPhantomFly;
	PhantomFlyInfo m_CurPhantomFly;

	DWORD m_dwPhantomFlyEnableTime;

	// wait
	DWORD m_dwPhantomFlyWaitTime;

	// start
	DWORD m_dwPhantomFlyStartTime;
	CEncrypt<float> m_fPhantomFlyStartRange;

	// delay & move
	ioHashString m_CurMoveAni;

	// target
	ioHashString m_PhantomTarget;
	ioHashString m_PhantomTargetSkill;

	// trace
	D3DXVECTOR3 m_vPhantomFlyTraceDir;
	CEncrypt<float> m_fPhantomFlyTraceRange;

	// possession

	// end

	// key
	bool m_bTargetRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	// phantom gauge
	CEncrypt<float> m_fMaxPhantomGauge;
	CEncrypt<float> m_fNeedPhantomGauge;

	CEncrypt<float> m_fIncreasePhantomGauge;
	CEncrypt<float> m_fDecreasePhantomGauge;
	CEncrypt<float> m_fFirstDecreasePhantomGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreasePhantomGauge;
	CEncrypt<float> m_fPossessionDecreaseGauge;

	CEncrypt<float> m_fCurPhantomGauge;

	// etc
	bool m_bUsePhantomFly;

	ioHashString m_EnablePhantomEffect;
	UniqueObjID m_dwEnablePhantomEffect;

	UniqueObjID m_dwFlyMoveEffect;

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

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadPhantomFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );

	void CheckPhantomFlyMoveAni( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CheckPossessionKeyInput( ioBaseChar *pOwner );

public:
	void SetPhantomFlyState( ioBaseChar *pOwner, const PhantomFlyInfo &rkFlyInfo, bool bSendNet );
	void SetPhantomFlyStateAttack( ioBaseChar *pOwner );

	void ChangePhantomFlyStart( ioBaseChar *pOwner );
	void ChangePhantomFlyMove( ioBaseChar *pOwner );
	void ChangePhantomFlyTrace( ioBaseChar *pOwner );
	void ChangePhantomFlyPossession( ioBaseChar *pOwner, const ioHashString &szTargetName );
	void ChangePhantomFlyEnd( ioBaseChar *pOwner, bool bEndBuff );
	
	void ProcessPhantomFlyState( ioBaseChar *pOwner );

	bool ProcessPhantomFlyStart( ioBaseChar *pOwner );
	bool ProcessPhantomFlyMove( ioBaseChar *pOwner );
	bool ProcessPhantomFlyTrace( ioBaseChar *pOwner );
	bool ProcessPhantomFlyTraceMove( ioBaseChar *pOwner );
	bool ProcessPhantomFlyPossession( ioBaseChar *pOwner );

	bool CheckPhantomFlyEnableTime();
	bool IsPhantomFlyCharCollisionSkipState();
	bool IsPhantomFlyWeaponCollisionSkipState();
	bool IsEnablePossessionState();

	inline const int GetPhantomFlyState() const { return (int)m_PhantomFlyState; }
	ioUserKeyInput::DirKeyInput GetCurDirKey();

protected:
	void CheckPhantomFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void CheckFlyAttTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );

	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool IsEnableTargetTraceState( ioBaseChar *pTarget );
	bool IsEnableTargetPossessionState( ioBaseChar *pTarget );

	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CheckTraceTargetValidate( ioBaseChar *pOwner );
	bool CheckPossessionTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

	void EquipPhantomMesh( ioBaseChar *pOwner );
	void ReleasePhantomMesh( ioBaseChar *pOwner );

	void UpdatePhantomFlyMoveEffect( ioBaseChar *pChar );
	void ReleasePhantomFlyMoveEffect( ioBaseChar *pChar );

// phantom Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitPhantomGauge();
	void MaxPhantomGauge();

	float GetCurPhantomGauge();
	void SetCurPhantomGauge( float fGauge );
	void DecreasePhantomGauge( float fGauge );

	void SetEnablePhantomEffect( ioBaseChar *pChar );
	void ReleaseEnablePhantomEffect( ioBaseChar *pChar );

	void CheckEnablePhantomGauge( ioBaseChar *pChar );
	
	bool IsEnablePhantomGauge();
	bool IsPhantomState();
	void ClearPhantomState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetPhantomTarget();
	const ioHashString& GetPhantomTargetSkill();
	void ClearPhantomTargetSkill();

	bool IsBallTarget();

public:
	ioPhantomItem();
	ioPhantomItem( const ioPhantomItem &rhs );
	virtual ~ioPhantomItem();
};

inline ioPhantomItem* ToPhantomItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PHANTOM )
		return NULL;

	return dynamic_cast< ioPhantomItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioPhantomFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_PHANTOM_FLY"; }
	virtual const CharState GetState(){ return CS_PHANTOM_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

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
	ioPhantomFlySpecialState();
	virtual ~ioPhantomFlySpecialState();
};

inline ioPhantomFlySpecialState* ToPhantomFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_PHANTOM_FLY )
		return NULL;

	return dynamic_cast< ioPhantomFlySpecialState* >( pState );
}