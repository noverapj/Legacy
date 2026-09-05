#include "stdafx.h"
#include ".\ioLocalIndonesia.h"

ioLocalIndonesia::ioLocalIndonesia(void)
{
}

ioLocalIndonesia::~ioLocalIndonesia(void)
{
}

ioLocalManager::LocalType ioLocalIndonesia::GetType()
{
	return ioLocalManager::LCT_INDONESIA;
}

const char * ioLocalIndonesia::GetMonitorPassword()
{
	return "pwlskreon98";
}