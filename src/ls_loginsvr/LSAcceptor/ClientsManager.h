#pragma once
#include <atlcoll.h>
typedef ATL::CAtlList<ClientNode*> LISTCLIENTNODE;
//템플릿으로 돌릴지 고민할것 
//이거 상속받아서 하고싶은데 서버는 굳이 리스트로 할필요가 없음
//기능도 약간 다름 
class ClientsManager
{
public:
	ClientsManager(void);
	virtual ~ClientsManager(void);

public:
	virtual void InitMemoryPool();
	virtual void ReleaseMemoryPool();
	virtual void AddClient(ClientNode* node);
	virtual bool DelClient(ClientNode* node);
	virtual bool CreateClientNode(SOCKET S);
	virtual int  GetNodeSize(){return m_nodes.GetCount();}
	virtual void BraodcastFilter( const int connectType, SP2Packet &rkPacket );
	virtual void Broadcast( SP2Packet &rkPacket );
	int			 GetMemPoolSize(){return m_memPool.GetCount();};
	bool		 SendMessageNode(int fd,SP2Packet &rkPacket);
	bool		 SendCloseMessage(int fd);
	void		 Flush();
	void		 DelGostClient();
	
protected:
	MemPooler<ClientNode>	m_memPool;
	//MemPooler<ClientNode>	m_memPool;
	LISTCLIENTNODE			m_nodes;

};

