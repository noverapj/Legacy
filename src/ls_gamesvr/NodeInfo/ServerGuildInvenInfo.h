#pragma once

typedef boost::unordered_map<DWORD, int> GUILDINVENINFO;	// <아이템코드, 수량>

struct GuildInvenInfo
{
	__int64 i64UpdateVer;

	GUILDINVENINFO mInvenInfo;

	GuildInvenInfo()
	{
		i64UpdateVer	= 0;
		mInvenInfo.clear();
	}
};

class ServerGuildInvenInfo : public Singleton< ServerGuildInvenInfo >
{
public: 
	ServerGuildInvenInfo();
	virtual ~ServerGuildInvenInfo();

	void Init();
	void Destroy();

public:
	static ServerGuildInvenInfo& GetSingleton();

protected:
	void SQLCheckInvenData(const DWORD dwGuildIndex, const DWORD dwUserIndex, const int iRequestType);

public:
	int GetItemCount(const DWORD dwGuildIndex, const DWORD dwItemCode);

public:
	void RequestGuildInvenData(const DWORD dwGuildIndex, const DWORD dwUserIndex, const int iRequestType);
	void InsertInvenData(const DWORD dwGuildIndex, CQueryResultData* query_data);
	void UpdateInvenVer(const DWORD dwGuildIndex, const __int64 i64Ver);
	
	BOOL FillGuildInvenData(const DWORD dwGuildIndex, SP2Packet& kPacket);

	BOOL IsPrevData(const DWORD dwGuildIndex, const __int64 i64Ver);

protected:
	typedef boost::unordered_map<DWORD, GuildInvenInfo> SERVERGUILDINVENINFO;

protected:
	SERVERGUILDINVENINFO m_mServerGuildInvenInfo;
};

#define g_ServerGuildInvenMgr ServerGuildInvenInfo::GetSingleton()