#ifndef __ioChannelingNodeDaum_h__
#define __ioChannelingNodeDaum_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeDaum : public ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeDaum(void);
	virtual ~ioChannelingNodeDaum(void);
};

#endif // __ioChannelingNodeDaum_h__
