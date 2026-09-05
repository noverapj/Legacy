#pragma once

class IOCP_SOCKET_API CSendBufferManager
{
public:
	CSendBufferManager( int min = 1000, int max = 50000 );
	~CSendBufferManager(void);

public:
	static CSendBufferManager* GetInstance();
	static void ReleaseInstance();

private:
	void Init( int min, int max );
	void RecordMaxUsingCount();

public:
	CSendBuffer* Pop();
	void Push( CSendBuffer* sendBuffer );

	const int GetRemainCount()		{ return m_memNode.GetCount(); }
	const int GetToalCount()		{ return m_memNode.GetTotalCount(); }
	const int GetMaxUsingCount()	{ return m_maxUsingCount; }

private:
	static CSendBufferManager* _this;

	int32 m_maxUsingCount;
	MemPooler< CSendBuffer > m_memNode;

};

#define g_SendBufferManager	CSendBufferManager::GetInstance()

