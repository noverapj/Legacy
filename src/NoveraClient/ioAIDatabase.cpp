#include "stdafx.h"
#include "ioAISymbolTable.h"
#include "ioAIDatabase.h"
#include "ioAIBehaviorTree.h"
#include "ioNpcChar.h"

#include <string>
#include <regex>

#include "stringext.h"

template<> ioAIDatabase* Singleton< ioAIDatabase >::ms_Singleton = 0;

ioAIDatabase::ioAIDatabase()
	:m_pAppearanceDB( (ioAIXMLDoc*)0 )
{
	BuildAIDatabase();
}

ioAIDatabase::~ioAIDatabase()
{
	ClearDatabase();
}

void	ioAIDatabase::ClearDatabase()
{
	for( XMLList::iterator it = m_vBehaviorDB.begin(); it != m_vBehaviorDB.end(); it ++ )
	{
		SAFEDELETE( *it );
	}
	m_vBehaviorDB.clear();

	for( XMLList::iterator it = m_vKeySeqDefs.begin(); it != m_vKeySeqDefs.end(); it++ )
	{
		SAFEDELETE( *it );
	}
	m_vKeySeqDefs.clear();

	if( m_pAppearanceDB )
		SAFEDELETE( m_pAppearanceDB );
}

void	ioAIDatabase::BuildAIDatabase()
{
	ClearDatabase();

	for( ioAISymbolTable::ConfigList::iterator 
				it = g_AISymbolTable.GetConfigList().begin();
				it != g_AISymbolTable.GetConfigList().end();
				it++ )
	{
		switch( (*it)->GetConfigType() )
		{
		case ioAIConfigData::AI_CONFIG_NPCTEXT:
			BuildTextItemDB( (*it)->GetFile(), (*it)->GetVersion() );		
			break;

		case ioAIConfigData::AI_CONFIG_APPEARANCE:
			BuildAppearanceDB( (*it)->GetFile(), (*it)->GetVersion() );		
			break;

		case ioAIConfigData::AI_CONFIG_KEYSEQ:
			BuildKeySequenceDB( (*it)->GetFile(), (*it)->GetVersion() );		
			break;

		case ioAIConfigData::AI_CONFIG_BEHAVIOR:
			BuildBehaviorTreeDB( (*it)->GetFile(), (*it)->GetVersion() );		
			break;
		}
	}
}

void	ioAIDatabase::BuildKeySequenceDB( const std::string& filename, const std::string& ver )
{
	ioAIXMLDoc*	xmlFile = new ioAIXMLDoc;

	if( xmlFile->Load( filename.c_str(), ver ) )
	{
		if( !xmlFile->GetRootElement().IsEmpty() )
		{
			m_vKeySeqDefs.push_back(xmlFile);
		}
		else SAFEDELETE( xmlFile );
	}
	else LOG.PrintTimeAndLog(0, "[AI] ERROR : failed to load key seq db : %s", filename.c_str() );
}

void	ioAIDatabase::BuildBehaviorTreeDB( const std::string& filename, const std::string& ver )
{
	ioAIXMLDoc	*xmlFile = new ioAIXMLDoc;

	if( xmlFile->Load( filename.c_str(), ver ) )
	{
		if( !xmlFile->GetRootElement().IsEmpty() )
		{
			m_vBehaviorDB.push_back(xmlFile);
		}
		else SAFEDELETE( xmlFile );
	}
	else LOG.PrintTimeAndLog(0, "[AI] ERROR : failed to load behavior db : %s", filename.c_str() );
}

void	ioAIDatabase::BuildTextItemDB( const std::string& filename, const std::string& version )
{
	ioAIXMLDoc		doc;

	if( doc.Load( filename.c_str(), version, true ) )
	{
		if( !doc.GetRootElement().IsEmpty() )
		{
			ioXMLElement*	group = &(doc.GetRootElement().FirstChild());
			while( !group->IsEmpty() )
			{
				if( std::string( group->GetTagName() ) == "TEXTGROUP" )
				{
					ioXMLElement*	child = &(group->FirstChild());
					while( !child->IsEmpty() )
					{
						if( std::string( child->GetTagName() ) == "TEXTITEM" )
						{
							m_mapTextItem	[group->GetStringAttribute_e( "groupname" )]
											[child->GetStringAttribute_e( "name" )] 
										= 	child->GetStringAttribute_e( "text" );
						}
						else LOG.PrintTimeAndLog( 0 , "Text item BD : This is not text item [%s]", child->GetTagName() );
						child = &(child->NextSibling());
					}
				}
				else LOG.PrintTimeAndLog( 0 , "Text item BD : This is not text group [%s]", group->GetTagName() );
				group = &(group->NextSibling());
			}
		}
		else LOG.PrintTimeAndLog( 0 , "Text Item DB load failed : cannot find root node" );
	}
	else LOG.PrintTimeAndLog( 0 , "Text Item DB load failed : no such file" );
}

void	ioAIDatabase::BuildAppearanceDB( const std::string& filename, const std::string& ver )
{
	ioAIXMLDoc	*xmlFile = new ioAIXMLDoc;

	if( xmlFile->Load( filename.c_str(), ver ) )
	{
		if( !xmlFile->GetRootElement().IsEmpty() )
		{
			if( m_pAppearanceDB )
				SAFEDELETE( m_pAppearanceDB );
			
			m_pAppearanceDB = xmlFile;
		}
		else SAFEDELETE( xmlFile );
	}
	else LOG.PrintTimeAndLog(0, "[AI] ERROR : failed to load appearancee db : %s", filename.c_str() );
}

void	ioAIDatabase::CopyKeySequence( 
							const std::string&	keySeqName, 
				std::vector<ioAIKeyBehavior>&	keySeq,
							int					inputTypeOverride,
							int					stateTypeOverride,
							unsigned int		durationOverride)
{
	keySeq.clear();

	for( XMLList::iterator it = m_vKeySeqDefs.begin(); it != m_vKeySeqDefs.end(); it++ )
	{
		ioXMLElement* root = &(*it)->GetRootElement();

		while( !root->IsEmpty() )
		{
			//PrintXMLLogForBvT( root, 0 );
			AIConfigType t = g_AISymbolTable.GetConfigurationType( root->GetTagName() );
			if( t == AI_CT_KEYSEQUENCEDB )
			{
				ioXMLElement* cur = &root->FirstChild();
				while( !cur->IsEmpty() )
				{
					AIConfigType tk = g_AISymbolTable.GetConfigurationType( cur->GetTagName() );
					if( tk == AI_CT_KEYSEQUENCE )
					{
						if( cur->GetStringAttribute_e("name") == keySeqName )
						{
							ioXMLElement* curSeq = &cur->FirstChild();
							while( !curSeq->IsEmpty() )
							{
								ioAIKeyBehavior ka( curSeq );

								if( inputTypeOverride != -100 )	ka.m_inputType = (AIKeyInputType)inputTypeOverride;
								if( stateTypeOverride != -100 )	ka.m_stateType = (AIKeyStateType)stateTypeOverride;
								if( durationOverride != 0 )	ka.m_duration = (unsigned int)durationOverride;

								keySeq.push_back( ka );
								curSeq = &curSeq->NextSibling();
							}
						}
					}
					cur = &cur->NextSibling();
				}
			}
			root = &root->NextSibling();
		}
	}
}

ioAIBvT*	ioAIDatabase::GenerateBehaviorTree( const std::string&	elemName, int depth )
{
	LOG.PrintTimeAndLog( 0, "Start to generate  BvT from AIDB... " );
	LOG.PrintTimeAndLog( 0, "using element name : %s", elemName.c_str() );
	
	for( XMLList::iterator it = m_vBehaviorDB.begin(); it != m_vBehaviorDB.end(); it++ )
	{
		ioXMLElement* cur = &(*it)->GetRootElement().FirstChild();

		while( !cur->IsEmpty() )
		{
			//PrintXMLLogForBvT( cur, 0 );
			std::string	tagname = cur->GetTagName();
			AIConfigType tp = g_AISymbolTable.GetConfigurationType( tagname );
			if( tp == AI_CT_BEHAVIOR )
			{
				if( cur->GetStringAttribute_e("name") == elemName )
				{
					ioXMLElement* curSeq = &cur->FirstChild();
					LOG.PrintTimeAndLog( 0, "Start to build BvT from node :");
					LOG.PrintTimeAndLog( 0, "\tname = %s", cur->GetStringAttribute_e("name"));

					ioAIBvT* bvt = new ioAIBvT( curSeq, depth );
					if( bvt )
					{
						LOG.PrintTimeAndLog( 0, "BvT build on %x", bvt);
						if( bvt->GetRootNode() )
							bvt->GetRootNode()->SetNodeName( elemName );
					}
					else
						LOG.PrintTimeAndLog( 0, "BvT building failed : no BvT has been returned.");

					return bvt;
				}
			}
			else if( tp == AI_CT_ERROR )
			{
				LOG.PrintTimeAndLog( 0, "Tagname %s do not has approtiate type value.", tagname.c_str() );
			}
			cur = &cur->NextSibling();
		}
	}
	LOG.PrintTimeAndLog( 0, "No element found name %s", elemName.c_str() );

	return (ioAIBvT*)0;
}

std::string	ioAIDatabase::GetTextItem( const std::string& sect, const std::string& key )
{
	StringMap::iterator	it1 = m_mapTextItem.find( sect );

	if( it1 != m_mapTextItem.end() )
	{
		StringArray::iterator	it2 = it1->second.find( key );
		if( it2 != it1->second.end() )
		{
			return m_mapTextItem[sect][key];
		}
	}

	return std::string();
}

std::string	ioAIDatabase::GetRandomTextItem( const std::string& sect )
{
	StringMap::iterator	it1 = m_mapTextItem.find( sect );

	if( it1 != m_mapTextItem.end() )
	{
		IORandom	ioRand;
		
		ioRand.Randomize();

		unsigned int index = ioRand.Random( it1->second.size() );

		for( StringArray::iterator	it2 = it1->second.begin();
									it2 != it1->second.end();
									it2++ )
		{
			if( index <= 0 )
			{
				return it2->second;
			}
			else	
			{
				index--;
			}
		}
	}

	return std::string();
}

ioNpcAppearance*	ioAIDatabase::GetRandomAppearanceData()
{
	ioXMLElement*	root = &(m_pAppearanceDB->GetRootElement());
	IORandom		r;

	if( std::string( root->GetTagName() ) == "APPEARANCE" )
	{
		r.Randomize();

		int raceindex = r.Random( root->GetChildCount() );

		ioXMLElement* race = &(root->FirstChild());
		for( int i = 0; i < raceindex && !race->IsEmpty(); i++ )
		{
			race = &(race->NextSibling());
		}
		if( race && !race->IsEmpty() )
		{
			r.Randomize();
			int sexindex = r.Random( race->GetChildCount() );

			ioXMLElement* sex = &(race->FirstChild());
			for( int i = 0; i < sexindex && !sex->IsEmpty(); i++ )
			{
				sex = &(sex->NextSibling());
			}
			if( sex && !sex->IsEmpty() )
			{
				return new ioNpcAppearance(
					raceindex, 
					sexindex,
					GetRandomResult( &sex->FirstChild( "BEARD" ) ),
					GetRandomResult( &sex->FirstChild( "ACCESSORY" ) ),
					GetRandomResult( &sex->FirstChild( "FACE" ) ),
					GetRandomResult( &sex->FirstChild( "HAIR" ) ),
					GetRandomResult( &sex->FirstChild( "SKINCOLOR" ) ),
					GetRandomResult( &sex->FirstChild( "HAIRCOLOR" ) ),
					GetRandomResult( &sex->FirstChild( "UNDERWEAR" ) ),
					std::string( sex->GetStringAttribute_e( "model" ) ),
					std::string( sex->GetStringAttribute_e( "raceinfo" ) )
					);
			}
			else LOG.PrintTimeAndLog( 0, "No valid <SEX> node in given xml file with raceindex %d, sexindex %d.", raceindex, sexindex );
		}
		else LOG.PrintTimeAndLog( 0, "No valid <RACE> node in given xml file with raceindex %d.", raceindex );
	}
	else LOG.PrintTimeAndLog( 0, "No valid <APPEARANCE> node in given xml file." );

	return (ioNpcAppearance*)0;
}

int	ioAIDatabase::GetRandomResult( ioXMLElement* node )
{
	if( node )
	{
		std::string		valid( node->GetStringAttribute_e( "valid" ) );

		std::vector<std::string>	vlist = explode( valid, "," );

		IORandom	r;

		r.Randomize();

		int index = r.Random( vlist.size() );

		std::string	valstr = vlist[r.Random( vlist.size() )];

		valstr = trim( valstr );

		return ::atoi( valstr.c_str() );
	}
	return 1;	// basic 
}

ioAIDatabase& ioAIDatabase::GetSingleton()
{
	return Singleton< ioAIDatabase >::GetSingleton();
}
