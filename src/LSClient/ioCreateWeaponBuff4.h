#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateWeaponBuff4 : public ioBuff
{
	struct FindBuffInfo
	{
		int nCnt;
		int nFireCnt;
	};

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


	int m_nMaxFireCnt;

protected:
	bool m_bEnableOutLine;

	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;

	DWORD m_dwFireStartTime;
	DWORD m_dwNextFireTime;
	DWORD m_dwFireTic;
	float m_fFindLength;
	//WeaponInfoList m_WeaponInfoList;
	WeaponInfo m_WeaponInfo;

	std::vector<FindBuffInfo> m_vFindBuffInfo;
	

	int m_nFireIndex;

	float m_fFireHeightOffSet;
	DWORD m_dwStartSearchTime;

	int m_target_wound_type;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	void SetWeaponDeadIndex( DWORD dwWeaponDeadIndex ){ m_dwWeaponDeadIndex = dwWeaponDeadIndex; }
	void StartFireCheckTime( ioBaseChar* pOwner );

private:
	bool FindEnemy( ioBaseChar* pOwner );
	void CreateWeapon(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir );
	bool IsEnableTargetState( ioBaseChar* pTarget );

protected:
	//bool CheckOwnerStateCheck();
	//bool CheckRestoreStateCheck();

public:
	ioCreateWeaponBuff4();
	ioCreateWeaponBuff4( const ioCreateWeaponBuff4 &rhs );
	virtual ~ioCreateWeaponBuff4();
};


inline ioCreateWeaponBuff4* ToCreateWeaponBuff4( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_WEAPON_BUFF4 )
		return NULL;

	return dynamic_cast< ioCreateWeaponBuff4* >( pBuff );
}


