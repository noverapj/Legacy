#pragma once

class ConnectAssist :public Thread
{
public:
	ConnectAssist(void);
	virtual ~ConnectAssist(void);

public:
	void		Run(); 
	void		PutQueue(Connect_* data);
	Connect_*	GetQueue();
	void		ConnectClient(SVRCONNECTINFO_& addr);
	void		Init();
	void		Finit();
	int			GetMemPoolSize(){return m_memPool.GetCount();};
	void		PushClient(LSConnector* node);
	void		CreateTimer(SVRCONNECTINFO_& addr);
	void		PutConnectData(int opid,SVRCONNECTINFO_& addr,LSConnector* node = NULL);

protected:
	boost::locking_queue<Connect_*> m_queue;
	//MemPooler<LSConnector>    m_memPool;
	MemPooler<LSConnector>    m_memPool;
};

