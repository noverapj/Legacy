#pragma once


#include "UserTypeDefine.h"
  
class IOCP_SOCKET_API BufferPool
{
public:
	BufferPool(void);
	virtual ~BufferPool(void);

public:
	void init(int seed);
	void init(int seed_64, int seed_128,int seed_256, int seed_1024, int seed_2048);
	void DestroyBufferPool();
	char* Get(int size);
	void Push(char* buf,int size);
	
public:
	int GetBufferPoolTotalCount() {return m_buf64.GetCount() + m_buf128.GetCount() 
								   + m_buf256.GetCount() + m_buf1024.GetCount() + m_buf2048.GetCount();}
	long GetBuffer64PoolCount();
	long GetBuffer128PoolCount();
	long GetBuffer256PoolCount();
	long GetBuffer1024PoolCount();
	long GetBuffer2048PoolCount();
	long GetMax64PopCount() const { return m_max64PopCount; }
	long GetMax128PopCount() const { return m_max128PopCount; }
	long GetMax256PopCount() const { return m_max256PopCount; }
	long GetMax1024PopCount() const { return m_max1024PopCount; }
	long GetMax2048PopCount() const { return m_max2048PopCount; }
	long Get64DropCount()   
	{ 
		long val = m_64DropCount;
		SetZeroValue(m_64DropCount);
		return val;
	}
	long Get128DropCount()  
	{ 
		long val = m_128DropCount;
		SetZeroValue(m_128DropCount);
		return val;
	}
	long Get256DropCount() 
	{ 
		long val = m_256DropCount;
		SetZeroValue(m_256DropCount);
		return val;
	}
	long Get1024DropCount() 
	{ 
		long val = m_1024DropCount;
		SetZeroValue(m_1024DropCount);
		return val;
	}
	long Get2048DropCount() 
	{ 
		long val = m_2048DropCount;
		SetZeroValue(m_2048DropCount);
		return val;
	}
	void SetMaxRemainder( long popCount,long pushCount ,long& remainderCount );
	long Get64Remainder();
	long Get128Remainder();
	long Get256Remainder();
	long Get1024Remainder();
	long Get2048Remainder();
	void IncreaseValue(long &val);
	void SetZeroValue(long &val);
	
	void Increase64PoolCount() { IncreaseValue(m_64DropCount); }
	void Increase128PoolCount() { IncreaseValue(m_128DropCount); }
	void Increase256PoolCount() { IncreaseValue(m_256DropCount); }
	void Increase1024PoolCount() { IncreaseValue(m_1024DropCount); }
	void Increase2048PoolCount() { IncreaseValue(m_2048DropCount); }

protected:
	long m_64PopCount;
	long m_128PopCount;
	long m_256PopCount;
	long m_1024PopCount;
	long m_2048PopCount;
	long m_64PushCount;
	long m_128PushCount;
	long m_256PushCount;
	long m_1024PushCount;
	long m_2048PushCount;
	long m_max64PopCount;
	long m_max128PopCount;
	long m_max256PopCount;
	long m_max1024PopCount;
	long m_max2048PopCount;
	long m_64DropCount; 
	long m_128DropCount; 
	long m_256DropCount; 
	long m_1024DropCount;
	long m_2048DropCount;
	long m_64RemainderCount;
	long m_128RemainderCount;
	long m_256RemainderCount;
	long m_1024RemainderCount;
	long m_2048RemainderCount;
	int bigSizeCount;

protected:
	MemPooler<BUF64_> m_buf64;
	MemPooler<BUF128_> m_buf128;
	MemPooler<BUF256_> m_buf256;
	MemPooler<BUF1024_> m_buf1024;
	MemPooler<BUF2048_> m_buf2048;

};

