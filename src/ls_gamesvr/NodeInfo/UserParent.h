#ifndef _UserParent_h_
#define _UserParent_h_

enum UserPos
{
	UP_HEADQUARTERS = 1,                //본부
	UP_LOBBY,                           //로비
	UP_TRAINING,                        //광장
	UP_BATTLE_ROOM,                     //전투방
	UP_LADDER_TEAM,                     //진영팀
	UP_TOURNAMENT,						//대회
	UP_SHUFFLE,							//오늘의모드
	UP_GUILD_HQ,						//길드 본부. 
	UP_HOUSE,							//개인 본부.
	UP_PRACTICE,						//수련장
};

class UserParent
{
	//가상함수의 향연
	public:
	virtual DWORD GetUserIndex() const = 0;
	virtual DWORD GetUserDBAgentID() const = 0;
	virtual const DWORD GetAgentThreadID() const = 0;
	virtual const ioHashString& GetPrivateID() const = 0;
	virtual const ioHashString& GetPublicID() const = 0;
	virtual int GetUserCampPos() const = 0;
	virtual int GetGradeLevel() = 0;
	virtual int GetUserPos() = 0;
	virtual int GetKillDeathLevel() = 0;
	virtual int GetLadderPoint() = 0;
	virtual bool IsSafetyLevel() = 0;
	virtual const bool IsUserOriginal() = 0;
	virtual bool RelayPacket( SP2Packet &rkPacket ) = 0;
	virtual ModeType GetModeType() = 0;
	virtual const bool IsDeveloper() = 0;
	virtual int GetUserRanking() = 0;
	virtual DWORD GetPingStep() = 0;
	virtual bool  IsGuild() = 0;
	virtual DWORD GetGuildIndex() = 0;
	virtual DWORD GetGuildMark() = 0;
	virtual bool IsBestFriend( DWORD dwUserIndex ) = 0;
	virtual void GetBestFriend( DWORDVec &rkUserIndexList ) = 0;
	virtual void SetLadderMatchTime( DWORD dwMatchTime ) = 0;

	// 2019-02-14 by bckim, 배틀 모드 추가
	virtual void SetBattleModeOrder( int iBattleModeOrder ) = 0;
	// 2019-02-14 by bckim, 배틀 모드 추가



	public:
	UserParent();
	virtual ~UserParent();
};
typedef std::vector< UserParent * > vUserParent;
typedef vUserParent::iterator vUserParent_iter;
#endif