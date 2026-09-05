#include "../stdafx.h"
#include ".\iochannelingnodemgame.h"

ioChannelingNodeMgame::ioChannelingNodeMgame(void)
{
	
}

ioChannelingNodeMgame::~ioChannelingNodeMgame(void)
{
}

ChannelingType ioChannelingNodeMgame::GetType()
{
	return CNT_MGAME;
}

bool ioChannelingNodeMgame::IsShowDaumWindow( const char *szRootDir )
{
	return false;
}
