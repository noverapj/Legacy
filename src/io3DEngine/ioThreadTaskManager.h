

#ifndef _ioThreadTaskManager_h_
#define _ioThreadTaskManager_h_

class __EX IThreadTask
{
public:
	virtual void Process() = 0;	// TaskThread Call
	virtual void Done() = 0;	// MainThread Call

public:
	virtual int  GetPriority() const = 0;

public:
	IThreadTask(){}
	virtual ~IThreadTask(){}
};

class __EX ioThreadTaskManager
{
protected:
	typedef std::deque< IThreadTask* > ThreadTaskQueue;
	ThreadTaskQueue m_TaskQueue;
	ThreadTaskQueue m_DoneTaskQueue;
	CRITICAL_SECTION m_TaskSection;

	HANDLE m_hQueueSemaphore;
	HANDLE m_hTaskThread;

	IThreadTask *m_pWorkTask;

	bool m_bTaskThreadDone;
	bool m_bDone;

public:
	void Initialize();
	void AddTask( IThreadTask *pTask );
	void NotifyDoneTasks();

public:
	int ThreadTaskProc();

protected:
	void DestroyThreadObjects();

public:
	static ioThreadTaskManager& GetInstance();
	static void ReleaseInstance();
	static ioThreadTaskManager *GetInstancePtr(){ return m_pManager; }

protected:
	static ioThreadTaskManager *m_pManager;

public:
	ioThreadTaskManager();
	virtual ~ioThreadTaskManager();
};

#define g_ThreadTaskMgr ioThreadTaskManager::GetInstance()

#endif