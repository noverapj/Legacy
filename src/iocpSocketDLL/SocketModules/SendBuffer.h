#pragma once

class IOCP_SOCKET_API CSendBuffer : public IOBufferedContext
{
public:
	CSendBuffer(void);
	~CSendBuffer(void);

private:
	char m_sendBuffer[ MAX_SENDBUFFER ];
	int m_totalSize;

public:
	void Init();
	void WriteBuffer( const char* buffer, const int size );
	const int GetBufferSize() const;

private:
	void UpdateBufferSize( const int size );
	char* GetSendBuffer();
	
};
