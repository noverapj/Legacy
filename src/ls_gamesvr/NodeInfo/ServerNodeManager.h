#ifndef _ServerNodeManager_h_
#define _ServerNodeManager_h_

#include "ServerNode.h"

using namespace std;

typedef vector< DWORD > vServerIndex;
typedef vector< ServerNode * > vServerNode;
typedef vServerNode::iterator vServerNode_iter;

class ServerNodeManager
{
protected:
	static ServerNodeManager	*sg_Instance;
	vServerNode					m_vServerNode;
	vServerIndex				m_vServerIndex;
	MemPooler< ServerNode >		m_MemNode;

	DWORD						m_dwServerIndex;

protected:
	int						  m_iMaxNodes;
	int						  m_iDisperseCount;
	int						  m_iPartitionIndex;
	DWORD                     m_dwServerRoomCreateClosePingTime;
	bool					  m_bBlockState;

	enum RoomTypes
	{
		ROOM_TYPE_NONE,
		ROOM_TYPE_PLAZA,
		ROOM_TYPE_BATTLEROOM,
		ROOM_TYPE_LADDERTEAM,
		ROOM_TYPE_HEADQUARTER,
		ROOM_TYPE_END
	};

public:
	static ServerNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();
	void LoadINI();
public:
	ServerNode *CreateServerNode( SOCKET s );
	void CreateClientNode( DWORD dwServerIndex, SOCKET s );
	void AddServerNode( ServerNode *pNewNode );
	void CalculateMaxNode();
	int CalculatePartition( const int iPartitionCount, const int iServerIndex );
	void CollectMyPartitions();
	bool WasMyPartition( const int iServerIndex );
	void RearrangePartition( const int iPartitionCount );

	void RemoveNode( ServerNode *pServerNode );
	void RemoveNode( const int iServerIndex );

	bool IsConnectWorkComplete();

public:
	int RemainderNode()	{ return m_MemNode.GetCount(); }
	int GetNodeSize()	{ return m_vServerNode.size(); }
	
public:
	bool ConnectTo( DWORD dwServerIndex, const char *ServerIP, int iSSPort );

public:
	//int  ExceptionRemoveUserNode( DWORD dwIndex );

public:
	void SetServerIndex( DWORD dwServerIndex );
	void SetBlockState(const bool bBlockState);
	void SetPartitionIndex(const int iPartitionIndex);

	DWORD GetServerIndex()						{ return m_dwServerIndex; }
	bool IsBlocked()							{ return m_bBlockState; }
	bool IsAfford();

public:
	int GetMaxServerNodes()						{ return m_iMaxNodes; }
	int GetDisperseCount()						{ return m_iDisperseCount; }
	int GetPartitionIndex()						{ return m_iPartitionIndex; }
	DWORD GetServerRoomCreateClosePingTime()	{ return m_dwServerRoomCreateClosePingTime; }

public:
	void GetServerNodes(bool bAll, vServerIndex& vServerIndexes);
	ServerNode *GetServerNode( DWORD dwServerIndex );
	ServerNode *GetUserIndexToServerNode( DWORD dwUserIndex );
	bool GetSelectPlazaServer( int iPlazaCount, DWORD& dwServerIndex );
	bool GetSelectBattleRoomServer( int iBattleRoomCount, DWORD& dwServerIndex );
	bool GetSelectLadderTeamServer( int iLadderTeamCount, bool bHeroMode, DWORD& dwServerIndex );
	bool GetSelectShuffleRoomServer( int iShuffleRoomCount, DWORD& dwServerIndex );

public:
	void ProcessPing();
	void ProcessFlush();

public:
	void SendMessageAllNode( SP2Packet &rkPacket, const DWORD dwServerIndex = 0 );
	void SendMessageToPartitions( SP2Packet &rkPacket );
	void SendMessageRelay( SP2Packet& rkPacket );
	bool SendMessageNode( int iServerIndex, SP2Packet &rkPacket );
	bool SendMessageArray( int iServerArray, SP2Packet &rkPacket );

public:
	void FillAllServerIndex( SP2Packet &rkPacket );

private:     	/* Singleton Class */
	ServerNodeManager();
	virtual ~ServerNodeManager();
};
#define g_ServerNodeManager ServerNodeManager::GetInstance()
#endif