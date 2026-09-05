#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGoomihoItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum GoomihoFlyState
	{
		MFS_NONE,
		MFS_START,
		MFS_ATTACK_MOVE,
		MFS_ATTACK_MOVE_JUMP,
		MFS_AUTO_ATTACK,
		MFS_AUTO_PLUS_ATTACK,
		//MFS_GRAPPLING_LOOP,
		//MFS_GRAPPLING_MOVE,
		//MFS_END_ATTACK,
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
	int m_nAttackCnt;

protected:
	GoomihoFlyState m_GoomihoFlyState;

	CEncrypt<float> m_fGoomihoFlyCurRange;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	GoomihoFlyInfo m_AttackGoomihoFly;
	GoomihoFlyInfo m_CurGoomihoFly;

	// start
	DWORD m_dwGoomihoFlyStartTime;

	// attack move
	DWORD m_dwGoomihoFlyAttackMoveTime;

	// grappling loop
	DWORD m_dwGoomihoFlyGrapplingLoopTime;
	DWORD m_dwGoomihoFlyGrapplingLoopDuration;
	DWORD m_dwGoomihoFlyGrapplingEnableMoveTime;

	// grappling move
	bool m_bGrapplingDirMove;
	DWORD m_dwGoomihoFlyGrapplingMoveTime;

	// end attack
	DWORD m_dwGoomihoFlyEndAttackTime;

	// end
	ioHashString m_GoomihoFlyEndAni;
	float m_fGoomihoFlyEndAniRate;

	DWORD m_dwGoomihoFlyEndTime;

	D3DXVECTOR3 m_vFlyMoveDir;
	CEncrypt<float> m_fFlyMoveSpeed;

	//
	ioHashString m_GoomihoTarget;
	ioHashString m_GoomihoTargetSkill;

	ioHashString m_GoomihoGrapplingTarget;

	//
	ioHashString m_GoomihoAttackMoveEffect;
	ioHashString m_GoomihoGrapplingMoveEffect;
	ioHashString m_GoomihoGrapplingDirMoveEffect;

protected:
	bool m_bInitGoomihoGaugeDown;

	CEncrypt<float> m_fMaxGoomihoGauge;
	CEncrypt<float> m_fNeedGoomihoGauge;

	CEncrypt<float> m_fIncreaseGoomihoGauge;
	CEncrypt<float> m_fDecreaseGoomihoGauge;

	CEncrypt<float> m_fHighDecreaseHeight;
	CEncrypt<float> m_fHighDecreaseGoomihoGauge;

	CEncrypt<float> m_fCurGoomihoGauge;

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
	void LoadGoomihoFlyInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	bool IsChargeAutoTarget( ioBaseChar *pOwner );
	void ChangeToChargePlusAttack( ioBaseChar *pOwner );

public:
	void SetGoomihoFlyState( ioBaseChar *pOwner, const GoomihoFlyInfo &rkFlyInfo, bool bSendNet );
	void SetGoomihoFlyStateByJumpDash( ioBaseChar *pOwner, const GoomihoFlyInfo &rkFlyInfo );

	void ChangeGoomihoFlyStart( ioBaseChar *pOwner );
	void ChangeGoomihoFlyAttackMove( ioBaseChar *pOwner );
	//void ChangeGoomihoFlyEndAttack( ioBaseChar *pOwner );
	void ChangeGoomihoFlyEnd( ioBaseChar *pOwner );

	void ProcessGoomihoFlyState( ioBaseChar *pOwner );
	bool ProcessGoomihoFlyStart( ioBaseChar *pOwner );

	void ProcessGoomihoFlyMove( ioBaseChar *pOwner );

	bool IsGoomihoFlyCharCollisionSkipState();

	inline const int GetGoomihoFlyState() const { return (int)m_GoomihoFlyState; }
	inline const D3DXVECTOR3& GetGoomihoFlyMoveDir() const { return m_vFlyMoveDir; }
	inline const float GetGoomihoFlyMoveSpeed() const { return m_fFlyMoveSpeed; }

protected:
	void CheckGoomihoFlyState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void FindFlyAttTarget( ioBaseChar *pOwner );
	bool IsEnableTargetState( ioBaseChar *pTarget );
	bool CompareTarget( ioBaseChar *pOwner );

	bool CheckGrapplingTargetValidate( ioBaseChar *pOwner );

	void AttachAttackMoveEffect( ioBaseChar *pOwner );
	void ReleaseAttackMoveEffect( ioBaseChar *pOwner );

	void AttachGrapplingMoveEffect( ioBaseChar *pOwner );
	void ReleaseeGrapplingMoveEffect( ioBaseChar *pOwner );

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

protected:
	bool SetChangeAutoFireMotion( ioBaseChar *pOwner );
	void OnAutoAttack( ioBaseChar *pOwner );
	void OnAutoPlusAttack( ioBaseChar *pOwner );
	void SetChangeEndState( ioBaseChar *pOwner );

	void SetNextAutoPlusAttack( ioBaseChar *pOwner );

public:
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	bool IsAirAttack();

	// Goomiho Gauge
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

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );

	void CheckDecreaseGauge();

	//
	void CheckEnableGoomihoGauge( ioBaseChar *pChar );

	bool IsEnableGoomihoGauge();

	void ClearGoomihoFlyState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	const ioHashString& GetGoomihoTargetSkill();
	void ClearGoomihoTargetSkill();

	bool IsBallTarget();

public:
	ioGoomihoItem();
	ioGoomihoItem( const ioGoomihoItem &rhs );
	virtual ~ioGoomihoItem();
};

inline ioGoomihoItem* ToGoomihoItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GOOMIHO )
		return NULL;

	return dynamic_cast< ioGoomihoItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioGoomihoFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GOOMIHO_LAND_FLY"; }
	virtual const CharState GetState(){ return CS_GOOMIHO_LAND_FLY; }	
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
	ioGoomihoFlySpecialState();
	virtual ~ioGoomihoFlySpecialState();
};

inline ioGoomihoFlySpecialState* ToGoomihoFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GOOMIHO_LAND_FLY )
		return NULL;

	return dynamic_cast< ioGoomihoFlySpecialState* >( pState );
}