#ifndef __ioChannelingNodeNaver_h__
#define __ioChannelingNodeNaver_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeNaver : public ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeNaver(void);
	virtual ~ioChannelingNodeNaver(void);
};

#endif // __ioChannelingNodeNaver_h__