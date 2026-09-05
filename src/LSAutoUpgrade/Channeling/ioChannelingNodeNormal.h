#ifndef __ioChannelingNodeNormal_h__
#define __ioChannelingNodeNormal_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeNormal : public ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeNormal(void);
	virtual ~ioChannelingNodeNormal(void);
};

#endif // __ioChannelingNodeNormal_h__
