#pragma once

class CCompletionHandler;

class iocpHandler : public CCompletionHandler
{
	static iocpHandler *sg_Instance;
	
public:
	static iocpHandler &GetInstance();
	static void ReleaseInstance();
	
public:
	bool Initialize();
	
private: /* Singleton */
	iocpHandler();
	virtual ~iocpHandler();
};

#define g_iocp iocpHandler::GetInstance()