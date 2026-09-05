#pragma once


#include <atlcoll.h>

//템플릿으로 돌릴지 고민할것 
//이거 상속받아서 하고싶은데 서버는 굳이 리스트로 할필요가 없음
//기능도 약간 다름 
class MonitorsManager
{
public:
	MonitorsManager(void);
	virtual ~MonitorsManager(void);

public:
	virtual void InitMemoryPool();
	virtual void ReleaseMemoryPool();
	virtual void AddClient(MonitorNode* node);
	virtual bool DelClient(MonitorNode* node);
	virtual bool CreateClientNode(SOCKET S);
	virtual int  GetNodeSize(){return m_nodes.GetCount();}
	virtual void BraodcastFilter( const int connectType, SP2Packet &rkPacket );
	virtual void Broadcast( SP2Packet &rkPacket );
	int			 GetMemPoolSize(){return m_memPool.GetCount();};
	bool		 SendMessageNode(int fd,SP2Packet &rkPacket);
	bool		 SendCloseMessage(int fd);
	void		 SendBufferFlush();
	
public:
	typedef ATL::CAtlList<MonitorNode*> LISTCLIENTNODE;

protected:
	MemPooler<MonitorNode>	m_memPool;
	//MemPooler<MonitorNode>	m_memPool;
	LISTCLIENTNODE			m_nodes;
};

