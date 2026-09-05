// RoomNodeManager.h: interface for the RoomNodeManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Room.h"
#include "RoomCopyNode.h"
#include "BlockNode.h"

using namespace std;

#define PLAZA_ROOM_SORT_HALF_POINT             10000000

typedef vector<Room*> vRoom;
typedef vRoom::iterator vRoom_iter;

typedef struct tagSortRoom
{
	RoomParent *m_pRoomParent;
	int         m_iRoomPoint;
	tagSortRoom()
	{
		m_pRoomParent= NULL;
		m_iRoomPoint = PLAZA_ROOM_SORT_HALF_POINT;
	}
}SortRoom;
typedef vector< SortRoom > vSortRoom;
typedef vSortRoom::iterator vSortRoom_iter;
class RoomInfoSort
{
public:
	bool operator()( const SortRoom &lhs , const SortRoom &rhs ) const
	{
		if( lhs.m_iRoomPoint < rhs.m_iRoomPoint )
		{
			return true;
		}				
		return false;
	}
};

typedef struct tagSortPlazaRoom
{
	enum
	{
		PRS_ACTIVE				= 1,
		PRS_FULL_USER			= 2,
		PRS_NOT_MIN_LEVEL_MATCH = 3,
		PRS_NOT_MAX_LEVEL_MATCH = 4,
	};

	enum
	{
		PRS_SUB_NONE = 0,
		PRS_SUB_NPC_EVENT = 1,
	};

	RoomParent *m_pNode;
	int	        m_iPoint;
	int         m_iState;
	int			m_iSubState;
	tagSortPlazaRoom()
	{
		m_pNode  = NULL;
		m_iPoint = 0;
		m_iSubState = 0;
		m_iState = PRS_ACTIVE;
	}
}SortPlazaRoom;
typedef vector< SortPlazaRoom > vSortPlazaRoom;
typedef vSortPlazaRoom::iterator vSortPlazaRoom_iter;
class PlazaRoomSort
{
public:
	bool operator()( const SortPlazaRoom &lhs , const SortPlazaRoom &rhs ) const
	{
		if( lhs.m_iPoint < rhs.m_iPoint )
			return true;
		return false;
	}
};

class DamageTableSort
{
public:
	bool operator()( const DamageTable &lhs , const DamageTable &rhs ) const
	{
		if( lhs.iDamage > rhs.iDamage )
		{
			return true;
		}
		//else if( lhs.iDamage == rhs.iDamage )
		//{
		//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
		//	if( iCmpValue < 0 )
		//		return true;
		//}
		return false;
	}
};

class RoomNodeManager : public BlockNode
{
private:
	static RoomNodeManager *sg_Instance;

	// 원본 룸 메모리 풀(대전 + 광장)
	MemPooler< Room >	m_MemNode;

	// 원본 대전 룸
	vRoom      m_vRoomNode;

	// 원본 광장 룸
	vRoom      m_vPlazaNode;

	// 원본 유저 룸
	vRoom      m_vHeadquartersNode;

	// 복사 대전 룸
	vRoomCopyNode m_vRoomCopyNode;

	// 복사 광장 룸
	vRoomCopyNode m_vPlazaCopyNode;

	// 복사 유저 룸
	vRoomCopyNode m_vHeadquartersCopyNode;


	// 룸 프로세스(대전 + 광장)
	DWORD	   m_dwRoomProcessTime;

	// 광장 생성 정보
	typedef vector< ioHashString > vioHashString;
	vioHashString m_vPlazaNameList;

	// 광장 룸 넘버 시작
	int m_iPlazaRoomNumber;

	DWORD m_dwGuildRoomLifeTime;	

public:
	static RoomNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitPlazaInfo();
	void InitMemoryPool( const DWORD dwServerIndex );
	void ReleaseMemoryPool();

public:
	int RemainderNode(){ return m_MemNode.GetCount(); }
	bool IsAfford();

public:
	int GetRoomNodeSize(){ return m_vRoomNode.size(); }
	int GetPlazaNodeSize(){ return m_vPlazaNode.size(); }
	int GetHeadquartersNodeSize(){ return m_vHeadquartersNode.size(); }
	int GetCopyRoomNodeSize(){ return m_vRoomCopyNode.size(); }
	int GetCopyPlazaNodeSize(){ return m_vPlazaCopyNode.size(); }
	int GetCopyHeadquartersNodeSize(){ return m_vHeadquartersCopyNode.size(); }
	
public:  // 정보
	int GetSafetySurvivalRoomUserCount(); // 초보전용 서바이벌 유저수
	int GetPlazaUserCount();
	int GetHeadquartersUserCount();
	void SendCurLadderRoomList( User *pUser, int iPage, int iMaxCount ); //레더룸 정보 전달해주기
	void SendLadderRoomJoinInfo( UserParent *pUserParent, DWORD dwIndex, int iPrevBattleIndex );
	ioHashString GetPlazaRandomName();
	Room *GetRoomNode( int iRoomIndex );

public:  // 생성 및 삭제
	Room* CreateNewRoom();
	Room* CreateNewPlazaRoom( int iSubType = -1, int iModeMapNum = -1 );
	Room* CreateNewHeadquartersRoom( int iSubType = -1, int iModeMapNum = -1 );

	void RemoveRoom( Room *pRoom );
	void RemovePlazaRoom( Room *pRoom );
	void RemoveHeadquartersRoom( Room *pRoom );

	void AddCopyRoom( RoomCopyNode *pRoom );
	void RemoveCopyRoom( RoomCopyNode *pRoom );

public:  // 서버 동기화
	void ConnectServerNodeSync( ServerNode *pServerNode );

public: //대전 룸 매칭테이블
	void CreateMatchingTable();

public:
	

	// 동일 룸 입장
	Room* GetExitRoomJoinPlazaNode( int iMyLevel );								//접속 가능한 광장 룸( 없으면 생성하여 리턴 ) 룸 이탈시 

	// 정렬 포인트
	int GetSortPlazaRoomPoint( SortPlazaRoom &rkSortRoom, int iKillDeathLevel );

	// 접속 동일 서버 광장
	Room* GetJoinPlazaNode( int iMyLevel, Room *pMyRoom );                      //접속 가능한 광장 룸( 없으면 생성하여 리턴 ) 빠른 참여시
	Room* GetAutoCreatePlazaNode( int iMyLevel );
	Room* GetJoinPlazaNodeByNum( int iRoomIndex );
		
	// 접속 타 서버 광장
	RoomCopyNode* GetJoinPlazaCopyNode( int iMyLevel );
	RoomCopyNode* GetJoinPlazaCopyNodeByNum( int iRoomIndex );

	// 접속 모든 서버 광장
	RoomParent* GetJoinGlobalPlazaNode( int iMyLevel, RoomParent *pMyRoom );
	RoomParent* GetJoinGlobalPlazaNodeByNum( int iRoomIndex );
	RoomParent* GetExitRoomJoinGlobalPlazaNode( int iMyLevel, RoomParent *pMyRoom ); //접속 가능한 광장 룸( 없으면 생성하여 리턴 ) 룸 이탈시 

	Room*		GetGlobalLadderRoomNode( DWORD dwRoomIndex );

	//
	Room *GetHeadquartersNode( int iRoomIndex );
	RoomParent* GetHeadquartersGlobalNode( int iRoomIndex );          // 본부

	//
	RoomParent* GetGlobalPlazaNodeByNum( int iRoomIndex );
	RoomParent* GetPlazaNodeByNum( int iRoomIndex );

	void SendPlazaRoomList( User *pUser, int iCurPage, int iMaxCount );         //광장 룸 리스트 전송
	void SendPlazaRoomJoinInfo( UserParent *pUserParent, DWORD dwRoomIndex );
	
	DWORD GetGuildRoomLifeTime();

	Room * CreateNewPersonalHQRoom( int iSubType = -1, int iModeMapNum = -1 );

protected:
	void INILoadToGuildRoomLifeTime();

public:
	void NagleOptionChange( bool bNagleAlgorithm, bool bPlazaNagleAlgorithm );

public:
	void RoomProcess();
	//해외 레더룸 전용
	int GetSortLadderRoomPoint( SortRoom& rkSortRoom );
	void FillLadderRoomInfo( SP2Packet& rkPacket, Room& rkRoomNode );
	void EnterUserToLadderRoom( User* pUser, Room& rkRoomNode, bool bObserver );
private:     	/* Singleton Class */
	RoomNodeManager();
	virtual ~RoomNodeManager();
};

inline Room* ToRoomOriginal( Room *pRoom )
{
	if( !pRoom || !pRoom->IsRoomOriginal() )
		return NULL;

	return static_cast< Room* >( pRoom );
}

#define g_RoomNodeManager RoomNodeManager::GetInstance()

