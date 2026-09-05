#include "StdAfx.h"
#include "ioININode.h"
#include "ioINIWriter.h"

ioININode::ioININode(void)
{
	ZeroMemory(m_fileName, sizeof(m_fileName));
	ZeroMemory(m_section, sizeof(m_section));
	ZeroMemory(m_key, sizeof(m_key));
	m_saveType	= 0;
	m_value		= 0;
}

ioININode::~ioININode(void)
{
}

bool ioININode::SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const int32 value)
{
	if(lstrlen(section) > MAX_SIZE) 
		return false;
	if(lstrlen(key) > MAX_SIZE) 
		return false;

	strcpy_s( m_fileName, sizeof( m_fileName ), fileName );
	strcpy_s( m_section, sizeof( m_section ), section );
	strcpy_s( m_key, sizeof( m_key ), key );
	m_value	= value;
	m_saveType	= 1;
	
	return true;
}

bool ioININode::SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const float value, const int floatType)
{
	if(strlen(section) > MAX_SIZE) 
		return false;
	if(strlen(key) > MAX_SIZE) 
		return false;

	strcpy_s( m_fileName, sizeof( m_fileName ), fileName );
	strcpy_s( m_section, sizeof( m_section ), section );
	strcpy_s( m_key, sizeof( m_key ), key );
	m_value	= value;
	m_saveType	= floatType;

	return true;
}

bool ioININode::SetData(const TCHAR* fileName, const TCHAR* section, const TCHAR* key, const bool value)
{
	if(strlen(section) > MAX_SIZE) 
		return false;
	if(strlen(key) > MAX_SIZE) 
		return false;

	strcpy_s( m_fileName, sizeof( m_fileName ), fileName );
	strcpy_s( m_section, sizeof( m_section ), section );
	strcpy_s( m_key, sizeof( m_key ), key );
	m_value	= value;
	m_saveType	= 2;

	return true;
}