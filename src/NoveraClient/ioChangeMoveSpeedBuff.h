#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioChangeMoveSpeedBuff : public ioBuff
{
public:
	enum ChangeSpeedType
	{
		CST_DECREASE,
		CST_INCREASE,
	};

protected:
	ChangeSpeedType m_ChangeSpeedType;

	CEncrypt<float> m_fMinChangeSpeed;
	CEncrypt<float> m_fMaxChangeSpeed;
	CEncrypt<float> m_fCurChangeSpeed;

	CEncrypt<float> m_fChangeGap;
	CEncrypt<DWORD> m_dwChangeTic;
	CEncrypt<DWORD> m_dwChangeCheckTime;

	bool m_bNoChangeDelay;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckChangeMoveSpeed( float fTimePerSec );

public:
	float GetCurChangeMoveSpeed() const;

public:
	ioChangeMoveSpeedBuff();
	ioChangeMoveSpeedBuff( const ioChangeMoveSpeedBuff &rhs );
	virtual ~ioChangeMoveSpeedBuff();
};

inline ioChangeMoveSpeedBuff* ToChangeMoveSpeedBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHANGE_MOVE_SPEED )
		return NULL;

	return dynamic_cast< ioChangeMoveSpeedBuff* > ( pBuff );
}




