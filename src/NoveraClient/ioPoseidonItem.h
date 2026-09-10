#pragma once
#include "ioweaponitem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioZoneEffectWeapon.h"
#include "ioSpecialStateBase.h"

class ioEntityGroup;

class ioPoseidonItem :	public ioWeaponItem
{
protected:
	enum	ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_CHARGE_ATTACK,
		CS_CHARGE_ATTACK_MOVE,
		CS_CHARGE_END_ATTACK
	};

	enum	PoseidonSpecialState
	{
		PSS_NONE,
		PSS_READY,
		PSS_SUMMON_SELECT,
		PSS_SUMMON_SELECT_WAIT,
		PSS_SUMMON
	};

public:
	enum	SummonState
	{
		SS_LEVEL_1,
		SS_LEVEL_2,
		SS_LEVEL_MAX
	};

	typedef struct tagSummonAniInfo
	{
		ioHashString	m_AniName;
		float	m_fAniRate;

		tagSummonAniInfo()
		{
			Init();
		}

		void Init()
		{
			m_fAniRate = 0.0f;
		}

	} SummonAniInfo;

	//typedef std::vector< SummonAniInfo > SummonAniInfoList;

	typedef std::vector< std::list< int > > MinionInfoList;

	typedef std::vector< std::vector< D3DXVECTOR3 > > MinionOffsetList;

protected:
	// normal attack
	ChargeState	m_ChargeState;
	DWORD	m_dwAttackStartTime;
	int	m_iCurCombo;

	ioHashStringVec	m_AttackReadyAniList;

	// special state
	PoseidonSpecialState	m_SpecialState;

	// gauge
	float	m_fMaxBullet;
	float	m_fNeedBullet;
	float	m_fCurBullet;

	float	m_fRecoverGaugeDelay;
	float	m_fRecoverGaugeRun;
	float	m_fRecoverGaugeDash;
	float	m_fRecoverGaugeWound;
	float	m_fRecoverGaugeRateWhileSummon;

	// summon
	//AttackAttribute	m_SummonAttack;
	vDummyCharLoadInfoList	m_DummyCharList;
	DWORD	m_dwSummonStartTime;
	DWORD	m_dwSummonEndTime;

	SummonAniInfo	m_szSummonChargeAniInfo;
	SummonAniInfo	m_szSummonAniInfo;
	ioHashStringVec	m_vSummonReadyEffectList;
	ioHashStringVec	m_vSummonUpEffectList;
	ioHashStringVec	m_vSummonLeftEffectList;
	ioHashStringVec	m_vSummonRightEffectList;

	IntVec	m_vMaxSummonDummyByType;
	int	m_iMaxSummonDummyCnt;
	MinionInfoList	m_vSummonedMinionInfoList;
	IntVec	m_vSummonNeedBullet;
	MinionOffsetList	m_vMinionOffsetInfoList;
	IntVec	m_vCurrentSummonIdx;
	int	m_iMaxSummonType;
	bool	m_bDummyDestroyOnReleased;

	SummonState	m_PreSummonState;

	// charge
	WeaponInfo	m_AttachWeapon;
	ioHashString	m_szChargingEffect;
	DWORD	m_dwChargeStartTime;
	ioHashString	m_szChargingStartAnimation;
	float	m_fChargingStartAniRate;
	ioHashString	m_szChargingAnimation;
	float	m_fChargingAniRate;
	AttackAttribute	m_ChargingEndAttack;
	DWORD	m_dwMotionEndTime;
	//ioZoneEffectWeapon*	m_pChargeAttackWeapon;
	DWORD	m_dwChargeAttackWeaponIdx;
	D3DXVECTOR3	m_ChargeAttackPos;
	D3DXVECTOR3	m_ChargeAttackDir;

	DWORD	m_dwChargingAttackDuration;
	float	m_fChargingAttackSpeed;
	float	m_fChargingAttackRotateSpeed;
	float	m_fChargingAttackEndJumpPower;

	//ioHashString	m_szSummonEffectBuff;

	int	m_iChargeNeedBullet;

	// Rotate
	bool	m_bTargetRot;
	D3DXVECTOR3	m_vWeaponMoveDir;
	D3DXVECTOR3	m_vWeaponPos;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );
	virtual	ioItem*	Clone();

	virtual	WeaponSubType	GetSubType() const;

	virtual	void	OnEquiped( ioBaseChar *pOwner );
	virtual	void	OnReleased( ioBaseChar *pOwner );

public:
	virtual	void	CheckNormalAttack( ioBaseChar *pOwner );
	virtual	void	SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual	void	SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void	UpdateExtraData( ioBaseChar *pOwner );	

	virtual void	OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void	OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual	void	SetJumpping( ioBaseChar *pOwner, bool bFullTime );

	virtual	void	SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

protected:
	void	Init();

	void	OnCharging( ioBaseChar* pOwner );
	void	ChangeToNormalAttack( ioBaseChar* pOwner );
	void	ChangeToChargeAttack( ioBaseChar* pOwner );
	void	ChangeToChargeAttackMove( ioBaseChar* pOwner );
	void	ChangeToChargeEndAttack( ioBaseChar* pOwner );

	void	ProcessNormalAttack( ioBaseChar* pOwner );
	void	ProcessChargeAttack( ioBaseChar* pOwner );
	void	ProcessChargeAttackMove( ioBaseChar* pOwner );
	void	ProcessChargeEndAttack( ioBaseChar* pOwner );

public:
	void	SetSummonReadyState( ioBaseChar* pOwner );
	void	SetSummonState( ioBaseChar* pOwner );

	void	ProcessSpecialState( ioBaseChar* pOwner );

	void	SetSelectMInionState( ioBaseChar* pOwner );
	void	ProcessSelectMinionWaitState( ioBaseChar* pOwner );
	void	SummonMinion( ioBaseChar* pOwner, int iSummonType );
	ioDummyChar*	CreateDummyChar( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szDummyCharName, int iDummyIndex, D3DXVECTOR3 vPos, float fStartAngle = 0 );

	void	FinalizeSpecialState( ioBaseChar *pOwner );

public:
	virtual	int	GetNeedBullet();
	virtual	int	GetMaxBullet();
	virtual	int	GetCurBullet();
	virtual	void	SetCurBullet( int iCurBullet );

private:
	void	CheckKeyInput( ioBaseChar *pOwner );
	void	ProcessRotate( ioBaseChar *pOwner );

	void	RemoveAllSummonEffect( ioBaseChar *pOwner );

public:
	void	CheckDummyCharCreate( ioBaseChar* pOwner, int iIndex );
	void	CheckDummyCharDestroy( ioBaseChar* pOwner, int iIndex );

	D3DXVECTOR3	GetMinionOffset( int iType, int iIdx );
	inline	const	MinionInfoList	GetDummyList()	{	return	m_vSummonedMinionInfoList;	}

private:
	SummonState	CheckSummonState();
	ioHashString	GetSummonEffectInfo( int iSummonType );

public:
	ioPoseidonItem(void);
	ioPoseidonItem( const ioPoseidonItem& rhs );
	virtual ~ioPoseidonItem(void);
};

inline	ioPoseidonItem*	ToPoseidonItem( ioItem *pItem )
{
	ioWeaponItem*	pWeapon	= ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_POSEIDON_ITEM )
		return	NULL;

	return dynamic_cast< ioPoseidonItem* >( pWeapon );
}

class ioPoseidonSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_POSEIDON_SPECIAL"; }
	virtual const CharState GetState(){ return CS_POSEIDON_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioPoseidonSpecialState();
	virtual ~ioPoseidonSpecialState();
};

inline ioPoseidonSpecialState* ToPoseidonSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_POSEIDON_SPECIAL )
		return NULL;

	return dynamic_cast< ioPoseidonSpecialState* >( pState );
}