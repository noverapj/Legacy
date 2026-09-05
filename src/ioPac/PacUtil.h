//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACUTIL_H__9ACDC264_A790_4B11_94EC_301E485FA05C__INCLUDED_)
#define AFX_PACUTIL_H__9ACDC264_A790_4B11_94EC_301E485FA05C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PAC_API CPacUtil  
{
public:
	enum ReturnValue
	{
		RV_OK       = 0,
		RV_ERROR_1,
		RV_ERROR_2,
		RV_ERROR_3,
		RV_ERROR_4,
		RV_ERROR_5,
		RV_ERROR_6,
		RV_ERROR_7,
		RV_ERROR_8,
		RV_ERROR_9,
		RV_ERROR_10,
	};
protected:
	CPacUtil();
	virtual ~CPacUtil();

public:
	void ReplaceChar(OUT char *pSource,IN const int iSourceSize,IN char chOld,IN char chNew);
};

#endif // !defined(AFX_PACUTIL_H__9ACDC264_A790_4B11_94EC_301E485FA05C__INCLUDED_)
