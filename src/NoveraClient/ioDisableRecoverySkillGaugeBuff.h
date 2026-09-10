#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDisableRecoverySkillGaugeBuff : public ioBuff
{
protected:
	
	enum DisableRecoverySkillGaugeType
	{
		DRST_DUMMY,
		DRST_DURATION,
		DRST_OBJECTITEM,
	};

	DisableRecoverySkillGaugeType m_DisableType;
	CEncrypt<int> m_iArmorType;
	ioHashString m_DummyCharName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	void CheckDisableBuff();
	int inline GetOwerItemEquipSlot(){ return m_iArmorType; }

public:
	ioDisableRecoverySkillGaugeBuff();
	ioDisableRecoverySkillGaugeBuff( const ioDisableRecoverySkillGaugeBuff &rhs );
	virtual ~ioDisableRecoverySkillGaugeBuff();
};

inline ioDisableRecoverySkillGaugeBuff* ToDisableRecoverySkillGaugeBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DISABLE_RECOVERY_SKILL_GAUGE )
		return NULL;

	return dynamic_cast< ioDisableRecoverySkillGaugeBuff* >( pBuff );
}