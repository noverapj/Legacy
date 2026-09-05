#ifndef __ioChannelingNodeParent_h__
#define __ioChannelingNodeParent_h__

class User;

class ioChannelingNodeParent
{
public: 
	virtual bool IsShowDaumWindow( const char *szRootDir ) = 0;

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeParent(void);
	virtual ~ioChannelingNodeParent(void);
};

#endif // __ioChannelingNodeParent_h__