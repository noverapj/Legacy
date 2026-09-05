#include "stdafx.h"
#include ".\iolocalThailand.h"

ioLocalThailand::ioLocalThailand(void)
{
}

ioLocalThailand::~ioLocalThailand(void)
{
}

ioLocalManager::LocalType ioLocalThailand::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}

const char * ioLocalThailand::GetMonitorPassword()
{
	return "as741255pw";
}