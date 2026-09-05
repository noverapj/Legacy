#ifndef __ioLocalUS_h__
#define __ioLocalUS_h__

#include "ioLocalParent.h"

class ioLocalUS : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalUS(void);
	virtual ~ioLocalUS(void);
};

#endif // __ioLocalUS_h__