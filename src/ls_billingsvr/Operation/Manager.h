#pragma once

#include "../../include/cSingleton.h"

class SchedulerNode;

class Manager
{
public:
	Manager(void);
	~Manager(void);

public:
	void Init();
	void Destroy();

protected:
	BOOL Startup(const char* scriptName);
	void SetHeapInformation();
	void DisplayHeapInfo( HANDLE heap );
	void InitScheduler();
	BOOL Prepare();
	void Timer();

public:
	void ChangeDeleteBillInfoScheduleTime();
	
public:
	BOOL Run(const char* scriptName);
	int GetErrorCode() { return m_error; }
	
protected:
	int m_error;

protected:
	

protected:
	SchedulerNode	*m_scheduler;
};

typedef cSingleton<Manager> S_Manager;
#define g_Manager (*S_Manager::GetInstance())