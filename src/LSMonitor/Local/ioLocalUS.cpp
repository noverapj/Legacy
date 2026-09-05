#include "stdafx.h"
#include ".\iolocalus.h"

ioLocalUS::ioLocalUS(void)
{
}

ioLocalUS::~ioLocalUS(void)
{
}

ioLocalManager::LocalType ioLocalUS::GetType()
{
	return ioLocalManager::LCT_US;
}

const char * ioLocalUS::GetMonitorPassword()
{
	return "lsogppw09";
}