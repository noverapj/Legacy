#pragma once


#include "ioWeapon.h"

class ioContactWeapon3 : public ioWeapon
{
protected:
	ioOrientBox m_PreAttackBox;
	ioOrientBox m_MidAttackBox;

	bool m_bFirstCheck;
	bool m_bDeadWait;

	bool m_bDisableOwnerScale;

	float m_fWeaponScaleRate;

protected:
	enum UpdateState
	{
		CONTACT_CREATED,
		CONTACT_FIRST,
		CONTACT_NORMAL,
	};
	UpdateState m_UpdateState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void SetStartCollisionBox();
	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual void GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList );

	virtual void LoadProperty( ioINILoader &rkLoader );

	void SetWeaponScaleRate( float fRate );

public:
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual WeaponType GetType() const;

	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

	void StartCollisionBoxByOwner();
	void StartCollisionBoxByStruct();
	void StartCollisionBoxByDummyChar();
	void StartCollisionBoxByMachine();

	void UpdateAttackBoxByOwner( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByStruct( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByDummyChar( float fTimePerSec, bool bSetNull );
	void UpdateAttackBoxByMachine( float fTimePerSec, bool bSetNull );

public:
	ioContactWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioContactWeapon3();
};

inline ioContactWeapon3* ToContactWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_CONTACT3 )
		return NULL;

	return dynamic_cast< ioContactWeapon3* >( pWeapon );
}

