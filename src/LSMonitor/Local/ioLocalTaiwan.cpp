#include "stdafx.h"
#include ".\iolocaltaiwan.h"

ioLocalTaiwan::ioLocalTaiwan(void)
{
}

ioLocalTaiwan::~ioLocalTaiwan(void)
{
}

ioLocalManager::LocalType ioLocalTaiwan::GetType()
{
	return ioLocalManager::LCT_TAIWAN;
}

const char * ioLocalTaiwan::GetMonitorPassword()
{
	return "gamonsterpw1587";
}