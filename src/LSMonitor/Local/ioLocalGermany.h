#ifndef __ioLocalGermany_h__
#define __ioLocalGermany_h__

#include "ioLocalParent.h"

class ioLocalGermany : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalGermany(void);
	virtual ~ioLocalGermany(void);
};

#endif // __ioLocalGermany_h__