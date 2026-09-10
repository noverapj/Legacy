#pragma once

#include "ioSpecialStateBase.h"

class ioHanzoItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_NORMAL_ATTACK,
		CS_CHARGING,
		CS_GATHERING,
		CS_JUMP_GATHERING,
		CS_EXTEND_ATTACK,
		CS_EXTEND_ATTACK_END,
		CS_DEFENSE_POSITION,
		CS_DEFENSE_POS_ATTACK,
		CS_TELEPORT,
		CS_TELEPORT_END,
		CS_JUMP_POSITION,
		CS_JUMP_POS_ATTACK,
		CS_JUMP_POS_LANDING,
		CS_LAND_ATTACK,
		CS_LAND_BUFF,
		CS_DEFENSE_READY,
		CS_DEFENSE_DELAY,
		CS_DEFENSE_END,
		CS_DEFENSE_ATTACK,

		SST_ROTATE,
		SST_EXTEND_ATTACK_ADD,
	};

private:
	//State
	ChargeState m_ChargeState;

	//Normal Attack
	ioHashStringVec	m_AttackReadyAniList;
	int m_iCurCombo;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwMotionStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwReserveTime;
	DWORD m_dwPositionRotateTime;

	float m_fGatherMoveSpeedRate;
	ioHashString m_szGatherLoopMotion;
	ioHashString m_szGatherMoveLoopMotion;
	ioHashString m_szJumpGatherLoopMotion;
	float m_fGatherAniRate;
	bool m_bGatherMoveAni;
	DWORD m_dwJumpGatheringTime;

	AttackAttribute	m_ExtendAttack;
	AttackAttribute	m_JumpingExtendAttack;
	DWORD m_dwExtendAttackEnableTime;
	int m_iCurExtendAttackCnt;
	int m_iMaxExtendAttackCnt;

	ioHashString m_szExtendAttackEndAni;
	ioHashString m_szJumpingExtendAttackEndAni;
	float m_fExtendAttackEndAniRate;

	ioHashString m_szDefensePositionAni;
	ioHashString m_szJumpingDefensePositionAni;
	float m_fDefensePositionAniRate;
	DWORD m_dwDefensePositionTime;
	AttackAttribute	m_DefensePosAttack;
	AttackAttribute	m_JumpingDefensePosAttack;

	ioHashString m_szTeleportTarget;
	ioHashString m_szTeleportStartMotion;
	ioHashString m_szJumpingTeleportStartMotion;
	ioHashString m_szTeleportEndMotion;
	ioHashString m_szJumpingTeleportEndMotion;
	float m_fTeleportStartMotionRate;
	float m_fTeleportEndMotionRate;
	float m_fTeleportRange;
	float m_fTeleportTargetOffset;
	float m_fTeleportNoTargetOffset;

	ioHashString m_szJumpPositionAni;
	ioHashString m_szJumpingJumpPositionAni;
	float m_fJumpPositionAniRate;
	DWORD m_dwJumpPositionTime;
	AttackAttribute	m_JumpPosAttack;
	AttackAttribute	m_JumpingJumpPosAttack;
	AttackAttribute	m_JumpPosLanding;
	AttackAttribute	m_LandAttack;
	int m_iCurJumpPosAttackCnt;
	int m_iMaxJumpPosAttackCnt;
	float m_fLandAttackJumpPower;
	float m_fLandAttackGravity;
	int m_iJumpPosBuffCnt;
	ioHashStringVec m_szJumpPosBuff;

	ioHashString m_szDefenseReadyMotion;
	float m_fDefenseReadyMotionRate;
	ioHashString m_szDefenseMotion;
	float m_fDefenseMotionRate;
	ioHashString m_szDefenseEndMotion;
	float m_fDefenseEndMotionRate;
	AttackAttribute	m_DefenseAttack;

	float m_fExtendEndJumpPower;
	float m_fPositionEndJumpPower;
	float m_fTeleportEndJumpPower;
	float m_fJumpKeyEndJumpPower;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;
	bool m_bUseJumpGathering;

protected:
	void ClearData();
	void ClearState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessGathering( ioBaseChar *pOwner );
	void ProcessExtendAttack( ioBaseChar *pOwner );
	void ProcessForceMove( ioBaseChar *pOwner );
	void ProcessDefensePosition( ioBaseChar *pOwner );
	void ProcessJumpPosition( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );
	void ProcessJumpGathering( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar* pOwner );
	void ChangeToGathering( ioBaseChar* pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );
	void ChangeToNextExtendAttack( ioBaseChar *pOwner );
	void ChangeToExtendAttackEnd( ioBaseChar *pOwner );	
	void ChangeToDefensePosAttack( ioBaseChar *pOwner );
	void ChangeToTeleport( ioBaseChar *pOwner );
	void ChangeToTeleportEnd( ioBaseChar *pOwner, bool bSendPacket );	
	void ChangeToJumpPosAttack( ioBaseChar *pOwner );
	void ChangeToJumpPosLanding( ioBaseChar *pOwner );
	void ChangeToLandAttack( ioBaseChar *pOwner );
	void ChangeToJumpPosBuff( ioBaseChar *pOwner );
	void ChangeToDefenseReady( ioBaseChar *pOwner );
	void ChangeToDefenseDelay( ioBaseChar *pOwner );
	void ChangeToDefenseAttack( ioBaseChar *pAttacker, ioBaseChar *pOwner );
	void ChangeToDefenseEnd( ioBaseChar *pOwner );

	void CheckGatherMoveState( ioBaseChar *pOwner );
	void CheckKeyInput( ioBaseChar *pOwner );
	bool CheckLanding( ioBaseChar *pChar );	

	void FindTeleportTarget( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual float GetAttackSpeedRate() const;

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void CheckWeaponDefenseState( ioBaseChar *pAttacker, ioBaseChar *pOwner );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void SetLandingState( ioBaseChar *pOwner );
	
	void ClearSpecialState( CharState eNewState = CS_DELAY );
	void ProcessSpecialState( ioBaseChar *pOwner );

	float GetLandAttackGravity()	{ return m_fLandAttackGravity; }
	ChargeState GetChargeState()	{ return m_ChargeState; }

	bool CheckDefenseSpecialState( ioBaseChar *pOwner );

	void ChangeToDefensePosition( ioBaseChar *pOwner );
	void ChangeToJumpPosition( ioBaseChar *pOwner, bool bStart );
	void ChangeToJumpGatheringState( ioBaseChar *pOwner );

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	void CheckDecreaseGauge();

	bool IsEnableJumpGathering();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	ioHanzoItem(void);
	ioHanzoItem( const ioHanzoItem &rhs );
	~ioHanzoItem(void);
};

inline ioHanzoItem* ToHanzoItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_HANZO_ITEM )
		return NULL;

	return dynamic_cast< ioHanzoItem* >( pItem );
}

class ioHanzoSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_HANZO_SPECIAL"; }
	virtual const CharState GetState(){ return CS_HANZO_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioHanzoSpecialState();
	virtual ~ioHanzoSpecialState();
};

inline ioHanzoSpecialState* ToHanzoSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HANZO_SPECIAL )
		return NULL;

	return dynamic_cast< ioHanzoSpecialState* >( pState );
}