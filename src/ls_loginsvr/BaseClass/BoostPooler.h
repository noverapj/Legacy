#pragma once

#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#include <boost/pool/singleton_pool.hpp>
#pragma pop_macro("min")
#pragma pop_macro("max")

template<class T>
class BoostPooler
{
public:
	static void* operator new(size_t size)
	{
		return boost::singleton_pool<T,sizeof(T),boost::default_user_allocator_new_delete,boost::details::pool::null_mutex>::malloc();
	}
	static void operator delete(void* p)
	{
		boost::singleton_pool<T,sizeof(T),boost::default_user_allocator_new_delete,boost::details::pool::null_mutex>::free(p);
	}
};

template<class T>
class BoostPoolerL
{
public:
	static void* operator new(size_t size)
	{
		return boost::singleton_pool<T,sizeof(T),boost::default_user_allocator_new_delete>::malloc();
	}
	static void operator delete(void* p)
	{
		boost::singleton_pool<T,sizeof(T),boost::default_user_allocator_new_delete>::free(p);
	}
};
