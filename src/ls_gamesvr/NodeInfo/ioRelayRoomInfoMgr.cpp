#include "stdafx.h"
#include "ioRelayRoomInfoMgr.h"


ioRelayRoomInfoMgr::ioRelayRoomInfoMgr(void)
{
	InitData();
}


ioRelayRoomInfoMgr::~ioRelayRoomInfoMgr(void)
{
}

void ioRelayRoomInfoMgr::InitData()
{
	m_roomInfoMap.InitHashTable(9949,true);
	//ºó°ªÀ» ¼Ò¼ö·Î Áà¾ßÇÔ //http://primes.utm.edu/lists/small/10000.txt
}

void ioRelayRoomInfoMgr::InsertRoomInfo( const DWORD dwUserIndex,const DWORD dwRoomIndex )
{
	ThreadSync ts(this);

	PROOMINFORESULT prData = m_roomInfoMap.Lookup(dwUserIndex);
	if(prData)
	{
		DWORD& dwTempRoomIndex = prData->m_value;
		dwTempRoomIndex = dwRoomIndex;

		LOG.PrintTimeAndLog( 0, "Test - RelayRoomInfo InsertInfo : ExistData: %d, %d", (int)dwRoomIndex, (int)dwUserIndex );
	}
	else
	{
		m_roomInfoMap.SetAt(dwUserIndex,dwRoomIndex);

		LOG.PrintTimeAndLog( 0, "Test - RelayRoomInfo InsertInfo : NewData: %d, %d", (int)dwRoomIndex, (int)dwUserIndex );
	}

	/*LOG.PrintTimeAndLog( 0, "Test - RelayRoomInfo  roomInfoMap info : %d %d", m_roomInfoMap.GetCount(), GetCurrentThreadId() );
	
	POSITION pos = m_roomInfoMap.GetStartPosition();
	while (pos)
	{
		DWORD dwUserIndx = m_roomInfoMap.GetKeyAt(pos);
		DWORD dwRoomIndex	= m_roomInfoMap.GetValueAt(pos);
		m_roomInfoMap.GetNext(pos);
		//DWORD dwRoomIndex = m_roomInfoMap.GetNextValue(pos);

		LOG.PrintTimeAndLog( 0, "Test - info : %d %d", dwUserIndx, dwRoomIndex );
	}*/
}

void ioRelayRoomInfoMgr::RemoveRoomInfo( const DWORD dwUserIndex )
{
	ThreadSync ts(this);

	if(dwUserIndex != 0)
	{
		if(m_roomInfoMap.RemoveKey(dwUserIndex) == false)
		{
			LOG.PrintTimeAndLog(0,"RemoveRoomInfo fail - %d", dwUserIndex );
		}
	}
}

DWORD ioRelayRoomInfoMgr::GetRoomIndexByUser( const DWORD dwUserIndex )
{
	ThreadSync ts(this);

	PROOMINFORESULT prResult = m_roomInfoMap.Lookup(dwUserIndex);
	if(prResult)
	{
		return prResult->m_value;
	}
	else 
	{
		LOG.PrintTimeAndLog(0,"GetRoomIndexByUser %d error ",dwUserIndex);
		return 0;
	}
}
