

#ifndef _ioListIterator_h_
#define _ioListIterator_h_

template <class T>
class ioListIterator
{
private:
	typename T::iterator m_Current;
	typename T::iterator m_End;

	ioListIterator(){};

public:
	ioListIterator( typename T::iterator iStart,
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

#endif