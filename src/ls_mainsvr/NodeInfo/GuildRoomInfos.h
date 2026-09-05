# pragma once

#include <boost/unordered/unordered_map.hpp>

class GuildRoomInfos : public Singleton< GuildRoomInfos >
{
public:
	GuildRoomInfos();
	virtual ~GuildRoomInfos();

	void Init();
	void Destroy();

public:
	BOOL IsExistGuildRoom(DWORD dwGuildIndex);
	BOOL IsPrepare(DWORD dwGuildIndex);

	BOOL AddGuildRoomInfo(DWORD dwGuildIndex);	//방 번호가 만들어기 전 단계, 룸index는 0
	void SetGuildRoomIndex(DWORD dwGuildIndex, DWORD dwRoomIndex);

	void DeleteGuildRoomInfo(DWORD dwGuildIndex, DWORD dwRoomIndex);

	DWORD GetGuildRoomIndex(DWORD dwGuildIndex);
	int	GetRoomResultByGuildIndex(DWORD dwGuildIndex);

	BOOL UpdateGuildRoomIndex(DWORD dwGuildIndex, DWORD dwRoomIndex);

	BOOL IsActive() { return m_bActive; }
	void SetActive(BOOL bVal) { m_bActive = bVal; }

	void AddRequestCount();
	void DecreaseRequestCount();

public:
	static GuildRoomInfos& GetSingleton();

protected:
	typedef boost::unordered_map<DWORD,DWORD> GUILDROOMINFOS;

protected:
	GUILDROOMINFOS m_mGuildRoomInfos;
	BOOL	m_bActive;
	int		m_iReqCount;
};
#define g_GuildRoomMgr GuildRoomInfos::GetSingleton()