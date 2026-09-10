#include "stdafx.h"
#include "ioAIConfigData.h"

#include <string.h>

ioAIConfigData::ioAIConfigData() 
	: m_Type( AI_CONFIG_ERROR )	
{
}

ioAIConfigData::ioAIConfigData( const ioAIConfigData& d )
{
	this->m_Type			= d.m_Type;
	this->m_iIndex		= d.m_iIndex;
	this->m_szName		= d.m_szName;
	this->m_szFile		= d.m_szFile;
	this->m_szVersion	= d.m_szVersion;
}

ioAIConfigData::ioAIConfigData(	std::string		typestr,
								int				index,
								std::string		name,
								std::string		file,
								std::string		version )
{
	SetConfig( typestr, index, name, file, version );
}

void	ioAIConfigData::SetConfig(	std::string		typestr,
									int				index,
									std::string		name,
									std::string		file,
									std::string		version )
{
	// at this time, no symbol db was loaded.
	// so guide it manually
	if( typestr == "NPCDB" )				m_Type = AI_CONFIG_NPCLIST;
	else if( typestr == "NPCTEXTDB" )		m_Type = AI_CONFIG_NPCTEXT;
	else if( typestr == "APPEARANCEDB" )	m_Type = AI_CONFIG_APPEARANCE;
	else if( typestr == "REGEXDB" )			m_Type = AI_CONFIG_REGEX;
	else if( typestr == "SYMBOLDB" )		m_Type = AI_CONFIG_SYMBOL;
	else if( typestr == "KEYSEQUENCEDB" )	m_Type = AI_CONFIG_KEYSEQ;
	else if( typestr == "BEHAVIORTREEDB" )	m_Type = AI_CONFIG_BEHAVIOR;
	else									m_Type = AI_CONFIG_ERROR;

	m_iIndex		= index;
	m_szName		= name;
	m_szFile		= file;
	m_szVersion		= version;
}

ioAIConfigData::~ioAIConfigData()	
{
}

ioAIConfigData& ioAIConfigData::operator=( const ioAIConfigData& d )
{
	this->m_Type			= d.m_Type;
	this->m_iIndex		= d.m_iIndex;
	this->m_szName		= d.m_szName;
	this->m_szFile		= d.m_szFile;
	this->m_szVersion	= d.m_szVersion;

	return *this;
}
