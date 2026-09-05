#pragma once

class CLogData;

//--------------------------------
// Log Interface class
//--------------------------------
class ioILogger
{
public:
	ioILogger(void);
	virtual ~ioILogger(void);

private:
	void Init();
	void Destroy();

private:
	void Register();

public:
	virtual void ExcuteOpen(CLogData* logData) = 0;
	virtual void ExcuteWrite(CLogData* logData) = 0;
	virtual void ExcuteClose(CLogData* logData) = 0;
	virtual void ExcuteSetCategory(CLogData* logData) = 0;
	virtual void ExcuteInitData() = 0;

public:
	virtual BOOL GetTcpState() = 0;

public:
	void Enqueue(CLogData* logData);

protected:
	CLogData* PopData();
	void PushData(CLogData* logData);

};

