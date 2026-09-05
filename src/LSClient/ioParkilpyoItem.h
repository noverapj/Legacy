#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

#define MAX_COMMAND_CNT 11

class ioParkilpyoItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_CHARGE_ATTACK,
		CS_CHARGE_COMBO,
		CS_EQUIP_OBJECT,
		CS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_RESET_ATTACK_MOVE,
		ST_CHARGE_ATTACK,
		ST_CHARGE_COMBO,
		ST_GATHERING,
		ST_OBJECT_EQUIP,
	};

	enum EquipObjectState
	{
		EOS_NONE,
		EOS_NORMAL,
		EOS_JUMP,
		EOS_DASH,
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
	AttackAttribute m_ChargeAttackAttribute;
	AttributeList m_ChargeComboList;

	DWORD m_dwInputMaxTime;
	DWORD m_dwInputGapTime;
	DWORD m_dwInputStartGapTime;

	ioHashString m_szEquipCmd;

protected:
	ChargeState	m_ChargeState;

	DWORD m_dwAttackStartTime;
	int m_nCurCombo;
	DWORD m_dwMotionEndTime;
	DWORD m_dwEnableKeyTime;

	int m_nCurChargeCombo;
	
	bool m_bEquipObjectItem;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	int m_nCurCmdIndex;
	ioUserKeyInput::DirKeyInput m_PreDirKey;
	DWORD m_dwInputStartTime;
	DWORD m_dwInputCheckTime;
	
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
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

public:	
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );

protected:
	void Init();
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToChargeComboAttack( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );
	bool ChangeToEquipObject( ioBaseChar* pOwner, EquipObjectState eState );

	bool CheckChargeComboInput( ioBaseChar *pOwner );

public:
	void ChangeEquipObjectState( ioBaseChar* pOwner, EquipObjectState eState = EOS_NORMAL );

	bool IsEnableEquipObject();
	bool IsEquipObject( ioBaseChar *pOwner );
	void ReleaseObject( ioBaseChar *pOwner );

	virtual void EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	void FillParkilpyoInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetParkilpyoInfo( ioBaseChar *pOwner, SP2Packet &rkPacket  );
	bool CheckReleaseObjectItem( ioBaseChar *pOwner );

public:
	void ClearCmdInfo();
	void ClearCmdTime();
	
	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );

	bool CheckCmdInputState( ioBaseChar *pOwner );

protected:
	bool ChangeToCmdEquip( ioBaseChar *pOwner );
	bool CheckCmdInputTime();
	bool CheckCmdInput( ioBaseChar *pOwner );

public:
	ioParkilpyoItem();
	ioParkilpyoItem( const ioParkilpyoItem &rhs );
	virtual ~ioParkilpyoItem();
};

inline ioParkilpyoItem* ToParkilpyoItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PARKILPYO_ITEM )
		return NULL;

	return dynamic_cast< ioParkilpyoItem* >( pItem );
}

