#pragma once

#include <atlcoll.h>
#include "../../include/cSingleton.h"

class CConnectNode;


class ConnectAssist : public Thread, cIocpQueue
{
public:
	ConnectAssist(void);
	virtual ~ConnectAssist(void);

public:
	void Init();
	void Destroy();

public:
	void Run();

public:
	bool Connect(CConnectNode* connectNode);
	void PushNode(CConnectNode* connectNode);
	void PushLogicThread(CConnectNode* connectNode);

protected:
	template <typename T>
	bool ConnectTo(CConnectNode* connectNode)
	{ 
		bool connectState = false;

		T* node = reinterpret_cast<T*>(connectNode); 
		connectState = node->ConnectTo(false); 

		return connectState;
	}

public:
	bool GetRunState() const { return m_runState; }
	void SetRunState(bool val) { m_runState = val; }

protected:
	bool m_runState;

};

typedef cSingleton<ConnectAssist> S_ConnectAssist;
#define g_ConnectAssist (*S_ConnectAssist::GetInstance())