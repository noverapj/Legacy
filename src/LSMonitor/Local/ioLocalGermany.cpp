#include "stdafx.h"
#include ".\ioLocalGermany.h"

ioLocalGermany::ioLocalGermany(void)
{
}

ioLocalGermany::~ioLocalGermany(void)
{
}

ioLocalManager::LocalType ioLocalGermany::GetType()
{
	return ioLocalManager::LCT_GERMANY;
}

const char * ioLocalGermany::GetMonitorPassword()
{
	return "lsogppw09";
}