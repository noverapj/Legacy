// cSingleton.h: interface for the cSingleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSINGLETON_H__84137C84_0822_4457_A94B_3A0F1EB45167__INCLUDED_)
#define AFX_CSINGLETON_H__84137C84_0822_4457_A94B_3A0F1EB45167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000





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




#endif // !defined(AFX_CSINGLETON_H__84137C84_0822_4457_A94B_3A0F1EB45167__INCLUDED_)
