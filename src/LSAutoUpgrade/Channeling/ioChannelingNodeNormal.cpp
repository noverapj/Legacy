#include "../stdafx.h"
#include ".\iochannelingnodenormal.h"


ioChannelingNodeNormal::ioChannelingNodeNormal(void)
{
}

ioChannelingNodeNormal::~ioChannelingNodeNormal(void)
{
}

ChannelingType ioChannelingNodeNormal::GetType()
{
	return CNT_NORMAL;
}

bool ioChannelingNodeNormal::IsShowDaumWindow( const char *szRootDir )
{
	return false;
}
