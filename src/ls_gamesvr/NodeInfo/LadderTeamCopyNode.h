#ifndef _LadderTeamCopyNode_h_
#define _LadderTeamCopyNode_h_

#include "LadderTeamParent.h"
#include "CopyNodeParent.h"

class LadderTeamCopyNode : public LadderTeamParent,
						  public CopyNodeParent					 
{
protected:
	// ±âº» Á¤º¸
	DWORD m_dwIndex;
	int   m_iCampType;
	ioHashString m_szTeamName;
	ioHashString m_szTeamPW;
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildMark;
	DWORD m_dwTeamState;
	DWORD m_dwJoinGuildIndex;

	int	m_iMaxPlayer;
    int m_iCurPlayer;
	int m_iAverageMatchLevel;
	int m_iHeroMatchPoint;

	int m_iSelectMode;
	int m_iSelectMap;

	bool m_bHeroMatchMode;
	
	// ½Â - ¹« - ÆÐ - ¿¬½Â - ÆÀ·©Å·
	int m_iWinRecord;
	int m_iLoseRecord;
	int m_iVictoriesRecord;
	int m_iTeamRanking;

	//Option
	bool m_bSearchLevelMatch;
	bool m_bSearchSameUser;
	bool m_bBadPingKick;

	int  m_iSearchCount;


protected:
	void InitData();

public:
	void ApplySyncChangeInfo( SP2Packet &rkPacket );
	void ApplySyncChangeRecord( SP2Packet &rkPacket );
	void ApplySyncCreate( SP2Packet &rkPacket );

public:
	virtual void OnCreate( ServerNode *pCreator );
	virtual void OnDestroy();

public:
	virtual const DWORD GetIndex(){ return m_dwIndex; }
	virtual void EnterUser( const DWORD dwUserIndex, const ioHashString &szPublicID, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint,  const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark,
							const ioHashString &szPublicIP, const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iClientPort, const int iTransferPort );
	virtual bool LeaveUser( const DWORD dwUserIndex, const ioHashString &szPublicID );
	virtual void SendPacketTcp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void SendPacketTcpUser( SP2Packet &rkPacket, const ioHashString &rkSenderName );
	virtual void SendPacketUdp( SP2Packet &rkPacket, const DWORD dwUserIndex = 0 );
	virtual void UserInfoUpdate( const DWORD dwUserIndex, const int iGradeLevel, const int iAbilityLevel, const int iHeroMatchPoint, const int iLadderPoint, const DWORD dwGuildIndex, const DWORD dwGuildMark, const int iClientPort, const ioHashString &szTransferIP, const int iTransferPort );
	virtual void UserUDPChange( const DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szPublicIP, const int iClientPort,
								const ioHashString &szPrivateIP, const ioHashString &szTransferIP, const int iTransferPort );

	virtual const bool IsOriginal(){ return false; }

	virtual int  GetCampType() const { return m_iCampType; }
	virtual int	 GetJoinUserCnt() const { return m_iCurPlayer; }
	virtual int  GetMaxPlayer() const { return m_iMaxPlayer; }
	virtual ioHashString GetTeamName() const { return m_szTeamName; }
	virtual ioHashString GetPW() const { return m_szTeamPW; }
	virtual DWORD GetGuildIndex() const;
	virtual DWORD GetGuildMark() const;
	virtual DWORD GetTeamState() const { return m_dwTeamState; }
	virtual bool  IsGuildTeam() const;
	virtual int   GetTeamRanking() const { return m_iTeamRanking; }
	virtual DWORD GetJoinGuildIndex() const { return m_dwJoinGuildIndex; }
	virtual bool IsFull() const;
	virtual bool IsEmptyUser();
	virtual bool IsSearchLevelMatch();
	virtual bool IsSearchSameUser();
	virtual bool IsBadPingKick();
	virtual bool IsSearching();
	virtual bool IsMatchPlay();
	virtual bool IsHeroMatchMode();

	virtual int  GetAbilityMatchLevel();
	virtual int  GetHeroMatchPoint();
	virtual int  GetTeamLevel();
	virtual int  GetWinRecord(){ return m_iWinRecord; }
	virtual int  GetLoseRecord(){ return m_iLoseRecord; }
	virtual int  GetVictoriesRecord(){ return m_iVictoriesRecord; }
	virtual int  GetSelectMode() const;
	virtual int  GetSelectMap() const;
	virtual int  GetRankingPoint();
	virtual void UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam );
	virtual void UpdateRanking( int iTeamRanking );	
	virtual void SetTeamState( DWORD dwState );

	// ¸ÅÄª & ·ë »ý¼º
	virtual void MatchRoomRequest( DWORD dwRequestIndex );
	virtual void MatchRoomRequestJoin( DWORD dwOtherTeamIndex, ModeType eModeType, int iModeSubNum, int iModeMapNum, int iCampType, int iTeamType );
	virtual void MatchReserveCancel();
	virtual void MatchPlayEndSync();
	
public:        //ÆÐÅ¶ Ã³¸®
	virtual bool OnProcessPacket( SP2Packet &rkPacket, UserParent *pUser );
	virtual void OnLadderTeamInfo( UserParent *pUser );

public:
	void SetIndex( DWORD dwIndex ){ m_dwIndex = dwIndex; }
	void SetJoinUserCnt( int iJoinUserCnt ){ m_iCurPlayer = iJoinUserCnt; }
	void SetMaxPlayer( int iMaxPlayer ){ m_iMaxPlayer = iMaxPlayer; }

	int GetSearchCount() { return m_iSearchCount; }

public:
	LadderTeamCopyNode();
	virtual ~LadderTeamCopyNode();
};

typedef std::vector< LadderTeamCopyNode * > vLadderTeamCopyNode;
typedef vLadderTeamCopyNode::iterator vLadderTeamCopyNode_iter;
#endif