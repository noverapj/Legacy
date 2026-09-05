#include "../stdafx.h"
#include ".\iochannelingnodeparent.h"
#include "../Local/ioLocalManager.h"

ioChannelingNodeParent::ioChannelingNodeParent(void)
{
}

ioChannelingNodeParent::~ioChannelingNodeParent(void)
{
}

ChannelingType ioChannelingNodeParent::GetType()
{
	return CNT_NONE;
}
