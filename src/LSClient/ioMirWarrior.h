#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;


class ioMirWarrior : public ioWeaponItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,		
		AS_NORMAL_ATTACK,
		AS_EXTEND_ATTACK,
		AS_CONDITIONAL_SPECIAL_DASH,
		AS_MOVE_AND_ATTACK,
		AS_END,
	};

	enum SpecialState
	{
		SS_NONE,
		SS_EXTEND_MOVE,
		SS_EXTEND_MOVE_ATTACK,
		SS_EXTEND_JUMP_DOWN,
		SS_EXTEND_JUMP_MOVE,
		SS_EXTEND_DASH_MOVE,
		SS_LANDING,		
		SS_END,
	};

	enum MoveState
	{
		MS_MOVE,			//이동중
		MS_LANDING,			//이동중 지형과 충돌
		MS_ATTACK,			//이동 주단
		MS_END,				//이동 완료
	};

	struct ExtendMoveInfo
	{
		float			m_fMinSpeed;
		float			m_fMaxSpeed;
		float			m_fAccelSpeed;		
		float			m_fMaxRange;
		float			m_fAngle;

		DWORD			m_dwAccelTime;
		DWORD			m_dwWeaponIdx;
		DWORD			m_dwMoveIgonreTime;

		ioHashString	m_Animation;
		float			m_fAnimationRate;

		ExtendMoveInfo()
		{
			m_fMinSpeed			= 0.0f;
			m_fMaxSpeed			= 0.0f;
			m_fAccelSpeed		= 0.0f;
			m_fMaxRange			= 0.0f;
			m_fAngle			= 0.0f;
			m_dwAccelTime		= 0;
			m_dwMoveIgonreTime	= 0;
			m_fAnimationRate	= 0.0f;
		}
	};	

protected:
	AttackState			m_AttackState;
	SpecialState		m_SpecialState;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;
	CEncrypt<float>		m_fNeedGauge;
	ioHashStringVec		m_PreAnimationList;

protected:
	AttributeList		m_vExtendAttributeList;
	DWORD				m_dwExtendAttackEndTime;

protected:
	D3DXVECTOR3			m_vMoveDir;
	ExtendMoveInfo		m_ExtendMove;
	ExtendMoveInfo		m_ExtendJumpDown;
	ExtendMoveInfo		m_ExtendJumpMove;
	ExtendMoveInfo		m_ExtendDashMove;

	float				m_fCurrMoveRange;
	DWORD				m_dwMoveWeaponIdx;
	DWORD				m_dwCurrWeaponIdx;

	ioHashString		m_LandingAniamation;
	float				m_fLandingRate;
	DWORD				m_dwLandingEndTime;	
	float				m_fDropZoneJumpPowerAmt;

protected:
	DWORD				m_dwExtendMoveStartTime;	

protected:
	AttackAttribute		m_MoveAttackEndAttribute;
	DWORD				m_dMoveAttackEndTime;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadExtendMoveProperty( ioINILoader &rkLoader, const char* szTitle, ExtendMoveInfo& Info );
	virtual ioItem* Clone();

public:
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendMove( ioBaseChar *pOwner );
	void ChangeToExtendJumpDown( ioBaseChar *pOwner );
	void ChangeToExtendJumpMove( ioBaseChar *pOwner );
	void ChangeToExtendDashMove( ioBaseChar *pOwner );
	void ChangeToExtendMoveEndAttack( ioBaseChar *pOwner );
	void ChangeToLading( ioBaseChar *pOwner );
	void ChangeToEnd( ioBaseChar *pOwner );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnExtendAttack( ioBaseChar *pOwner );
	void OnExtendMove( ioBaseChar *pOwner );
	void OnExtendMoveEndAttack( ioBaseChar *pOwner );
	void OnExtendJumpDown( ioBaseChar *pOwner );
	void OnExtendJumpMove( ioBaseChar *pOwner );
	void OnExtendDashMove( ioBaseChar *pOwner );
	void OnLadning( ioBaseChar *pOwner );

public:
	void ProcessSpecialState( ioBaseChar *pOwner, ioPlayStage* pStage );
	ioMirWarrior::MoveState ProcessExtendMove( float fTimePerSec, ioBaseChar* pOwner, const ExtendMoveInfo& Info, bool bKeyCheck );
	void ProcessTranslate( ioBaseChar* pOwner, D3DXVECTOR3 vMoveAmt );
	bool CheckWeaponLive( ioBaseChar *pOwner );

public:
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	ioWeapon* AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex, FireTimeType eType );

	void ClearWeapon( ioBaseChar* pOwner );
	void ClearState( ioBaseChar* pOwner );

public:
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );	

public:	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

public:	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	bool IsLandTouch( ioBaseChar *pOwner );

public:
	ioMirWarrior::SpecialState GetSpecialState() const { return m_SpecialState; }

public:
	ioMirWarrior();
	ioMirWarrior( const ioMirWarrior &rhs );
	virtual ~ioMirWarrior();
};

inline ioMirWarrior* ToMirWarrior( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MIR_WARRIOR )
		return NULL;

	return static_cast< ioMirWarrior* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMirWarriorDashSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MIR_WARRIOR_DASH_STATE"; }
	virtual const CharState GetState(){ return CS_MIR_WARRIOR_DASH_STATE; }
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
	virtual bool IsStateEnd( ioBaseChar* pOwner );

public:
	ioMirWarriorDashSpecialState();
	virtual ~ioMirWarriorDashSpecialState();
};

inline ioMirWarriorDashSpecialState* ToMirWarriorDashSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MIR_WARRIOR_DASH_STATE )
		return NULL;

	return dynamic_cast< ioMirWarriorDashSpecialState* >( pState );
}
