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
	void Startup(const char* scriptName);
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

