#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMagicEngineerItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
		CS_FLY,
	};
	enum MagicEngineerAttackState
	{
		MFS_NONE,
		MFS_START,
		MFS_END,
		MFS_FLY,
		MFS_LAND,
	};
	enum DummyMoveState
	{
		DMS_NORMAL = 0,
		DMS_HALF,
		DMS_JUMP,
		DMS_JUMPHALF,
		DMS_DASH,
		DMS_DASHHALF,
	};
	enum SpecialMove
	{
		SM_UP,
		SM_DOWN,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	MagicEngineerAttackState m_MagicEngineerAttackState;

	DWORD m_dwFireSTime;
	DWORD m_dwAttackStartTime;
	DWORD m_dwFullChargeTime;
	int m_iCurCombo;

	ioHashString m_szGatheringAni;
	float		 m_fGatheringAniRate;

	ioHashString m_szLandingAni;
	float		 m_fLandingAniRate;

	MagicEngineerAttackInfo m_AttackInfo;
	MagicEngineerAttackInfo m_HalfAttackInfo;
	MagicEngineerAttackInfo m_CurAttackInfo;
	MagicEngineerAttackInfo	m_JumpHalfChargeInfo;
	MagicEngineerAttackInfo	m_JumpFullChargeInfo;
	MagicEngineerAttackInfo	m_DashHalfChargeInfo;
	MagicEngineerAttackInfo	m_DashFullChargeInfo;

	ioHashStringVec m_vDestroyDummyCharList;
	int m_iCurDummyCharIndex;
	bool m_bCreateDummy;
	bool m_bJump;
	std::map< int, MEDummyMoveInfo > m_vMoveInfo;
	int m_iMoveIndex;

	DWORD m_dwMotionEndTime;

	RedHoodFlyInfo	m_SpecialMoveInfo[2];
	DWORD m_dwSpecialMoveMacroTime_S;
	DWORD m_dwSpecialMoveMacroTime_E;		
	DWORD m_dwSpecialMoveStartTime;
	DWORD m_dwSpecialMoveEndTime;
	int m_iSpecialMoveDir;

	float m_fFlyCurRange;
	float m_fMaxGauge;
	float m_fNeedGauge;
	float m_fIncreaseGauge;
	float m_fCurGauge;
	float m_fJumpHeightGap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeGatheringState( ioBaseChar *pOwner );

public:
	void SetAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump, bool bDash, bool bHalf );
	void SetSpecailMoveState( ioBaseChar *pOwner, bool bSendNet, int iDir );

	void ProcessSpecialState( ioBaseChar *pOwner );

	inline const int GetMagicEngineerState() const	{ return (int)m_MagicEngineerAttackState; }
	inline const int GetDummyCharIndex()			{ return  m_iCurDummyCharIndex; }

protected:
	void ChangeAttackStart( ioBaseChar *pOwner );
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeLandState( ioBaseChar *pOwner );
	void ChangeToDummyChar( ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo );

	void CreateDummyCharState( IN ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo, OUT D3DXVECTOR3& vPos, OUT int& iIndex );
	void DestroyDummyChar( ioBaseChar *pOwner );
	void SetMoveDummyPos( ioBaseChar *pOwner, const MEDummyMoveInfo& cInfo );

	void SetMacroTime( ioEntityGroup* pGrp, int iAniID, float fTimeRate, DWORD dwEndTime );
	void ChangeSpecialMoveEndJump( ioBaseChar *pOwner );

	bool ProcessFlyStart( ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void  SetCurExtraGauge( float fGauge );

	void DecreaseGauge();
	bool IsEnableGauge();
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	void ClearAttackState( ioBaseChar *pOwner );
	void CheckDummyCharState( ioBaseChar *pOwner, int iIndex );
	void ClearJumpDummy()		{ m_bJump = false; }
	bool IsJumpDummy()			{ return m_bJump; }

public:
	ioMagicEngineerItem();
	ioMagicEngineerItem( const ioMagicEngineerItem &rhs );
	virtual ~ioMagicEngineerItem();
};

inline ioMagicEngineerItem* ToMagicEngineerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGICENGINEER_ITEM )
		return NULL;

	return dynamic_cast< ioMagicEngineerItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioMagicEngineerSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MAGICENGINEER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_MAGICENGINEER_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime,
		DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const;

public:
	ioMagicEngineerSpecialState();
	virtual ~ioMagicEngineerSpecialState();
};

inline ioMagicEngineerSpecialState* ToMagicEngineerSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MAGICENGINEER_SPECIAL )
		return NULL;

	return dynamic_cast< ioMagicEngineerSpecialState* >( pState );
}