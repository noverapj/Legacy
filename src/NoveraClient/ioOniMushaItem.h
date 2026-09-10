#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioOniMushaItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_EQUIP_OBJECT,
		CS_CHARGE_RUN,
		CS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_GATHERING,
		ST_OBJECT_EQUIP,
		ST_RUN_START,
		ST_RUN_END,
		ST_INPUT_JUMP,
		ST_INPUT_ATTACK,
		ST_ROTATE,
	};

	enum EquipObjectState
	{
		EOS_NONE,
		EOS_NORMAL,
		EOS_JUMP,
		EOS_DASH,
	};

	enum RunState
	{
		RS_NONE,
		RS_WAIT,
		RS_RUN,
		RS_RUN_ATTACK,
		RS_END,
	};

protected:
	float m_fNeedGauge;

	DWORD m_dwObjectItem;

	ioHashStringVec	m_szPreGatherAniList;
	ioHashString m_szGatherAni;
	float m_fGatherAniRate;
	
	AttackAttribute	m_EquipAttribute;
	AttackAttribute	m_JumpEquipAttribute;
	AttackAttribute	m_DashEquipAttribute;

	float m_fRunSpeed;
	DWORD m_dwRunRotateTime;

	ioHashString m_RunWaitAni;
	float m_fRunWaitAniRate;

	ioHashString m_RunLoopAni;
	float m_fRunLoopAniRate;
	DWORD m_dwRunLoopTime;
	
	AttackAttribute	m_RunAttack;
	AttackAttribute	m_RunEndAttack;
	ioHashStringVec m_RunBuffList;
	
	bool m_bOnlyTeamCharColSkip;

	float m_fChargeUseGauge;

protected:
	ChargeState	m_ChargeState;
	RunState m_RunState;

	DWORD m_dwAttackStartTime;
	int m_nCurCombo;
	DWORD m_dwMotionEndTime;
	
	bool m_bEquipObjectItem;

	DWORD m_dwRunLoopEndTime;

	bool m_bTargetRot;
	bool m_bLeftRot;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
		
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );		
	
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual bool IsMoveAttackState( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

public:	
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	float GetChargeUseGauge();

protected:
	void Init();
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	bool ChangeToEquipObject( ioBaseChar* pOwner, EquipObjectState eState );

public:
	void ChangeToRunState( ioBaseChar *pOwner );

protected:
	void ChangeToRunning( ioBaseChar *pOwner );
	void ChangeToRunAttack( ioBaseChar *pOwner );
	void ChangeToRunEnd( ioBaseChar *pOwner );

	void OnRunState( ioBaseChar *pOwner );
	void OnRunWait( ioBaseChar *pOwner );
	void OnRunning( ioBaseChar *pOwner );
	void OnRunAttack( ioBaseChar *pOwner );
	void OnRunEnd( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckDirKeyInput( ioBaseChar *pOwner );
	void ProcessRotate( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	bool IsNoDropState( ioBaseChar *pOwner );

public:
	void ChangeEquipObjectState( ioBaseChar* pOwner, EquipObjectState eState = EOS_NORMAL );
	bool IsEnableEquipObject();
	bool IsEquipObject( ioBaseChar *pOwner );
	void ReleaseObject( ioBaseChar *pOwner );

public:
	void FillOnimushaInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetOnimushaInfo( ioBaseChar *pOwner, SP2Packet &rkPacket  );
	bool CheckReleaseObjectItem( ioBaseChar *pOwner );

protected:
	void LoadRunBuff( ioINILoader &rkLoader, const char* szName, ioHashStringVec &BuffList );
	void AddRunBuff( ioBaseChar *pOwner );
	void RemoveRunBuff( ioBaseChar *pOwner );

public:
	ioOniMushaItem();
	ioOniMushaItem( const ioOniMushaItem &rhs );
	virtual ~ioOniMushaItem();
};

inline ioOniMushaItem* ToOniMushaItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ONIMUSHA_ITEM )
		return NULL;

	return dynamic_cast< ioOniMushaItem* >( pItem );
}