#pragma once


class ConnectAssist :public Thread
{
public:
	ConnectAssist(void);
	virtual ~ConnectAssist(void);

public:
	void Init();
	void Destroy();

public:
	void InitMemoryPool();
	
public:
	void Run(); 
	void PutQueue(Connect_* data);
	Connect_* GetQueue();
	void ConnectClient(SVRCONNECTINFO_& addr);
	int GetMemPoolSize(){return m_memPool.GetCount();};
	void PushClient(GameServerNode* node);
	void CreateTimer(SVRCONNECTINFO_& addr);
	void PutConnectData(int opid,SVRCONNECTINFO_& addr,GameServerNode* node = NULL);

protected:
	boost::locking_queue<Connect_*> m_queue;
	MemPooler<GameServerNode>    m_memPool;
};

