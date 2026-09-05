#pragma once

//const int MAX_PACKET_SIZE = 1024 * 4;
const int MAX_PACKET_SIZE = 1024 * 16;

class ioCSocket
{
public:
	ioCSocket(void);
	virtual ~ioCSocket(void);

	void Init();
	void Destroy();

public:
	BOOL Startup();
	void Cleanup();

public:
	virtual BOOL Connect(const char *IP, int port) = 0;
	virtual void Disconnect() = 0;
	virtual BOOL Send(const char *buffer, int length) = 0;
	virtual BOOL Receive(char *buffer, int& length) = 0;

	virtual BOOL IsConnected()	{ return m_connected; }
 
	virtual void OnConnect()	{}
	virtual void OnDisconnect()	{}
	virtual void OnSend()		{}
	virtual void OnReceive(char* buffer, int& length)	{}

public:
	virtual void Debug(const TCHAR *format, ...);
	virtual void Error();

	int GetSocketError()				{ return m_error; }
	unsigned int GetReceiveThreadID()	{ return m_threadId; }

protected:
	BOOL m_connected;

	SOCKET m_socket;
	SOCKADDR_IN	m_sockAddr;

	int m_error;
	unsigned int m_threadId;

};
