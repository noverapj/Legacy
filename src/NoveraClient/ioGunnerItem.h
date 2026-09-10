#pragma once


#include "ioChargeCommandItem.h"
#include "ioWeaponItem.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGunnerItem : public ioWeaponItem
{
	enum CmdInputType
	{
		CIT_ALL		= 0,
		CIT_DIR		= 1,
		CIT_ACTION	= 2,
	};
public:
	enum GunnerState
	{
		HS_NONE,
		HS_CHARGING,
		HS_HALF_GATHER_ATTACK,
		HS_NORMAL_ATTACK,
		HS_CMD_ATTACK,
		HS_FULL_CHARGE_SELLECT,
		HS_BOMB_EQUIP_START,
		HS_BOMB_EQUIP,
		HS_BOMB_THROW_FIRE,

		HS_DELAY,
	};

	enum CmdSyncType
	{
		ST_CMD_STATE,
		ST_CMD_STATE_D,
		ST_CMD_STATE_S,
	};

protected:
	GunnerState m_GunnerState;

	DWORD m_dwChargeStartTime;
	DWORD m_dwEquipBombTime;
	DWORD m_dwEquipEndTime;
	int m_iCurCombo;

	DWORD m_dwCmdAttackEndTime;

protected:
	ioHashString m_EquipAnimation;
	ioHashString m_EquipAnimation_Left;
	ioHashString m_EquipAnimation_Up;
	ioHashString m_EquipAnimation_Right;

	ioHashString m_BranchDelayAnimation;

	ioHashString m_ChargingMoveAnimation;
	bool m_bSetChargingMoveAni;

	float m_fEquipTimeRate;

protected:
	AttributeList m_vExtendAttributeList;

	//ioHashString m_BombEquipMesh;
	ioHashString m_BombEquipEffect;

	ioHashString m_BombEquipMesh_Left;
	ioHashString m_BombEquipMesh_Up;
	ioHashString m_BombEquipMesh_Right;


	DWORD	m_dwExplosionDuration;

	int		m_iWoundedDropRate;

protected:
	DWORD	m_dwExplosionStartTime;
	bool	m_bWoundedDrop;
	D3DXVECTOR3 m_vDropMoveDir;

//////////////////////////////////////////////////////////////////////////
private:
	CmdInputType m_iCmdInputType;
	ChargeCmdInfoList m_ChargeCmdInfoList;
	bool m_bCmdInputD;
	bool m_bCmdInputS;

	int m_iCurCmdCombo;
	ChargeCmdInfo m_CurChargeCmdInfo;

	ioUserKeyInput::DirKeyInput m_PreDirKey;

	char m_CurCmdInfo[MAX_COMMAND_CNT];
	int m_iCurCmdIndex;

	DWORD m_dwInputMaxTime;
	DWORD m_dwInputStartGapTime;

	DWORD m_dwInputCheckTime;
	DWORD m_dwInputGapTime;

	DWORD m_dwInputStartTime;

	int m_nCurCmd_ID;
	int m_nCurCmd_RepeateCnt;

private:
	AttributeList	m_NormalDownAttributeList;

	DWORD			m_dwHalfChargeTime;
	int				m_nSelectedBombIndex;
	AttackAttribute m_HalfChargeAttack;

	DWORD			m_dwHalfChargeAttackStartTime;
	DWORD			m_dwMotionEndTime;
	ioHashString	m_stHalfChargeBuffName;

	ioHashString	m_stFullChargeBuffName;
	ioHashString	m_stFullChargeAni;
	float			m_fFullChargeAniRate;

//////////////////////////////////////////////////////////////////////////
	ioHashString	m_GrapplingTargetName;

	float           m_fGrapOffsetX;
	float           m_fGrapOffsetY;
//////////////////////////////////////////////////////////////////////////

	CEncrypt<float> m_fIncreaseExtraGauge;
	CEncrypt<float> m_fNeedGauge;

public:
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pOwner );

private:
	void LoadCmdInfoList( ioINILoader &rkLoader );

	void ChangeHalfChargeAttack( ioBaseChar* pOwner );
	void ChangeFullChargeState( ioBaseChar* pOwner );

	void OnFullChargeState( ioBaseChar* pOwner );

	bool CheckCmdInputTime();
	bool ChangeToCmdSet( ioBaseChar *pOwner );
	bool CheckCmdInput( ioBaseChar *pOwner );
	bool ChangeToNewCmdSet( ioBaseChar *pOwner );
	bool ChangeToComboCmdSet( ioBaseChar *pOwner );

	void ClearState();

	//true면 ok  false면 해당 커맨드를 실패해야 한다.
	bool CheckCmdRepeate();

public:
	bool CheckCmdInputState( ioBaseChar *pOwner );
	virtual void CheckCmdState( ioBaseChar *pOwner );
	void ClearCmdInfo();
	void ClearCmdTime();

	void SetCmdInputStart( int iType, ioBaseChar *pOwner );
	void SetCmdInputEnd( ioBaseChar *pOwner );

protected:
	virtual void SetCmdAttack( ioBaseChar *pOwner,
		const AttackAttribute &rkAttr,
		float fExtraAniRate=1.0f,
		float fExtraForceRate=1.0f, 
		float fNeedGauge=0.0f );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnEndNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void FillNormalAttackExtraInfo( SP2Packet &rkPacket );
	virtual void ApplyNormalAttackExtraInfo( SP2Packet &rkPacket );

	virtual bool CheckOnHand();

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	void ExplosionOnHand( ioBaseChar *pOwner );

	bool CheckWoundedDropCondition();
	bool CheckExplosionOnHand( ioBaseChar *pOwner );
	void ChangeToBombOnDrop( const D3DXVECTOR3 &vMoveDir, ioBaseChar *pOwner );

public:
	const WeaponInfo& GetExplosionAttributeIdx() const;
	DWORD GetRemainDuration() const;

	//inline const ioHashString& GetBombMeshName() const { return m_BombEquipMesh; }
	const ioHashString& GetBombMeshName() const;

	bool SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	GunnerState GetGunnerState();


protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnBombEquiping( ioBaseChar *pOnwer );
	void OnBombEquip( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToBombEquip( ioBaseChar *pOwner, int nBomb );
	void ChangeToBombEquiped( ioBaseChar *pOwner );
	void ChangeToBombEquipEnd( ioBaseChar *pOwner );

	void AddBombEntity( ioBaseChar *pOwner );
	void RemoveBombEntity( ioBaseChar *pOwner );

	void CheckChargingMoveState( ioBaseChar *pOwner );

	void ApplyCmdSet( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetD( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );
	void ApplyCmdSetS( ioBaseChar *pOwner, ioHashString szCmdInfo, int iCmdCombo );

public:
	bool EnableGauge( float fNeedGauge );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	virtual int GetCurBullet();
	virtual int GetMaxBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual int GetNeedBullet();
	void DecreaseGauge( float fNeedGauge );

public:
	ioGunnerItem();
	ioGunnerItem( const ioGunnerItem &rhs );
	virtual ~ioGunnerItem();
};

inline ioGunnerItem* ToGunnerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GUNNER )
		return NULL;

	return dynamic_cast< ioGunnerItem* >( pWeapon );
}

inline ioGunnerItem* ToGunnerItem( ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GUNNER )
		return NULL;

	return dynamic_cast< ioGunnerItem* >( pWeapon );
}

inline const ioGunnerItem* ToGunnerItem( const ioWeaponItem *pWeapon )
{
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GUNNER )
		return NULL;

	return dynamic_cast< const ioGunnerItem* >( pWeapon );
}
