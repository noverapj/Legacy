#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT  11
 
class ioGrapplingExpansionItem : public ioWeaponItem
{
	typedef struct tagAttackInfo
	{
		AttackAttribute		m_LandAttack;
		ioHashString		m_stAttackMotion;
		float				m_fAttackMotionRate;

		ioHashString		m_stTargetMotion;
		float				m_fTargetMotionRate;

		float				m_fGapRange;

		tagAttackInfo::tagAttackInfo()
		{
			m_fAttackMotionRate = FLOAT1;
			m_fTargetMotionRate = FLOAT1;

			m_fGapRange = 0.f;
		}
	};
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_JUMP_GRAP,
		CS_JUMP_GRAP_LAND_ATTACK,
	};

	enum GrapAttackType
	{
		GAT1_MOTION,
		GAT1_LAND_ATTACK,
		GAT2_MOTION,
		GAT2_LAND_ATTACK,
	};

	enum GrapItemSync
	{
		GIS_NORMAL_ATTACK,
		GIS_A_M,
		GIS_S_M,
		GIS_L_A,
	};

	enum GrapItemCancelSync
	{
		GICS_GRAP_JUMP,
		GICS_DIR,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	//////////////////////////////////////////////////////////////////////////
	int m_nMaxLoopCnt;
	int m_nCurLoopCnt;

	std::vector< tagAttackInfo > m_vA_AttackList;
	std::vector< tagAttackInfo > m_vS_AttackList;

	std::vector< tagAttackInfo > m_vA_AttackList_CmdFail;
	std::vector< tagAttackInfo > m_vS_AttackList_CmdFail;

	GrapAttackType m_GrapAttackType;

	//Cmd
	ioHashStringVec m_Cmd_a;
	ioHashStringVec m_Cmd_s;

	//처음 시작이 a로 시작인가?
	bool m_bIsCmd_A_Routine;

	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	int m_iCurCmdIndex;

	DWORD m_dwInputMaxTime;
	DWORD m_dwInputStartGapTime;

	DWORD m_dwInputCheckTime;
	DWORD m_dwInputGapTime;

	DWORD m_dwInputStartTime;

	//버프(탈출)
	ioHashString m_stTargetTryoutBuff;

	bool m_bSuccessCmdKey;
	bool m_bEndReady;

protected:
	ChargeState m_ChargeState;
	GrapplingState m_GrapplingState;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;
	ioHashString m_CurGrapplingTargetName;


	//공중 잡기
	ItemJumpAttackInfo m_CurJumpAttackInfo;
	DWORD m_dwMotionEndTime;
	ioHashString m_GrapplingTargetName;

	float m_fGrpplingCharToCharGap;
	float m_fGrpplingCharMidPosRate;

protected:
	float m_fGrapplingRange;
	float m_fGrapplingMaxAngle;
	float m_fGrapplingMinAngle;

	DWORD m_dwGrapplingPullEndTime;
	DWORD m_dwGrapplingMoveEndTime;
	DWORD m_dwGrapplingPushEndTime;
	DWORD m_dwGrapplingCancelEndTime;

	DWORD m_dwGrapplingAttackEndTime;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	bool m_bSetGrapplingMoveAni;
	CEncrypt<float> m_fGrapplingMoveSpeedRate;
	float m_fGrapplingGapRange;

	// attacker
	ioHashString m_GrapplingPullMotion;
	float m_fGrapplingPullMotionRate;

	ioHashString m_GrapplingPushMotion;
	float m_fGrapplingPushMotionRate;

	ioHashString m_GrapplingMotion;
	ioHashString m_GrapplingMoveMotion;
	float m_fGrapplingMoveMotionRate;

	ioHashString m_GrapplingCancelMotion;
	float m_fGrapplingCancelMotionRate;

	bool m_bTargetRot;
	DWORD m_dwGrapplingRotateSpeed;

	// target
	GrapplingWoundedInfo m_TargetGrapplingInfo;

	// pushed
	GrapplingPushedInfo m_PushedGrapplingInfo;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetEndGrappling( ioBaseChar *pOwner );

	virtual bool IsCmdCheckEnableTime();
	virtual bool IsCmdSuccess();

public:
	virtual WeaponSubType GetSubType() const;

	int GetState();

public:
	bool IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	bool IsEnableGrapplingMoveState( ioBaseChar *pOwner );

	void SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void CheckGrapplingState( ioBaseChar *pOwner );

	void ChangeGrapplingMove( ioBaseChar *pOwner, bool bRotate );
	void ChangeGrapplingPush( ioBaseChar *pOwner );
	void ChangeGrapplingCancel( ioBaseChar *pOwner );

	void ChangeGrappling_A_AttackMotion( ioBaseChar *pOwner );
	void ChangeGrappling_S_AttackMotion( ioBaseChar *pOwner );
	void ChangeGrapplingLandAttack( ioBaseChar *pOwner );

	void CheckGrapplingMoveKey( ioBaseChar *pOwner );
	void CheckGrapplingMoveAni( ioBaseChar *pOwner );
	void CheckGrapplingMoveDir( ioBaseChar *pOwner );

	void GetGrapplingMoveInfo( OUT int &iCurDir, OUT bool &bSetMotion );

	inline float GetGrapplingMoveSpeedRate() const { return m_fGrapplingMoveSpeedRate; }
	float GetGrapplingGapRange();
	bool CanUseGap();

	// target
	inline GrapplingWoundedInfo GetTargetGrapplingInfo() const { return m_TargetGrapplingInfo; }

	// pushed
	inline GrapplingPushedInfo GetPushedGrapplingInfo() const { return m_PushedGrapplingInfo; }

	//공중잡기//////////////////////////////////////////////////////////////////////////
public:
		void SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info );
		bool SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
		float GetGrapGravity();
		bool IsLandAttackMotionEnd( DWORD dwActionStop ) const;
		D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );

		void ProcessGrapplerJumpGrapMove( ioBaseChar *pOwner );
		bool ProcessGrapplerFlyLandAttack( ioBaseChar *pOwner );

protected:
	bool CheckLanding( ioBaseChar *pChar );
	void StartLandAttack( ioBaseChar *pOwner );
	//////////////////////////////////////////////////////////////////////////


protected:
	void OnGrapplingPull( ioBaseChar *pOwner );
	void OnGrapplingMove( ioBaseChar *pOwner );
	void OnGrapplingPush( ioBaseChar *pOwner );
	void OnGrapplingCancel( ioBaseChar *pOwner );

	void OnGrapplingAttackMotion( ioBaseChar *pOwner );
	void OnGrapplingLandAttack( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

	void FindGrappling( ioBaseChar *pOwner );

	void CheckCmdInput( ioBaseChar *pOwner );

public:
	ioGrapplingExpansionItem();
	ioGrapplingExpansionItem( const ioGrapplingExpansionItem &rhs );
	virtual ~ioGrapplingExpansionItem();
};

inline ioGrapplingExpansionItem* ToGrapplingExpansionItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GRAPPLING_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< ioGrapplingExpansionItem* >( pItem );
}

inline const ioGrapplingExpansionItem* ToGrapplingExpansionItemConst( const ioItem *pItem )
{
	const ioWeaponItem *pWeapon = ToWeaponItemConst( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GRAPPLING_EXPANSION_ITEM )
		return NULL;

	return dynamic_cast< const ioGrapplingExpansionItem* >( pItem );
}


//////////////////////////////////////////////////////////////////////////
class ioGrapplerJumpAttackSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_GRAPPLER_JUMP_GRAP"; }
	virtual const CharState GetState(){ return CS_GRAPPLER_JUMP_GRAP; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioGrapplerJumpAttackSpecialState();
	virtual ~ioGrapplerJumpAttackSpecialState();
};

inline ioGrapplerJumpAttackSpecialState* ToGraplperJumpAttackSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_GRAPPLER_JUMP_GRAP )
		return NULL;

	return dynamic_cast< ioGrapplerJumpAttackSpecialState* >( pState );
}

