#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioGeomjeItem : public ioWeaponItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,
		AS_ATTACK_FIRE,
		AS_EXTEND_ATTACK_FIRE,		
		AS_SWORD_ADD,
		AS_DUMMY_ATTACK,
	};

	enum AttackSyncState
	{
		ASA_ATTACK,
		ASA_EXTEND_ATTACK,
		ASA_SWORD_ADD,
		ASA_DUMMY_ATTACK,
		ASA_CLEAR_BUFF,
		ASA_END_SWORD,
	};

	enum SwordChangeState
	{
		SCS_NONE,
		SCS_PRE_SWORD,
		SCS_SWORD,
	};

	enum SwordAttackState
	{
		SAS_NONE,
		SAS_NORMAL_ATTACK,
		SAS_DASH_ATTACK,
		SAS_EXTRA_DASH_ATTACK,
		SAS_EXTEND_DASH_ATTACK,
	};

	struct OffsetProperty
	{
		D3DXVECTOR3		m_vOffset;
		float			m_fVert;
		float			m_fHorz;
		DWORD			m_dwAttributeIndex;

		OffsetProperty()
		{
			m_vOffset			= ioMath::VEC3_ZERO;
			m_fVert				= 0.0f;
			m_fHorz				= 0.0f;
			m_dwAttributeIndex	= 0;
		}
	};
	typedef std::vector<OffsetProperty> OffsetPropertyList;

	struct AttackInfoProperty
	{
		DWORD			m_dwAttributeIndex;
		DWORD			m_dwFireTime;
		
		OffsetProperty	m_OffsetProperty;
		float			m_fNeedGauge;		

		AttackInfoProperty()
		{
			m_dwAttributeIndex	= 0;
			m_dwFireTime		= 0;
			m_fNeedGauge		= 0.0f;
		}
	};
	typedef std::vector<AttackInfoProperty> AttackInfoPropertyList;

	struct AttackInfo
	{
		AttackInfoPropertyList m_AttackInfoPropertyList;

		AttackInfo()
		{
		}
	};
	typedef std::vector<AttackInfo> AttackInfoList;
	typedef std::vector<AttackAttribute> AttackAttributeList;

protected:
	AttackState					m_AttackState;
	SwordChangeState			m_SwordChangeState;

	DWORD						m_dwAttackStartTime;
	DWORD						m_dwBaseWeaponIndex;
	int							m_iCurCombo;
	CEncrypt<float>				m_fNeedGauge;
	ioHashStringVec				m_PreAnimationList;	

	AttackInfoList				m_NormalAttackInfoList;

	AttackInfoList				m_DashAttackInfoList;
	AttackInfoList				m_DashExtraAttackInfoList;
	AttackInfoList				m_DashExtendAttackInfoList;

	AttackInfoList				m_JumpAttackInfoList;
	AttackInfoList				m_JumpExtendAttackInfoList;
	AttackInfoList				m_JumpDashAttackInfoList;

protected:	
	AttackAttribute				m_ExtendAttribute;
	AttackAttribute				m_SwordExtendAttribute;
	OffsetPropertyList			m_SwordExtendAttributeOffsetList;

protected:
	AttackAttribute				m_SwordAddChangeAttribute;
	DWORD						m_dwSwordAddChangeEndTime;

protected:
	DWORD						m_dwSwordStateChangeTime;
	DWORD						m_dwSwordStateChangeStartTime;

protected:
	AttackInfoPropertyList		m_CurrAttackInfoList;
	FireTimeType				m_CurrFireTimeType;
	DWORD						m_dwCurrAttackStartTime;

protected:
	ioHashString				m_SwordDummyBuff;
	ioHashString				m_SwordTargetDummy;
	
	AttackAttributeList			m_SwordControlAttributeList;

	int							m_iSwordDummyDefaultAttackCode;
	int							m_iCurSwordControlCombo;

	DWORD						m_dwClearStartTime;
	DWORD						m_dwDummyAttackEndTime;
	float						m_fSwordAttackNeedGauge;

protected:
	ioHashString				m_SwordEffect;
	ioHashString				m_SwordEndEffect;
	UniqueObjID					m_SwordEffectID;
	UniqueObjID					m_SwordEndEffectID;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadAnimation( const char* szTitle, ioINILoader &rkLoader, ioHashStringVec& List );
	void LoadAttrbute( const char* szTitle, AttackInfoList& List, ioINILoader &rkLoader );
	void LoadAttrbuteProperty( const char* szTitle, AttackInfoPropertyList& List, ioINILoader &rkLoader );
	void LoadOffsetProperty( const char* szTitle, OffsetProperty& Offset, ioINILoader &rkLoader );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	void SetBaseAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );
	void SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );

	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve );
	ioWeapon* AttackFire( ioBaseChar *pOwner, DWORD dwAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir, DWORD dwWeaponIndex, FireTimeType eType );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );	
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );
	void ChangeToSwordAddChange( ioBaseChar *pOwner );
	void ChangeToExtendAttackFireBySword( ioBaseChar *pOwner );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void ProcessAttackFireList( ioBaseChar *pOwner, FireTimeType eType );

	void OnCharging( ioBaseChar *pOwner );
	void OnSwordAdd( ioBaseChar *pOwner );

	void CheckSwordAddChange( ioBaseChar *pOwner );

public:
	void SetDashAttack( ioBaseChar *pOwner, int iCurCombo );
	void SetExtraDashAttack( ioBaseChar *pOwner, int iCurCombo );
	void SetExtendDashAttack( ioBaseChar *pOwner, int iCurCombo );
	void CheckPowerDash( ioBaseChar *pOwner );

public:
	void SetJumpAttack( ioBaseChar *pOwner, int iCurCombo );
	void SetJumpExtendAttack( ioBaseChar *pOwner, int iCurCombo );
	void SetJumpDashAttack( ioBaseChar *pOwner, int iCurCombo );

	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessLanding( ioBaseChar *pOwner );

protected:
	bool CheckToSwordExtend( ioBaseChar *pOwner );
	void ChangeToSwordExtend( ioBaseChar *pOwner );
	void OnSwordExtend( ioBaseChar *pOwner );
	void ClearSwordControl( ioBaseChar* pOwner );

protected:
	void ChangeToSwordEnd( ioBaseChar *pOwner );

public:
	virtual void CallItemProcess( ioBaseChar* pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	void FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	int GetDummyBuffUserCount( ioBaseChar *pOwner );

protected:
	void CreateEffect( ioBaseChar *pOwner );
	void DestroyEffect( ioBaseChar *pOwner );

	void CreateEndEffect( ioBaseChar *pOwner );
	void DestroyEndEffect( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

public:
	bool IsEnableSwordAdd( ioBaseChar* pOwner );
	ioGeomjeItem::SwordChangeState IsSwordChangeState(){ return m_SwordChangeState; }

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	int GetSwordAttackNeedBullet();

public:
	ioGeomjeItem();
	ioGeomjeItem( const ioGeomjeItem &rhs );
	virtual ~ioGeomjeItem();
};

inline ioGeomjeItem* ToGeomjeItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_GEOMJE_ITEM )
		return NULL;

	return static_cast< ioGeomjeItem* >( pItem );
}