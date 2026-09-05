#include "../stdafx.h"
#include ".\iolocalparent.h"

ioLocalParent::ioLocalParent(void)
{
	m_nSubType = 0;
}

ioLocalParent::~ioLocalParent(void)
{
}

void ioLocalParent::EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN bool bPassword )
{
	enum { MAX_KEY_TYPE = 2,  MAX_KEY = 30, };
	BYTE byKey[MAX_KEY_TYPE][MAX_KEY]={255,1,2,9,89,32,123,39,34,211,222,244,100,129,23,1,4,3,29,30,1,4,5,7,8,233,89,1,98,67, // password ¿ë
	                                   48,29,96,1,9,48,57,213,178,123,67,90,2,4,254,255,6,8,9,23,90,44,214,199,108,119,3,2,2,};
	int iKeyType = 0;
	if( !bPassword )
		iKeyType = 1;

	for(int i =0; i < iSourceSize; i++)
	{
		if( i >= iResultSize ) break;
		szResultData[i] = szSourceData[i] ^ byKey[iKeyType][i%MAX_KEY];
		szResultData[i] = szResultData[i] ^ byKey[iKeyType][(iSourceSize-i)%MAX_KEY];
	}
}
