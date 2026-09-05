#pragma once


#define _ASSERTC(expr) \
	{ \
	if ( !(expr) ) \
		{ \
		int* dummy = 0 ; \
		*dummy = 0xDEADBEEF ; \
		} \
	}

template <class T>
class MPMCQueue
{
public:

	MPMCQueue() : m_headPos(NULL), m_tailPos(NULL) //, m_Elems(NULL)
	{
		m_maxSize = 0;
		m_currentSize = 0;
	}
	void Init(int max) //가능한 2의 제곱으로 해주는게 좋음 
	{
		try
		{
		 
// 			m_maxSize = MakeSize(max);
// 			m_Elems = new T*[m_maxSize];
// 			memset(m_Elems,0,sizeof(T*)*m_maxSize);
// 			m_maxSizeMask = m_maxSize -1;
			m_maxSize = 1048576;
			m_maxSizeMask = 1048576 - 1;
		}
		catch(std::exception& e)
		{

		}

	}
	int MakeSize(int size)
	{
		int nsqrt = 0;
		int resultval = size;
		while(resultval != 1)
		{
			resultval = resultval/2;
			nsqrt++;
		}
		int rtval = pow((double)2,nsqrt+1);
		return rtval;

	}
	void Push(T* newElem)
	{
	 
		__int64 insertPos = InterlockedIncrement64(&m_tailPos) - 1 ;
		_ASSERTC( insertPos - m_headPos < m_maxSize) ; ///< overflow
		m_Elems[insertPos&(m_maxSizeMask)] = newElem ;	
	}
	T* Pop()
	{
		 
		T* popVal = (T*) InterlockedExchangePointer((void**)&m_Elems[m_headPos&(m_maxSizeMask)], nullptr) ;
		if ( popVal != nullptr )
			InterlockedIncrement64(&m_headPos) ;	
		return popVal ;
	}

	__int64 GetSize() const
	{
		 
		return m_tailPos - m_headPos ;
	}
private:
	int m_currentSize;
	int m_maxSize;
	int m_maxSizeMask;
	//T**		m_Elems  ;
	T*          m_Elems[1048576];
	volatile __int64	m_headPos ;
	volatile __int64	m_tailPos ;
};

template <class T>
class ORIMPMCQueue
{
public:

	template<int E>
	struct PowerOfTwo
	{
		enum { value = 2 * PowerOfTwo<E-1>::value } ;
	} ;

	template<>
	struct PowerOfTwo<0>
	{
		enum { value = 1 } ;
	} ;

	enum
	{
		/// 큐의 크기 설정: 반드시 2의 승수로 해야 한다.
		QUEUE_MAX_SIZE	= PowerOfTwo<16>::value, 
		QUEUE_SIZE_MASK	= QUEUE_MAX_SIZE - 1
	} ;

	ORIMPMCQueue() : mHeadPos(0), mTailPos(0) 
	{
		memset(mElem, 0, sizeof(mElem)) ;
	}

	void Push(T* newElem)
	{
		__int64 insertPos = InterlockedIncrement64(&mTailPos) - 1 ;
		_ASSERTC(insertPos - mHeadPos < QUEUE_MAX_SIZE)
		if(insertPos - mHeadPos < QUEUE_MAX_SIZE)///< overflow
		{
			printf("QQQQQQQQQQQQQ\n");
		}

		mElem[insertPos&QUEUE_SIZE_MASK] = newElem ;	
	}

	T* Pop()
	{
		T* popVal = (T*) InterlockedExchangePointer((void**)&mElem[mHeadPos&QUEUE_SIZE_MASK], nullptr) ;

		if ( popVal != nullptr )
			InterlockedIncrement64(&mHeadPos) ;	

		return popVal ;
	}

	__int64 GetSize() const
	{
		return mTailPos - mHeadPos ;
	}



private:

	T*			mElem[QUEUE_MAX_SIZE] ;
	volatile __int64	mHeadPos ;
	volatile __int64	mTailPos ;

} ;