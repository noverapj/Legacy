#pragma once

// 패킷큐 노드에 대한 타입
enum PacketQueueTypes
{
	PK_QUEUE_NONE = 0,
	PK_QUEUE_SESSION,
	PK_QUEUE_QUERY,
	PK_QUEUE_INTERNAL,
	PK_QUEUE_ACCEPT,
	PK_QUEUE_UDP,
};
struct IOCP_SOCKET_API NodeEntry
{
	NodeEntry* volatile  next; 
	NodeEntry() : next(nullptr) {}
}; 

struct IOCP_SOCKET_API NodeData
{
	NodeEntry m_nodeEntry;
	NodeData() {}
};


class IOCP_SOCKET_API PacketQueue :public NodeData
{
protected:
	BYTE	m_packetType;
	DWORD   m_node;
	PacketQueueTypes m_type;

	char   *m_buffer;
	int     m_maxSize;
	int     m_currentSize;
	int     m_position;
	int		m_defaultSize;

public:
	DWORD GetNode()	{ return m_node; }
	const PacketQueueTypes GetType() { return m_type; }

	const char* GetBuffer()		{ return m_buffer; }
	const int GetMaxSize()		{ return m_maxSize; }
	const int GetSize()			{ return m_currentSize; }
	const int GetPosition()		{ return m_position; }
	const int GetDefaultSize()	{ return m_defaultSize; }

public:
	void Create( const uint32 defaultSize );
	void Clear();

	void Set( DWORD node, CPacket &packet, PacketQueueTypes type );

protected:
	void Copy( const char* buffer, const uint32 size, const uint32 pos );
	void Realloc( const uint32 packeSize );

public:
	PacketQueue();
	~PacketQueue();
};


class IOCP_SOCKET_API PacketQueueType64 : public PacketQueue
{
public:
	PacketQueueType64()
	{
		PacketQueue::Create(64);
	}
};

class IOCP_SOCKET_API PacketQueueType256 : public PacketQueue
{
public:
	PacketQueueType256()
	{
		PacketQueue::Create(256);
	}
};

class IOCP_SOCKET_API PacketQueueType1024 : public PacketQueue
{
public:
	PacketQueueType1024()
	{
		PacketQueue::Create(1024);
	}
};

class IOCP_SOCKET_API PacketQueueTypeBig : public PacketQueue
{
public:
	PacketQueueTypeBig()
	{
		PacketQueue::Create(1024 * 32);
	}
};

class IOCP_SOCKET_API PacketQueueTypeAccept : public PacketQueue
{
public:
	PacketQueueTypeAccept()
	{
		PacketQueue::Create(64);
		m_socket = INVALID_SOCKET;
	}

	void SetSockHandle(SOCKET socket)	{ m_socket = socket; }
	SOCKET GetSocketHandle()			{ return m_socket; }

protected:
	SOCKET m_socket;
};