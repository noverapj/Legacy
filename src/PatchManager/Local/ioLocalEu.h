#pragma once

#include "ioLocalParent.h"


class ioLocalEU : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalEU(void);
	virtual ~ioLocalEU(void);
};

