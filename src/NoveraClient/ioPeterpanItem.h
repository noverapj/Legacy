#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPeterpanItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum PeterpanFlyState
	{
		PFS_NONE,
		PFS_START,
		PFS_MOVE,
		PFS_END,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_EFFECT,
		SST_FLY_JUMP,//잡아와서 점프를 뛸때
		SST_FLY_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	PeterpanFlyState m_FlyState;
	//LuciferFlyAttackState m_FlyAttackState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// move
	DWORD m_dwFlyStartTime;
	DWORD m_dwFlyMoveTime;
	DWORD m_dwFlyAttackMoveStartTime;
	DWORD m_dwFlyAttackMoveEndTime;
	DWORD m_dwFlyAttackEndTime;
	DWORD m_dwFlyAttackTime;

	CEncrypt<float> m_fFlyCurRange;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	CEncrypt<bool> m_bSetUpKey;
	CEncrypt<bool> m_bSetDownKey;

	D3DXVECTOR3 m_vFlyForceDir;
	DWORD m_dwFlyForceTime;

	D3DXVECTOR3 m_vFlyAttackMoveDir;
	CEncrypt<float> m_fFlyAttackMoveSpeed;

	//
	ioHashString m_Target;
	ioHashString m_TargetSkill;

	//
	PeterpanFlyInfo m_AttackFly;
	PeterpanFlyInfo m_CurFly;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

	ioHashString m_EnableEffect;
	UniqueObjID m_dwEnableEffect;

	UniqueObjID m_dwFlyMoveEffect;

	//->->
	//AttackAttribute m_DashAttack;


	DWORD m_dwMotionStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwPreDelayStartTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwMotionEndTime;


	DWORD m_dwJumpDashDelayTime;

	DWORD m_dwFlyDashAttackWeaponIndex;
	bool m_bFlyDashAttackToTarget;//타겟이 있어서 그넘한태 갈때

	//특수 공격시 이동, 회전 옵션
	//bool	m_bSpecialAttackRotate;
	//float	m_fSpecialAttackSpeed;

	//////////////////////////////////////////////////////////////////////////
	DWORD m_dwAreaWeaponIndex;
	ioHashString m_dwAreaWeaponName;

	bool  m_bWarningLanding;
	DWORD m_dwWarningLandTime;

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
	void LoadFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	//void CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove = false );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

	void DestroyAreaWeapon();

public:
	void SetFlyState( ioBaseChar *pOwner, const PeterpanFlyInfo &rkFlyInfo, bool bSendNet );
	PeterpanFlyState GetFlyState() { return m_FlyState; }
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyMove( ioBaseChar *pOwner );
	void ChangeFlyAttack( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	//void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	//bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();

	inline const int GetFlyState() const { return (int)m_FlyState; }
	inline const D3DXVECTOR3& GetFlyAttMoveDir() const { return m_vFlyAttackMoveDir; }
	inline const float GetFlyAttMoveSpeed() const { return m_fFlyAttackMoveSpeed; }

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void SetFlyMoveEffect( ioBaseChar *pChar );
	void UpdateFlyMoveEffect( ioBaseChar *pChar );
	void ReleaseFlyMoveEffect( ioBaseChar *pChar );

	void UpdateGuidEffect( ioBaseChar *pChar );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	void SetEnableEffect( ioBaseChar *pChar );
	void ReleaseEnableEffect( ioBaseChar *pChar );

	void CheckEnableGauge( ioBaseChar *pChar );

	bool IsEnableGauge();
	bool IsPeterpanState();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetTargetSkill();
	void ClearTargetSkill();

	bool IsBallTarget();

	void InitPeterpanGauge();

public:
	ioPeterpanItem();
	ioPeterpanItem( const ioPeterpanItem &rhs );
	virtual ~ioPeterpanItem();
};

inline ioPeterpanItem* ToPeterpanItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PETERPAN )
		return NULL;

	return dynamic_cast< ioPeterpanItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioPeterpanFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_PETERPAN_FLY"; }
	virtual const CharState GetState(){ return CS_PETERPAN_FLY; }	
	virtual bool IsProcessFlyMove(){ return true; }	

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
	ioPeterpanFlySpecialState();
	virtual ~ioPeterpanFlySpecialState();
};

inline ioPeterpanFlySpecialState* ToPeterpanFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_PETERPAN_FLY )
		return NULL;

	return dynamic_cast< ioPeterpanFlySpecialState* >( pState );
}