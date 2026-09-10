#pragma once

#include "ioWeaponItem.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"
#include "ioUroborusGuideArrow.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioMutantItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_ADD_EXTEND_ATTACK,
		CS_NORMAL_ATTACK,
		CS_NONE_CHARGE_NORAML,
	};

	enum MutantSpecialState
	{
		MSS_NONE,
		MSS_PRE_DELAY,
		MSS_MOVE,
	};

	enum SyncSubType
	{
		SST_ROTATE,//플라이 상태일때 상승 하강, 회전등을 처리할때
		SST_JUMP,
		SST_END,
	};

	struct MutantSpecialJumpPowerInfo
	{
		float fRange;
		float fJumpPower;

		ioHashString stJumpAni;
		float fJumpAniRate;

		MutantSpecialJumpPowerInfo()
		{
			fRange = 0.f;
			fJumpPower = 0.f;

			fJumpAniRate = FLOAT1;
		}
	};

protected:
	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	MutantSpecialState m_MsState;

	AttackAttribute m_SpecialMovePreDelayAttack;
	DWORD			m_SpecialMovePreDelayWeaponIndex;
	float			m_fSpecialMovePreDelayAttackAngle;

	DWORD			m_dwSpecialPreDelayAniFullTime;

	AttackAttribute m_SpecialMove_MoveAttack;
	
	DWORD			m_dwContactWeaponIndex;

	float			m_fSpecialMoveSpeed;
	float			m_fSpecialMoveMaxRange;
	float			m_fSpecialMoveVolumRate;

	ioUserKeyInput::DirKeyInput m_CurDirKey;
	ioUserKeyInput::DirKeyInput m_PreDirKey;

	DWORD			m_dwSpecialMoveRotateTime;

	std::vector<MutantSpecialJumpPowerInfo> m_vSpecialMovejumpInfo;

	float			m_fArrowWeaponAngle;

protected:
	GhostStealerInfo m_AttackInfo;

	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

	D3DXVECTOR3 m_vForceDir;

	// uroborus
	DWORD m_dwFireStartTime;

	CEncrypt<bool> m_bSetUroborusSkill;

	// uroborus bullet
	CEncrypt<DWORD> m_dwMaxCoolTime;
	CEncrypt<DWORD> m_dwNeedCoolTime;

	CEncrypt<float> m_fCurCoolTime;
	CEncrypt<float> m_fRecoveryTime;


	//악령 도둑 추가
	DWORD	m_dwFireEndTime;
	DWORD	m_dwFullAnimationTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner );

	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
		float &fRange, float &fMinAngle, float &fMaxAngle,
		AutoTargetType eType );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );

	//////////////////////////////////////////////////////////////////////////
	void SetSpecialMoveInitialize( ioBaseChar *pOwner );
	void CheckExtendAttack( ioBaseChar *pOwner );

	MutantSpecialState GetSpecialState();

	void SetEndSpecialState( ioBaseChar* pOwner );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();

	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

	inline float GetUroborusCurAngle() const { return m_AttackInfo.m_fCurAngle; }


	float GetSpecialMoveMaxRange();
	float GetSpecialMoveSpeed();
	float GetSpecialMoveVolumeRate();

	void SetSpecialEndJump( ioBaseChar* pOwner, float fCurMoveRange );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnAttackFire( ioBaseChar *pOwner );
	void OnAddExtendAttackFire( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	void ClearState();
	virtual void CheckReserve( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

	void ChangeSpecialMoveState( ioBaseChar *pOwner );
public:
	ioMutantItem();
	ioMutantItem( const ioMutantItem &rhs );
	virtual ~ioMutantItem();
};

inline ioMutantItem* ToMutantItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MUTANT_ITEM )
		return NULL;

	return dynamic_cast< ioMutantItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioMutantItemSpecialState : public ioSpecialStateBase
{
protected:

	float m_fCurRange;

	float m_fEndJumpAmt;

	float m_fPreAddSineValue;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_MUTANT_SPECIAL_STATE"; }
	virtual const CharState GetState(){ return CS_MUTANT_SPECIAL_STATE; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }		

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	void SetMutantMoveInfo( ioBaseChar* pOwner );

public:
	ioMutantItemSpecialState();
	virtual ~ioMutantItemSpecialState();
};

inline ioMutantItemSpecialState* ToMutantSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MUTANT_SPECIAL_STATE )
		return NULL;

	return dynamic_cast< ioMutantItemSpecialState* >( pState );
}