#ifndef __ioLocalIndonesia_h__
#define __ioLocalIndonesia_h__

#include "ioLocalParent.h"

class ioLocalIndonesia  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalIndonesia(void);
	virtual ~ioLocalIndonesia(void);
};

#endif // __ioLocalIndonesia_h__