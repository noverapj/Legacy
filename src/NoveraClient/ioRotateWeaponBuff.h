#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRotateWeaponBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

	CallAttributeList m_AttributeList;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void LoadCallAttributeList( ioINILoader &rkLoader );

	void CreateWeapon();
public:
	ioRotateWeaponBuff(void);
	ioRotateWeaponBuff( const ioRotateWeaponBuff &rhs );
	virtual ~ioRotateWeaponBuff();
};

inline ioRotateWeaponBuff* ToRotateWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ROTATE_WEAPON )
		return NULL;

	return dynamic_cast< ioRotateWeaponBuff* >( pBuff );
}
