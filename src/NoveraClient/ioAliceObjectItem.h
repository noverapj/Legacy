#pragma once

#include "ioObjectItem.h"

class ioWeapon;
class ioBaseChar;

class ioAliceObjectItem : public ioObjectItem
{
public:
	struct BulletSet
	{
		int				m_iMaxWeaponWay;		
		BulletInfoList	m_BulletElmentList;

		BulletSet()
		{
			m_iMaxWeaponWay = 0;			
		}
	};
	typedef std::vector<BulletSet> BulletSetList;

protected:
	BulletSetList	m_NormalBulletSetList;
	BulletSetList	m_DashBulletSetList;
	BulletSetList	m_JumpBulletSetList;
	BulletSetList	m_DashJumpBulletSetList;

	int				m_iWeaponWay;
	int				m_iCurCombo;

	bool			m_bReserveRelease;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadBulletProperty( ioINILoader &rkLoader, const ioHashString& szTitle, BulletSetList& rkList );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );	

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;

public:
	void NotifyDashAttack( ioBaseChar *pOwner );
	void NotifyJumpAttack( ioBaseChar *pOwner, int iCurCombo );
	void NotifyDashJumpAttack( ioBaseChar *pOwner, int iCurCombo );

public:
	bool IsReleaseItem( const ioBaseChar* pOwner ) const;
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner );

	bool CheckReleaseAliceGun(  ioPlayStage *pStage, ioBaseChar *pOwner, bool bDefenseKeyCheck = true );

public:
	ioAliceObjectItem();
	ioAliceObjectItem( const ioAliceObjectItem &rhs );
	virtual ~ioAliceObjectItem();
};

inline ioAliceObjectItem* ToAliceObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_ALICE )
		return NULL;

	return dynamic_cast< ioAliceObjectItem* >( pObject );
}

