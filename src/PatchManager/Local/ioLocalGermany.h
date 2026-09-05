#ifndef __ioLocalGermany_h__
#define __ioLocalGermany_h__

#include "ioLocalParent.h"

class ioLocalGermany : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalGermany(void);
	virtual ~ioLocalGermany(void);
};

#endif // __ioLocalGermany_h__