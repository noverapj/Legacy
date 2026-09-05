#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDummyCharItem2 : public ioWeaponItem
{
public:
	enum ItemMeshType
	{
		IMT_NORMAL,
		IMT_EXTRA,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	CEncrypt<float> m_fCurGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fDecreaseGauge;
	CEncrypt<float> m_fMaxGauge;

	ioHashString    m_stChangeExtraMotion;
	float			m_fChangeExtraMotionRate;

	DWORD			m_dwMotionEndTime;

	AttackAttribute m_ChargeAttackAtt;
	DWORD			m_dwChargeAttackDummyDelayTime;
	DWORD			m_dwChargeAttackDummyFireTime;

	ioHashString		m_stSpecialEffect;
	UniqueObjID			m_dwEffect;

	ioHashString	m_DelayAnimation_extra;
	ioHashString	m_RunAnimation_extra;
	ioHashString	m_DashAnimation_extra;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_CHANGE_EXTRA,
		CS_DUMMY_CHAR,

		CS_CHANGE_NORMAL,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;

protected:
	ioHashString m_DummyCharName;
	CEncrypt<int> m_iCurDummyCharIndex;

	D3DXVECTOR3  m_vDummyCharOffset;
	ioHashStringVec m_vDestroyDummyCharList;
	ItemMeshType	m_itemMeshType;

	ioHashString	m_stExtracEffectMesh;

	std::vector< StTopMoveInfo > m_vTopMoveInfo;
	StTopMoveInfo	m_TopMoveInfo_Charge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void Initialize( ioBaseChar *pOwner, ioPlayStage* pStage );
	void SetInitialize( const ioHashString& szName, ioPlayStage* pStage, int nMeshType );

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void ChangeAttackType( ioBaseChar *pOwner, ItemMeshType meshtype, bool bMustSend = false, bool bFirst = false, bool bMustChange = false );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnDummyCharState( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );
	void OnChangingExtraMotion( ioBaseChar *pOwner );

	void DestroyDummyChar( ioBaseChar *pOwner );

public:
	void CreateDummyCharState( IN ioBaseChar *pOwner, OUT D3DXVECTOR3& vPos, OUT int& iIndex );
	void CheckDummyCharState( ioBaseChar *pOwner, int iIndex );

	void SetCurDummyCharIndex( ioBaseChar *pOwner, int iCurIndex );
	inline int GetCurDummyCharIndex() const { return m_iCurDummyCharIndex; }

	void ApplyChangeToDummyChar( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void CallItemProcess( ioBaseChar *pOwner );

	const ItemMeshType GetMeshType() const { return m_itemMeshType; }


	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	void InitGauge();
	void MaxGauge();

	float GetCurGauge();
	void SetCurGauge( float fGauge );

	bool IsEnableGauge();

	void ChangeToDummyChar( ioBaseChar *pOwner );
	void SetMoveDummyPos( ioBaseChar *pOwner, const StTopMoveInfo& info );
	void ChangeExtaMotion( ioBaseChar *pOwner );
	bool IsExtracChangeMotion();

	bool HasDelayAnimation() const;
	bool HasRunAnimation() const;
	bool HasDashRunAnimation() const;
	const ioHashString& GetDelayAnimation() const;
	const ioHashString& GetRunAnimation() const;
	const ioHashString& GetDashRunAnimation() const;

public:
	ioDummyCharItem2();
	ioDummyCharItem2( const ioDummyCharItem2 &rhs );
	virtual ~ioDummyCharItem2();
};

inline ioDummyCharItem2* ToDummyCharItem2( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DUMMY_CHAR2 )
		return NULL;

	return dynamic_cast< ioDummyCharItem2* >( pItem );
}

