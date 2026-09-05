#ifndef __ioLocalChina_h__
#define __ioLocalChina_h__

#include "ioLocalParent.h"

class ioLocalChina : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalChina(void);
	virtual ~ioLocalChina(void);
};

#endif // __ioLocalChina_h__