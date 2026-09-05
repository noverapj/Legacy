#pragma once

class SchedulerNode;

class Manager
{
public:
	Manager(void);
	~Manager(void);

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
	BOOL Run(const char* scriptName);
	int GetErrorCode() { return m_error; }
	
protected:
	int m_error;
	SchedulerNode	*m_scheduler;
};

