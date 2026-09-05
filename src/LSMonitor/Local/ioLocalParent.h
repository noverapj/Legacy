#ifndef __ioLocalParent_h__
#define __ioLocalParent_h__

#include "ioLocalManager.h"

class ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType() = 0;
	virtual const char *GetMonitorPassword() = 0;
		
public:
	ioLocalParent(void);
	virtual ~ioLocalParent(void);
};

#endif // __ioLocalParent_h__
