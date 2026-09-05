#ifndef _TournamentManager_h_
#define _TournamentManager_h_

#include "TournamentNode.h"

class ServerNode;
typedef std::vector< TournamentNode * > TournamentVec;
class TournamentSort
{
public:
	bool operator()( const TournamentNode *lhs , const TournamentNode *rhs ) const
	{
		if( lhs->GetType() < rhs->GetType() )
		{
			return true;
		}
		else if( lhs->GetType() == rhs->GetType() )
		{
			if( lhs->GetIndex() < rhs->GetIndex() ) 
				return true;
		}
		return false;
	}
};
class TournamentManager : public Singleton< TournamentManager >
{
protected:
	enum
	{
		MAX_FIRST_PAGE_LIST = 9,
		MAX_NEXT_PAGE_LIST  = 12,
	};

protected:
	DWORD m_dwProcessTime;

protected:
	TournamentVec m_TournamentList;

protected:
	bool IsAlreadyTournament( DWORD dwTourIndex );

protected:
	void DeleteProcess();
	void DeleteTournament( DWORD dwTourIndex );

public:
	void Initialize();

public:
	void Process();

public:
	TournamentNode *CreateTournamentNode( CQueryResultData *pQueryData );
	void CreateCompleteSort();

public:
	TournamentTeamNode *CreateTournamentTeamNode( CQueryResultData *pQueryData );
	void CreateTeamCompleteRound();   
	
public: //유저 리그 정보
	void ApplyTournamentInfoDB( CQueryResultData *pQueryData );
	void ApplyTournamentRoundDB( CQueryResultData *pQueryData );
	void ApplyTournamentConfirmUserListDB( CQueryResultData *pQueryData );

public: //정규 리그 정보
	void ApplyTournamentPrevChampInfoDB( CQueryResultData *pQueryData );
	
public: //정규 리그
	TournamentNode *GetRegularTournament();

public:
	TournamentNode *GetTournament( DWORD dwIndex );
	int GetTournamentSeq( DWORD dwIndex );

public:
	void CreateTournamentTeam( ServerNode *pSender, SP2Packet &rkPacket );
	void DeleteTournamentTeam( SP2Packet &rkPacket );

public:
	void SendTournamentListServerSync( ServerNode *pSender );
	void SendTournamentList( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentTeamInfo( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentScheduleInfo( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentRoundTeamData( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentRoundCreateBattleRoom( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentRoundTeamChange( SP2Packet &rkPacket );
	void SendTournamentTeamAllocateList( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentTeamAllocateData( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentBattleResult( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentJoinConfirmCheck( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentJoinConfirmReg( SP2Packet &rkPacket );
	void ApplyTournamentAnnounceChange( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentTotalTeamList( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentCustomStateStart( ServerNode *pSender, SP2Packet &rkPacket );
	void SendTournamentCustomRewardList( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentCustomRewardRegCheck( ServerNode *pSender, SP2Packet &rkPacket );
	void ApplyTournamentCustomRewardRegUpdate( SP2Packet &rkPacket );
	void ApplyTournamentCheerDecision( ServerNode *pSender, SP2Packet &rkPacket );
	
public:
	bool IsTournamentTeamLeaveState( DWORD dwTourIndex );

public:
	void SetTournamentTeamLadderPointAdd( DWORD dwTourIndex, DWORD dwTeamIndex, int iLadderPoint );

public:
	void ServerDownAllSave();

public:
	void InsertRegularTournamentReward();

public:
	static TournamentManager& GetSingleton();

public:
	TournamentManager();
	virtual ~TournamentManager();
};
#define g_TournamentManager TournamentManager::GetSingleton()
#endif