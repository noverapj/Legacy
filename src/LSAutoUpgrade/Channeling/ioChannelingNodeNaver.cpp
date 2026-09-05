#include "../stdafx.h"
#include ".\ioChannelingNodeNaver.h"

ioChannelingNodeNaver::ioChannelingNodeNaver(void)
{
	
}

ioChannelingNodeNaver::~ioChannelingNodeNaver(void)
{
}

ChannelingType ioChannelingNodeNaver::GetType()
{
	return CNT_NAVER;
}

bool ioChannelingNodeNaver::IsShowDaumWindow( const char *szRootDir )
{
	return false;
}
