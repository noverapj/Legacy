#include "../iocpSocketDLL.h"
#include "SendBufferManager.h"


CSendBufferManager* CSendBufferManager::_this = NULL;

CSendBufferManager::CSendBufferManager( int min, int max ) : m_maxUsingCount(0)
{
	Init( min, max );
}

CSendBufferManager::~CSendBufferManager(void)
{

	m_memNode.DestroyPool();
}

void CSendBufferManager::Init( int min, int max )
{
	m_maxUsingCount = 0;

	// create
	m_memNode.CreatePool( min, max, TRUE );
}

CSendBufferManager* CSendBufferManager::GetInstance()
{
	if( NULL == _this )
	{
		_this = new CSendBufferManager;
	}
	return _this;
}

void CSendBufferManager::ReleaseInstance()
{
	if( _this )
	{
		delete _this;
		_this = NULL;
	}
}

CSendBuffer* CSendBufferManager::Pop()
{
	CSendBuffer* sendBuffer = m_memNode.Pop();
	if( sendBuffer != NULL )
	{
		sendBuffer->Init();

		// record
		RecordMaxUsingCount();
	}
	return sendBuffer;
}

void CSendBufferManager::Push( CSendBuffer* sendBuffer )
{
	m_memNode.Push( sendBuffer );
}

void CSendBufferManager::RecordMaxUsingCount()
{
	int usingCount = GetToalCount() - GetRemainCount();
	if( m_maxUsingCount < usingCount )
		m_maxUsingCount = usingCount;
}
