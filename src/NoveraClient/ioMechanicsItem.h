#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMechanicsItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum MechanicsFlyState
	{
		MFS_NONE,
		MFS_START,
		MFS_DELAY,
		MFS_MOVE,
		MFS_ATTACK_MOVE,
		MFS_ATTACK,
		MFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,
		SST_EFFECT,
		SST_MARKER,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	MechanicsFlyState m_MechanicsFlyState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// move
	DWORD m_dwMechanicsFlyStartTime;
	DWORD m_dwMechanicsFlyMoveTime;
	DWORD m_dwMechanicsFlyAttackMoveTime;
	DWORD m_dwMechanicsFlyAttackEndTime;

	CEncrypt<float> m_fMechanicsFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	D3DXVECTOR3 m_vFlyForceDir;
	DWORD m_dwFlyForceTime;

	D3DXVECTOR3 m_vFlyAttackMoveDir;
	CEncrypt<float> m_fFlyAttackMoveSpeed;

	//
	ioHashString m_MechanicsTarget;
	ioHashString m_MechanicsTargetSkill;

	//
	MechanicsFlyInfo m_AttackMechanicsFly;
	MechanicsFlyInfo m_CurMechanicsFly;

	// mechanics gauge
	CEncrypt<float> m_fMaxMechanicsGauge;
	CEncrypt<float> m_fNeedMechanicsGauge;

	CEncrypt<float> m_fIncreaseMechanicsGauge;
	CEncrypt<float> m_fDecreaseMechanicsGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseMechanicsGauge;

	CEncrypt<float> m_fCurMechanicsGauge;

	ioHashString m_EnableMechanicsEffect;
	UniqueObjID m_dwEnableMechanicsEffect;

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
	void LoadMechanicsFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	
	void OnCharging( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );

	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CheckMechanicsFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

public:
	void SetMechanicsFlyState( ioBaseChar *pOwner, const MechanicsFlyInfo &rkFlyInfo, bool bSendNet );
	MechanicsFlyState GetMechanicsFlyState() { return m_MechanicsFlyState; }
	void ChangeMechanicsFlyStart( ioBaseChar *pOwner );
	void ChangeMechanicsFlyDelay( ioBaseChar *pOwner );
	void ChangeMechanicsFlyMove( ioBaseChar *pOwner );
	void ChangeMechanicsFlyAttackMove( ioBaseChar *pOwner );
	void ChangeMechanicsFlyAttack( ioBaseChar *pOwner );
	void ChangeMechanicsFlyEnd( ioBaseChar *pOwner );
	
	void ProcessMechanicsFlyState( ioBaseChar *pOwner );
	bool ProcessMechanicsFlyStart( ioBaseChar *pOwner );
	bool ProcessMechanicsFlyMove( ioBaseChar *pOwner );

	bool IsMechanicsFlyCharCollisionSkipState();

	inline const int GetMechanicsFlyState() const { return (int)m_MechanicsFlyState; }
	inline const D3DXVECTOR3& GetMechanicsFlyAttMoveDir() const { return m_vFlyAttackMoveDir; }
	inline const float GetMechanicsFlyAttMoveSpeed() const { return m_fFlyAttackMoveSpeed; }

protected:
	void CheckMechanicsFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void CheckFlyAttTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

	void SetMechanicsFlyMoveEffect( ioBaseChar *pChar );
	void UpdateMechanicsFlyMoveEffect( ioBaseChar *pChar );
	void ReleaseMechanicsFlyMoveEffect( ioBaseChar *pChar );

// mechanics Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitMechanicsGauge();
	void MaxMechanicsGauge();

	float GetCurMechanicsGauge();
	void SetCurMechanicsGauge( float fGauge );

	void SetEnableMechanicsEffect( ioBaseChar *pChar );
	void ReleaseEnableMechanicsEffect( ioBaseChar *pChar );

	void CheckEnableMechanicsGauge( ioBaseChar *pChar );
	
	bool IsEnableMechanicsGauge();
	bool IsMechanicsState();
	void ClearMechanicsState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetMechanicsTargetSkill();
	void ClearMechanicsTargetSkill();

	bool IsBallTarget();

public:
	ioMechanicsItem();
	ioMechanicsItem( const ioMechanicsItem &rhs );
	virtual ~ioMechanicsItem();
};

inline ioMechanicsItem* ToMechanicsItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MECHANICS )
		return NULL;

	return dynamic_cast< ioMechanicsItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMechanicsFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MECHANICS_FLY"; }
	virtual const CharState GetState(){ return CS_MECHANICS_FLY; }
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
	ioMechanicsFlySpecialState();
	virtual ~ioMechanicsFlySpecialState();
};

inline ioMechanicsFlySpecialState* ToMechanicsFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MECHANICS_FLY )
		return NULL;

	return dynamic_cast< ioMechanicsFlySpecialState* >( pState );
}