

#ifndef _ioMapIterator_h_
#define _ioMapIterator_h_

#include "ioPrerequisites.h"

template <typename T>
class ioMapIterator
{
private:
	typename T::iterator m_CurIter;
	typename T::iterator m_End;

	/// Private constructor since only the parameterised constructor should be used
	ioMapIterator(){};

public:
	ioMapIterator( typename T::iterator begin,
		typename T::iterator end ) : m_CurIter(begin), m_End(end)
	{
	}

	bool HasMoreElements()
	{
		return m_CurIter != m_End;
	}

	typename T::mapped_type Next()
	{
		return (m_CurIter++)->second;
	}

	typename T::mapped_type PeekNext()
	{
		return m_CurIter->second;
	}

	typename T::key_type PeekNextKey()
	{
		return m_CurIter->first;
	}

	ioMapIterator<T>& operator=( ioMapIterator<T> &rhs )
	{
		m_CurIter = rhs.m_CurIter;
		m_End = rhs.m_End;
	}
};

#endif
