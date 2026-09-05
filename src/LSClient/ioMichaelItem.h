#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMichaelItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum MichaelFlyState
	{
		MFS_NONE,
		MFS_START,
		MFS_ATTACK_MOVE,
		MFS_GRAPPLING_LOOP,
		MFS_GRAPPLING_MOVE,
		MFS_END_ATTACK,
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

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

protected:
	MichaelFlyState m_MichaelFlyState;

	CEncrypt<float> m_fMichaelFlyCurRange;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	MichaelFlyInfo m_AttackMichaelFly;
	MichaelFlyInfo m_CurMichaelFly;

	// start
	DWORD m_dwMichaelFlyStartTime;

	// attack move
	DWORD m_dwMichaelFlyAttackMoveTime;

	// grappling loop
	DWORD m_dwMichaelFlyGrapplingLoopTime;
	DWORD m_dwMichaelFlyGrapplingLoopDuration;
	DWORD m_dwMichaelFlyGrapplingEnableMoveTime;

	// grappling move
	bool m_bGrapplingDirMove;
	DWORD m_dwMichaelFlyGrapplingMoveTime;

	// end attack
	DWORD m_dwMichaelFlyEndAttackTime;

	// end
	ioHashString m_MichaelFlyEndAni;
	float m_fMichaelFlyEndAniRate;

	DWORD m_dwMichaelFlyEndTime;
	
	D3DXVECTOR3 m_vFlyMoveDir;
	CEncrypt<float> m_fFlyMoveSpeed;

	//
	ioHashString m_MichaelTarget;
	ioHashString m_MichaelTargetSkill;

	ioHashString m_MichaelGrapplingTarget;

	//
	ioHashString m_MichaelAttackMoveEffect;
	ioHashString m_MichaelGrapplingMoveEffect;
	ioHashString m_MichaelGrapplingDirMoveEffect;

protected:
	bool m_bInitMichaelGaugeDown;

	CEncrypt<float> m_fMaxMichaelGauge;
	CEncrypt<float> m_fNeedMichaelGauge;

	CEncrypt<float> m_fIncreaseMichaelGauge;
	CEncrypt<float> m_fDecreaseMichaelGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseMichaelGauge;

	CEncrypt<float> m_fCurMichaelGauge;

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
	void LoadMichaelFlyInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

public:
	void SetMichaelFlyState( ioBaseChar *pOwner, const MichaelFlyInfo &rkFlyInfo, bool bSendNet );
	void SetMichaelFlyStateByJumpDash( ioBaseChar *pOwner, const MichaelFlyInfo &rkFlyInfo );
	
	void ChangeMichaelFlyStart( ioBaseChar *pOwner );
	void ChangeMichaelFlyAttackMove( ioBaseChar *pOwner );
	void ChangeMichaelFlyGrapplingLoop( ioBaseChar *pOwner, ioBaseChar *pTarget );
	void ChangeMichaelFlyGrapplingMove( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDirKey );
	void ChangeMichaelFlyEndAttack( ioBaseChar *pOwner );
	void ChangeMichaelFlyEnd( ioBaseChar *pOwner );
	
	void ProcessMichaelFlyState( ioBaseChar *pOwner );
	bool ProcessMichaelFlyStart( ioBaseChar *pOwner );

	void ProcessMichaelFlyMove( ioBaseChar *pOwner );

	bool IsMichaelFlyCharCollisionSkipState();

	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	inline const int GetMichaelFlyState() const { return (int)m_MichaelFlyState; }
	inline const D3DXVECTOR3& GetMichaelFlyMoveDir() const { return m_vFlyMoveDir; }
	inline const float GetMichaelFlyMoveSpeed() const { return m_fFlyMoveSpeed; }

protected:
	void CheckMichaelFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pOwner );

	bool CheckGrapplingTargetValidate( ioBaseChar *pOwner );

	void AttachAttackMoveEffect( ioBaseChar *pOwner );
	void ReleaseAttackMoveEffect( ioBaseChar *pOwner );

	void AttachGrapplingMoveEffect( ioBaseChar *pOwner );
	void ReleaseeGrapplingMoveEffect( ioBaseChar *pOwner );

// michael Gauge
public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

	void CheckDecreaseGauge();

	//
	void CheckEnableMichaelGauge( ioBaseChar *pChar );
	
	bool IsEnableMichaelGauge();
	bool IsMichaelFlyState();

	void ClearMichaelFlyState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetMichaelTargetSkill();
	void ClearMichaelTargetSkill();

	bool IsBallTarget();

public:
	ioMichaelItem();
	ioMichaelItem( const ioMichaelItem &rhs );
	virtual ~ioMichaelItem();
};

inline ioMichaelItem* ToMichaelItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MICHAEL )
		return NULL;

	return dynamic_cast< ioMichaelItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMichaelFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MICHAEL_FLY"; }
	virtual const CharState GetState(){ return CS_MICHAEL_FLY; }
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
	ioMichaelFlySpecialState();
	virtual ~ioMichaelFlySpecialState();
};

inline ioMichaelFlySpecialState* ToMichaelFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MICHAEL_FLY )
		return NULL;

	return dynamic_cast< ioMichaelFlySpecialState* >( pState );
}