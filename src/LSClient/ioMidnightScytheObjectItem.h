#pragma once
#include "ioobjectitem.h"
#include "ioSpecialStateBase.h"
#include "ioChargeCommandItem.h"

class ioWeapon;
class ioBaseChar;
class ioINILoader;

class ioMidnightScytheObjectItem :	public ioObjectItem
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

	enum ScytheState
	{
		SS_NONE,
		SS_PRE_ATTACK
	};

	enum CmdInputType
	{
		CIT_ALL		= 0,
		CIT_DIR		= 1,
		CIT_ACTION	= 2,
	};

	enum ScytheSyncType
	{
		SST_ATTACK_FIRE,
		SST_CMD_STATE,
		SST_CMD_STATE_D,
		SST_CMD_STATE_S,
	};

protected:
	AttackAttribute m_PreAttackGround;
	AttackAttribute m_PreAttackJump;
	DWORD m_dwPreAttackEndTime;
	float m_fPreAttackJumpGravity;

	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

	DWORD m_dwGatheringStartTime;

	bool m_bIsGaugeUseUp;
	bool m_bObjectRelease;

	//////////////////////////////////////////////////////////////////////////
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	int			m_iChargeCombo;
	DWORD		m_dwMotionEndTime;

	DWORD		m_dwComboAttackTimeEnd;
	DWORD		m_dwStartComboTime;

	//special state
	ScytheState m_ScytheState;

	// combo
	CmdInputType m_iCmdInputType;
	ChargeCmdInfoList m_ChargeCmdInfoList;
	FloatVec	m_vCmdGauge;

	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	CEncrypt<int> m_iCurCmdIndex;

	CEncrypt<bool> m_bCmdInputD;
	CEncrypt<bool> m_bCmdInputS;

	CEncrypt<DWORD> m_dwInputMaxTime;
	CEncrypt<DWORD> m_dwInputStartGapTime;
	CEncrypt<DWORD> m_dwInputGapTime;

	CEncrypt<DWORD> m_dwInputStartTime;
	CEncrypt<DWORD> m_dwInputCheckTime;

	DWORD m_dwCmdAttackEndTime;

	CEncrypt<int> m_iCurCmdCombo;
	ChargeCmdInfo m_CurChargeCmdInfo;
	float		  m_fCurCmdGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

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

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	//void OnGathering( ioBaseChar *pOwner );
	void OnFullGathering( ioBaseChar *pOwner );
	//void OnComboState( ioBaseChar *pOwner );

	void ChangeToPreAttack( ioBaseChar* pOwner );
	//void ChangeToGathering( ioBaseChar *pOwner );
	//void ChangeToComboState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeEnd( ioBaseChar *pOwner );

	//void SetNextComboState( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

public:
	void ChangeToFullGatherState( ioBaseChar *pOwner );

	void OnPreAttack( ioBaseChar* pOwner );

	void OnProcessScytheState( ioBaseChar* pOwner );

	bool IsPreAttackState();

protected:
	void LoadCmdInfoList( ioINILoader &rkLoader );

	bool ChangeToNewCmdSet( ioBaseChar *pOwner );
	bool ChangeToComboCmdSet( ioBaseChar *pOwner );
	bool ChangeToCmdSet( ioBaseChar *pOwner );

	virtual void SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fGauge, float fExtraAniRate=1.0f, float fExtraForceRate=1.0f );

	bool CheckCmdInput( ioBaseChar *pOwner );
	bool CheckCmdInputTime();

	void ClearState();

public:
	virtual void CheckCmdState( ioBaseChar *pOwner );
	void ClearCmdInfo();
	void ClearCmdTime();

	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );
	bool CheckCmdInputState( ioBaseChar *pOwner );

	void ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );

public:
	void FillMidnightScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket );
	void SetMidngihtScytheInfo( ioBaseChar* pOwner, SP2Packet& rkPacket );

	inline float GetPreAttackJumpGravity()		{	return m_fPreAttackJumpGravity;	}

public:
	ioMidnightScytheObjectItem();
	ioMidnightScytheObjectItem( const ioMidnightScytheObjectItem &rhs );
	virtual ~ioMidnightScytheObjectItem();
};

inline ioMidnightScytheObjectItem* ToObjectMidnightScytheItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_MIDNIGHT_SCYTHE )
		return NULL;

	return dynamic_cast< ioMidnightScytheObjectItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------

class ioMidnightScytheState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MIDNIGHT_SCYTHE"; }
	virtual const CharState GetState(){ return CS_MIDNIGHT_SCYTHE; }	
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
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioMidnightScytheState();
	virtual ~ioMidnightScytheState();
};

inline ioMidnightScytheState* ToMidnightScytheState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MIDNIGHT_SCYTHE )
		return NULL;

	return dynamic_cast< ioMidnightScytheState* >( pState );
}

