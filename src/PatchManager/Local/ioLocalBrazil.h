#ifndef __ioLocalBrazil_h__
#define __ioLocalBrazil_h__

#include "ioLocalParent.h"

class ioLocalBrazil : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalBrazil(void);
	virtual ~ioLocalBrazil(void);
};

#endif // __ioLocalBrazil_h__