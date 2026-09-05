#ifndef __ioLocalSingapore_h__
#define __ioLocalSingapore_h__

#include "ioLocalParent.h"

class ioLocalSingapore  : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalSingapore(void);
	virtual ~ioLocalSingapore(void);
};

#endif // __ioLocalSingapore_h__