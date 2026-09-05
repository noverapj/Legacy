#pragma once

#include "DATA.h"

class TestCase
{
public:
	TestCase(void);
	virtual ~TestCase(void);
 
	virtual void Run() = 0;

	template <class T>
	int unique_check(std::pair<T,bool> &result)
	{
		int rtval = 0;;
		if(result.second == true)
			rtval = 0;
		else
			rtval = -1;
		return rtval;
	}
};

