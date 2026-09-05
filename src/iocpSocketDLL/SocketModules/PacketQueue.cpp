#include "../iocpSocketDLL.h"
#include "PacketQueue.h"


PacketQueue::PacketQueue( ) : m_defaultSize(0), m_node(0), m_type(PK_QUEUE_NONE), m_buffer(NULL), m_maxSize(0), m_currentSize(0), m_position(0)
{
}

PacketQueue::~PacketQueue()
{
	if(m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}
}

void PacketQueue::Create( const uint32 defaultSize )
{
	m_defaultSize	= defaultSize;
	m_buffer		= new char[defaultSize];
	m_maxSize		= m_defaultSize;
}

void PacketQueue::Clear()
{       
	memset(&m_nodeEntry,0,sizeof(m_nodeEntry)); //kyg 중요 
	m_node			= 0;
	m_currentSize	= 0;
	m_position		= 0;
	if( m_maxSize > m_defaultSize )
	{
		Realloc(m_defaultSize);
	}
}   

void PacketQueue::Set( DWORD node, CPacket &packet, PacketQueueTypes type )
{
	m_node	= node;
	m_type	= type;

	if( m_maxSize >= packet.GetBufferSize() )
	{
		Copy(packet.GetBuffer(), packet.GetBufferSize(), packet.GetCurPos());
	}
	else
	{
		// 버퍼재할당
		Realloc(packet.GetBufferSize());
	
		Copy(packet.GetBuffer(), packet.GetBufferSize(), packet.GetCurPos());
	}
}    

void PacketQueue::Copy( const char* buffer, const uint32 size, const uint32 pos )
{
	memcpy( m_buffer, buffer, size );
	m_currentSize	= size;
	m_position		= pos;
}

void PacketQueue::Realloc( const uint32 packeSize )
{
	if(m_buffer)
	{
		delete[] m_buffer;

		m_maxSize	= packeSize;
		m_buffer	= new char[packeSize];
	}
}