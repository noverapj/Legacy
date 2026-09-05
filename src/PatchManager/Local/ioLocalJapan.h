#ifndef __ioLocalJapan_h__
#define __ioLocalJapan_h__

#include "ioLocalParent.h"

class ioLocalJapan : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalJapan(void);
	virtual ~ioLocalJapan(void);
};

#endif // __ioLocalJapan_h__