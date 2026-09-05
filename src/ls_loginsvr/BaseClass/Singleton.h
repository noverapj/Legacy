#pragma once
#include <boost/shared_ptr.hpp>     

template< typename T >     
class Singleton     
{     
private:     
	~Singleton() { };     
	Singleton() { };   
	Singleton( const T& other ) { };   
	T& operator=( const T& other ) { };   

public:     
	inline static T* instance();  

	static boost::shared_ptr< T > pInstance;   
};     

template< typename T >     
boost::shared_ptr< T > Singleton< T >::pInstance;     

template< typename T >     
T* Singleton< T >::instance()     
{        
	if( 0 == pInstance )        
		pInstance.reset( new T );   

	return pInstance.get();     
}     

