#ifndef __ioLocalUS_h__
#define __ioLocalUS_h__

#include "ioLocalParent.h"

class ioLocalUS : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalUS(void);
	virtual ~ioLocalUS(void);
};

#endif // __ioLocalUS_h__