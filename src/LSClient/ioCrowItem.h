#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioCrowItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum CrowState
	{
		BFS_NONE,
		BFS_SHADOW_CHARGING,

		BFS_WAIT,
		BFS_START,
		BFS_ATTACK,

		BFS_WARP,

		BFS_DASH_END,

		BFS_END,
	};

	enum SyncSubType
	{
		SST_SET_SPECIAL,
		SST_FLY_START,
		SST_FLY_END_ATTACK,
		SST_END,

		SST_CREATE_DASH_WEAPON,
		SST_WARP,

		SST_WARP_END_DELAY,
		SST_WARP_END_JUMP,

		SST_END_DASH,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	CrowState m_SpecialState;

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

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;

	CEncrypt<float> m_fCurGauge;

	DWORD m_dwMotionStartTime;
	DWORD m_dwPreDelayStartTime;
	DWORD m_dwMotionEndTime;

	DWORD m_dwSpecialStartTime;

	float m_fJumpRegHeight;

	float m_fFlyEndJumpAmt;

	ioHashString m_stLandingAni;
	float		 m_fLandingAniRate;

	ioHashString m_stBoardEffectName;
	DWORD			m_dwEffectIndex;

	DWORD	m_dwEnableJumpTime;

	DWORD	m_dwAttackKeyReservTime;

	//////////////////////////////////////////////////////////////////////////
	//지상
	DWORD m_dwSpecilWaitTime;

	ioHashString m_stSpecialWaitAni;
	float		 m_fSpecialWaitRate;

	ioHashString m_stSpecialFlyBuff;
	// Map Effect
	DWORD		m_dwMapEffectID;
	ioHashString	m_stMapEffect;

	DWORD        m_dwSpecailFlyKeyDelayTime;
	float		 m_fSpecialFlyForwardDistance;
	ioHashString m_stSpecialFlyAni;
	float		 m_fSpecialFlyAniRate;

	float		 m_fSpecialFlyAngle;
	float		 m_fSpecialFlyMaxRange;
	float		 m_fSpecialFlySpeed;

	DWORD		 m_dwSpecialWeaponIndex;

	//대쉬	
	//charging
	ioHashString m_stSpecialDashAni;
	float		 m_fSpecialDashAniRate;

	DWORD m_dwSpecialDashChargingStartTime;

	AttackAttribute m_StartAttribute;
	AttackAttribute m_EndAttribute;

	D3DXVECTOR3 m_vLookDir;
	D3DXVECTOR3 m_vShadowFirePrePos;
	float   m_fFireDistanceGap;
	float   m_fSpecailDashEndWarpLookOffSet;

	ioHashString m_stSpecialDashFindBuffName;

	float   m_fCurShadowHeight;
	float   m_fCheckSpecialHeight;
	DWORD   m_dwShadowFireTicTime;
	DWORD   m_dwNextShadowFireTime;

	int     m_nMaxShadowFireCnt;
	int		m_nCurShadowFireCnt;

	int     m_nShadowWeaponIndex;

	ioHashString m_stSpecailWarpMotion;
	float		 m_fSpecailWarpMotionRate;

	float   m_fSpecailDashEndJumpAmt;
	//////////////////////////////////////////////////////////////////////////

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadFlyInfo( ioINILoader &rkLoader );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void CheckCreateEffect( ioBaseChar *pChar );

	bool CheckAttackKey( ioBaseChar *pOwner );

	bool IsDropZone( ioBaseChar *pOwner );

	void AddBuff( ioBaseChar *pOwner );
	void EraseBuff( ioBaseChar *pOwner );
	void DestroySpecialEffect( ioBaseChar *pOwner );

public:
	void SetSpecialState( ioBaseChar *pOwner, bool bDash, bool bSendNet );
	CrowState GetSpecialState() { return m_SpecialState; }
	//지상
	void ChangeWait( ioBaseChar *pOwner );
	void ChangeFlyStart( ioBaseChar *pOwner, bool bSendNet );
	void ChangeFlyEndAttack( ioBaseChar *pOwner, bool bSendNet  );

	//대쉬
	void ChangeShadowDashChargingStart( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );
	void ChangeDashSpecialEnd( ioBaseChar *pOwner );
	void EraseBuff_ForNetWork( ioBaseChar *pOwner );
	void ChangeDashSpecialEndWarp( ioBaseChar *pOwner, const D3DXVECTOR3& vWarpPos, const D3DXVECTOR3& vLook );
	void ChangeDashSpecialEndJump( ioBaseChar *pOwner );

	void KillSpecailDashWeapon( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	//bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();

	inline const int GetSpecialState() const { return (int)m_SpecialState; }

	void SpecialWeaponKill( ioBaseChar *pOwner );

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool CheckLanding( ioBaseChar *pChar );



	void CreateShadowWeapon( ioBaseChar *pOwner, const D3DXVECTOR3& vPos, bool bSendNet );
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
	void CheckDecreaseGauge();

public:
	void InitBladeGauge();

public:
	ioCrowItem();
	ioCrowItem( const ioCrowItem &rhs );
	virtual ~ioCrowItem();
};

inline ioCrowItem* ToCrowItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CROW_ITEM )
		return NULL;

	return dynamic_cast< ioCrowItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioCrowItemSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_CROW_SPECIAL_STATE"; }
	virtual const CharState GetState(){ return CS_CROW_SPECIAL_STATE; }	
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
	virtual bool IsNoDropState() { return true; }

public:
	ioCrowItemSpecialState();
	virtual ~ioCrowItemSpecialState();
};

inline ioCrowItemSpecialState* ToCrowItemSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_CROW_SPECIAL_STATE )
		return NULL;

	return dynamic_cast< ioCrowItemSpecialState* >( pState );
}