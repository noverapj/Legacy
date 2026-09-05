
#include "stdafx.h"
#include "ioResource.h"

ioResource::ioResource( const ioHashString &name )
{
	m_Name = name;
	m_iRefCnt = 0;

	m_ErasePolicy = EP_AUTOMATIC;
	m_LoadState = RLS_NOT_LOADED;
}

ioResource::~ioResource()
{
}
