#ifndef __ioLocalThailand_h__
#define __ioLocalThailand_h__

#include "ioLocalParent.h"

class ioLocalThailand  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalThailand(void);
	virtual ~ioLocalThailand(void);
};

#endif // __ioLocalThailand_h__