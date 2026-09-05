#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioControlWeaponBuff2 : public ioBuff
{
protected:
	struct PreTargetInfo
	{
		ioHashString	m_Name;
		DWORD			m_dwNextFireTime;
	};
	typedef std::vector<PreTargetInfo>	PreTargetInfoList;

protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	CEncrypt<bool> m_bEscapeCatchState;

	CEncrypt<float> m_fSearchRange;
	CEncrypt<float> m_fSearchAngle;
	TargetWoundType m_TargetWoundType;
	//BaseCharDistanceSqList	m_vTargetList;
	ioHashStringVec	m_vTargetList;
	//PreTargetInfoList		m_vPreTargetList;

	CEncrypt<DWORD>	m_dwFireGapTime;
	DWORD m_dwFireTime;
	DWORD m_dwNextFireTime;

	int m_iMaxFireCnt;
	int m_iCurFireCnt;


	//////////////////////////////////////////////////////////////////////////
	int				m_nTargetMaxCount;
	float			m_fTargetUpHeight;
	float			m_fTargetDownHeight;

	WeaponInfo m_WeaponInfo;

	int				m_nTargetFindMode;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void FindTarget();
	bool IsEnableTargetState( ioBaseChar *pTarget );
	void ProcessControlWeapon();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

protected:

public:
	ioControlWeaponBuff2();
	ioControlWeaponBuff2( const ioControlWeaponBuff2 &rhs );
	virtual ~ioControlWeaponBuff2();
};


inline ioControlWeaponBuff2* ToControlWeaponBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CONTROL_WEAPON2 )
		return NULL;

	return dynamic_cast< ioControlWeaponBuff2* >( pBuff );
}


