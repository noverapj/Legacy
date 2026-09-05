#pragma once

#include "ioLocalParent.h"

class ioLocalLatin : public ioLocalParent
{
public:
	virtual ioLocalManager::LocalType GetType();
	virtual void GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType = 0 );

public:
	ioLocalLatin(void);
	virtual ~ioLocalLatin(void);
};

