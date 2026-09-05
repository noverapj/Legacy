#pragma once

class IOCP_SOCKET_API CProcessor
{
public:
	CProcessor(void);
	~CProcessor(void);

	void Init();
	void Destroy();
	
public:
	virtual void Process(uint32& idleTime) = 0;

	virtual void PrintTimeAndLog(int debuglv, LPSTR fmt ) = 0;
	virtual void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt ) = 0;
};
