#ifndef __ioChannelingNodeMgame_h__
#define __ioChannelingNodeMgame_h__

#include "ioChannelingNodeParent.h"

class ioChannelingNodeMgame : public ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir );

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeMgame(void);
	virtual ~ioChannelingNodeMgame(void);
};

#endif // __ioChannelingNodeMgame_h__