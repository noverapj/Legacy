#pragma once

//////////////////////////////////////////받은 패킷의 정보를 기록하는 매니저////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////jch/////////////////////////////////////////////////////////
class ioRecvInfoList
{
public:
	struct RecvPacketInfo
	{
		ioHashString szRecvInfoName;
		DWORD dwRecvTime;
		DWORD dwStartTime;
		int nCnt;

		void Init( ioHashString szName, DWORD StartTime, DWORD RecvTime )
		{
			szRecvInfoName = szName;
			dwStartTime = StartTime;
			dwRecvTime = RecvTime;
			nCnt = 1;
		}
		void AddCnt(DWORD dwTime)
		{
			nCnt++;
			dwRecvTime = dwTime;
		}
		void Reset()
		{
			nCnt = 0;
			dwStartTime = 0;
			dwRecvTime = 0;
		}
		ioHashString GetName()
		{
			return szRecvInfoName;
		}
		int GetCnt()
		{
			return nCnt;
		}
		DWORD GetRecvTime()
		{
			return dwRecvTime;
		}
		void SetStartTime( DWORD StartTime )
		{
			dwStartTime = StartTime;
		}
		DWORD GetStartTime()
		{
			return dwStartTime;
		}
	};

	typedef std::vector<RecvPacketInfo*> InfoList;

public:
	void AddInfo( ioHashString szName, DWORD Time );
	RecvPacketInfo* FindInfo( ioHashString szName );
	void PrintInfo( DWORD dwColor, ioHashString szCharName, int eType, ioHashString szRecvName, bool bReset );
	void DeleteAll();

protected:
	void ChatMsg( DWORD dwColor, const char *szID, const char *szChat );

protected:
	InfoList m_RecvList;

public:
	ioRecvInfoList();
	virtual ~ioRecvInfoList();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioRecvPacketLogManager : public Singleton< ioRecvPacketLogManager >
{
public:	
	//받은 패킷 타입
	enum RecvPacketType 
	{
		RPT_Skill = 0,
		RPT_Buff,
		RPT_Dummy,
		RPT_Special,
		RPT_MAX
	};

	//캐릭터별 받은 타입별 리스트
	typedef std::map< RecvPacketType, ioRecvInfoList* > TypeList;
	//모든 캐릭터 리스트
	typedef std::map< ioHashString, TypeList* > TotalInfoList;

protected:
	TotalInfoList m_TotalInfoList;

public:
	//패킷 보낸 캐릭터 이름, 받은 패킷 타입, 받은 패킷 정보(스킬이름, 더미이름, 버프이름)
	void AddCharRecvInfo( ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName );
	//받은 패킷 정보 찾기
	ioRecvInfoList::RecvPacketInfo* FindCharRecvInfo( ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName );
	//받은 패킷 정보 채팅창에 출력 ( 글자색, 캐릭터 이름, 패킷 타입, 확인 할 특정 패킷 이름(스킬이름, 더미이름, 버프이름), 출력 후 카운트 초기화 여부 )
	void PrintCharRecvInfo( DWORD dwColor, ioHashString szCharName, RecvPacketType eType, ioHashString szRecvName = "", bool bReset = false );
	void PrintChatLog( DWORD dwColor, ioHashString szCharName, ioHashString szLog = "" );
	//모든 정보 삭제
	void CleanRecvInfo();

public:
	static ioRecvPacketLogManager& GetSingleton();

public:
	ioRecvPacketLogManager();
	virtual ~ioRecvPacketLogManager();
};

#define g_RecvPacketLogManager ioRecvPacketLogManager::GetSingleton()