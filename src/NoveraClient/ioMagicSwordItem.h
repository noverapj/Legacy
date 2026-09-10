#pragma once
#include "ioweaponitem.h"
#include "ioSpecialStateBase.h"
class ioMagicSwordItem :	public ioWeaponItem
{
public:
	enum ExtendChargeState
	{
		ECS_NORMAL,
		ECS_DASH,
	};

protected:
	enum ChargeState
	{
		CGS_NONE,
		CGS_CHARGING,
		CGS_NORMAL_ATTACK,
		CGS_ELEMENT_ATTACK,
		CGS_MOVE_ATTACK,
		CGS_BASH_ATTACK,
		CGS_MAGIC_SWORD_ELEMENT_ATTACK,
		CGS_END,
	};

	enum SyncState
	{
		SS_NORMAL_ATTACK,
		SS_ELEMENT_ATTACK,
		SS_MOVE_ATTACK,
		SS_HIT_CHAR_BY_MOVE_ATTACK,
		SS_BASH_ATTACK,
		SS_MAGIC_SWORD_ELEMENT_ATTACK,

	};

	typedef struct _tagMagicSwordStateInfo
	{
		//Move Attack
		ioHashString			m_szMoveAttackAni;
		float					m_fMoveAttackRate;
		int						m_iMoveAttackWeaponIdx;
		DWORD					m_dwMoveAttackLoopDuration;
		float					m_fMoveAttackSpeed;
		float					m_fMaxMoveRange;

		//Bash & Elemet Attack
		AttackAttribute			m_BashAttack;
		AttributeList			m_ElementComboAttack;
		WeaponInfoList			m_vAttackKeyWeaponInfoList;
		WeaponInfoList			m_vDefenseKeyWeaponInfoList;
		WeaponInfoList			m_vJumpKeyWeaponInfoList;

		void Init()
		{
			m_szMoveAttackAni.Clear();
			m_fMoveAttackRate = 0.0f;
			m_iMoveAttackWeaponIdx = 0;
			m_dwMoveAttackLoopDuration = 0;
			m_fMoveAttackSpeed = 0.0f;
			m_fMaxMoveRange = 0.0f;

			m_BashAttack.Init();
			m_ElementComboAttack.clear();
			m_vAttackKeyWeaponInfoList.clear();
			m_vDefenseKeyWeaponInfoList.clear();
			m_vJumpKeyWeaponInfoList.clear();
		}
	}MagicSwordStateInfo;

protected:
	ChargeState					m_ChargeState;

	//Normal Attack
	ioHashStringVec				m_AttackReadyAniList;
	int							m_iCurCombo;

	DWORD						m_dwMotionStartTime;
	DWORD						m_dwMotionEndTime;

	//Elemental Attack
	AttackAttribute				m_AttackKeyAttribute;
	AttackAttribute				m_DefenseKeyAttribute;
	AttackAttribute				m_JumpKeyAttribute;

	//Charge Tracking
	float						m_fChargeAutoTragetRange;
	float						m_fChargeAutoTragetMinAngle;
	float						m_fChargeAutoTragetMaxAngle;

	//Magic Sword Move Attack
	MagicSwordStateInfo			m_CurMagicSwordStateInfo;
	MagicSwordStateInfo			m_NormalMagicSwordStateInfo;
	MagicSwordStateInfo			m_DashMagicSwordStateInfo;

	float						m_fCurMagicSwordAttackMoveSpeed;
	D3DXVECTOR3					m_vCurMagicSwordAttackMoveDir;
	float						m_fCurMaxRange;
	float						m_fCurRange;
	int							m_iCurMagicSwordElementCount;
	
	//Gauge
	float						m_fMaxGauge;
	float						m_fNeedGauge;
	float						m_fIncreaseGauge;
	float						m_fCurGauge;

	bool						m_bHitCharByMoveAttack;
	

protected:
	void ClearData();
	void ClearMagicSwordData();

	void LoadMagicSwordStateInfoProperty( ioINILoader &rkLoader );

	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessNormalAttack( ioBaseChar *pOwner );
	void ProcessElementalAttack( ioBaseChar *pOwner );

	//CS_MAGIC_SWORD_ATTACK
	void ProcessMoveAttack( ioBaseChar *pOwner );
	void ProcessMagicSwordMove( ioBaseChar *pOwner );
	void ProcessBashAttack( ioBaseChar *pOwner );
	void ProcessMagicSwordElementAttack( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToNormalElementAttack( ioBaseChar *pOwner );
	void ChangeToMoveAttack( ioBaseChar *pOwner );
	void ChangeToBashAttack( ioBaseChar *pOwner );
	void ChangeToMagicSwordElementAttack( ioBaseChar *pOwner );

	const ioBaseChar* TargetSearch( ioBaseChar *pOwner, float fTargetMinAngle, float fTargetMaxAngle, float fTargetRange );

public:
	void SetMagicSwordAttackState( ioBaseChar *pOwner, ExtendChargeState ExtendState );
	void ProcessMagicSwordAttackState( ioBaseChar *pOwner );
	bool IsEnableMagicSwordGauge();
	void DecreaseGauge();


public:
	virtual WeaponSubType GetSubType() const;
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

protected:
	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );
public:
	ioMagicSwordItem(void);
	ioMagicSwordItem( const ioMagicSwordItem &rhs );
	virtual ~ioMagicSwordItem(void);
};

inline ioMagicSwordItem* ToMagicSwordItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_MAGIC_SWORD_ITEM )
		return NULL;

	return dynamic_cast< ioMagicSwordItem* >( pItem );
}





//-----------------------------------------------------------------------------------------------------------

class ioMagicSwordSpecialState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_MAGIC_SWORD_ATTACK"; }
	virtual const CharState GetState(){ return CS_MAGIC_SWORD_ATTACK; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioMagicSwordSpecialState();
	virtual ~ioMagicSwordSpecialState();
};

inline ioMagicSwordSpecialState* ToMagicSwordSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_MAGIC_SWORD_ATTACK )
		return NULL;

	return dynamic_cast< ioMagicSwordSpecialState* >( pState );
}