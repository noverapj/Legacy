#pragma once


#include <string>

class LSLogic : public CProcessor
{
public:
	LSLogic(void);
	virtual ~LSLogic(void);

public://function
	void Init();
	void Destory();
	void SetQueue(LSPacketQueue* packetQueue);

public:
	virtual void Process(uint32& idleTime);
	void InitScheduler();
	void StartShcedule();
	SchedulerNode* PScheduler() const { return m_scheduler; }
	void PScheduler(SchedulerNode* val) { m_scheduler = val; }
	void SetNewdata(DWORD dwProcessTime = 0);
	void PrintTimeAndLog(int debuglv, LPSTR fmt );
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );
	void ShutDown(BOOL state);

private:
	LogicThread *m_logic;
	LSPacketQueue *m_queue;
	SchedulerNode* m_scheduler;
	DWORD m_logCheckTime;
};

