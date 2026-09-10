#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioStrikerItem : public ioWeaponItem
{
	struct StrikerAttackInfo
	{
		DWORD m_dwMoveRange;
		float m_fMoveSpeed;

		AttackAttribute m_AttackMove;
		AttackAttribute m_AddAttack;

		ioHashString m_PreDelayAni;
		float		 m_fPreDelayAniRate;

		ioHashString m_EndAni;
		float		 m_fEndAniRate;

		StrikerAttackInfo()
		{
			Init();
		}

		void Init()
		{
			m_dwMoveRange = 0;
			m_fMoveSpeed = FLOAT1;

			m_AttackMove.Init();
			m_AddAttack.Init();

			m_fPreDelayAniRate = FLOAT1;
			m_fEndAniRate = FLOAT1;
		}
	};

public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
	};

	enum StrikerState
	{
		SS_NONE,
		SS_ATTACK_MOVE_PRE,
		SS_ATTACK_MOVE,
		SS_ATTACK_MOVE_END,
		SS_AUTO_ATTACK,
		SS_JUMP_ATTACK,
		SS_JUMP_LAND_ATTACK,
		SS_END,
	};

	enum SyncSubType
	{
		SST_STATE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

protected:
	StrikerState m_StrikerState;

	CEncrypt<float> m_fSpecialMoveCurRange;

	StrikerAttackInfo m_SpecialAttackInfo;

	//점프 대쉬
	ItemJumpAttackInfo m_CurJumpAttackInfo;
	ioHashString	   m_GrapplingTargetName;	

	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;

	// attack_pre
	DWORD m_dwAttackMovePreTime;

	// attack move
	DWORD m_dwAttackMoveTime;
	DWORD m_dwAutoAttackTime;

	// attack end
	DWORD m_dwAttackMoveEndTime;

	D3DXVECTOR3 m_vMoveDir;

	DWORD m_dwMotionEndTime;

	DWORD m_dwSpecilAttackWeaponIdx;


protected:
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;

	CEncrypt<float> m_fIncreaseGauge;

	CEncrypt<float> m_fCurGauge;

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

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void LoadStrikerInfo( ioINILoader &rkLoader );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );

	void SetMoveAttack_Pre( ioBaseChar *pOwner );
	void SetMoveAttack( ioBaseChar *pOwner );
	void SetMoveAttack_End( ioBaseChar *pOwner );

public:

	void ChangeGoomihoFlyAttackMove( ioBaseChar *pOwner );

	void ProcessStrikerMoveState( ioBaseChar *pOwner );

	inline const int GetStrikerState() const { return (int)m_StrikerState; }

protected:
	void ProcessStrikerMove( ioBaseChar *pOwner );

public:
	void SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info );
	bool SetJumpDashGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	float GetGrapGravity();
	bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	bool IsCharCollisionSkipState();

protected:
	bool CheckLanding( ioBaseChar *pChar );
	void StartLandAttack( ioBaseChar *pOwner );

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwFireEndTime;
	DWORD m_dwFireDelayTime;

protected:
	bool SetAutoAttack( ioBaseChar *pOwner );
	void OnAutoAttack( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

public:
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

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
	void CheckEnableGoomihoGauge( ioBaseChar *pChar );

	bool IsEnableGauge();

	void ClearSpecialState( ioBaseChar *pOwner );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	ioStrikerItem();
	ioStrikerItem( const ioStrikerItem &rhs );
	virtual ~ioStrikerItem();
};

inline ioStrikerItem* ToStrikerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_STRIKER_ITEM )
		return NULL;

	return dynamic_cast< ioStrikerItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioStrikerMoveSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_STRIKER_MOVE"; }
	virtual const CharState GetState(){ return CS_STRIKER_MOVE; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

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

public:
	ioStrikerMoveSpecialState();
	virtual ~ioStrikerMoveSpecialState();
};

inline ioStrikerMoveSpecialState* ToStrikerMoveSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_STRIKER_MOVE )
		return NULL;

	return dynamic_cast< ioStrikerMoveSpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioStrikerLandAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_STRIKER_LAND_ATTACK"; }
	virtual const CharState GetState(){ return CS_STRIKER_LAND_ATTACK; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioStrikerLandAttackSpecialState();
	virtual ~ioStrikerLandAttackSpecialState();
};

inline ioStrikerLandAttackSpecialState* ToStrikerLandAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_STRIKER_LAND_ATTACK )
		return NULL;

	return dynamic_cast< ioStrikerLandAttackSpecialState* >( pState );
}