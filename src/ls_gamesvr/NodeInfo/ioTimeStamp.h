#pragma once

//#include <map>
#include <boost/unordered_map.hpp>

class ioTimeStamp
{
public:
	ioTimeStamp(void);
	~ioTimeStamp(void);

	void Init();
	void Destroy();

public:
	BOOL IsEnable(const int ID, const DWORD dwInterval);

protected:
	DWORD GetTimeStamp(const int ID);

protected:
	typedef boost::unordered_map<int,DWORD> TIMESTAMP_MAP;

	TIMESTAMP_MAP m_TimeStampMap;

};

