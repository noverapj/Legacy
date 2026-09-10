#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDevilItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum DevilFlyState
	{
		DFS_NONE,
		DFS_START,
		DFS_MOVE,
		DFS_FLY_ATTACK,
		DFS_END,
	};

	//LFS_FLY_ATTACK상황 안의 상황
	enum DevilFlyAttackState
	{
		DFAS_NONE,
		DFAS_DELAY,
		DFAS_ATTACK,
		DFAS_ATTACKED,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,//플라이 상태일때 상승 하강, 회전등을 처리할때
		SST_EFFECT,
		SST_FLY_ATTACK_START,//UroborusAttack시전을 시작할때
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DevilFlyState m_FlyState;
	DevilFlyAttackState m_FlyAttackState;

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

	//
	DevilFlyInfo m_AttackFly;
	DevilFlyInfo m_CurFly;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGauge;

	CEncrypt<float> m_fCurGauge;

	DWORD m_dwFireBallAttackEndTime;


	DWORD m_dwMotionStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwPreDelayStartTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwMotionEndTime;

	DWORD m_dwNextFireTime;


	//특수 공격시 이동, 회전 옵션
	bool	m_bSpecialAttackRotate;
	float	m_fSpecialAttackSpeed;

	DWORD	m_dwFlyCancelStartTime;
	bool	m_bAttackKeyDown;

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
	void CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove = false );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	

public:
	void SetFlyState( ioBaseChar *pOwner, const DevilFlyInfo &rkFlyInfo, bool bSendNet );
	DevilFlyState GetFlyState() { return m_FlyState; }
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyAttack( ioBaseChar *pOwner );
	void FlyFireBallAttackFire( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );

	//Delay->move로 바뀔때 혹은 Up_Down일때
	void ChangeFlyMove( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();

	void DecreaseFlyDashGauge( float fGauge );

	inline const int GetFlyState() const { return (int)m_FlyState; }

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckFlyAutoFire( ioBaseChar *pOwner );

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
	bool IsDevilFlyState();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );


public:
	ioDevilItem();
	ioDevilItem( const ioDevilItem &rhs );
	virtual ~ioDevilItem();
};

inline ioDevilItem* ToDevilItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DEVIL)
		return NULL;

	return dynamic_cast< ioDevilItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioDevilFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DEVIL_FLY"; }
	virtual const CharState GetState(){ return CS_DEVIL_FLY; }
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
	ioDevilFlySpecialState();
	virtual ~ioDevilFlySpecialState();
};

inline ioDevilFlySpecialState* ToDevilFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DEVIL_FLY )
		return NULL;

	return dynamic_cast< ioDevilFlySpecialState* >( pState );
}