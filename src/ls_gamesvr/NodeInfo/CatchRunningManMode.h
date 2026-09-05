

#ifndef _CatchRunningManMode_h_
#define _CatchRunningManMode_h_

#include "CatchMode.h"

class CatchRunningManMode : public CatchMode
{
public:
	enum
	{
		MAX_RUNNINGMAN_DECO = 8,
	};

protected:
	DWORDVec m_RunningManDecoList;         // 1 ~ 8

public:
	virtual void LoadINIValue();
	virtual void DestroyMode();

	virtual void AddNewRecord( User *pUser );
	virtual void RemoveRecord( User *pUser, bool bRoomDestroy = false );

public:
	virtual ModeType GetModeType() const;
	virtual const char* GetModeINIFileName() const;
	virtual void GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck = false );

public:
	DWORD GetRunningManDecoIndex();
	void  SetRunningManDecoIndex( DWORD dwIndex );

public:
	virtual bool OnModeChangeChar( User *pSend, int iCharArray, bool bWait, int iSelectCharArray, DWORD dwCharChangeIndex );

public:
	virtual bool ProcessTCPPacket( User *pSend, SP2Packet &rkPacket );

protected:
	void OnRunningManNameSync( User *pUser, SP2Packet &rkPacket );

public:
	CatchRunningManMode( Room *pCreator );
	virtual ~CatchRunningManMode();
};

inline CatchRunningManMode* ToCatchRunningManMode( Mode *pMode )
{
	if( !pMode || pMode->GetModeType() != MT_CATCH_RUNNINGMAN )
		return NULL;

	return static_cast< CatchRunningManMode* >( pMode );
}

#endif

