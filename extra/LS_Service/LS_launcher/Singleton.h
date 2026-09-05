#pragma once
#include <boost/shared_ptr.hpp>

template< typename T >     
class ioSingleton     
{     
private:     
	~ioSingleton() { };     
	ioSingleton() { };   
	ioSingleton( const T& other ) { };   
	T& operator=( const T& other ) { };   

public:     
	inline static T* instance();  

	static boost::shared_ptr< T > pInstance;   
};     

template< typename T >     
boost::shared_ptr< T > ioSingleton< T >::pInstance;     

template< typename T >     
T* ioSingleton< T >::instance()     
{        
	if( 0 == pInstance )        
		pInstance.reset( new T );   

	return pInstance.get();     
}     
