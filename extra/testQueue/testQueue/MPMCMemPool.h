
template <class T>
struct MemoryElement_
{
	void* data;
	MemoryElement_* volatile  next; 
}; 

template <class T>
class MPSCMempooler
{
public:
	MPSCMempooler(void)
	{

	};
	virtual ~MPSCMempooler(void)
	{

	};
	bool Init(int seed)
	{
		InitializeCriticalSection(&m_cs);
		m_head = m_tail = &m_stub;
		m_stub.next = 0;
		for(int i=0; i<seed; ++i)
		{
			MemoryElement_<T>* element = new MemoryElement_<T>;
			ZeroMemory(&element->data,sizeof(T));
			PushNode(element);
		}
		return true;
	}
public:
	bool Push(T* inputData)
	{
		MemoryElement_<T>* element= reinterpret_cast<MemoryElement_<T>*>(inputData);
		element->next = NULL;
		MemoryElement_<T>* prev =
			reinterpret_cast<MemoryElement_<T>*>(InterlockedExchangePointer((void**)(&m_head),element));
		prev->next = element;
		return true;
	}
	T* Pop()
	{
		MiniLock(m_cs);
		MemoryElement_<T>* tmptail = m_tail; 
		MemoryElement_<T>* next = tmptail->next; 
		if (tmptail == &m_stub) 
		{ 
			if (NULL == next) 
				return NULL; 
			m_tail = next; 
			tmptail = next; 
			next = next->next; 
		} 
		if (next) 
		{ 
			m_tail = next; 
			return reinterpret_cast<T*>(&tmptail->data); 
		} 
		MemoryElement_<T>* tmphead = m_head;
		if (tmptail != tmphead) 
			return NULL; 
		PushNode(&m_stub); 
		next = tmptail->next; 
		if (next) 
		{ 
			m_tail = next; 
			return reinterpret_cast<T*>(&tmptail->data); 
		} 
		return NULL; 
	}
private:
	void PushNode(MemoryElement_<T>* element)
	{
		element->next = NULL;
		MemoryElement_<T>* prev = 
			reinterpret_cast<MemoryElement_<T>*>(InterlockedExchangePointer((void**)(&m_head),element));
		prev->next =  element;
	}
protected:
	MemoryElement_<T>* volatile  m_head; 
	MemoryElement_<T>*           m_tail; 
	MemoryElement_<T>            m_stub; 
	CRITICAL_SECTION m_cs;
};

