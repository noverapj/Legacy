#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDestroyCallBuff : public ioBuff
{
protected:
	
	//대기 이펙트, 이펙트 고유 번호
	ioHashString m_szDelayEffect;
	DWORD m_dwDelayEffectID;
	
	//상태 제한 버프 이름
	ioHashString m_szRemoveBuffName;
	//공격키입력시 폭발할것인가
	bool m_bIsUseKeyDownDestroy;
	//폭발 시간은?
	DWORD m_dwDestroyStartTime;
	DWORD m_dwDestroyEndTime;

	bool m_bRcvDestroy;
	bool m_bDestroySeted;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool CheckDestroy( ioBaseChar *pOwner );
	bool CheckDestroyTime();

	//추가
protected:
	//데이터 읽기
	void LoadDestroyProperty( ioINILoader &rkLoader );

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	ioDestroyCallBuff();
	ioDestroyCallBuff( const ioDestroyCallBuff &rhs );
	virtual ~ioDestroyCallBuff();
};

inline ioDestroyCallBuff* ToDestroyCallBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CALL_DESTROY )
		return NULL;

	return dynamic_cast< ioDestroyCallBuff* >( pBuff );
}