#pragma once

class cIocpOperation;

class cIOCP
{
public:
	cIOCP(void);
	~cIOCP(void);

	void Init();
	void Destroy();

public:
	BOOL Create();
	void Close();

 	BOOL AssocInstance(	HANDLE socket, DWORD ioKey );
	BOOL GetIocpStatus(	LPDWORD bytesTransferred, 
						PULONG completionKey, 
						LPOVERLAPPED* overlapped, 
						DWORD milliseconds = INFINITE );

protected:
	HANDLE m_completionPort;
};
