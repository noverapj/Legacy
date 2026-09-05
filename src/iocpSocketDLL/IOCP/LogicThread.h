#pragma once

#include "../ThreadModules/Thread.h"

class CProcessor;
extern CProcessor* g_processor;


class IOCP_SOCKET_API LogicThread : public Thread  
{
public:
	LogicThread(void);
	~LogicThread(void);

	void Init();
	void Destroy();

public:
	void Run();

	void SetProcessor(CProcessor* processor)
	{
		g_processor = processor;
	}

//protected:
	//CProcessor* m_processor;
};
