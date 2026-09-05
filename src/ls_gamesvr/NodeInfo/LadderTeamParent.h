#ifndef _LadderTeamParent_h_
#define _LadderTeamParent_h_

class UserParent;
class SP2Packet;
class LadderTeamParent
{
	public:    
	enum
	{
		ENTER_USER = 100,
		LEAVE_USER,
		TEAM_STATE,
		MATCH_ROOM_REQUEST,
		MATCH_ROOM_REQUEST_JOIN,
		MATCH_ROOM_RESERVE_CANCEL,
		MATCH_ROOM_END_SYNC,
		TEAM_INFO,
		USER_INFO,
		UPDATE_RECORD,
		UPDATE_GUILDMARK,
		KICK_OUT,
		UDP_CHANGE,
		TRANSFER_PACKET,
		TRANSFER_PACKET_USER,
		USER_PACKET_TRANSFER,
		TRANSFER_PACKET_UDP,
	};    

	public:
	enum
	{
		TMS_READY          = 0,
		TMS_SEARCH_RESERVE = 1,
		TMS_SEARCH_PROCEED = 2,
		TMS_MATCH_RESERVE  = 3,
		TMS_MATCH_PLAY     = 4,
	};
	
	//가상함수의 향연
	public:
	virtual const DWORD GetIndex() = 0;
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort ) = 0;
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID ) = 0;
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 ) = 0;
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName ) = 0;
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 ) = 0;
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort ) = 0;
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort ) = 0;
	virtual const bool IsOriginal() = 0;
	
	virtual int  GetCampType() const = 0;
    virtual int	 GetJoinUserCnt() const = 0;
	virtual int  GetMaxPlayer() const = 0;
	virtual ioHashString GetTeamName() const = 0;
	virtual ioHashString GetPW() const = 0;
	virtual DWORD GetGuildIndex() const = 0;
	virtual DWORD GetGuildMark() const = 0;
	virtual DWORD GetTeamState() const = 0;
	virtual int   GetTeamRanking() const = 0;
	virtual DWORD GetJoinGuildIndex() const = 0;
	virtual bool IsGuildTeam() const = 0;
	virtual bool IsFull() const = 0;
	virtual bool IsEmptyUser() = 0;
	virtual bool IsSearchLevelMatch() = 0;
	virtual bool IsSearchSameUser() = 0;
	virtual bool IsBadPingKick() = 0;
	virtual bool IsSearching() = 0;
	virtual bool IsMatchPlay() = 0;
	virtual bool IsHeroMatchMode() = 0;

	virtual int  GetAbilityMatchLevel() = 0;
	virtual int  GetHeroMatchPoint() = 0;
	virtual int  GetTeamLevel() = 0;
	virtual int  GetWinRecord() = 0;
	virtual int  GetLoseRecord() = 0;
	virtual int  GetVictoriesRecord() = 0;
	virtual int  GetSelectMode() const = 0;
	virtual int  GetSelectMap() const = 0;
	virtual int  GetRankingPoint() = 0;
	virtual void UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam ) = 0;	
	virtual void UpdateRanking( int iTeamRanking ) = 0;	
	virtual void SetTeamState( DWORD dwState ) = 0;

	// 매칭 & 룸 생성
	virtual void MatchRoomRequest( DWORD dwRequestIndex ) = 0;
	virtual void MatchRoomRequestJoin( DWORD dwOtherTeamIndex, ModeType eModeType, int iModeSubNum, int iModeMapNum, int iCampType, int iTeamType ) = 0;
	virtual void MatchReserveCancel() = 0;
	virtual void MatchPlayEndSync() = 0;
	
public:      //패킷 처리
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser ) = 0;	
	virtual void OnLadderTeamInfo( UserParent *pUser ) = 0;
	
public:
	LadderTeamParent();
	virtual ~LadderTeamParent();
};
typedef std::vector< LadderTeamParent * > vLadderTeamParent;
typedef vLadderTeamParent::iterator vLadderTeamParent_iter;
#endif