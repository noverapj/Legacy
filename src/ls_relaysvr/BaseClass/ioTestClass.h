#pragma once
class ioTestClass
{
public:
	ioTestClass(void);
	virtual ~ioTestClass(void);

	void svc();
	void Run();
	void testfunc();
 
public:
	std::vector<std::pair<int,std::set<int>*>> m_threadDatas;

};

