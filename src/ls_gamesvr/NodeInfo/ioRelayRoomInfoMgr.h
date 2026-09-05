#pragma once


class ioRelayRoomInfoMgr : public SuperParent
{
public:
	ioRelayRoomInfoMgr(void);
	virtual ~ioRelayRoomInfoMgr(void);

protected:
	void InitData();

public:
	void InsertRoomInfo(const DWORD dwUserIndex,const DWORD dwRoomIndex);
	void RemoveRoomInfo(const DWORD dwUserIndex);
	DWORD GetRoomIndexByUser(const DWORD dwUserIndex);

protected:
	typedef ATL::CAtlMap<DWORD,DWORD> ROOMINFOMAP;//userindex,roomindex;
	typedef ROOMINFOMAP::CPair* PROOMINFORESULT;

	ROOMINFOMAP	m_roomInfoMap;
};

typedef cSingleton<ioRelayRoomInfoMgr> S_IORELAYROOMINFOMGR;
#define g_RelayRoomInfoMgr S_IORELAYROOMINFOMGR::GetInstance()