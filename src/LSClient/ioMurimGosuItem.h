#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

struct MurimCmdInfo
{
	ioHashString m_Cmd;

	AttackAttribute m_CmdAttack;
	RedHoodFlyInfo m_cFlyInfo;
	int m_fNeedGauge;
	
	MurimCmdInfo()
	{
		Init();
	}

	void Init()
	{
		m_Cmd.Clear();
		m_CmdAttack.Init();
		m_cFlyInfo.Init();

		m_fNeedGauge = 0.f;
	}
};
typedef std::vector< MurimCmdInfo > MurimCmdInfoList;

class ioMurimGosuItem : public ioWeaponItem
{
public:
	enum MurimGosuState
	{
		MGS_NONE,
		MGS_FLY,
		MGS_LAND,
		MGS_CMD,
		MGS_END,
	};
	enum SpecialMove
	{
		SM_FRONT,
		SM_UP,
		SM_DOWN,
		SM_SIZE,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	MurimCmdInfoList m_ChargeCmdInfoList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_FLY,
		CS_FLYEND,
		CS_CMD,

		ST_CMD_STATE,
	};
	enum CmdInputType
	{
		CIT_ALL		= 0,
		CIT_DIR		= 1,
		CIT_ACTION	= 2,
	};

	ChargeState m_ChargeState;
	MurimGosuState m_MurimGosuState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// PowerCharge
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;
	ioHashString m_szLandingAni;
	float m_fLandingAniRate;

	ioHashString m_GatheringEffect;
	UniqueObjID m_GatheringEffectID;
	DWORD m_dwGatheringStartTime;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;
	
	RedHoodFlyInfo	m_SpecialMoveInfo[3];
	DWORD m_dwSpecialMoveStartTime;
	DWORD m_dwSpecialMoveEndTime;
	int m_iSpecialMoveDir;
	float m_fFlyCurRange;
	float m_fJumpHeightGap;
	int m_iSpecialMoveCount;
	int m_iSpecialMoveMaxCount;

	CmdInputType m_iCmdInputType;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	CEncrypt<int> m_iCurCmdIndex;

	DWORD m_dwInputMaxTime;
	DWORD m_dwInputStartGapTime;
	DWORD m_dwInputGapTime;
	DWORD m_dwInputStartTime;
	DWORD m_dwInputCheckTime;

	MurimCmdInfo m_CurChargeCmdInfo;
	float m_fCurEndJumpPower;
	DWORD m_dwCmdAttackEndTime;

	//gauge
	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );
	void LoadCmdInfoList( ioINILoader &rkLoader );

public:
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	bool ProcessFlyStart( ioBaseChar *pOwner );
	bool ProcessCMDStart( ioBaseChar *pOwner );
	void SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet, int iDir );
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeLandState( ioBaseChar *pOwner );
	void ChangeSpecialMoveEndJump( ioBaseChar *pOwner );	

	inline const int GetMurimGosuState() const	{ return (int)m_MurimGosuState; }

	void ClearCmdInfo();
	void ClearSpecialMoveCount();
	bool IsCanSpecialMove();

	void CheckCmdState( ioBaseChar *pOwner, bool bMove );
	bool CheckCmdInputState( ioBaseChar *pOwner );
	bool CheckCmdInputTime();
	bool CheckCmdInput( ioBaseChar *pOwner );

	bool ChangeToCmdSet( ioBaseChar *pOwner );
	bool ChangeToNewCmdSet( ioBaseChar *pOwner );

	virtual void SetCmdAttack( ioBaseChar *pOwner, const AttackAttribute &rkAttr, float fExtraAniRate=1.0f, float fExtraForceRate=1.0f );
	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );

	void ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual int GetNeedBullet();
	virtual void SetCurBullet( int iCurBullet );	

	bool IsEnableGauge( float fNeedGauge );
	void DecreaseGauge( float fNeedGauge );

public:
	ioMurimGosuItem();
	ioMurimGosuItem( const ioMurimGosuItem &rhs );
	virtual ~ioMurimGosuItem();
};

inline ioMurimGosuItem* ToMurimGosuItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MURIMGOSU_ITEM )
		return NULL;

	return dynamic_cast< ioMurimGosuItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioMurimGosuSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MURIM_GOSU_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MURIM_GOSU_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
		DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;
	
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioMurimGosuSpecialState();
	virtual ~ioMurimGosuSpecialState();
};

inline ioMurimGosuSpecialState* ToMurimGosuSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MURIM_GOSU_SPECIAL )
		return NULL;

	return dynamic_cast< ioMurimGosuSpecialState* >( pState );
}