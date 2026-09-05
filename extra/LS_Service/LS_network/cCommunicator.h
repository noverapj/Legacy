#pragma once

class cCommandQueue;
class cConnectorQueue;

class cCommunicator  
{
public:
	cCommunicator(const uint32 maxCount);
	virtual ~cCommunicator();

	void Init(const uint32 maxCount);
	void Destroy();

protected:
	BOOL CreateCommandQueue(const uint32 maxCount, const uint32 timeout=INFINITE);
	BOOL CreateConnectorQueue();

	void DestroyCommandQueue();
	void DestroyConnectorQueue();

public:
	cCommandQueue*		GetCommandQueue()	{ return m_commandQueue;	}
	cConnectorQueue*	GetConnectorQueue()	{ return m_connectorQueue;	}

private:
	cCommandQueue*		m_commandQueue;
	cConnectorQueue*	m_connectorQueue;
};