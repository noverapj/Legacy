#pragma once


#include <boost/any.hpp>

class Manager
{
public:
	Manager(void);
	virtual ~Manager(void);

public:
	bool Init();

public:
	bool Run(TCHAR* scriptName);
	void SetTcpLog();
};

