#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSealSkillBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	int				m_iCurCount;
	CEncrypt<int>	m_iMaxCount;
	CEncrypt<DWORD>	m_dwAccumulateDuration;
	CEncrypt<DWORD>	m_dwAccumulateDurationNpc;
	CEncrypt<DWORD>	m_dwAccumulateDurationBossNpc;
	ioHashStringVec m_vNoStopPassiveSkill;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void AccumulateBuff();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();
	void SetReserveEndBuffEx();

public:
	ioSealSkillBuff();
	ioSealSkillBuff( const ioSealSkillBuff &rhs );
	virtual ~ioSealSkillBuff();
};


inline ioSealSkillBuff* ToSealSkillBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SEAL_SKILL )
		return NULL;

	return dynamic_cast< ioSealSkillBuff* >( pBuff );
}

