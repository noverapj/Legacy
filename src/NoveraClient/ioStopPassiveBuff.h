#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioStopPassiveBuff : public ioBuff
{
protected:
	ioHashStringVec m_vNoStopPassiveSkill;

protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;
	bool m_bDisableDownState;
	bool m_bEnableTimeEnd;

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
	ioStopPassiveBuff();
	ioStopPassiveBuff( const ioStopPassiveBuff &rhs );
	virtual ~ioStopPassiveBuff();

};

