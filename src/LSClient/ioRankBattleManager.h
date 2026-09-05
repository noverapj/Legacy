#pragma once

struct UserBattleRecord
{
	ioHashString m_szName;
	int m_iRank;
	int m_iRankPoint;
	int m_iWin;
	int m_iLose;
	int m_iGradeLevel;
	bool m_bLogin;

	UserBattleRecord()
	{
		m_szName.Clear();
		m_iRank = 0;
	};
};
typedef std::vector< UserBattleRecord > UserBattleRecordVec;

struct BattleHistory
{
	BYTE m_byWinState;
	int m_iWinGame;
	int m_iLoseGame;

	void Init()
	{
		m_byWinState = 0;
		m_iWinGame = m_iLoseGame = 0;
	}
};
typedef std::vector< BattleHistory > BattleHistoryVec;

struct MyBattleRecord
{
	int m_iRankPoint;
	int m_iRank;
	DWORD m_dwDate;
};
typedef std::vector< MyBattleRecord > MyBattleRecordVec;

class ioRankBattleManager : public Singleton< ioRankBattleManager >
{
private:
	struct BattleTimeInfo
	{
		int m_iType;
		int m_iStartTime;
		int m_iEndTime;
	};
	typedef std::map< int, BattleTimeInfo > BattleTimeInfoMap;

	struct BattleScheduleInfo
	{
		int m_iStartDay;
		int m_iEndDay;
		int m_WeekDayTime[7];
	};
	typedef std::vector< BattleScheduleInfo > BattleScheduleInfoVec;
	
	struct RankInfo
	{
		int m_iMaxPoint;
		ioHashString m_szRankName;
	};
	typedef std::vector< RankInfo > RankInfoVec;

private:
	ioHashString m_szMenuTooltip;
	ioHashString m_szInfoTooltip;
	int m_iHelpTipIdx;

	BattleTimeInfoMap m_TimeMap;
	BattleScheduleInfoVec m_Schedules;
	MyBattleRecordVec m_MyRecords;
	UserBattleRecordVec m_UserRecords;
	RankInfoVec m_RankInfos;
	BattleHistoryVec m_History;
	
	SYSTEMTIME m_CheckServerTime;
	bool m_bActive;
	bool m_bSendRequest;
	bool m_bSendCancel;
	bool m_bRankBattlePlaying;

	int m_iRankPoint;
	int m_iPrevRankGrade;
	int m_iRankGrade;
	int m_iTotalWin;
	int m_iTatalLose;
	int m_iWinRate;
	int m_iRank;
	int m_iWinStreakCount;

public:
	void LoadINIData();

	ioHashString GetMenuTooltip()		{ return m_szMenuTooltip; }
	ioHashString GetInfoTooltip()		{ return m_szInfoTooltip; }
	ioHashString GetRankGradeString();
	int GetHelpTipIdx()					{ return m_iHelpTipIdx; }
	int GetRankGrade()					{ return m_iRankGrade; }
	int GetPrevRankGrade()				{ return m_iPrevRankGrade; }
	int GetRankPoint()					{ return m_iRankPoint; }
	int GetTotalWin()					{ return m_iTotalWin; }
	int GetTotalLose()					{ return m_iTatalLose; }
	int GetWinRate()					{ return m_iWinRate; }
	int GetRank()						{ return m_iRank; }
	void SetRankGrade( int iPoint );
	void SetPrevRankGrade( int iGrade )	{ m_iPrevRankGrade = iGrade; }
	bool IsActive();
	bool IsSendRequest()				{ return m_bSendRequest; }
	bool IsRankBattlePlaying()			{ return m_bRankBattlePlaying; }

	bool ProcessTCPPacket( SP2Packet &rkPacket );
	void SetMyRankData( SP2Packet &rkPacket );
	void SetUserRankData( SP2Packet &rkPacket );
	void SetUserBattleData( SP2Packet &rkPacket );
	void SetRequest( SP2Packet &rkPacket );
	void SetCancel( SP2Packet &rkPacket );
	void OnGameStart( SP2Packet &rkPacket );
	void OnGameEnd( SP2Packet &rkPacket );

	void SendRankData();
	void SendTop100();
	void SendHistory( int iPage = 0 );
	void SendRequest();
	void SendCancel();
	MyBattleRecord GetRankRecord( int iIndex );
	UserBattleRecord GetUserBattleRecord( int iRank );
	int GetUserBattleRecordCount();

	bool GetBattleHistory( int iIndex, BattleHistory& rBattleHistory );
	void SetRankBattlePlaying( bool b )	{ m_bRankBattlePlaying = b; }

public:
	static ioRankBattleManager& GetSingleton();

public:   
	ioRankBattleManager();
	virtual ~ioRankBattleManager();
};
#define g_RankBattleMgr ioRankBattleManager::GetSingleton()

class UserRecordSort : public std::binary_function< const UserBattleRecord&, const UserBattleRecord&, bool >
{
public:
	bool operator()( const UserBattleRecord &lhs , const UserBattleRecord &rhs ) const
	{
		if( lhs.m_iRank > rhs.m_iRank )
			return true;
	
		return false;
	}
};