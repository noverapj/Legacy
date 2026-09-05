

#ifndef _ioTPtrArray_h_
#define _ioTPtrArray_h_

template <typename T>
class ioTPtrArray
{
protected:
	T **m_pBase;
	int m_iCount;

	int m_iMaxCount;
	int m_iGrowCount;

public:
	inline void CheckMaxCount( int iNeedCount )
	{
		if( m_iMaxCount < iNeedCount )
		{
			delete []m_pBase;

			m_pBase = new T*[iNeedCount + m_iGrowCount];
			m_iMaxCount = iNeedCount + m_iGrowCount;
		}
	}

public:
	inline void Add( T *pPtr )
	{
		m_pBase[m_iCount] = pPtr;
		m_iCount++;
	}

	inline void Clear() { m_iCount = 0; }
	inline int  GetCount() const { return m_iCount; }
	inline T*   GetAt( int i ) const { return m_pBase[i]; }
	inline T**  GetPtr() const { return m_pBase; }

public:
	ioTPtrArray( int iMaxCount, int iGrowCount )
	{
		m_pBase = new T*[iMaxCount];

		m_iCount = 0;
		m_iMaxCount  = iMaxCount;
		m_iGrowCount = iGrowCount;
	}

	~ioTPtrArray()
	{
		delete []m_pBase;
	}
};

#endif