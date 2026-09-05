#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCallRandomBuff : public ioBuff
{
protected:
	typedef std::vector<ioHashStringVec> vCallRandomBuff;

protected:
	bool m_bDisableDownState;

	CEncrypt<int>   m_iCallRandomBuffCount;
	CEncrypt<DWORD> m_dwCallRandomBuffTime;

	vCallRandomBuff m_vCallRandomBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

	void CheckCallingRandomBuff();

public:
	ioCallRandomBuff();
	ioCallRandomBuff( const ioCallRandomBuff &rhs );
	virtual ~ioCallRandomBuff();
};
