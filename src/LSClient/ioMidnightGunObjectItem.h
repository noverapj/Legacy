#pragma once

#include "ioObjectItem.h"
#include "ioSpecialStateBase.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;


class ioMidnightGunObjectItem :	public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_FULL_GATHERING,
		CS_ATTACK_FIRE,
		CS_COMBO,
		CS_END,
	};

	enum GunState
	{
		GS_NONE,
		GS_PRE_ATTACK,
		GS_CANCEL_DASH,
		GS_JUMP_CHARGE_ATTACK
	};

public:
	struct BulletSet
	{
		int				m_iMaxWeaponWay;		
		BulletInfoList	m_BulletElmentList;

		BulletSet()
		{
			m_iMaxWeaponWay = 0;			
		}
	};
	typedef std::vector<BulletSet> BulletSetList;

protected:
	BulletInfoList	m_PreBulletSetList;
	BulletInfoList	m_PreJumpBulletSetList;
	BulletInfoList	m_ChargeBulletSetList;
	BulletInfoList	m_JumpBulletSetList;

	int	m_iWeaponWay;

	AttackAttribute m_PreAttackGround;
	AttackAttribute m_PreAttackJump;
	DWORD m_dwPreAttackEndTime;

	ioHashStringVec m_AttackReadyAniList;
	AttackAttribute m_ExtendMaxAttribute;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	float m_fCancelDashEndJumpAmt;
	float m_fCancelDashRandRate;

	DWORD m_dwGatheringStartTime;

	//////////////////////////////////////////////////////////////////////////
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	int			m_iChargeCombo;
	DWORD		m_dwMotionEndTime;

	DWORD		m_dwComboAttackTimeEnd;
	DWORD		m_dwStartComboTime;
	//DWORD		m_dwMaxComboDuration;

	//special state
	GunState	m_GunState;

	// cancel dash
	DWORD				m_dwCancelDashMotionEndTime;
	AttackAttribute		m_CancelDashAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadBulletProperty( ioINILoader &rkLoader, const ioHashString& szTitle, BulletInfoList& rkList );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );

	virtual	bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual	void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual void OnEquiped( ioBaseChar *pOwner );
	//virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnFullGathering( ioBaseChar *pOwner );

	void ChangeToPreAttack( ioBaseChar* pOwner );
	void ChangeToFullGatherState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeEnd( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

	void ChangeReserveKeyInputState( ioBaseChar* pOwner, int iCurCombo = 0 );
	void ProcessCancelDashState( ioBaseChar* pOwner );

public:
	void OnPreAttack( ioBaseChar* pOwner );
	bool IsCollisionSkipState();
	void ChangeToJumpChargeAttack( ioBaseChar* pOwner );
	void ProcessJumpChargeAttack( ioBaseChar* pOwner );

	void OnProcessGunState( ioBaseChar* pOwner );

	bool IsJumpChargeAttackState( ioBaseChar* pOwner );

public:
	void FillMidnightScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket );
	void SetMidngihtScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket );

public:
	ioMidnightGunObjectItem();
	ioMidnightGunObjectItem( const ioMidnightGunObjectItem &rhs );
	virtual ~ioMidnightGunObjectItem();
};

inline ioMidnightGunObjectItem* ToObjectMidnightGunItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_MIDNIGHT_GUN )
		return NULL;

	return dynamic_cast< ioMidnightGunObjectItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------

class ioMidnightGunState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MIDNIGHT_GUN"; }
	virtual const CharState GetState(){ return CS_MIDNIGHT_GUN; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return true; }	
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
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }

public:
	ioMidnightGunState();
	virtual ~ioMidnightGunState();
};

inline ioMidnightGunState* ToMidnightGunState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MIDNIGHT_GUN )
		return NULL;

	return dynamic_cast< ioMidnightGunState* >( pState );
}

