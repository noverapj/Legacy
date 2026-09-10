#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioDummyGroup.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;


class ioEngineerItem : public ioWeaponItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,		
		AS_ATTACK_FIRE,
		AS_MAKE_DUMMY,
	};

protected:
	AttackState					m_AttackState;

	DWORD						m_dwAttackStartTime;	
	int							m_iCurCombo;
	CEncrypt<float>				m_fNeedGauge;
	ioHashStringVec				m_PreAnimationList;	

protected:
	GroupDummyInfoList			m_TurrretDummyInfoList;

	int							m_iRidingDummyCharIndex;

	ioHashString				m_TurretDummyMakeAnimation;
	float						m_fTurretDummyMakeAnimationRate;
	float						m_fTurretUpgradeRange;
	float						m_fTurretSetupOffset;
	DWORD						m_dwTurretDummyMakeEndTime;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

protected:
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToDummyMake( ioPlayStage* pStage, ioBaseChar *pOwner, int iCurIndex );
	void DummyUpgrade( ioBaseChar* pOwner, ioDummyGroup* pGroup, ioDummyChar* pCurDummy, ioDummyChar* pNewDummy );
	void DummyUpgradeAfter( ioBaseChar* pOwner, ioDummyChar* pNewDummy );

	void DestroyDummyChar( ioBaseChar *pOwner );

public:
	ioDummyGroup* GetDummyGroup( ioBaseChar* pOwner );

protected:
	const GroupDummyInfo* const GetGroupDummyInfo( const ioHashString& szDummyName );

	float GetDummyNeedGauge( const ioHashString& szDummyName );
	float GetTotalDummyGauge( ioBaseChar* pOwner );

	void FirstDummyName( OUT ioHashString& szDummyName );
	void NextDummyName( IN const ioHashString& szCurrDummyName, OUT ioHashString& szDummyName );

protected:
	ioDummyChar* CreateDummyChar( const ioHashString& szName, ioPlayStage *pStage, ioBaseChar *pOwner, int iCurIndex );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnDummyMake( ioBaseChar *pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual WeaponSubType GetSubType() const;

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioEngineerItem();
	ioEngineerItem( const ioEngineerItem &rhs );
	virtual ~ioEngineerItem();
};

inline ioEngineerItem* ToEngineerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ENGINEER_ITEM )
		return NULL;

	return static_cast< ioEngineerItem* >( pItem );
}