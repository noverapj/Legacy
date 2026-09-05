#include "stdafx.h"
#include "ioTimeStamp.h"


ioTimeStamp::ioTimeStamp(void)
{
	Init();
}

ioTimeStamp::~ioTimeStamp(void)
{
	Destroy();
}

void ioTimeStamp::Init()
{
	m_TimeStampMap.clear();
}

void ioTimeStamp::Destroy()
{
}

BOOL ioTimeStamp::IsEnable(const int ID, const DWORD dwInterval)
{
	DWORD dwCurrent = GetTickCount();
	DWORD dwLast	= GetTimeStamp(ID);

	if((0 == dwLast) || ((dwCurrent-dwLast) > dwInterval))
	{
		m_TimeStampMap[ID] = dwCurrent;
		return TRUE;
	}

	m_TimeStampMap[ID] = dwCurrent;
	return FALSE;
}

DWORD ioTimeStamp::GetTimeStamp(const int ID)
{
	TIMESTAMP_MAP::iterator it = m_TimeStampMap.find(ID);
	if(it != m_TimeStampMap.end())
	{
		return it->second;
	}
	return 0;
}