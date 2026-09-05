#ifndef __ioLocalTaiwan_h__
#define __ioLocalTaiwan_h__

#include "ioLocalParent.h"

class ioLocalTaiwan  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalTaiwan(void);
	virtual ~ioLocalTaiwan(void);
};

#endif // __ioLocalTaiwan_h__