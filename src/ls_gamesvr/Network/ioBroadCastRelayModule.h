#pragma once

//#define _USE_IOCP_ 1
#include "../../iocpSocketDLL/SocketModules/BufferPool.h"
#include <atlcoll.h>
#define _USE_IOCP_ 0
#if(_USE_IOCP_)
typedef cIocpQueue CURRENTQUEUE;
#else
typedef MPSCQueue CURRENTQUEUE;
#endif

class MPSCQueue;
class ServerNode;
class Room;
#ifdef ANTIHACK
class ioRelayGroupInfoMgr;
#endif


class ioBroadCastRelayModule : public Thread //릴레이 서버 / 가디언 서버 혼용 사용 될 수도 있으니까.. 릴레이 모듈에 작업 
{
public:
	ioBroadCastRelayModule(void);
	virtual ~ioBroadCastRelayModule(void);

public:
	virtual BOOL Init(int seed, int roomSeed, int threadCount);
	virtual void Run();

protected:
	void SetQueues(int queueCount);

public:

	virtual BOOL PushRelayPacket( sockaddr_in &client_addr, CPacket &kPacket );
	virtual void RemoveRelayGroupReserve( Room *pRoom, DWORD dwUserIndex = 0);
	void RelayServerRemoveGroup( Room *pRoom, DWORD dwUserIndex );

public:

	BOOL LocalPushRelayPacekt( CPacket &kPacket, sockaddr_in & client_addr );
	void LocalRemoveRelayGroupReserve( DWORD dwRoomIndex, DWORD dwUserIndex );
public:
	virtual int GetNodeSize();
	virtual int GetBuffer64PoolCount()		{ return m_bufferPool.GetBuffer64PoolCount();}
	virtual int GetBuffer128PoolCount()		{ return m_bufferPool.GetBuffer128PoolCount();}
	virtual int GetBuffer256PoolCount()		{ return m_bufferPool.GetBuffer256PoolCount();}
	virtual int GetBuffer1024PoolCount()	{ return m_bufferPool.GetBuffer1024PoolCount();}
	virtual int GetBuffer2048PoolCount()	{ return m_bufferPool.GetBuffer2048PoolCount(); }

	virtual long Get64DropCount() 			{ return m_bufferPool.Get64DropCount(); }
	virtual long Get128DropCount()			{ return m_bufferPool.Get128DropCount(); }
	virtual long Get256DropCount() 			{ return m_bufferPool.Get256DropCount(); }
	virtual long Get1024DropCount() 		{ return m_bufferPool.Get1024DropCount(); }
	virtual long Get2048DropCount()			{ return m_bufferPool.Get2048DropCount(); }

	virtual long Get64RemainderCount()		{ return m_bufferPool.Get64Remainder();}
	virtual long Get128RemainderCount()		{ return m_bufferPool.Get128Remainder();}
	virtual long Get256RemainderCount()		{ return m_bufferPool.Get256Remainder();}
	virtual long Get1024RemainderCount()	{ return m_bufferPool.Get1024Remainder();}
	virtual long Get2048RemainderCount()	{ return m_bufferPool.Get2048Remainder(); }

	virtual long InputCount()				{ long rtval = m_inputCount; InterlockedExchange(&m_inputCount,0); return rtval;}
	virtual long ProcessCount()				{ long rtval = m_processCount; InterlockedExchange(&m_processCount,0); return rtval;}

	virtual long GetMax64PopCount() const		{ return m_bufferPool.GetMax64PopCount(); }
	virtual long GetMax128PopCount() const		{ return m_bufferPool.GetMax128PopCount(); }
	virtual long GetMax256PopCount() const		{ return m_bufferPool.GetMax256PopCount(); }
	virtual long GetMax1024PopCount() const		{ return m_bufferPool.GetMax1024PopCount(); }
	virtual long GetMax2048PopCount() const		{ return m_bufferPool.GetMax2048PopCount(); }

public:
	void AddRelayServerInfo(ServerNode* node);
	BOOL DelRelayServerInfo(ServerNode* node);

public:
	BOOL IsRelayServerOn(DWORD dwRoomIndex,int iRelayServerIndex);
	BOOL IsRelayServerOn();
	ServerNode* GetRelayServer();
	ServerNode* GetRelayServer(const int relayServerID, BOOL ForceState = FALSE);
	ServerNode* FindRelayServer(int relayServerIndex, BOOL ForceState = FALSE);

public:
	void MakeInfoPacket(SP2Packet& pk);

protected:	
	void PushPacket( UDPPacket* pData );

protected:

protected:
	void InsertRoomInfo(const DWORD dwUserIndex,const DWORD dwRoomIndex);
	void RemoveRoomInfo(const DWORD dwUserIndex);
	DWORD GetRoomIndexByUser(DWORD dwUserIndex);
protected:
	void IncementInputCount() { InterlockedIncrement(&m_inputCount);}
	void IncrementProcessCount() {InterlockedIncrement(&m_processCount); }

protected:
	BOOL IsRelayServerMode(ServerNode* node);
	BOOL IsRelayServerMode();

public:
	int GetRelayServerIndex()				{ return m_iRelayServerIndex++; }
	void SetRelayServerIndex(int iIndex)	{ m_iRelayServerIndex = iIndex; }
	BOOL IsUsingRelayServer() const			{ return m_bUseRelayServer; }
	void SetUseRelayServer(short iIndex)	{ m_bUseRelayServer = iIndex; }

	/************************************************************************/
	/* For queueus                                                                     */
	/************************************************************************/
protected:
	void InitQueues(int iQueueCount);	
	void* Dequeue(CURRENTQUEUE* queue);
	CURRENTQUEUE* GetMyqueue();



public:
	CURRENTQUEUE* GetqueueByUser(DWORD dwUserIndex);
	CURRENTQUEUE* Getqueue(int queueId);
public:
	void GetProcessCounts(std::vector<int>& queueCounts);


protected:
	int  m_iRelayServerIndex;
	long m_inputCount;
	long m_processCount;
	BOOL m_bUseRelayServer;
	long m_queueId;

protected:
	BufferPool m_bufferPool;
	ATL::CAtlList<ServerNode*> m_relayServerNodes;

protected:
	long m_iQueueIndex;
	typedef std::vector<CURRENTQUEUE*> QUEUES;
	QUEUES m_queues;

	//////////////////////////////////////////////////////////////////////////
#ifndef ANTIHACK
public:
	virtual void InsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID );
	void RelayServerInsertGroup( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID );
	void LocalInsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort );
	BOOL SendRelayPacket( DWORD dwUserIndex, SP2Packet& rkPacket );
	BOOL SendPacket( RelayHeader* pRelayHeader ,DWORD queueId = 0);
	void RemoveRoom( RelayHeader* pRelayHeader );
	void InsertRoom( RelayHeader* pRelayHeader );

	void EnqueueByUser(DWORD dwUserIndex,void* pData);

	//////////////////////////////////////////////////////////////////////////
#else
private:
	float m_fAntiErrorRate;
	int m_iAntiWaitTime;	
	int m_iPenguinCount;
	int m_iKickCount;
	int m_iTimeDecrease;

	int m_iSkillHackCount;
	int m_iSkillKickCount;
	int m_iSkillTimeDecrease;

public:
	void ProcessPacket( CURRENTQUEUE* pQueue, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	BOOL BroadcastPacket( DWORD dwUserIndex, SP2Packet& rkPacket, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void ExtractPacket( UDPPacket* pData, SP2Packet &rkPacket, sockaddr_in &sockAddr );
	void InsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID, DWORD dwUserSeed, DWORD dwNPCSeed, int iCoolType
		, int iModeType, int iRoomStyle, int iTeamType );
	void RelayServerInsertGroup( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID);
	void LocalInsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, DWORD dwUserSeed, DWORD dwNPCSeed, int iCoolType, int iModeType, int iRoomStyle, int iTeamType );

	void RemoveRoom( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void InsertRoom( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );

	BOOL ReceivePacket( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	BOOL Broadcast( SP2Packet &rkPacket, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );


	void UpdateWinCnt( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void UpdateScore( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void UpdateTimer( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void UpdateSPPotion( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );
	void OnUpdateDieState( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );


	void ReloadAntihack( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr );

	void EnqueueByIndex(DWORD dwUserIndex,void* pData);
	void InitRelayGroups(int iThreadCount, int iRoomSeed);
	ioRelayGroupInfoMgr* GetMyRelayRoomInfo();
	ioRelayGroupInfoMgr* GetMyRelayGroupInfoFromRoomIndex(DWORD dwRoomIndex);
	DWORD GetRoomIndexByRelayPacket(SP2Packet& rkPacket);

	//update 처리	
	void UpdateDieState(const DWORD dwRoomIndex, const DWORD dwUserIndex, const BOOL bDieState);
	void UpdateRelayGroupWin( DWORD dwRoomIndex, int iRedTeamWinCnt, int iBlueTeamWinCnt );
	void UpdateRelayGroupScore( DWORD dwRoomIndex, int iTeamType );
	void UpdateSPPotion( DWORD dwRoomIndex, DWORD dwUserIndex );


	//relaygroupinfo
	long m_iRelayGroupIndex;
	typedef std::vector<ioRelayGroupInfoMgr*>  RELAYGROUPS;
	RELAYGROUPS m_relayGroups;


	void InsertTimerOperation();
	void OnAntihackReload();
#endif
	//////////////////////////////////////////////////////////////////////////
};

