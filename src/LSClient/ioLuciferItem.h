#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioLuciferItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum LuciferFlyState
	{
		LFS_NONE,
		LFS_START,
		LFS_DELAY,
		//LFS_MOVE,
		//LFS_DASH_ATTACK_READY,
		LFS_DASH_ATTACK_MOVE,
		//LFS_ATTACK_MOVE,
		LFS_FLY_ATTACK,
		LFS_FLY_GRAP,
		LFS_LAND_ATTACK,
		LFS_END,
	};

	//LFS_FLY_ATTACK상황 안의 상황
	enum LuciferFlyAttackState
	{
		LFAS_NONE,
		LFAS_DELAY,
		LFAS_ATTACK,
		LFAS_ATTACKED,
	};

	enum SyncSubType
	{
		SST_FLY_STATE,
		SST_ROTATE,//플라이 상태일때 상승 하강, 회전등을 처리할때
		SST_EFFECT,
		SST_MARKER,//타겟팅 마크
		SST_FLY_ATTACK_START,//UroborusAttack시전을 시작할때
		SST_FLY_ATTACK_GRAP,//근처에 왔을때 잡기 공격 시전할때
		SST_FLY_ATTACK_JUMP,//잡아와서 점프를 뛸때
		SST_FLY_DASH_ATTACK,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	LuciferFlyState m_FlyState;
	LuciferFlyAttackState m_FlyAttackState;

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
	LuciferFlyInfo m_AttackFly;
	LuciferFlyInfo m_CurFly;

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
	AttackAttribute m_DashAttack;
	AttackAttribute m_UroborusAttack;
	DWORD m_dwUroborusAttackEndTime;

	//잡기 성공이후 정보들(점프 ,중력, 랜드어택, 애니)
	ItemJumpAttackInfo m_CurJumpAttackInfo;
	ItemJumpAttackInfo m_SpecialJumpAttackInfo;

	//공중잡기
	UniqueObjID m_GuidEffectID;
	ioHashString m_GrapplingTargetName;
	ioHashString m_GrapplingAniName;
	float m_fGrapAniTimeRate;

	//잡고 올라갈때 케익터간의 떨어질 거리
	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;

	DWORD m_dwMotionStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwPreDelayStartTime;
	DWORD m_dwFireStartTime;
	DWORD m_dwMotionEndTime;

	float m_fGrapReturnDirRate;
	float m_fGrapReturnCheckLenth;
	float m_fGrapReturnPosAddY;
	DWORD m_dwGrapWeaponIdx;

	DWORD m_dwJumpDashDelayTime;

	DWORD m_dwFlyDashAttackWeaponIndex;
	bool m_bFlyDashAttackToTarget;//타겟이 있어서 그넘한태 갈때

	//특수 공격시 이동, 회전 옵션
	bool	m_bSpecialAttackRotate;
	float	m_fSpecialAttackSpeed;

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
	void WaitTargetReturn( ioBaseChar *pOwner );
	void StartLandAttack( ioBaseChar *pOwner );

	void ApplyFlyDashAttackMove( ioBaseChar *pOwner );

public:
	void StartGrappAttack( ioBaseChar *pOwner );

public:
	void SetFlyState( ioBaseChar *pOwner, const LuciferFlyInfo &rkFlyInfo, bool bSendNet );
	LuciferFlyState GetFlyState() { return m_FlyState; }
	void ChangeFlyStart( ioBaseChar *pOwner );
	void ChangeFlyDelay( ioBaseChar *pOwner );
	void ChangeFlyDashAttackMove( ioBaseChar *pOwner );
	void ChangeFlyAttack( ioBaseChar *pOwner );
	void FlyAttackFire( ioBaseChar *pOwner );
	void ChangeFlyEnd( ioBaseChar *pOwner );

	//Delay->move로 바뀔때 혹은 Up_Down일때
	void ChangeFlyMove( ioBaseChar *pOwner );

	void ProcessFlyState( ioBaseChar *pOwner, float fHeightGap );
	void ProcessRotate( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessFlyMove( ioBaseChar *pOwner, bool bOptionMove = false );

	bool IsFlyCharCollisionSkipState();

	inline const int GetFlyState() const { return (int)m_FlyState; }
	inline const D3DXVECTOR3& GetFlyAttMoveDir() const { return m_vFlyAttackMoveDir; }
	inline const float GetFlyAttMoveSpeed() const { return m_fFlyAttackMoveSpeed; }

	//->->공격중에 땅에 닿을때
	void DashAttachToLand( ioBaseChar *pOwner );
	bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

	bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
	float GetGrapGravity();

	D3DXVECTOR3 CalcReturnUroborusPosition(ioWeapon *pWeapon);
	bool SetTarget( const ioHashString &szTargetName );

	bool IsDashToTarget(){ return m_bFlyDashAttackToTarget; }
	DWORD GetDashAttackWeaponIndex(){ return m_dwFlyDashAttackWeaponIndex; }

	void SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info );

protected:
	void CheckFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void CheckFlyAttTarget( ioBaseChar *pOwner );
	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CheckTargetValidate( ioBaseChar *pOwner );
	bool CompareTarget( ioBaseChar *pOwner );

	void SetFlyMoveEffect( ioBaseChar *pChar );
	void UpdateFlyMoveEffect( ioBaseChar *pChar );
	void ReleaseFlyMoveEffect( ioBaseChar *pChar );

	void UpdateGuidEffect( ioBaseChar *pChar );

	bool IsCanDash( ioBaseChar *pOwner, float fHeightGap );

	bool CheckLanding( ioBaseChar *pChar );
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
	bool IsLuciferState();
	void ClearState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetTargetSkill();
	void ClearTargetSkill();

	bool IsBallTarget();

	void InitLuciferGauge();

public:
	ioLuciferItem();
	ioLuciferItem( const ioLuciferItem &rhs );
	virtual ~ioLuciferItem();
};

inline ioLuciferItem* ToLuciferItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_LUCIFER )
		return NULL;

	return dynamic_cast< ioLuciferItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioLuciferFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_LUCIFER_FLY"; }
	virtual const CharState GetState(){ return CS_LUCIFER_FLY; }	
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
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	
public:
	ioLuciferFlySpecialState();
	virtual ~ioLuciferFlySpecialState();
};

inline ioLuciferFlySpecialState* ToLuciferFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_LUCIFER_FLY )
		return NULL;

	return dynamic_cast< ioLuciferFlySpecialState* >( pState );
}