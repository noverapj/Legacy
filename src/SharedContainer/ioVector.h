#pragma once

struct SharedParent_ // sharedList를 사용하기위해선 무조건 이놈을 상속받아 써야함 
{
	int index;
	short state;
	SharedParent_()
	{
		index = -1;
		state = -1;
	};
};

class SharedVector;
class ioVector
{
public:
	ioVector(void);
	~ioVector(void);
	bool Set(TCHAR* name,unsigned int count,int size,bool processMutexState = false);
public:
	int GetFreeIndex(); // if notfreeindex return -1
	void* GetAt(unsigned int index);
	bool RemoveAt(unsigned int index); // state만 초기화 
	int Size();
	int GetFreeMemorySize(); // 전체 공유메모리의 사이즈 
private:
	void RemoveVector(); // 벡터 삭제 
protected:
	tstring   m_lastError;
	SharedVector* m_list;
};

