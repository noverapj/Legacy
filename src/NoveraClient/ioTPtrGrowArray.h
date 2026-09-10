#pragma once

template <typename T>
class ioTPtrGrowArray
{
private:
	T** m_pBase;

	int m_iMaxCount;
	int m_iGrowCount;

	int m_iUseCount;

public:
	inline void Add( T *pPtr )
	{
		if( !pPtr )	return;

		int i=0;
		for( i=0 ; i<m_iMaxCount ; i++ )
		{
			if( NULL == m_pBase[i] )
			{
				m_pBase[i] = pPtr;
				++m_iUseCount;
				return;
			}
		}

		// Not Enough
		T** pTemp = new T*[m_iMaxCount + m_iGrowCount];
		for( i=0 ; i<m_iMaxCount ; i++ )
		{
			pTemp[i] = m_pBase[i];
		}

		pTemp[i++] = pPtr;
		++m_iUseCount;

		m_iMaxCount += m_iGrowCount;
		for( ; i<m_iMaxCount ; i++ )
		{
			pTemp[i] = NULL;
		}

		delete []m_pBase;
		m_pBase = pTemp;
	}

	inline void Remove( int i )
	{
		if( i>=0 && i<m_iMaxCount )
		{
			m_pBase[i] = NULL;
			--m_iUseCount;
		}
	}

	inline T* GetAt( int i ) const
	{
		if( i>=0 && i<m_iMaxCount )
			return m_pBase[i];

		return NULL;
	}

	inline int MaxCount() const { return m_iMaxCount; }
	inline int UseCount() const { return m_iUseCount; }

	inline bool Empty() const
	{
		if( m_iUseCount == 0 )
			return true;

		return false;
	}

	inline void Clear()
	{
		for( int i=0 ; i<m_iMaxCount; i++ )
		{
			m_pBase[i] = NULL;
		}
	}

public:
	ioTPtrGrowArray( int iMaxCount, int iGrowCount )
	{
		m_pBase = new T*[iMaxCount];

		m_iMaxCount  = iMaxCount;
		m_iGrowCount = iGrowCount;
		m_iUseCount  = 0;

		Clear();
	}

	~ioTPtrGrowArray()
	{
		delete []m_pBase;
	}
};

