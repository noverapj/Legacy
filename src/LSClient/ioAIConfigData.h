#pragma once

#include <string>

class	ioAIConfigData
{
public:
	enum AIConfigType
	{
		AI_CONFIG_ERROR,
		AI_CONFIG_NPCLIST,
		AI_CONFIG_NPCTEXT,
		AI_CONFIG_APPEARANCE,
		AI_CONFIG_REGEX,
		AI_CONFIG_SYMBOL,
		AI_CONFIG_KEYSEQ,
		AI_CONFIG_BEHAVIOR,
	};

	ioAIConfigData();
	ioAIConfigData( const ioAIConfigData& d );
	ioAIConfigData(	std::string		typestr,
					int				index,
					std::string		name,
					std::string		file,
					std::string		version );

	void	SetConfig(	std::string		typestr,
						int				index,
						std::string		name,
						std::string		file,
						std::string		version );
	~ioAIConfigData();

	ioAIConfigData& operator=( const ioAIConfigData& d );

	AIConfigType&	GetConfigType()	{	return m_Type;		   }
	int&			GetIndex()		{	return m_iIndex;	   }
	std::string&	GetName()		{	return m_szName;	   }
	std::string&	GetFile()		{	return m_szFile;	}
	std::string&	GetVersion()	{	return m_szVersion;	   }

private:
	AIConfigType	m_Type;
	int				m_iIndex;
	std::string		m_szName;
	std::string		m_szFile;
	std::string		m_szVersion;
};

