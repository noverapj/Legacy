#pragma once

#include "Thread.h"

class cWorkerThread;
class cCommandThread;
class cConnectorThread;


class cModule  
{
public:
	cModule();
	virtual ~cModule();

	void Init();
	void Destroy();

public:
	// start/close
	BOOL StartModule( E_THREADID threadId, uint32 count );
	void EndModule();

protected:
	// thread module
	BOOL StartCommandThread( uint32 count = 1 );
	BOOL StartWorkerThread( uint32 count = 2 );
	BOOL StartConnectorThread( uint32 count = 1 );

	void CloseCommandThread();
	void CloseWorkerThread();
	void CloseConnectorThread();

private:
	cWorkerThread*		m_workerThread;
	cCommandThread*		m_commandThread;
	cConnectorThread*	m_connectorThread;
};