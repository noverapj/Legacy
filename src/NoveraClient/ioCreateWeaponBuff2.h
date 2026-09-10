#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateWeaponBuff2 : public ioBuff
{
	enum BuffCheckState
	{
		BS_NONE,
		BS_FIRE,
		BS_WAIT,
		BS_END,
	};
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDropDownState;
	CEncrypt<bool> m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

	DWORD m_dwWeaponDeadIndex;

	BuffCheckState m_BuffState;

protected:
	bool m_bEnableOutLine;

	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;

	DWORD m_dwFireStartTime;
	DWORD m_dwNextFireTime;
	WeaponInfoList m_WeaponInfoList;

	int m_nFireIndex;

	float m_fFireHeightOffSet;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	void SetWeaponDeadIndex( DWORD dwWeaponDeadIndex ){ m_dwWeaponDeadIndex = dwWeaponDeadIndex; }
	void StartFireCheckTime();

private:
	void CreateWeapon();

protected:
	//bool CheckOwnerStateCheck();
	//bool CheckRestoreStateCheck();

public:
	ioCreateWeaponBuff2();
	ioCreateWeaponBuff2( const ioCreateWeaponBuff2 &rhs );
	virtual ~ioCreateWeaponBuff2();
};


inline ioCreateWeaponBuff2* ToCreateWeaponBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_WEAPON_BUFF2 )
		return NULL;

	return dynamic_cast< ioCreateWeaponBuff2* >( pBuff );
}


