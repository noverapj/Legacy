#include "../stdafx.h"
#include ".\iochannelingnodeBuddy.h"

ioChannelingNodeBuddy::ioChannelingNodeBuddy(void)
{
	
}

ioChannelingNodeBuddy::~ioChannelingNodeBuddy(void)
{
}

ChannelingType ioChannelingNodeBuddy::GetType()
{
	return CNT_BUDDY;
}

bool ioChannelingNodeBuddy::IsShowDaumWindow( const char *szRootDir )
{
	return false;
}
