#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCurseBuff : public ioBuff
{
protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;
	bool m_bDisableDownState;

	ioHashString m_CurseRollingAni;
	float m_fCurseRollingAniRate;
	ForceInfoList m_vCurseForceInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void SetExtraDurationByGrowth( float fDuration );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );
	void CheckOwnerStateCheck();

public:
	ioCurseBuff();
	ioCurseBuff( const ioCurseBuff &rhs );
	virtual ~ioCurseBuff();

};

