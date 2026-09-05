#pragma once

#include "ioSpecialStateBase.h"

class ioNeoItem :	public ioWeaponItem
{
protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_EFFECT_CHARGING,
		CS_EFFECT_ATTACK,
		CS_NORMAL_ATTACK,
	};
	enum NeoFlyState
	{
		SFS_NONE,
		SFS_READY,
		SFS_START,
		SFS_DELAY,
		SFS_MOVE,
		SFS_ATTACK_MOVE,
		SFS_FIRE,
		SFS_CHARGING,
		SFS_GATHERING,
		SFS_CHARGE_ATTACK,
		SFS_CHARGE_EFFECT,
		SFS_END,
	};
	enum SyncType
	{
		ST_NONE,
		ST_SET_TARGET,
		ST_SET_DIR,
		ST_NEO_FLY,
		ST_FLY_FIRE,
	};
	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_LANDING,	//이동중 지형과 충돌
		MS_END,		//이동 완료
	};

public:
	enum OBotState
	{
		OBS_NORMAL,
		OBS_JUMP,
		OBS_DASH,
		OBS_SPECIAL,
	};

protected:
	typedef struct tagNeoFlyInfo
	{
		//start 
		ioHashString m_StartAni;
		float m_fStartAniRate;

		float m_fStartAngle;
		float m_fStartMaxRange;
		DWORD m_dwStartDuration;

		//delay
		ioHashString m_DelayAni;
		float m_fDelayAniRate;

		//move
		ioHashString m_MoveAniFr;
		ioHashString m_MoveAniBk;
		ioHashString m_MoveAniRt;
		ioHashString m_MoveAniLt;
		float m_fMoveAniRate;
		float m_fMoveForceAmt;
		float m_fMoveForceFric;

		//Attack Move
		AttackAttribute m_AttackMove;
		float m_fAttackMoveSpeed;
		float m_fAttackMoveRange;

		//target
		TargetWoundType m_TargetWoundType;
		float m_fTargetAngle;
		float m_fTargetRange;
		float m_fTargetUpHeight;
		float m_fTargetDownHeight;
		float m_fTargetMarkerRange;
		float m_fFlyJumpAmt;

		tagNeoFlyInfo()
		{
			Init();
		}

		void Init()
		{
			m_fStartAniRate = FLOAT1;

			m_fStartAngle = 0.0f;
			m_fStartMaxRange = 0.0f;
			m_dwStartDuration = 0;

			m_fDelayAniRate = FLOAT1;
			m_fMoveAniRate = FLOAT1;
			m_fMoveForceAmt = 0.0f;
			m_fMoveForceFric = FLOAT1;

			m_TargetWoundType = TWT_DOWN_WOUND_EXCEPT;
			m_fTargetAngle = 30.0f;
			m_fTargetRange = 30.0f;
			m_fTargetUpHeight = 30.0f;
			m_fTargetDownHeight = 30.0f;

			m_fAttackMoveSpeed = 0.0f;
			m_fAttackMoveRange = 0.0f;
			m_fFlyJumpAmt = 0.0f;
			m_AttackMove.Init();
		}
	} NeoFlyInfo;

	struct EffectInfo
	{
		DWORD m_dwEffectID;
		int m_iPosIndex;
	};
	typedef std::vector< EffectInfo > EffectInfoVec;

protected:
	ChargeState m_ChargeState;
	NeoFlyState m_FlyState;
	OBotState m_OBotState;

	//Normal Attack
	ioHashStringVec m_AttackReadyAniList;
	int m_iCurCombo;

	DWORD		m_dwMotionStartTime;
	DWORD		m_dwMotionEndTime;

	//target
	ioHashString		m_szTargetName;
	float				m_fCurTargetRange;
	float				m_fCurTargetAngle;
	float				m_fCurTargetUpHeight;
	float				m_fCurTargetDownHeight;
	TargetWoundType		m_CurTargetWoundType;

	float				m_fLandingRate;
	float				m_fEnableEndJumpHeight;
	
	ioHashString m_szGatheringAni;
	float m_fGatheringAniRate;

	AttackAttribute m_NormalEffectAttribute;
	AttackAttribute m_JumpEffectAttribute;
	AttackAttribute m_DashEffectAttribute;
	AttackAttribute m_FlyEffectAttribute;

protected:
	NeoFlyInfo	m_FlyInfo;
	NeoFlyInfo	m_JumpFlyInfo;
	NeoFlyInfo	m_CurFlyInfo;

	float m_fFlyCurRange;
	D3DXVECTOR3 m_vFlyForceDir;
	DWORD m_dwFlyForceTime;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	D3DXVECTOR3 m_vFlyMoveDir;
	float		m_fFlyMoveSpeed;

	ValkyrieFlyInfo	m_FlyFireInfo;
	ioHashString m_szFlyChargingAni;
	float m_fFlyChargingAniRate;
	ioHashString m_szFlyGatheringAni;
	float m_fFlyGatheringAniRate;

	DWORD m_dwUpStartTime;
	float m_fUpCurrRange;

	IntVec m_WeaponIndexList;
	D3DXVECTOR3	m_vAttackDir;

	ioHashString m_szChargeEffectAni;
	ioHashString m_szJumpChargeEffectAni;
	ioHashString m_szDashChargeEffectAni;
	ioHashString m_szFlyChargeEffectAni;
	float m_fChargeEffectAniRate;
	float m_fChargeEffectEndJumpAmt;

	bool m_bEffectAttackKey;
	DWORD m_dwEffectAttackRepeatTime;

	ioHashString m_szEffectName;
	Vector3Vec m_vEffectOffsetList;
	EffectInfoVec m_vEffectInfoList;
	int m_iCurEffectChargeIndex;

	ioHashString m_szBoardEffectName;
	DWORD m_dwBoardEffectIndex;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;
	CEncrypt<float> m_fCurGauge;

	DWORD m_dwGatheringTime;
	DWORD m_dwFlyChargeTime;
	DWORD m_dwFlyGatheringTime;

protected:
	void ClearData();

	void LoadFlyInfo( ioINILoader &rkLoader );
	void LoadJumpFlyInfo( ioINILoader &rkLoader );
	void LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo );

	void ApplyFly( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );

	void CreateBoardEffect( ioBaseChar *pChar );

protected:
	void ProcessFlyStartState( ioBaseChar *pOwner );
	bool ProcessFlyStartMoving( ioBaseChar *pOwner );
	void ProcessFlyDelayState( ioBaseChar *pOwner );
	void ProcessFlyMoveState( ioBaseChar *pOwner );
	void ProcessFlyMoving( ioBaseChar *pOwner );
	void ProcessFlyChargingState( ioBaseChar *pOwner );
	void ProcessFlyGatheringState( ioBaseChar *pOwner );

	void CheckFlyKeyInput( ioBaseChar *pOwner );
	void CheckFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyMove( ioBaseChar *pOwner );
	void ChangeFlyAttackMove( ioBaseChar *pOwner );
	void ChangeFlyComboAttackChargingState( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );
	void ChangeFlyFireState( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eDir );
	void ChangeFlyChargingState( ioBaseChar *pOwner );
	void ChangeFlyGatheringState( ioBaseChar *pOwner );
	void ChangeGatheringState( ioBaseChar *pOwner );

	void CheckAttTarget( ioBaseChar *pOwner );
	void FindAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );
	void SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr );

	ioNeoItem::MoveState ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding );
	void ProcessWeapon( ioBaseChar *pOwner );
	void RemoveWeapon( ioBaseChar *pOwner );

	void CreateEffectChargeEffect( ioBaseChar *pOwner );
	void RemoveEffectList( ioPlayStage *pStage );
	D3DXVECTOR3 RemoveEffect( ioBaseChar *pOwner );

public:
	bool IsCollisionSkipState();

	void SetFlyReadyState( ioBaseChar *pOwner, bool bJumpState );
	void SetFlyState( ioBaseChar *pOwner );
	void ProcessFlyState( ioBaseChar *pOwner );
	void ClearFlyData( ioBaseChar *pOwner =NULL );
	void ProcessAttackState( ioBaseChar *pOwner );
	void ClearAttackState( ioBaseChar *pOwner );

	bool IsEnableChargeState( bool bHalf );
	void SetAttackState( ioBaseChar *pOwner, bool bSendNet, int iState );
	void SetHalfAttackState( ioBaseChar *pOwner, bool bSendNet, int iState );
	void SetFlyAttackState( ioBaseChar *pOwner, bool bSendNet );
	void SetFlyHalfAttackState( ioBaseChar *pOwner, bool bSendNet );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnReleasedByEndGame( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );

	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual WeaponSubType GetSubType() const;

	virtual void GetAutoTargetValue( ioBaseChar *pOwner, float &fRange, float &fMinAngle, float &fMaxAngle,	AutoTargetType eType );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CallItemProcess( ioBaseChar* pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

public:
	ioNeoItem(void);
	ioNeoItem( const ioNeoItem &rhs );
	virtual ~ioNeoItem(void);
};

inline ioNeoItem* ToNeoItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NEO_ITEM )
		return NULL;

	return dynamic_cast< ioNeoItem* >( pItem );
}
//-----------------------------------------------------------------------------------------------------------
class ioNeoFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NEO_FLY"; }
	virtual const CharState GetState(){ return CS_NEO_FLY; }	
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
	ioNeoFlySpecialState();
	virtual ~ioNeoFlySpecialState();
};

inline ioNeoFlySpecialState* ToNeoFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NEO_FLY )
		return NULL;

	return dynamic_cast< ioNeoFlySpecialState* >( pState );
}

class ioNeoAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NEO_ATTACK"; }
	virtual const CharState GetState(){ return CS_NEO_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	ioNeoAttackSpecialState();
	virtual ~ioNeoAttackSpecialState();
};

inline ioNeoAttackSpecialState* ToNeoAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NEO_ATTACK )
		return NULL;

	return dynamic_cast< ioNeoAttackSpecialState* >( pState );
}