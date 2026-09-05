#pragma once

#include "../ioComplexStringPrinter.h"
class SP2Packet;

typedef std::vector< ioHashString > vStringNode;
typedef vStringNode::iterator vStringNode_iter;

typedef struct tagChatInfo
{
	ioComplexStringPrinter m_Printer;
	ioComplexStringPrinter m_OverPrinter;
	ioHashString           m_szID;
	DWORD                  m_Index;
	bool                   m_bFirst;

	tagChatInfo()
	{
		m_Printer.ClearList();
		m_OverPrinter.ClearList();
		m_szID.Clear();
		m_Index  = 0;
		m_bFirst = false;
	}

}ChatInfo;

typedef std::vector< ChatInfo * > vChatInfo;
typedef vChatInfo::iterator vChatInfo_iter;

class ChannelNode
{
protected:
	int          m_iChannelIdx;
	int          m_iUserSize;
	vStringNode  m_vUserID;
	ioHashString m_szManToManID;
	vChatInfo    m_vChatInfo;
	bool         m_bUpdate;
	DWORD        m_dwSentenceIndexer;

public:
	void SetIndex( int iIndex ){ m_iChannelIdx = iIndex; }
	int  GetIndex(){ return m_iChannelIdx; }

	void GetUserName( char *szName, int iMaxSize );
	int  GetUserSize();
	void SetUserSize( int iUserSize ){ m_iUserSize = iUserSize; }
	
	const ioHashString &GetManToManID(){ return m_szManToManID; }
	void SetManToManID( ioHashString szManToManID ){ m_szManToManID = szManToManID; }

	int  GetChatInfoSize(){ return m_vChatInfo.size(); }
	bool IsChannelUser( ioHashString szName );

	void AddSentenceIndex();
	
public:
	ChatInfo *GetChatInfo( int iArray );	
	void SetChatInfo( ioComplexStringPrinter &szChat , const char *szID , bool bFirst );
	bool IsMannerTrialChatHaveID( const ioHashString &rszID );
	void GetMannerTrialChat( OUT ioHashString &rkChatList );

public:
	void JoinUser( ioHashString szID );
	void LeaveUser( ioHashString szID );

public:
	bool IsUpdate(){ return m_bUpdate; }
	void ClearUpdate(){ m_bUpdate = false; }

public:
	ChannelNode();
	virtual ~ChannelNode();
};

typedef std::vector< ChannelNode* > vChannelNode;
typedef vChannelNode::iterator vChannelNode_iter;

#define MAX_CHANNEL           2
class ioChannelManager : public Singleton< ioChannelManager >
{
	public:
	enum
	{
		CHAT_ID_COLOR       = 0xFFFF8200,
		CHAT_NORMAL_COLOR   = 0xFFFFB400,
		CHAT_ANNOUNCE_COLOR = 0xFFE5E5E5,
	};

	protected:
	vChannelNode         m_vChannelNode;
		
	public:
	void RemoveNode( ChannelNode *node );
		
	public:
	int GetNodeSize(){ return m_vChannelNode.size(); }
	int GetIndexToArray( int iIndex );
	ChannelNode *GetNodeArray( int iArray );
	ChannelNode *GetNodeIndex( int iIndex );
	ChannelNode *GetNodeUser( ioHashString szName, int iUserSize );

	public:
	void ShowChatPopup( int iIndex, ioHashString szID, ioHashString szChat );
	void ShowInvitePopup( int iIndex, ioHashString szID, ioHashString szInvitedID );

	public:
	bool IsUpdate( int iIndex );
	void ClearUpdate( int iIndex );
	void LeaveExceptionChannel( ioHashString szManToManID, int iChangeIndex );

	public:
	void AddAnnounce( int iIndex, ioHashString szAnnounce );
	void AddChat( int iIndex, const char *szID, ioHashString szChat );

	public:   // ÆÐÅ¶
	void OnInviteUser( int iIndex, ioHashString szInviteUser, ioHashString szJoinUser );
	void OnJoinUser( int iIndex, int iUserSize, ioHashString szJoinUser, ioHashString szManToManID );
	void OnChannelAllUser( SP2Packet &rkPacket );
	void OnLeaveUser( int iIndex, int iUserSize, ioHashString szLeaveUser, ioHashString szManToManID );
	void OnChat( int iIndex, ioHashString szID, ioHashString szChat );

	protected:
	void LeaveByBlackList( int iIndex );

	public:
	static ioChannelManager &GetSingleton();
		
	public:
	ioChannelManager();
	virtual ~ioChannelManager();	
};
#define g_ChannelManager ioChannelManager::GetSingleton()
