#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHide4Buff : public ioBuff
{
protected:
	enum BuffState
	{
		BS_NONE,
		BS_LOOP,
		BS_ENEMY_LOOP,
		BS_ENEMY_FIX,
	};

	enum EnemyVisibleType
	{
		EVT_LOOP,
		EVT_FIX,
	};

protected:
	BuffState m_BuffState;
	EnemyVisibleType m_EnemyVisibleType;
	bool m_bSetAlphaRateDel;
	int m_nCurAlphaRateSpeed;
	DWORD m_dwCheckAlphaDelayTime;
	DWORD m_dwCheckAlphaChangeTime;

protected:
	bool m_bDisableDownState;
	float m_fAddSpeed;
	
	int m_nAlphaRateSpeed;
	DWORD m_dwAlphaDelayTime;
	DWORD m_dwAlphaChangeTime;

	float m_fFixedAlphaRate;
	bool m_bReverseTestTeam;

	bool m_bTeamCircleHide;
	float m_fFixedEnemyAlphaRate;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );		

protected:
	bool CheckOwnerStateCheck();

protected:
	void Init();
	void SetFuncState( ioBaseChar *pOwner );
	void ProcessAlphaRate( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );
	bool IsTeamTest( TeamType eOwnerType, TeamType ePlayerType );

public:
	ioHide4Buff();
	ioHide4Buff( const ioHide4Buff &rhs );
	virtual ~ioHide4Buff();
};

inline ioHide4Buff* ToHide4Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HIDE4 )
		return NULL;

	return dynamic_cast< ioHide4Buff* >( pBuff );
}