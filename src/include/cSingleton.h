// cSingleton.h: interface for the cSingleton class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


template <class objectT>
class cSingleton
{
private:
	cSingleton()	{}
	~cSingleton()	{}

public:
	static objectT* GetInstance() 
	{
		if( !_this ) 
		{
			_this = new objectT;
		}
		return _this;
	}
        
private:
	static objectT* _this;
};

template <class objectT>
objectT* cSingleton<objectT>::_this;


