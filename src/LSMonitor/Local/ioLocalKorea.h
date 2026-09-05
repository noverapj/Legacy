#ifndef __ioLocalKorea_h__
#define __ioLocalKorea_h__

#include "ioLocalParent.h"

class ioLocalKorea : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalKorea(void);
	virtual ~ioLocalKorea(void);
};

#endif // __ioLocalKorea_h__