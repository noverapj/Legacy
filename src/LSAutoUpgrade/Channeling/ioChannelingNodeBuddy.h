#ifndef __ioChannelingNodeBuddy_h__
#define __ioChannelingNodeBuddy_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeBuddy : public ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeBuddy(void);
	virtual ~ioChannelingNodeBuddy(void);
};

#endif // __ioChannelingNodeBuddy_h__