#include "stdafx.h"
#include "ioAISymbolTable.h"
#include "ioAIMyselfExecElem.h"
#include "ioAITargetExecElem.h"
#include "stringext.h"

#include "ioAIXMLDoc.h"

#include "ioAIVersions.h"

template<> ioAISymbolTable* Singleton< ioAISymbolTable >::ms_Singleton = 0;

ioAISymbolTable::ioAISymbolTable()
{
}

ioAISymbolTable::~ioAISymbolTable()
{
	for( ConfigList::iterator	it = m_vConfigs.begin(); it != m_vConfigs.end(); it++ )
	{
		SAFEDELETE( *it );
	}
}

void	ioAISymbolTable::Init()
{
	// the order of loading sequence should be perserved
	LoadConfiguration( "xml/ai/npc_ai_configuration.xml" );

	InitSymbolTable();
	InitRegExTable();	// regex generation uses symbol table. so keep this order always.
}

bool	ioAISymbolTable::LoadConfiguration( const std::string& configfile )
{
	ioAIXMLDoc		doc;

	if( doc.Load( configfile.c_str(), g_szAIConfigVersion, true ) )
	{
		if( !doc.GetRootElement().IsEmpty() )
		{
			ioXMLElement*	child = &(doc.GetRootElement().FirstChild());
			while( !child->IsEmpty() )
			{
				// all symbol matchings will use string comparison
				// since no symbol table was been built yet
				ioAIConfigData*	cd = new ioAIConfigData(	child->GetTagName(),
															child->GetIntAttribute_e( "index" ),
															child->GetStringAttribute_e( "name" ),
															child->GetStringAttribute_e( "file" ),
															child->GetStringAttribute_e( "version" ) );
				
				m_vConfigs.push_back( cd );


				child = &(child->NextSibling());
			}
			return true;
		}
	}

	LOG.PrintTimeAndLog( 0, "[AI] FATAL ERROR : Failed to load ai configuration file [%s].",
									configfile.c_str() );

	return false;
}

void	ioAISymbolTable::InitRegExTable()
{
	for( ConfigList::iterator  it = m_vConfigs.begin(); it != m_vConfigs.end(); it++ )
	{
		if( (*it)->GetConfigType() == ioAIConfigData::AI_CONFIG_REGEX )
		{
			if( !LoadRegExFile( (*it)->GetFile(), (*it)->GetVersion() ) )
			{
				LOG.PrintTimeAndLog( 0, "Failed to load reg ex from [%s]", (*it)->GetFile().c_str() );
			}
		}
	}
}

bool	ioAISymbolTable::LoadRegExFile( const std::string& filename, const std::string& version )
{
	ioAIXMLDoc		doc;

	if( doc.Load( filename.c_str(), version, true ) )
	{
		if( !doc.GetRootElement().IsEmpty() )
		{
			ioXMLElement*	child = &(doc.GetRootElement().FirstChild());
			while( !child->IsEmpty() )
			{
				if( std::string( child->GetTagName() ) == "REGEX" &&
					IsClientSymbol( child->GetIntAttribute_e( "loadtype" ) ) )
				{
					switch( child->GetIntAttribute_e( "buildtype" ) )
					{
					case 0:		
						SetSymbolTable( child->GetStringAttribute_e( "name" ), 
										child->GetStringAttribute_e( "regstr" ) );
						break;
					case 1:		
						BuildRegEx( child );	
						break;
					}
				}

				child = &(child->NextSibling());
			}
			return true;
		}
	}
	return false;
}

void	ioAISymbolTable::InitSymbolTable()
{
	for( ConfigList::iterator  it = m_vConfigs.begin(); it != m_vConfigs.end(); it++ )
	{
		if( (*it)->GetConfigType() == ioAIConfigData::AI_CONFIG_SYMBOL )
		{
			if( !LoadSymbolTableFile( (*it)->GetFile(), (*it)->GetVersion() ) )
			{
				LOG.PrintTimeAndLog( 0, "Failed to load symbol table from [%s]", (*it)->GetFile().c_str() );
			}
		}
	}
}

bool	ioAISymbolTable::LoadSymbolTableFile( const std::string& filename, const std::string& version )
{
	ioAIXMLDoc		doc;

	if(	doc.Load( filename.c_str(), version, true ) )
	{
		if( !doc.GetRootElement().IsEmpty() )
		{
			ioXMLElement*	listelem = &(doc.GetRootElement().FirstChild());
			while( !listelem->IsEmpty() )
			{
				ioXMLElement*	elem = &(listelem->FirstChild());
				while( !elem->IsEmpty() ) 
				{
					int	loadtype = elem->GetIntAttribute_e( "loadtype" );
					if( IsClientSymbol( loadtype ) )	
					{
						std::string sec, key;
						int			val;

						sec.clear();
						key.clear();
						if( std::string( listelem->GetTagName() ) == "SYMBOLTYPE_LIST" && 
							std::string( elem->GetTagName() ) == "SYMBOL_TYPE" )
						{
							sec	= elem->GetStringAttribute_e( "location" );
							key	= elem->GetStringAttribute_e( "enumname" );
							val	= elem->GetIntAttribute_e( "value" );
						}
						else if(	std::string( listelem->GetTagName() ) == "SYMBOLLIST" && 
									std::string( elem->GetTagName() ) == "SYMBOL" )
						{
							sec	= elem->GetStringAttribute_e( "client_enum" );
							key	= elem->GetStringAttribute( "str" );
							val	= elem->GetIntAttribute_e( "client_val" );
						
						}

						SetSymbolTable( sec, key, val, IsForceIntSet( loadtype ) );
					}
					elem = &(elem->NextSibling());
				}
				listelem = &(listelem->NextSibling());
			}
			return true;
		}
	}
	return false;
}

void	ioAISymbolTable::BuildRegEx( ioXMLElement* node )
{
	if( node )
	{
		std::string name		= node->GetStringAttribute_e( "name" );
		std::string elemstr		= node->GetStringAttribute_e( "buildelem" );
		std::string targetstr	= node->GetStringAttribute_e( "buildtarget" );

		std::vector<std::string>	tags;

		if( elemstr == "-9999" ) 	// load all
		{
			for( InvSymTable::iterator 
					it = g_AISymbolTable.GetInvSymTable( targetstr ).begin();
					it != g_AISymbolTable.GetInvSymTable( targetstr ).end();
					it++ )
			{
				tags.push_back( it->second );
			}
		}
		else 
		{
			std::vector<std::string> elemlist;
			elemlist = explode( elemstr, "," );
			for( std::vector<std::string>::iterator it = elemlist.begin(); it != elemlist.end(); it++ )
			{
				tags.push_back( g_AISymbolTable.GetSymbolKey( targetstr, ::atoi( (*it).c_str() ) ) );
			}
		}

		SetSymbolTable( name, "(" + implode( tags, "|" ) + ")" );
	}
}

//////////////////////////////////////////////////////////////////////////////////
int	ioAISymbolTable::GetSymbolValue( const std::string& sec, const std::string& key )
{
	std::string newsec = sec; 
	std::string newkey = key; 

	newsec = tolower(trim(newsec));
	newkey = tolower(trim(newkey));

	SymbolMap::iterator iter = m_mapAISymbolMap.find( newsec );

	if( iter != m_mapAISymbolMap.end() )
	{
		SymbolTable::iterator it = iter->second.find( newkey );
		if( it != m_mapAISymbolMap[newsec].end() )
		{
			return m_mapAISymbolMap[newsec][newkey].n;		
		}
	}
	
	LOG.PrintTimeAndLog( 0, "Symbol not found in AI Symbol table in section [%s] with GetSymbolValue: [%s]", 
									newsec.c_str(), newkey.c_str() );
	return 0;
}


CharState ioAISymbolTable::GetCharStateType( const std::string& key )
{	
	return (CharState)GetSymbolValue( std::string("CharState"), key );
}

AIDatabaseType ioAISymbolTable::GetDatabaseType( const std::string& key )
{	
	return (AIDatabaseType)GetSymbolValue( std::string("AIDatabaseType"), key );
}

AIBvTNodeType ioAISymbolTable::GetBvTNodeType( const std::string& key )
{	
	return (AIBvTNodeType)GetSymbolValue( std::string("AIBvTNodeType"), key );
}

AIBvTHaltType ioAISymbolTable::GetBvTHaltType( const std::string& key )
{	
	return (AIBvTHaltType)GetSymbolValue( std::string("AIBvTHaltType"), key );
}

AIBvTRoleType ioAISymbolTable::GetBvTRoleType( const std::string& key )
{	
	return (AIBvTRoleType)GetSymbolValue( std::string("AIBvTRoleType"), key );
}

AIConfigType ioAISymbolTable::GetConfigurationType( const std::string& key )
{	
	return (AIConfigType)GetSymbolValue( std::string("AIConfigType"), key );
}

AIBrainElemType ioAISymbolTable::GetBrainElemType( const std::string& key )
{	
	return (AIBrainElemType)GetSymbolValue( std::string("AIBrainElemType"), key );
}

AIDefElemType ioAISymbolTable::GetDefElemType( const std::string& key )
{	
	return (AIDefElemType)GetSymbolValue( std::string("AIDefElemType"), key );
}

AIKeyInputType ioAISymbolTable::GetKeyInputType( const std::string& key )
{	
	return (AIKeyInputType)GetSymbolValue( std::string("AIKeyInputType"), key );
}

AIKeyStateType ioAISymbolTable::GetKeyStateType( const std::string& key )
{	
	return (AIKeyStateType)GetSymbolValue( std::string("AIKeyStateType"), key );
}

AIExecVariableType ioAISymbolTable::GetExecVariableType( const std::string& key )
{	
	return (AIExecVariableType)GetSymbolValue( std::string("AIExecVariableType"), key );
}

AIExecOperatorType ioAISymbolTable::GetExecOperatorType( const std::string& key )
{	
	return (AIExecOperatorType)GetSymbolValue( std::string("AIExecOperatorType"), key );
}

AIExecMethodType ioAISymbolTable::GetExecMethodType( const std::string& key )
{	
	return (AIExecMethodType)GetSymbolValue( std::string("AIExecMethodType"), key );
}

AICompareType ioAISymbolTable::GetCompareType( const std::string& key )
{	
	return (AICompareType)GetSymbolValue( std::string("AICompareType"), key );
}

int	ioAISymbolTable::GetSymbolValue( const std::string& key )
{	
	return GetSymbolValue( std::string("default"), key );
}

std::string	ioAISymbolTable::GetSymbolString( const std::string& key )
{	
	std::string newkey = key;
	
	newkey = tolower(newkey);
	StringTable::iterator it = m_mapAIStringMap.find( newkey );
	if( it == m_mapAIStringMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "Symbol not found in AI Symbol table GetSymbolValue: %s", newkey.c_str() );
		return 0;
	}
	return m_mapAIStringMap[newkey];		
}


/////////////////////////////////////////////////////////
std::string	ioAISymbolTable::GetSymbolKey( const std::string& sec, const int& value )
{
	std::string newsec = sec; 

	newsec = tolower(trim(newsec));

	InvSymMap::iterator iter = m_mapAIInvSymMap.find( newsec );

	if( iter != m_mapAIInvSymMap.end() )
	{
		InvSymTable::iterator it = iter->second.find( value );
		if( it != m_mapAIInvSymMap[newsec].end() )
		{
			return m_mapAIInvSymMap[newsec][value];		
		}
	}
	
	LOG.PrintTimeAndLog( 0, "Key not found in AI Symbol table in section [%s] with GetSymbolValue: [%d]", 
									newsec.c_str(), value );
	return 0;
}

std::string ioAISymbolTable::GetSymbolKey( const CharState&	val )
{
	return GetSymbolKey("CharState", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIDatabaseType& val )
{
	return GetSymbolKey("AIDatabaseType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIBvTNodeType& val )
{
	return GetSymbolKey("AIBvTNodeType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIBvTHaltType& val )
{
	return GetSymbolKey("AIBvTHaltType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIBvTRoleType& val )
{
	return GetSymbolKey("AIBvTRoleType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIKeyInputType& val )
{
	return GetSymbolKey("AIKeyInputType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIKeyStateType& val )
{
	return GetSymbolKey("AIKeyStateType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIBrainElemType& val )
{
	return GetSymbolKey("AIBrainElemType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIDefElemType& val )
{
	return GetSymbolKey("AIDefElemType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIConfigType& val )
{
	return GetSymbolKey("AIConfigType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIExecVariableType& val )
{
	return GetSymbolKey("AIExecVariableType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIExecOperatorType& val )
{
	return GetSymbolKey("AIExecOperatorType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AIExecMethodType& val )
{
	return GetSymbolKey("AIExecMethodType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const AICompareType& val )
{
	return GetSymbolKey("AICompareType", val );	
}

std::string ioAISymbolTable::GetSymbolKey( const int& val )
{
	return GetSymbolKey("default", val );	
}

//////////////////////////////////////////////////////////////////////////////////
void ioAISymbolTable::SetSymbolTable( const std::string& sec, const std::string& key, const int& val, bool force  )
{
	std::string		newsec	= sec;
	std::string		newkey	= key;

	newsec = tolower(trim(newsec));
	newkey = tolower(trim(newkey));

	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		if( force )
		{
			m_mapAISymbolMap[newsec][newkey].n	= val;
			m_mapAIInvSymMap[newsec][val]		= key;		
		}
		else
		{
			int	enumtypeval = GetSymbolValue( std::string("ClientEnumTypes"), newsec );

			m_mapAIInvSymMap[newsec][val]		= key;

			switch( enumtypeval )
			{
			case 1:		m_mapAISymbolMap[newsec][newkey].det	= (AIDefElemType)val;		break;
			case 2:		m_mapAISymbolMap[newsec][newkey].bet	= (AIBrainElemType)val;		break;
			case 3:		m_mapAISymbolMap[newsec][newkey].dt		= (AIDatabaseType)val;		break;
			case 4:		m_mapAISymbolMap[newsec][newkey].btnt	= (AIBvTNodeType)val;		break;
			case 5:		m_mapAISymbolMap[newsec][newkey].btht	= (AIBvTHaltType)val;		break;
			case 6:		m_mapAISymbolMap[newsec][newkey].btrt	= (AIBvTRoleType)val;		break;
			case 7:		m_mapAISymbolMap[newsec][newkey].kit	= (AIKeyInputType)val;		break;
			case 8:		m_mapAISymbolMap[newsec][newkey].kst	= (AIKeyStateType)val;		break;
			case 9:		m_mapAISymbolMap[newsec][newkey].ct		= (AIConfigType)val;		break;
			case 10:	m_mapAISymbolMap[newsec][newkey].cmpt	= (AICompareType)val;		break;
			case 11:	m_mapAISymbolMap[newsec][newkey].avt	= (AIExecVariableType)val;	break;
			case 12:	m_mapAISymbolMap[newsec][newkey].aot	= (AIExecOperatorType)val;	break;
			case 13:	m_mapAISymbolMap[newsec][newkey].amt	= (AIExecMethodType)val;	break;
			case 14:	m_mapAISymbolMap[newsec][newkey].cs		= (CharState)val;			break;
			default:	m_mapAISymbolMap[newsec][newkey].n		= val;

				LOG.PrintTimeAndLog( 0, "No predefined enum type named %s : assuming as int value %d, on key %s", 
										sec.c_str(), val, key.c_str() );
				break;
			}
		}
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const CharState&	val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("CharState"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].cs	= val;
		m_mapAIInvSymMap[newsec][val]		= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIDatabaseType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIDatabaseType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].dt	= val;
		m_mapAIInvSymMap[newsec][val]		= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIBvTNodeType&	val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIBvTNodeType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].btnt	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIBvTHaltType&	val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIBvTHaltType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].btht	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIBvTRoleType&	val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIBvTRoleType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].btrt	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIKeyInputType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIKeyInputType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].kit	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIKeyStateType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIKeyStateType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].kst	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIBrainElemType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIBrainElemType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].bet	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIDefElemType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIDefElemType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].det	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIConfigType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIConfigType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].ct	= val;
		m_mapAIInvSymMap[newsec][val]		= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIExecVariableType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIExecVariableType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].avt	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIExecOperatorType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIExecOperatorType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].aot	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AIExecMethodType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AIExecMethodType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].amt	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const AICompareType& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("AICompareType"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].cmpt	= val;
		m_mapAIInvSymMap[newsec][val]			= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const int& val )
{
	std::string		newkey	= key;
	std::string		newsec	= tolower(std::string("default"));
	
	newkey = tolower(trim(newkey));
	if( !IsDuplicatedSymbolValue( newsec, newkey ) )
	{
		m_mapAISymbolMap[newsec][newkey].n	= val;
		m_mapAIInvSymMap[newsec][val]		= key;
	}
}

void ioAISymbolTable::SetSymbolTable( const std::string& key, const std::string& val )
{
	std::string		newkey = key;
	newkey = tolower(trim(newkey));

	StringTable::iterator it = m_mapAIStringMap.find( newkey );
	if( it != m_mapAIStringMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "Duplicated symbol found in AI Symbol table (ignore new one): %s", newkey.c_str() );
	}
	else m_mapAIStringMap[newkey] = val;
}

bool ioAISymbolTable::IsDuplicatedSymbolValue( const std::string& sec, const std::string& key )
{
	std::string		newsec = sec;
	std::string		newkey = key;

	newsec = tolower( trim( newsec ) );
	newkey = tolower( trim( newkey ) );

	SymbolMap::iterator iter = m_mapAISymbolMap.find( newsec );
	
	if( iter != m_mapAISymbolMap.end() )
	{
		SymbolTable::iterator it = iter->second.find( newkey );
		if( it != m_mapAISymbolMap[newsec].end() )
		{
			LOG.PrintTimeAndLog( 0, "Duplicated symbol found in AI Symbol value table %s (ignore new one): key = %s", 
									newsec.c_str(), newkey.c_str() );
			return true;
		}
	}

	return false;
}

bool ioAISymbolTable::IsDuplicatedSymbolString( const std::string& key )
{
	std::string		newkey = key;

	newkey = tolower( trim( newkey ) );
	StringTable::iterator it = m_mapAIStringMap.find( newkey );
	if( it != m_mapAIStringMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "Duplicated symbol found in AI Symbol String table (ignore new one): key = %s", 
								newkey.c_str() );
		return true;
	}

	return false;
}

bool	ioAISymbolTable::IsClientSymbol( const int& loadtype )
{
	//	0	:	do not load
	//	100	:	client, type-check loading
	//	101	:	client, int loading (no warning)
	//	200	:	editor, type-check loading
	//	201	:	editor, int loading (no warning)
	//	300	:	client & editor, type-check loading
	//	301	:	client & editor, int loading (no warning)

	return ((( loadtype / 100 ) % 2) == 1);
}

bool	ioAISymbolTable::IsForceIntSet( const int& loadtype )
{
	return (( loadtype % 100 ) == 1 );
}

ioAISymbolTable::SymbolTable&	ioAISymbolTable::GetSymbolTable( const std::string& sect )
{
	std::string		newsect = sect;

	newsect = tolower( trim( newsect ) );

	return m_mapAISymbolMap[newsect];
}

ioAISymbolTable::InvSymTable&	ioAISymbolTable::GetInvSymTable( const std::string& sect )
{
	std::string		newsect = sect;

	newsect = tolower( trim( newsect ) );

	return m_mapAIInvSymMap[newsect];
}

ioAISymbolTable::SymbolMap&		ioAISymbolTable::GetSymbolMap()
{
	return m_mapAISymbolMap;
}

ioAISymbolTable::InvSymMap&		ioAISymbolTable::GetInvSymMap()
{
	return m_mapAIInvSymMap;
}

ioAISymbolTable::ConfigList&	ioAISymbolTable::GetConfigList()
{
	return m_vConfigs;
}

/////////////////////////////////////////////////////////////////////////
ioAISymbolTable& ioAISymbolTable::GetSingleton()
{
	return Singleton< ioAISymbolTable >::GetSingleton();
}

