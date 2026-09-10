#pragma once

#include "EncryptValue.h"
#include "ioGashaponMgr.h"

class SP2Packet;
class ioGuildInfo;
class ioGuildData
{
	friend class ioGuildInfo;
protected:
	DWORD			m_dwGuildIndex;
	DWORD			m_dwGuildMark;
	DWORD			m_dwGuildRank;
	DWORD			m_dwGuildLevel;
	DWORD			m_dwGuildJoinUser;
	DWORD			m_dwGuildMaxUser;
	DWORD			m_dwGuildPoint;
	DWORD			m_dwCurGuildPoint;
	DWORD			m_dwGuildRegDate;
	float			m_fGuildBonus;

	ioHashString	m_szGuildName;
	ioHashString	m_szGuildTitle;

	int				m_iGuildYesterdayAttendCount;	//어제 출석한 길드원
	int				m_iGuildTodayAttendCount;		//오늘 출석한 길드원

	BOOL			m_bGuildRoomOpen;

	struct GuildUser
	{
		DWORD			m_dwTableIndex;
		DWORD			m_dwUserIndex;

		int				m_iUserLevel;
		int				m_iUserPos;
		int				m_iSortPoint;

		ioHashString	m_szUserID;
		ioHashString	m_szUserPos;
		
		bool			m_bOnline;				//자신의 길드원은 친구와 동일하게 동기화 시킨다.		
		bool			m_bAttendedYesterday;	//전날 출석정보
		bool			m_bAttendedToday;		//오늘 출석정보

		GuildUser()
		{
			m_dwTableIndex			= 0;
			m_dwUserIndex			= 0;
			m_iUserLevel			= 0;
			m_iSortPoint			= 0;
			m_iUserPos				= 1;

			m_bOnline				= false;

			m_bAttendedYesterday	= false;
			m_bAttendedToday		= false;			
		}
	};
	class GuildUserSort : public std::binary_function< const GuildUser&, const GuildUser&, bool >
	{
	public:
		bool operator()( const GuildUser &lhs , const GuildUser &rhs ) const
		{
			if( lhs.m_iSortPoint > rhs.m_iSortPoint )
			{
				return true;
			}
			else if( lhs.m_iSortPoint == rhs.m_iSortPoint )
			{
				if( lhs.m_dwTableIndex < rhs.m_dwTableIndex )
				{
						return true;
				}
			}
			return false;
		}
	};
	typedef std::vector<GuildUser> vGuildUser;
	vGuildUser m_vUserList;

	DWORD      m_dwSyncTime;
	DWORD      m_dwSyncTitleTime;
	DWORD      m_dwSyncUserListTime;
	bool       m_bUserListUpdate;
	bool       m_bSync;          // 한번이라도 동기화된 길드 정보인가?

public:
	inline DWORD GetGuildIndex(){ return m_dwGuildIndex; }
	inline DWORD GetGuildMark(){ return m_dwGuildMark; }
	inline DWORD GetGuildRank(){ return m_dwGuildRank; }
	inline DWORD GetGuildLevel(){ return m_dwGuildLevel; }
	inline DWORD GetGuildJoinUser(){ return m_dwGuildJoinUser; }
	inline DWORD GetGuildMaxUser(){ return m_dwGuildMaxUser; }
	inline DWORD GetGuildPoint(){ return m_dwGuildPoint; }
	inline DWORD GetGuildCurPoint(){ return m_dwCurGuildPoint; }
	inline DWORD GetGuildRegYear(){ return m_dwGuildRegDate / 10000; }
	inline DWORD GetGuildRegMonth(){ return (m_dwGuildRegDate % 10000) / 100; }
	inline DWORD GetGuildRegDay(){ return m_dwGuildRegDate % 100; }
	inline float GetGuildBonus(){ return m_fGuildBonus; }
	inline ioHashString GetGuildName(){ return m_szGuildName; }
	inline ioHashString GetGuildTitle(){ return m_szGuildTitle; }
	inline int GetYesterdayAttendedCount() { return m_iGuildYesterdayAttendCount; }
	inline int GetTodayAttendedCount() { return m_iGuildTodayAttendCount; }
	inline BOOL GetGuildRoomOpen(){ return m_bGuildRoomOpen; }

	DWORD GetGuildMaster();
	const ioHashString &GetGuildMasterName();
	DWORD GetGuildSecondMaster();
	const ioHashString &GetGuildSecondMasterName();

	DWORD GetUserIndex( const ioHashString &szUserName );
	DWORD GetUserTableIndex( const ioHashString &szUserName );
	const ioHashString &GetUserName( DWORD dwUserIndex );

	int GetOnlineUserSize();
	void GetUserInfo( int iUserArray, DWORD &dwTableIndex, DWORD &dwUserIndex, int &iUserLevel, ioHashString &szUserID, ioHashString &szUserPos );
	void GetUserInfo( int iUserArray, ioHashString &szUserID, int &iUserLevel, int &iUserPos, bool &bOnline );
	void GetUserInfo( int iUserArray, ioHashString &szUserID, bool &bOnline );
	void GetUserInfo( int iUserArray, ioHashString &szUserID, int &iUserLevel, bool &bAttendYesterday, bool &bAttendToday );
	void GetUserPos( DWORD dwUserIndex, ioHashString& szUserPos );
	void FillUserList( SP2Packet &rkPacket );

	void ChangeGuildMaster( DWORD dwGuildMasterIndex );
	void ChangeGuildPosition( DWORD dwTargetIndex, const ioHashString &szChangePosition );
	void ChangeUserID( const ioHashString &rszUserID, const ioHashString &rszNewUserID );

public:
	 void SetGuildIndex( DWORD dwGuildIndex );
	 void SetGuildCampPos( DWORD dwGuildCampPos );
	 void SetGuildMark( DWORD dwGuildMark );
	 void SetGuildRank( DWORD dwGuildRank );
	 void SetGuildLevel( DWORD dwGuildLevel );
	 void SetGuildJoinUser( DWORD dwGuildJoinUser );
	 void SetGuildMaxUser( DWORD dwGuildMaxUser );
	 void SetGuildPoint( DWORD dwGuildPoint );
	 void SetGuildCurPoint( DWORD dwCurGuildPoint );
	 void SetGuildRegDate( DWORD dwGuildRegDate );	
	 void SetGuildBonus( float fGuildBonus );
	 void SetGuildName( const ioHashString &szGuildName );
	 void SetGuildTitle( const ioHashString &szGuildTitle );
	 void SetGuildUserLogIn( const ioHashString &szUserID );
	 void SetGuildUserLogOut( const ioHashString &szUserID );
	 void SetGuildUserPos( const ioHashString &szUserID, int iUserPos, int iUserLevel );     // 길드 직책이 아니라 훈련/광장/전투의 위치다.

	 void SetGuildInfo( SP2Packet &rkPacket );
	 void SetGuildUserList( SP2Packet &rkPacket );
	 void SetGuildUserListEx( SP2Packet &rkPacket );
	 void SetGuildRoomOpen( BOOL bGuildRoomOpen );

	 void LeaveGuildUser( const ioHashString &szUserName );


	 //길드 출석 관련
public:
	 void SetMyGuildMemberTodayAttendInfoByPacket( SP2Packet &rkPacket );
	 void SetMyGuildMemberYesterdayAttendInfoByPacket( SP2Packet &rkPacket );
	 void SetGuildTodayAttendedUser( DWORD dwUserIndex );
	 void SetGuildYesterdayAttendedUser( DWORD dwUserIndex );
protected:
	 void InitGuildTodayAttendedMember();
	 void InitGuildYesterdayAttendedMember();
	 

public:
	void InitSyncTimer(){ m_dwSyncTime = 0; }
	void ProcessSync( bool bMyGuildOnlyInfo = false );
	void ProcessTitleSync();
	void ProcessUserListSync();
	bool IsUserListUpdate();
	bool IsSync(){ return m_bSync; }
	void Sync(){ m_bSync = true; }

protected:
	ioGuildData();
	virtual ~ioGuildData();
};

struct GuildRewardItemInfo
{
	int					m_iPresentType;	//PresentType
	int					m_iValue1;		//PresentValue1
	int					m_iValue2;		//PresentValue2
	
	bool IsEmpty()
	{
		if ( m_iPresentType == 0 && m_iValue1 == 0 && m_iValue2 == 0 )
			return true;
		return false;
	}
	GuildRewardItemInfo()
	{
		m_iPresentType = m_iValue1 = m_iValue2 = 0;
	}
};

struct GuildAttendRwardInfo
{
public:
	ioHashString			 m_szName;
	int						 m_iMaxAttendedCount;
	GuildRewardItemInfo		 m_RewardItem;			  //획득 아이템(가챠)
	vGashaponPresent		 m_RewardPossibleItemVec; //획득 가능 목록(가챠 안에 내용물)
};
typedef std::vector< GuildAttendRwardInfo* > GuildAttendRewardVector;

struct GuildRankRwardInfo
{
public:
	ioHashString			 m_szName;
	int						 m_iRank;
	GuildRewardItemInfo		 m_RewardItem;			  //획득 아이템(가챠)
	vGashaponPresent		 m_RewardPossibleItemVec; //획득 가능 목록(가챠 안에 내용물)
};
typedef std::vector< GuildRankRwardInfo* > GuildRankRewardVector;

class ioGuildInfo : public Singleton< ioGuildInfo >
{
public:
	enum
	{
		GUILD_EVENT_NONE			= 0,	//이벤트없음
		GUILD_JOIN					= 1,	//길드 가입
		GUILD_LEAVE					= 2,	//탈퇴유저
		GUILD_SECOND_MASTER			= 3,	//부길마 임명
		GUILD_MASTER				= 4,	//길마 위임
		GUILD_SECOND_MASTER_DISMISS = 5,	//부길마 해임
		GUILD_DELETE				= 6,	//길드 삭제
		GUILD_CREATE				= 7,	//길드 생성
		GUILD_MARK_BLOCK			= 8,	//길드 마크 블럭
		GUILD_HOME_ADMIN_SET		= 9,	//길드 본부 편집권한 부여
		GUILD_HOME_ADMIN_RELEASE	= 10	//길드 본부 편집권한 해제
	};

protected:
	//길드 INI 정보
	int m_iGuildCreatePeso;
	int m_iGuildMarkChangePeso;
	int m_iGuildCreateLimitLevel;
	CEncrypt<int> m_iGuildKickOutPeso;
	CEncrypt<int> m_iGuildLeavePeso;
	DWORDVec m_dwLevelToEntryCount;
	
	//가입 대기중 길드 리스트
	DWORDVec m_dwEntryDelayGuildList;       

	//내 길드 정보
	DWORD m_dwGuildIndex;
	ioHashString m_szGuildName;
	ioHashString m_szGuildPosition;

	//전체 길드 데이터
	typedef std::vector< ioGuildData * > vGuildData;
	typedef vGuildData::iterator vGuildData_iter;
	vGuildData m_vGuildList;

	//길드 권유한 유저 리스트
	struct InvitationUser
	{
		ioHashString m_szUserName;
		DWORD m_dwInvitationTimer;
		InvitationUser()
		{
			m_dwInvitationTimer = 0;
		}
	};
	typedef std::vector< InvitationUser > vInvitationUser;
	vInvitationUser m_vInvitationUser;
	
	//길드 마크 변경중. . . .
	bool m_bGuildMarkChanging;

	// 길드 출석, 랭크 보상 INI 정보
	int	m_iAttendRenewalHour;	//길드 출석 갱신 및 보상 갱신 시간.
	GuildAttendRewardVector	m_AttendRewardVec;
	GuildRankRewardVector	m_RankRewardVec;

	//길드 출석 관련
	CTime		 m_GuildCreatedTime;		//길드 생성 시간.
	CTime		 m_EnableAttendTime;		//출석 가능한 시간.
	CTime		 m_EnableRewardTime;		//보상 받기 가능한 시간.
	CTime		 m_LoginTime;				//로그인시간
	CTime		 m_MemberAttendInfoRequestTime;

public:
	void Initialize();
	//길드 출석관련
	void InitializeGuildReward();
public:
	inline int GetGuildCreatePeso(){ return m_iGuildCreatePeso; }
	inline int GetGuildMarkChangePeso(){ return m_iGuildMarkChangePeso; }
	inline int GetGuildCreateLimitLevel(){ return m_iGuildCreateLimitLevel; }
	inline int GetGuildKickOutPeso(){ return m_iGuildKickOutPeso; }
	inline int GetGuildLeavePeso(){ return m_iGuildLeavePeso; }
	DWORD GetGuildLevelToMaxEntry( DWORD dwGuildLevel );

public:     //내 길드 정보
	void InitMyGuildData();
	void SetMyGuildData( DWORD dwIndex, const ioHashString &szGuildName, const ioHashString &szGuildPosition, DWORD dwGuildMark, BOOL bGuildRoomOpen );

	inline DWORD GetGuildIndex(){ return m_dwGuildIndex; }
	inline ioHashString GetGuildPosition(){ return m_szGuildPosition; }
	bool IsGuildMaster();
	bool IsGuildSecondMaster();
	DWORD GetMyGuildLevel();
	void SetMyGuildPosition( const ioHashString &szGuildPosition ){ m_szGuildPosition = szGuildPosition; }
	void SetMyGuildUserList( SP2Packet &rkPacket );

	void SetMyGuildUserLogIn( const ioHashString &szUserID );
	void SetMyGuildUserLogOut( const ioHashString &szUserID );
	void SetMyGuildUserPos( const ioHashString &szUserID, int iUserPos, int iUserLevel );     // 길드 직책이 아니라 훈련/광장/전투의 위치다.

	void SetGuildMarkChanging( bool bChange ){ m_bGuildMarkChanging = bChange; }
	
	void ChangeMyGuildUserID( const ioHashString &szUserID, const ioHashString &szNewUserID );

public:
	bool IsGuildEntryDelay( DWORD dwGuildIndex );
	bool IsGuildEntryDelayListDelete();
	bool IsMyGuild( DWORD dwGuildIndex );
	bool IsMyGuildUser( const ioHashString &szUserID );
	bool IsGuild();
	bool IsGuildMarkChanging(){ return m_bGuildMarkChanging; }

public:
	void ApplyEntryDelayGuildList( SP2Packet &rkPacket );
	void AddEntryDelayGuildList( DWORD dwGuildIndex );
	void DeleteEntryDelayGuildList();
	void DeleteEntryDelayGuildList( DWORD dwGuildIndex );

	// 모든 길드 정보를 관리한다.
public:
	ioGuildData *GetGuildData( DWORD dwGuildIndex, bool bNetwork = false );
	ioGuildData *GetGuildDataExist( const ioHashString &rszGuildName );
	void SetGuildUserList( SP2Packet &rkPacket );
	void SetGuildListInfo( DWORD dwGuildIndex, DWORD dwGuildRank, DWORD dwGuildLevel, DWORD dwGuildMark, ioHashString szGuildName,
						   DWORD dwGuildJoinUser, DWORD dwGuildMaxUser, float fGuildBonus, int iGuildPoint );
	void SetGuildListInfo( SP2Packet &rkPacket );
	void SetGuildSimpleInfo( DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwGuildMark );
	void SetGuildMark( DWORD dwGuildIndex, DWORD dwGuildMark );
	void SetGuildMark( const ioHashString &szUserID, DWORD dwGuildIndex, DWORD dwGuildMark );
	void SetGuildTitle( DWORD dwGuildIndex, const ioHashString &szGuildTitle );
	void SetGuildCampInfo( DWORD dwGuildIndex, DWORD dwGuildMark, DWORD dwGuildLevel, DWORD dwGuildRank, ioHashString szGuildName, DWORD dwCurGuildPoint );

public:
	void SetInvitationUser( const ioHashString &szInvitationUser );
	bool IsInvitationUser( const ioHashString &szInvitationUser );

	//길드 출석 관련
protected:
	void LoadRankReward( ioINILoader& rkLoader );
	void LoadAttendReward( ioINILoader& rkLoader );
	void SetAttendRewardTime( CTime RecievedTime );
	void SetAttendTime( CTime AttendedTime );
public:
	int GetAttendRewardSize();
	int GetRankRewardSize();
	const GuildAttendRwardInfo* const GetAttendRewardInfo( int iIndex ) const ;
	const GuildRankRwardInfo* const GetRankRewardInfo( int iIndex ) const ;


	INT64 GetEnableAttendTime() { return m_EnableAttendTime.GetTime(); }
	INT64 GetEnableAttendRewardTime() { return m_EnableRewardTime.GetTime(); }
	INT64 GetAttendInfoRequestTime() { return m_MemberAttendInfoRequestTime.GetTime(); }

	void SetMyGuildTimeSetting( DWORD dwRecievedTime, DWORD dwCreatedTime, DWORD dwLoginTime );
	void SetMyAttendRewardTimeByPacket( SP2Packet &rkPacket );
	void SetMyGuildMemberTodayAttendInfoByPacket( SP2Packet &rkPacket );
	void SetMyGuildMemberYesterdayAttendInfoByPacket( SP2Packet &rkPacket );
	void SetMyGuildAttendedUserInfoByPacket( SP2Packet &rkPacket, bool& bIsMe );
	bool CheckGuildCreatedTime();
	bool CheckEnableAttendRewardTime();
	bool CheckEnableAttendTime();
	bool CheckMemberAttendInfoRequestTime();
	//

public:
	static ioGuildInfo& GetSingleton();

public:
	ioGuildInfo();
	virtual ~ioGuildInfo();
};

#define g_GuildInfo ioGuildInfo::GetSingleton()
