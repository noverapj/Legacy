#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGalaxyItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_EQUIP_OBJECT,
		CS_END,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_GATHER,
		ST_RESET_GATHER,
	};

protected:
	ChargeState		m_ChargeState;

	DWORD			m_dwAttackStartTime;
	int				m_iCurCombo;
		
	CEncrypt<float>	m_fNeedGPGauge;
		
	DWORD			m_dwObjectItem;
	ioHashString	m_ObjectEquipAnimation;
	float			m_fObjectEquipAnimationRate;

	//개더링 관련
	CEncrypt<float>	m_fGatheringScaleRate;
	CEncrypt<float>	m_fGatheringScaleMaxRate;
	float			m_fCurrScaleRate;

	ioHashStringVec	m_PreGatherAnimationList;
	ioHashString	m_GatherAnimation;
	ioHashString	m_GatherEffect;
	ioHashString	m_GatherMeteoEffect;
	float			m_fHeightOffSet;

	UniqueObjID		m_dwGatherEffectID;
	UniqueObjID		m_dwGatherMeteoEffectID;
	float			m_fMaxScaleRate;

	//D꾹 이동관련
	bool			m_bChangeDir;
	bool			m_bExtendMotionSetted;
	bool			m_bFirstExtendMoveMotion;
	float			m_fExtendMoveAniRate;
	float			m_fMoveSpeedRateExtend;
	ioHashString	m_ExtendMoveFront;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

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
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	

	virtual float GetAttackSpeedRate() const;
	virtual void ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void ResetGathering( ioBaseChar* pOwner );

	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );

	void ChangeToGatheringState( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	
	void CheckExtendMoveAniState( ioBaseChar *pOwner );	
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	virtual void EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	bool IsEquipObject( ioBaseChar *pOwner );
	void EquipObjectItem( ioBaseChar *pOwner );
	void ReleaseObject( ioBaseChar *pOwner );
		
public:
	void FillGalaxyInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetGalaxyInfo( ioBaseChar *pOwner, SP2Packet &rkPacket  );

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

public:
	ioGalaxyItem();
	ioGalaxyItem( const ioGalaxyItem &rhs );
	virtual ~ioGalaxyItem();
};

inline ioGalaxyItem* ToGalaxyItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GALAXY_ITEM )
		return NULL;

	return dynamic_cast< ioGalaxyItem* >( pItem );
}

