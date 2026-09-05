#ifndef __ioLocalJapan_h__
#define __ioLocalJapan_h__

#include "ioLocalParent.h"

class ioLocalJapan  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalJapan(void);
	virtual ~ioLocalJapan(void);
};

#endif // __ioLocalJapan_h__