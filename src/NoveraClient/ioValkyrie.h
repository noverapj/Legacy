#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioValkyrie : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_FLY,
	};

	enum FlyState
	{
		FS_NONE,
		FS_WAIT,
		FS_UP,
		FS_TARGET_FIND,
		FS_DOWN,
		FS_REUP,
		FS_LANDING,
		FS_END,
	};

	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_LANDING,	//이동중 지형과 충돌
		MS_END,		//이동 완료
	};

protected:
	ChargeState			m_ChargeState;
	FlyState			m_FlyState;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;
	float				m_fNeedGauge;
	float				m_fReUpNeedGauge;

	ioHashStringVec		m_PreGatherAnimationList;
	ValkyrieFlyInfo		m_ValkyrieUpInfo;
	ValkyrieFlyInfo		m_ValkyrieDownInfo;
	ValkyrieFlyInfo		m_ValkyrieReUpInfo;
	
	DWORD				m_dwWaitTime;
	
	DWORD				m_dwUpStartTime;
	DWORD				m_dwDownStartTime;
	DWORD				m_dwSpecialAttackStartTime;
	DWORD				m_dwSpecialAttackEndTime;
	DWORD				m_dwCurrWeaponIdx;
	DWORD				m_dwKeyReserveTime;

	float				m_fUpCurrRange;
	float				m_fDownCurrRange;
	AttackAttribute		m_LandAttribute;

	float				m_fTargetFindAngle;
	float				m_fTargetFindMinRange;
	float				m_fTargetFindMaxAngle;
	float				m_fHighDecreaseHeight;
	float				m_fHighDecreaseGauge;

	bool				m_bDownTarget;
	D3DXVECTOR3			m_vTargetDir;

	float				m_fChargeEndNextJumpAmt;
	bool				m_bRserveReUp;

	int					m_iBuffUseEnableJumpCount;
	ioHashString		m_szDoubleJumpFlyBuff;
	ioHashStringVec		m_ChargeEndNextJumpRemoveBuffList;

public:
	void Init();
	void ClearWeapon( ioBaseChar* pOwner );
	void ClearState( ioBaseChar* pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo );

	virtual ioItem* Clone();

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void WeaponMapColDead( ioWeapon *pWeapon );

public:
	virtual WeaponSubType GetSubType() const;

public:
	ioValkyrie::FlyState GetFlyState(){ return m_FlyState; }
	const ioHashString& GetDoubleJumpFlyBuff(){ return m_szDoubleJumpFlyBuff; }
	int GetBuffUseEnableJumpCount(){ return m_iBuffUseEnableJumpCount; }

	bool IsCharCollisionSkipState();

protected:
	void OnCharging( ioBaseChar *pOwner );

public:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToSpecialState( ioBaseChar *pOwner );
	void ChangeToWaitState( ioBaseChar *pOwner );
	void ChangeToUpState( ioBaseChar *pOwner );
	void ChangeToTargetFindState( ioBaseChar *pOwner, ioPlayStage* pStage, ioUserKeyInput::DirKeyInput eCurrKey, D3DXVECTOR3 vTargetDir );
	void ChangeToDownState( ioBaseChar *pOwner );
	void ChangeToReUpState( ioBaseChar *pOwner );
	void ChangeToLanding( ioBaseChar *pOwner );
	void ChangeToEndState( ioBaseChar *pOwner );

	void SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr );

public:
	void ProcessValkyrieFlyState( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessUp( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessDown( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessReUp( ioBaseChar *pOwner, ioPlayStage* pStage );
	void ProcessLadning( ioBaseChar *pOwner, ioPlayStage* pStage );

	ioValkyrie::MoveState ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioValkyrie();
	ioValkyrie( const ioValkyrie &rhs );
	virtual ~ioValkyrie();
};

inline ioValkyrie* ToValkyrie( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_VALKYRIE )
		return NULL;

	return dynamic_cast< ioValkyrie* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioValkyrieFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_VALKYRIE_FLY"; }
	virtual const CharState GetState(){ return CS_VALKYRIE_FLY; }	
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

protected:
	int GetValkyrieFlyState( ioBaseChar* pOwner );

public:
	ioValkyrieFlySpecialState();
	virtual ~ioValkyrieFlySpecialState();
};

inline ioValkyrieFlySpecialState* ToValkyrieFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_VALKYRIE_FLY )
		return NULL;

	return dynamic_cast< ioValkyrieFlySpecialState* >( pState );
}