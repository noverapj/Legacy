#include "../iocpSocketDLL.h"
#include "BufferPool.h"


BufferPool::BufferPool(void)
{
	bigSizeCount = 0;
	m_64PopCount = 0;
	m_128PopCount = 0;
	m_256PopCount = 0;
	m_1024PopCount = 0;
	m_2048PopCount = 0;

	m_max64PopCount = 0;
	m_max128PopCount = 0;
	m_max256PopCount = 0;
	m_max1024PopCount = 0;
	m_max2048PopCount = 0;

	m_64DropCount = 0;
	m_128DropCount = 0;
	m_256DropCount = 0;
	m_1024DropCount = 0;
	m_2048DropCount = 0;

	m_64RemainderCount = 0;
	m_128RemainderCount = 0;
	m_256RemainderCount = 0;
	m_1024RemainderCount = 0;
	m_2048RemainderCount = 0;

	m_64PushCount = 0;
	m_128PushCount = 0;
	m_256PushCount = 0;
	m_1024PushCount = 0;
	m_2048PushCount = 0;
}

BufferPool::~BufferPool(void)
{
}

void BufferPool::init( int seed )
{
	m_buf64.CreatePool(seed,300000,TRUE);
	m_buf128.CreatePool(seed,500000,TRUE);
	m_buf256.CreatePool(seed,300000,TRUE);
	m_buf1024.CreatePool(seed,50000,TRUE);
	m_buf2048.CreatePool(seed,10,TRUE);
}

void BufferPool::init(int seed_64, int seed_128,int seed_256, int seed_1024, int seed_2048)
{
	m_buf64.CreatePool(seed_64,300000,TRUE);
	m_buf128.CreatePool(seed_128,500000,TRUE);
	m_buf256.CreatePool(seed_256,300000,TRUE);
	m_buf1024.CreatePool(seed_1024,50000,TRUE);
	m_buf2048.CreatePool(seed_2048,10,TRUE);
}

char* BufferPool::Get( int size )
{
	if(size < 64)
	{
		char* rtval = reinterpret_cast<char*>(m_buf64.Pop());

		if(rtval == NULL)
		{
			IncreaseValue(m_64DropCount);
			return NULL;
		}

		IncreaseValue(m_64PopCount);
		ZeroMemory(rtval,size);
		return rtval;
	}
	else if(size >= 64 && size < 128)
	{	
		char* rtval = reinterpret_cast<char*>(m_buf128.Pop());

		if(rtval == NULL)
		{
			IncreaseValue(m_128DropCount);
			return NULL;
		}

		IncreaseValue(m_128PopCount);
		ZeroMemory(rtval,size);
		return rtval;
	}
	else if(size >= 128 && size < 256)
	{	
		char* rtval = reinterpret_cast<char*>(m_buf256.Pop());

		if(rtval == NULL)
		{
			IncreaseValue(m_256DropCount);
			return NULL;
		}

		IncreaseValue(m_256PopCount);
		ZeroMemory(rtval,size);
		return rtval;
	}
	else if(size >= 256 && size < 1024)
	{
		char* rtval = reinterpret_cast<char*>(m_buf1024.Pop());

		if(rtval == NULL)
		{
			IncreaseValue(m_1024DropCount);
			return NULL;
		}

		IncreaseValue(m_1024PopCount);
		ZeroMemory(rtval,size);
		return rtval;
	}
	else if(size >= 1024 && size < 2048)
	{
		PrintTimeAndLog(0,"[UDPNODE] Big size Packet Request(%d)",size);

		char* rtval = reinterpret_cast<char*>(m_buf2048.Pop());

		if(rtval == NULL) 
		{
			IncreaseValue(m_2048DropCount);
			return NULL;
		}

		IncreaseValue(m_2048PopCount);
		ZeroMemory(rtval,size);
		return rtval;
	}
	
	PrintTimeAndLog(0,"[UDPNODE] Big size Packet Request(%d)",size);
	return NULL;
}

void BufferPool::Push( char* buf,int size )
{
	if(buf == NULL)
	{
		PrintTimeAndLog(0,"Error BufferPool Push buf Is NULL");
		return;
	}
	if(size < 64)
	{
		m_buf64.Push(reinterpret_cast<BUF64_*>(buf));

		IncreaseValue(m_64PushCount );
		SetMaxRemainder(m_64PopCount,m_64PushCount,m_64RemainderCount);
	}
	else if(size >= 64 && size < 128)
	{
		m_buf128.Push(reinterpret_cast<BUF128_*>(buf));

		IncreaseValue(m_128PushCount);
		SetMaxRemainder(m_128PopCount,m_128PushCount,m_128RemainderCount);
	}
	else if(size >= 128 && size < 256)
	{
		m_buf256.Push(reinterpret_cast<BUF256_*>(buf));

		IncreaseValue(m_256PushCount);
		SetMaxRemainder(m_256PopCount,m_256PushCount,m_256RemainderCount);
	}
	else if(size >= 256 && size < 1024)
	{
		m_buf1024.Push(reinterpret_cast<BUF1024_*>(buf));

		IncreaseValue(m_1024PushCount);
		SetMaxRemainder(m_1024PopCount,m_1024PushCount,m_1024RemainderCount);
	}
	else if(size >= 1024 && size < 2048)
	{
		m_buf2048.Push(reinterpret_cast<BUF2048_*>(buf));

		IncreaseValue(m_2048PushCount);
		SetMaxRemainder(m_2048PopCount,m_2048PushCount,m_2048RemainderCount);
		PrintTimeAndLog(0,"[UDPNODE] Big size Packet Push(%d)",size);
	}
	else
	{
		PrintTimeAndLog(0,"[UDPNODE] Big size Packet Push(%d)",size);
	}
}

void BufferPool::DestroyBufferPool()
{
	m_buf64.DestroyPool();
	m_buf128.DestroyPool();
	m_buf256.DestroyPool();
	m_buf1024.DestroyPool();
	m_buf2048.DestroyPool();
}	

void BufferPool::IncreaseValue( long &val )
{
	InterlockedIncrement(&val);
	if(val > 10000000)
		SetZeroValue(val);
}

void BufferPool::SetZeroValue( long &val )
{
	InterlockedExchange(&val,0);
}

long BufferPool::GetBuffer64PoolCount()
{
	long rtval = m_64PopCount;
	if(m_max64PopCount < m_64PopCount)
		m_max64PopCount = m_64PopCount;
	SetZeroValue(m_64PopCount);
	return rtval;
}

long BufferPool::GetBuffer128PoolCount()
{
	long rtval = m_128PopCount;
	if(m_max128PopCount < m_128PopCount)
		m_max128PopCount = m_128PopCount;
	SetZeroValue(m_128PopCount);
	return rtval;
}

long BufferPool::GetBuffer256PoolCount()
{
	long rtval = m_256PopCount;
	if(m_max256PopCount < m_256PopCount)
		m_max256PopCount = m_256PopCount;
	SetZeroValue(m_256PopCount);
	return rtval;
}

long BufferPool::GetBuffer1024PoolCount()
{
	long rtval = m_1024PopCount;
	if(m_max1024PopCount < m_1024PopCount)
		m_max1024PopCount = m_1024PopCount;
	SetZeroValue(m_1024PopCount);
	return rtval;
}

long BufferPool::GetBuffer2048PoolCount()
{
	long rtval = m_2048PopCount;
	if(m_max2048PopCount < m_2048PopCount)
		m_max2048PopCount = m_2048PopCount;
	SetZeroValue(m_2048PopCount);
	return rtval;
}

void BufferPool::SetMaxRemainder( long popCount,long pushCount ,long& remainderCount )
{
	if(remainderCount < popCount - pushCount)
		remainderCount = popCount - pushCount;
}

long BufferPool::Get64Remainder()
{
	long rtval = m_64RemainderCount;
	SetZeroValue(m_64PushCount);
	return rtval;
}

long BufferPool::Get128Remainder()
{
	long rtval = m_128RemainderCount;
	SetZeroValue(m_128PushCount);
	return rtval;
}

long BufferPool::Get256Remainder()
{
	long rtval = m_256RemainderCount;
	SetZeroValue(m_256PushCount);
	return rtval;
}

long BufferPool::Get1024Remainder()
{
	long rtval = m_1024RemainderCount;
	SetZeroValue(m_1024PushCount);
	return rtval;
}

long BufferPool::Get2048Remainder()
{
	long rtval = m_2048RemainderCount;
	SetZeroValue(m_2048PushCount);
	return rtval;
}