#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioInfectionBuff : public ioBuff
{
protected:

	bool m_bDisableDownState;
	BaseCharList m_TargetList;

	CEncrypt<float> m_fInfectionRange;

	CallBuffList m_TargetCallBuffList;
	ioHashStringVec m_TargetCallExceptBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void SetExtraDurationByGrowth( float fDuration );

public:	
	void LoadCallBuffList( ioINILoader &rkLoader );
	void LoadBuffEndRemoveBuffList( ioINILoader &rkLoader );
	void CheckExtendCallingProcess();
	void ProcessActive( float fTimePerSec );

protected:

	void CheckNonInfectionList( ioBaseChar *pInfectionHost, ioBaseChar *pInfectioner, ioPlayStage *pStage );
	void SetTargetInfection( ioHashString pInfectionHost, ioHashString pInfectionHostItemName );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );
	void CheckOwnerStateCheck();

public:
	ioInfectionBuff();
	ioInfectionBuff( const ioInfectionBuff &rhs );
	virtual ~ioInfectionBuff();

};

inline ioInfectionBuff* ToInfectionBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_INFECTION )
		return NULL;

	return dynamic_cast< ioInfectionBuff* > ( pBuff );
}

