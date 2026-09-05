#ifndef __ioLocalIndonesia_h__
#define __ioLocalIndonesia_h__

#include "ioLocalParent.h"

class ioLocalIndonesia  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual const char *GetMonitorPassword();

public:
	ioLocalIndonesia(void);
	virtual ~ioLocalIndonesia(void);
};

#endif // __ioLocalIndonesia_h__