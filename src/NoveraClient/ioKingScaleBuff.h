#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioKingScaleBuff : public ioBuff
{
protected:
	ioHashString m_ChangeSkeletonHm;
	ioHashString m_ChangeSkeletonHw;
	ioHashString m_ChangeSkeletonEm;
	ioHashString m_ChangeSkeletonEw;
	ioHashString m_ChangeSkeletonDm;
	ioHashString m_ChangeSkeletonDw;

	float m_fScaleRate;
	bool m_bRemoveBuffByDrop;
	bool m_bDisableDownState;

	ioHashString m_CurSkeleton;
	float m_fCurScaleRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();
	
public:
	ioKingScaleBuff();
	ioKingScaleBuff( const ioKingScaleBuff &rhs );
	virtual ~ioKingScaleBuff();
};

