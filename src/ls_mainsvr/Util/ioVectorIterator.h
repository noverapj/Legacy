

#ifndef _ioVectorIterator_h_
#define _ioVectorIterator_h_

template <class T>
class ioVectorIterator
{
private:
	typename T::iterator m_Current;
	typename T::iterator m_End;

	ioVectorIterator(){};

public:
	ioVectorIterator( typename T::iterator iStart,
					  typename T::iterator iEnd ) : m_Current(iStart), m_End(iEnd)
	{
	}

	bool HasMoreElements() const
	{
		return m_Current != m_End;
	}

	typename T::value_type Next()
	{
		return *m_Current++;
	}

	typename T::value_type PeekNext()
	{
		return *m_Current;
	}
};

template < class T >
class ioConstVectorIterator
{
private:
	mutable typename T::const_iterator m_Current;
	typename T::const_iterator m_End;

	ioConstVectorIterator(){};

public:
	ioConstVectorIterator( typename T::const_iterator iStart, typename T::const_iterator iEnd )
		: m_Current(iStart), m_End(iEnd)
	{
	}

	bool HasMoreElements() const
	{
		return m_Current != m_End;
	}

	typename T::value_type Next()
	{
		return *m_Current++;
	}

	typename T::value_type PeekNext()
	{
		return *m_Current;
	}
};

#endif
