#pragma once

#include "ioObjectItem.h"

class ioWeapon;
class ioBaseChar;

class ioSpiralChargeObjectItem : public ioObjectItem
{
public:
	struct ScaleInfo
	{
		float			m_fScaleRate;
		DWORD			m_dwScaleTime;

		AttackAttribute	m_JumpAttackAttribute;
		AttackAttribute	m_NormalAttackAttribute;

		ScaleInfo()
		{
			Init();
		}
		void Init()
		{
			m_fScaleRate	= 0;
			m_dwScaleTime	= 0;

			m_JumpAttackAttribute.Init();
			m_NormalAttackAttribute.Init();		
		}
	};
	typedef std::vector<ScaleInfo> ScaleInfoList;

protected:
	ScaleInfoList	m_vChargingInfoList;
	ioHashString	m_szChargingSound;
	DWORD			m_dwChargingSoundID;

	ioHashString	m_WeaponChargingEffect;
	UniqueObjID		m_WeaponChargingEffectID;

	float			m_fMaxScaleRate;
	float			m_fMinScaleRate;
	DWORD			m_fMaxScaleTime;
	int				m_iCurChargingIndex;

	D3DXVECTOR3		m_vLinkOffsetPos;	

protected:
	WeaponInfoList	m_WoundedAttackList;

	DWORD			m_dwStartTime;
	DWORD			m_dwMotionEndTime;
	int				m_iReduceGaugeSlot;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	void LoadChargingScale( IN ioINILoader &rkLoader, IN const ioHashString& szTitle, OUT ScaleInfoList& vList );

public:
	virtual ioItem* Clone();

public:
	void Init();

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDefense() const;
	virtual bool IsEnableDash() const;

public:
	virtual ObjectSubType			GetObjectSubType() const;
	virtual const AttackAttribute*	GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	virtual const WeaponInfoList&	GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ForceInfoList&	GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;

	D3DXVECTOR3 GetLinkPosOffset( ioEffect* pEffect, ioBaseChar* pOwner );
	D3DXVECTOR3 GetWoundedPos(){ return m_vLinkOffsetPos; }

	int	GetCurrActiveCount( ioBaseChar* pOwner );

protected:
	void CreateEffect( ioBaseChar* pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	int SetAttackMotionTime( ioBaseChar *pOwner, AttackType eType );

public:
	void SetDashAttack( ioBaseChar *pOwner );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	void UpdateEffect( ioPlayStage *pStage, ioBaseChar *pOwner );
	void UpdateCharging( ioPlayStage *pStage, ioBaseChar *pOwner );
	void UpdateSound( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	void CheckCount( ioPlayStage *pStage, ioBaseChar *pOwner );

protected:
	void ReleaseChargingEffect( ioBaseChar *pOwner );	

public:	
	const bool IsAttackEnd( ioBaseChar* pOwner );

public:
	ioSpiralChargeObjectItem();
	ioSpiralChargeObjectItem( const ioSpiralChargeObjectItem &rhs );
	virtual ~ioSpiralChargeObjectItem();
};

inline ioSpiralChargeObjectItem* ToSpiralChargeObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_SPIRAL_CHARGE )
		return NULL;

	return dynamic_cast< ioSpiralChargeObjectItem* >( pObject );
}

