
#ifndef _ioUserGuild_h_
#define _ioUserGuild_h_

class User;
class CQueryResultData;
class ioUserGuild
{
public:
	enum						//0 : 이벤트없음, 1 : 길드가입, 2 : 탈퇴유저, 3 : 부길마임명, 4 : 길마 위임, 5 : 부길마해임, 6 : 길드삭제
	{
		GUILD_EVENT_NONE	= 0,
		GUILD_JOIN			= 1,
		GUILD_LEAVE			= 2,
		GUILD_SECOND_MASTER = 3,
		GUILD_MASTER		= 4,
		GUILD_SECOND_MASTER_DISMISS = 5,
		GUILD_DELETE		= 6,
		GUILD_CREATE        = 7,
		GUILD_MARK_BLOCK    = 8,
	};

private:
	User *m_pUser;

protected:
	DWORD m_dwGuildIndex;
	ioHashString m_szGuildName;
	ioHashString m_szGuildPosition;
	DWORD m_dwGuildMark;

	//출석 보상, 랭크 보상 받은 날 저장
	DWORD m_dwRecvAttendRewardDate;
	DWORD m_dwRecvRankRewardDate;
	DWORD m_dwJoinDate;
	int	  m_iGuildLevel;
	int	  m_iYesterdayAttendance;
	int	  m_iYesterdayDate;
	BOOL  m_bTodayAttendance;
	BOOL  m_bActiveGuildRoom;

	struct GuildUserData
	{
		DWORD m_dwTableIndex;
		DWORD m_dwUserIndex;
		int   m_iUserLevel;
		int   m_iLadderPoint;
		ioHashString m_szUserID;
		ioHashString m_szGuildPosition;
		GuildUserData()
		{
			m_dwTableIndex = 0;
			m_dwUserIndex  = 0;
			m_iUserLevel   = 0;
			m_iLadderPoint = 0;
		}
	};
	typedef std::vector< GuildUserData > vGuildUserData;
	typedef vGuildUserData::iterator vGuildUserData_iter;
	vGuildUserData m_UserList;

public:
	void Initialize( User *pUser );
	void InitMyGuildData( bool bSync );
	void LeaveGuildInitInfo( bool bSync );

	// 내 길드 정보
public:
	void SetGuildData( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, bool bSync );
	void SetGuildDataEntryAgree( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, bool bSync );
	void SetGuildPosition( const ioHashString &szGuildPosition ){ m_szGuildPosition = szGuildPosition; }
	void SetGuildMark( DWORD dwGuildMark ){ m_dwGuildMark = dwGuildMark; }
	void SetGuildName( const ioHashString &szGuildName ){ m_szGuildName = szGuildName; }

	void SetRecvRewardDate( DBTIMESTAMP& AttendRewardRecvDate, DBTIMESTAMP& RankRewardRecvDate );

	void SetRecvAttendRewardDate(DWORD dwDate) { m_dwRecvAttendRewardDate = dwDate; }
	void SetRecvRankRewardDate(DWORD dwDate) { m_dwRecvRankRewardDate = dwDate; }

	void SetJoinDate( DBTIMESTAMP& JoinDate);
	void SetJoinDate( DWORD dwJoinDate ) { m_dwJoinDate = dwJoinDate; }
	void SetGuildLevel(int iLevel) { m_iGuildLevel = iLevel; }
	void SetTodayAttendanceInfo( BOOL bOk ) { m_bTodayAttendance = bOk; }

	BOOL IsPrevAttendanceInfo();

	BOOL DidTakeAction(DWORD dwActionDate);
	DWORD GetStandardDate(DWORD dwDate);
	BOOL IsRecvDate(DWORD dwPrevRecvDate);

public:
	inline DWORD GetGuildIndex(){ return m_dwGuildIndex; }
	inline ioHashString GetGuildName(){ return m_szGuildName; }
	inline ioHashString GetGuildPosition(){ return m_szGuildPosition; }
	inline DWORD GetGuildMark(){ return m_dwGuildMark; }
	inline DWORD GetRecvAttendRewardDate() { return m_dwRecvAttendRewardDate; }
	inline DWORD GetRecvRankRewardDate() { return m_dwRecvRankRewardDate; }
	inline DWORD GetGuildJoinDate() { return m_dwJoinDate; }
	inline int GetGuildLevel() { return m_iGuildLevel; }

	bool IsGuildMaster();
	bool IsGuildSecondMaster();
	bool IsBuilder();

public:
	void LeaveGuildUserSync();
	void KickOutGuildUserSync( const ioHashString &szUserID );
	void GuildNameChangeSync();
	void GuildMarkChangeSync( bool bBlock = false );
	void GuildEntryAgreeSync();

	// 내 길드원 정보
public:
	void SendRelayPacketTcp( SP2Packet &rkPacket, BOOL bMe = FALSE );	
	void SetGuildUserData( CQueryResultData *query_data );
	void CheckGuildLeaveUser( ioUserGuild::vGuildUserData &rkNewUserList );

	void DeleteGuildUser( const ioHashString &szUserID );
	void DeleteGuildUser( const DWORD &dwUserIndex );	

public:
	int  GetGuildMemberCount(){ return m_UserList.size(); }
	bool IsGuildUser( const ioHashString &szUserID );
	bool IsGuildUser( const DWORD &dwUserIndex, const ioHashString &szUserID );
	
public:
	//길드 출석
	int CanAttending( SP2Packet &rkPacket );
	void DoRecvGuildRankReward();
	int DoRecvGuildAttendReward();

	void SetGuildAttendanceData( CQueryResultData *query_data );

	void SQLUpdateAttendDate();
	void SQLUpdateAttendRewardDate();
	void SQLUpdateRankRewardDate();

	int GetYesterdayAttendance() { return m_iYesterdayAttendance; }
    
	void ActiveGuildRoom();
	void SetGuildRoomState(BOOL bActive) { m_bActiveGuildRoom = bActive; }
	BOOL IsActiveGuildRoom() { return m_bActiveGuildRoom; }
	
	void NotifyGuildRoomActive(const DWORD dwUserIndex);

	// 서버 이동
public:
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false );

public:
	ioUserGuild();
	virtual ~ioUserGuild();
};

#endif