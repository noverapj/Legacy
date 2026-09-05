

#ifndef _ioStringInterface_h_
#define _ioStringInterface_h_

// original version : OgreStringInterface

#include "io3DCommon.h"

class __EX ioParameterDef
{
public:
	ioHashString  m_Name;
	ParameterType m_eParamType;

public:
	ioParameterDef( const char *szName, ParameterType eType ) 
		:m_Name(szName), m_eParamType(eType) 
	{

	}
};

typedef std::vector< ioParameterDef > ParameterList;

class __EX ioParamCommand
{
public:
	virtual bool IsSkipSave( const void *pTarget ) const { return false; }
	virtual std::string Get( const void *pTarget ) const = 0;
	virtual void Set( void *pTarget, const std::string &kVal ) = 0;
};

typedef std::map< ioHashString, ioParamCommand* > ParamCommandMap;

class __EX ioParamDictionary
{
	friend class ioStringInterface;
protected:
	ParameterList	m_ParamDefList;
	ParamCommandMap	m_ParamCommandMap;

	ioParamCommand* GetParamCommand( const ioHashString &kName )
	{
		ParamCommandMap::iterator iter = m_ParamCommandMap.find( kName );
		if( iter != m_ParamCommandMap.end() )
		{
			return iter->second;
		}

		return NULL;
	}

	const ioParamCommand* GetParamCommand( const ioHashString &kName ) const
	{
		ParamCommandMap::const_iterator iter = m_ParamCommandMap.find( kName );
		if( iter != m_ParamCommandMap.end() )
		{
			return iter->second;
		}

		return NULL;
	}

public:
	void AddParameter( const ioParameterDef &kParamDef, ioParamCommand *pParamCmd )
	{
		m_ParamDefList.push_back( kParamDef );
		m_ParamCommandMap.insert( ParamCommandMap::value_type( kParamDef.m_Name, pParamCmd ) );
	}

	const ParameterList& GetParameters() const { return m_ParamDefList; }

public:
	ioParamDictionary(){}
};

typedef std::map< ioHashString, ioParamDictionary > ParamDictionaryMap;

class __EX ioStringInterface
{
protected:
	static ParamDictionaryMap m_ParamDictionary;

	ioHashString	m_ParamDictName;

protected:
	bool CreateParamDictionary( const ioHashString &kClassName )
	{
		m_ParamDictName = kClassName;
		if( m_ParamDictionary.find( kClassName ) == m_ParamDictionary.end() )
		{
			m_ParamDictionary.insert( ParamDictionaryMap::value_type( kClassName,
																	  ioParamDictionary() ) );

			return true;
		}

		return false;
	}

public:
	// Retrieves the parameter dictionary for this class
	ioParamDictionary* GetParamDictionary()
	{
		ParamDictionaryMap::iterator iter=m_ParamDictionary.find( m_ParamDictName );
		if( iter != m_ParamDictionary.end() )
		{
			return &( iter->second );
		}

		return NULL;
	}

	const ioParamDictionary* GetParamDictionary() const
	{
		ParamDictionaryMap::const_iterator iter = m_ParamDictionary.find( m_ParamDictName );
		if( iter != m_ParamDictionary.end() )
		{
			return &( iter->second );
		}

		return NULL;
	}

	const ParameterList& GetParameters() const
	{
		static ParameterList emptyList;

		const ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
			return pDict->GetParameters();

		return emptyList;
	}

	virtual bool SetParameter( const ioHashString &kName, const std::string &kValue );

	virtual std::string GetParameter( const ioHashString &kName ) const
	{
		const ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			const ioParamCommand *pCmd = pDict->GetParamCommand( kName );
			if( pCmd )
			{
				return pCmd->Get(this);
			}
		}

		return "";
	}

	virtual const ioParamCommand* GetParameterCmd( const ioHashString &kName ) const
	{
		const ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			return pDict->GetParamCommand( kName );
		}

		return NULL;
	}

public:
	virtual ~ioStringInterface(){}
};

#endif
