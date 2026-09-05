#ifndef __ioLocalParent_h__
#define __ioLocalParent_h__

#include "ioLocalManager.h"

class ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType() = 0;
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 ) = 0;

	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN bool bPassword );
		
public:
	ioLocalParent(void);
	virtual ~ioLocalParent(void);
};

#endif // __ioLocalParent_h__
