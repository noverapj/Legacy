#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioCheckChainBuff2 : public ioBuff
{
public:
	enum BuffState
	{
		BS_START,
		BS_WAIT,
	};
	
protected:
	int m_nChainWeapon;
	ioHashStringVec m_OwnerBuffList;
	ioHashString m_szCreatorEffect;
	D3DXVECTOR3 m_vCreatorEffectOffset;
	float m_fCheckDist;

protected:
	BuffState m_BuffState;
	DWORD m_dwChainWeaponIndex;
	DWORD m_dwCreatorEffectID;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void CreateChain( ioBaseChar *pOwner );
	void SetChainPos( ioBaseChar *pOwner );
	
protected:
	void SetEndState( ioBaseChar *pOwner );

protected:
	void AddOwnerBuff( ioBaseChar *pOwner );
	void RemoveOwnerBuff( ioBaseChar *pOwner );

protected:
	DWORD CreateEffect( ioPlayStage *pStage, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioPlayStage *pStage, DWORD& dwEffectID );

protected:
	bool CheckDist( ioBaseChar *pCreator, ioBaseChar *pOwner );

public:
	ioCheckChainBuff2();
	ioCheckChainBuff2( const ioCheckChainBuff2 &rhs );
	virtual ~ioCheckChainBuff2();
};

inline ioCheckChainBuff2* ToCheckChainBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_CHAIN2 )
		return NULL;

	return dynamic_cast< ioCheckChainBuff2* >( pBuff );
}