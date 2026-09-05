#ifndef _GuildNodeManager_h_
#define _GuildNodeManager_h_

#include "GuildNode.h"
using namespace std;

class ServerNode;
typedef vector< GuildNode * > vGuildNode;
typedef vGuildNode::iterator vGuildNode_iter;
class GuildSort
{
public:
	bool operator()( const GuildNode *lhs , const GuildNode *rhs ) const
	{
		if( lhs->GetCurGuildPoint() > rhs->GetCurGuildPoint() )
		{
			return true;
		}
		else if( lhs->GetCurGuildPoint() == rhs->GetCurGuildPoint() )
		{
			if( lhs->GetGuildLevel() > rhs->GetGuildLevel() ) 
				return true;
			else if( lhs->GetGuildIndex() < rhs->GetGuildIndex() )
			{
				return true;
			}
		}
		return false;
	}
};

class GuildNodeManager : public SuperParent
{
public:
	enum
	{
		// m_bFastUpdateGuildNode == true
		MAX_FAST_GUILD_UPDATE = 1000,			// 1000개씩
		FAST_GUILD_UPDATE_TIME= 5000,           // 5초마다

		// m_bFastUpdateGuildNode == false
		MAX_GUILD_UPDATE      = 100,			// 100개씩
	};

protected:
	static GuildNodeManager *sg_Instance;

protected:
	vGuildNode m_vGuildNode;
	vGuildNode m_vUpdateGuildNode;
	bool       m_bFastUpdateGuildNode;

protected:
	DWORD m_dwDefaultGuildPoint;
	DWORD m_dwDefaultGuildMaxEntry;
	DWORD m_dwGuildLevelUPEntryGap;
	struct GuildLevel
	{
		DWORD m_dwLevel;
		DWORD m_dwMaxEntry;
		float m_fLevelPer;
		float m_fLevelBonus;
		GuildLevel()
		{
			m_dwLevel    = 0;
			m_dwMaxEntry = 0;
			m_fLevelPer  = 0.0f;
			m_fLevelBonus= 0.0f;
		}
	};
	typedef std::vector< GuildLevel > vGuildLevel;
	bool m_bLevelCheckReverse;
	vGuildLevel m_GuildLevelList;
	GuildLevel  m_GuildLevel_Default;

protected:
	DWORD m_dwCurUpdateTime;
	DWORD m_dwUpdateTime;

public:
	static GuildNodeManager &GetInstance();
	static void ReleaseInstance();

protected:
	GuildNodeManager::GuildLevel &CheckGuildLevel( float fGuildRankPer );

public:
	void InitNodeINI();

public:
	GuildNode *CreateGuildNode( SP2Packet &rkPacket );
	GuildNode *CreateGuildNode( CQueryResultData *pQueryData );

public:
	int GetNodeSize(){ return m_vGuildNode.size(); }
	int GetUpdateNodeSize(){ return m_vUpdateGuildNode.size(); }
	int GetGuildCampSize( CampType eCampType );
	DWORD GetDefaultGuildMaxEntry(){ return m_dwDefaultGuildMaxEntry; }
	DWORD GetGuildLevelUPEntryGap(){ return m_dwGuildLevelUPEntryGap; }
	float GetGuildLevelBonus( DWORD dwLevel );

public:
	void RemoveGuild( GuildNode *pGuild );
	GuildNode *GetGuildNode( DWORD dwGuildIndex );
	GuildNode *GetGuildNodeExist( const ioHashString &rszGuildName );
	GuildNode *GetGuildNodeLowercaseExist( const ioHashString &rszGuildName );
	
public:
	void UpdateGuildNode( GuildNode *pGuildNode, bool bDirectSave = false );

public:
	void ChangeGuildPointSort( GuildNode *pGuildNode );
	void SortGuildRankAll();
	void ApplyChangeGuildRank();

public:
	void ServerDownAllUpdateGuild();

public:
	void ProcessUpdateGuild();
	void ProcessUpdateGuildLevel();
	void ProcessGuildNode();
	void StartFastUpdateGuild(){ m_bFastUpdateGuildNode = true; }

public:
	void SendCurGuildList( ServerNode *pServerNode, SP2Packet &rkPacket );
	void SendCurGuildInfo( ServerNode *pServerNode, SP2Packet &rkPacket );
	void SendCurGuildSimpleInfo( ServerNode *pServerNode, SP2Packet &rkPacket );
	void SendGuildMarkBlockInfo( ServerNode *pServerNode, SP2Packet &rkPacket );
	void SendGuildTitleSync( ServerNode *pServerNode, SP2Packet &rkPacket );	
	void SendGuildNameChange( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	DWORD ChangeGuildMaxUser( DWORD dwGuildIndex, DWORD dwGuildMaxUser );
	bool GuildEntryUserAgree( DWORD dwGuildIndex );
	void GuildLeaveUser( DWORD dwGuildIndex );
	void GuildTitleChange( DWORD dwGuildIndex, const ioHashString &szGuildTitle );
	void GuildJoinUser( DWORD dwGuildIndex, DWORD dwGuildJoinUser );
	void GuildMarkChange( DWORD dwGuildIndex, DWORD dwGuildMark );
	void GuildAddLadderPoint( DWORD dwGuildIndex, int iAddLadderPoint );	
	void GuildDecreaseUserCount(DWORD dwGuildIndex);

public:
	float GetGuildBonus( DWORD dwGuildIndex );
		
public:
	void OnLadderModeResult( DWORD dwGuildIndex, SP2Packet &rkPacket );

private:     	/* Singleton Class */
	GuildNodeManager();
	virtual ~GuildNodeManager();
};
#define g_GuildNodeManager GuildNodeManager::GetInstance()
#endif
