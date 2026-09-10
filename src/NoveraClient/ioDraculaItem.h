#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDraculaItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum FlyState
	{
		LFS_NONE,
		LFS_START,
		LFS_DELAY,
		LFS_END,
	};


	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,//플라이 상태일때 상승 하강, 회전등을 처리할때
		SST_EFFECT,
		SST_FLY_ATTACK_START,//UroborusAttack시전을 시작할때
		SST_FLY_ATTACK_GRAP,//근처에 왔을때 잡기 공격 시전할때
		SST_FLY_ATTACK_JUMP,//잡아와서 점프를 뛸때
		SST_FLY_DASH_ATTACK,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	FlyState m_FlyState;

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


	//
	ioHashString m_Target;
	ioHashString m_TargetSkill;

	//
	DraculaFlyInfo m_AttackFly;
	DraculaFlyInfo m_CurFly;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;

	CEncrypt<float> m_fCurGauge;

	//지상 공중 판정 높이
	float			m_fCheckLandHeight;
	float			m_fAirJumpPower;

	//변신 시간
	DWORD			m_dwMaxSpecialTime;
	DWORD			m_dwSpecialStartTime;

	// Map Effect
	DWORD		m_dwMapEffectID;
	ioHashString	m_stMapEffect;

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


public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner, bool bOptionMove = false );

	void DecreaseGauge();
	void AddBuff( ioBaseChar *pOwner );
	void EraseBuff( ioBaseChar *pOwner );

	void ChangeLandNormalAttack( ioBaseChar *pOwner );
	void ChangeAirNornmalAttack( ioBaseChar *pOwner, bool bAttack );

public:
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );
	void CheckFlyState( ioBaseChar *pOwner, bool bAttack );

	//Delay->move로 바뀔때 혹은 Up_Down일때
	void ChangeFlyMove( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner );

	bool IsFlyCharCollisionSkipState();
	
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
	bool IsDraculaState();
	void ClearState( ioBaseChar *pOwner, bool bEraseBuff = false );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	void SetFlyState( ioBaseChar *pOwner, const DraculaFlyInfo &rkFlyInfo, bool bSendNet );

public:
	const ioHashString& GetTargetSkill();
	void ClearTargetSkill();

	bool IsBallTarget();

	void InitLuciferGauge();

public:
	ioDraculaItem();
	ioDraculaItem( const ioDraculaItem &rhs );
	virtual ~ioDraculaItem();
};

inline ioDraculaItem* ToDraculaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DRACULA )
		return NULL;

	return dynamic_cast< ioDraculaItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioDraculaFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DRACULA_FLY"; }
	virtual const CharState GetState(){ return CS_DRACULA_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioDraculaFlySpecialState();
	virtual ~ioDraculaFlySpecialState();
};

inline ioDraculaFlySpecialState* ToDraculaFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DRACULA_FLY )
		return NULL;

	return dynamic_cast< ioDraculaFlySpecialState* >( pState );
}