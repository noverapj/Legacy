

#include "stdafx.h"

#include "ioINIParser.h"

#include "ioStream.h"
//#include "../Window.h"
#include "../MainProcess.h"

using namespace std;

ioINIParser::ioINIParser()
{
	m_pPreList = NULL;
}

ioINIParser::~ioINIParser()
{
	Clear();
}

bool ioINIParser::ParsingFile( const char *szFileName )
{
	Clear();

	ioTextStream kStream;
	if( !kStream.OpenFile( szFileName ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioINIParser::Parsing - %s Load Failed", szFileName );
		return false;
	}
	
	ParseINI( kStream );

	return true;
}

void ioINIParser::ParseINI( ioTextStream &rkStream )
{
	std::string line, lastline;

	while( !rkStream.IsEOF() )
	{
		if( lastline.empty() || lastline.at(0) != '[' )
		{
			rkStream.GetLine( line );
		}
		else
		{
			line = lastline;
			lastline.at(0) = '\0';
		}

		if( line.length() == 0 || line.at(0) != '[' )
			continue;

		lastline = ParseTitle( line, rkStream );
	}
}

std::string ioINIParser::ParseTitle( const std::string &szTitle, ioTextStream &rkStream )
{
	string line;

	KeyList *pKeyList = new KeyList;
	if( !pKeyList )
		return line;
	
	while( !rkStream.IsEOF() )
	{
		rkStream.GetLine( line );

		if( line.length() == 0 || line.at(0) == ';' )
			continue;

		if( line.at(0) == '[' )	// New Title
			break;

		ParseKey( line, pKeyList );
	}
	
	if( !pKeyList->empty() )
	{
		string szTitleText = szTitle.substr( 1, szTitle.length() - 2 );
		
		ioStringConverter::toLowerCase( szTitleText );

		m_TitleList.insert( TitleList::value_type( ioHashString(szTitleText.c_str()), pKeyList ) );
	}
	else
	{
		delete pKeyList;
	}

	return line;
}

void ioINIParser::ParseKey( const std::string &line, KeyList *pKeyList )
{
	const StringVector &vecparams = ioStringConverter::Split( line, "=", 1 );
	if( vecparams.size() != 2 || vecparams[0].empty() )
		return;

	string szKeyName, szValue;

	string::size_type i, tSize;
	tSize = vecparams[0].length();
	for(i=tSize-1 ; i>=0 ; i-- )
	{
		if( vecparams[0].at(i) != ' ' &&
			vecparams[0].at(i) != '\t' &&
			vecparams[0].at(i) != '\r' )
		{
			szKeyName = vecparams[0].substr( 0, i+1 );
			break;
		}
	}

	tSize = vecparams[1].length();
	for(i=0 ; i<tSize ; i++ )
	{
		if( vecparams[1].at(i) != ' ' &&
			vecparams[1].at(i) != '\t' &&
			vecparams[1].at(i) != '\r' )
		{
			szValue = vecparams[1].substr( i );
			break;
		}
	}

	if( !szKeyName.empty() && !szValue.empty() )
	{
		ioStringConverter::toLowerCase( szKeyName );
		pKeyList->insert( KeyList::value_type( ioHashString(szKeyName.c_str()), szValue ) );
	}
}

void ioINIParser::Clear()
{
	TitleList::iterator iter;
	for( iter=m_TitleList.begin() ; iter!=m_TitleList.end() ; ++iter )
	{
		delete iter->second;
	}
	m_TitleList.clear();

	m_PreTitle.Clear();
	m_pPreList = NULL;
}

int ioINIParser::GetNumTotalTitle() const
{
	return m_TitleList.size();
}

int ioINIParser::GetNumTotalKey( int iTitleIdx ) const
{
	if( COMPARE( iTitleIdx, 0, GetNumTotalTitle() ) )
	{
		TitleList::const_iterator iter = m_TitleList.begin();
		std::advance( iter, iTitleIdx );
		return iter->second->size();
	}

	return 0;
}

const char* ioINIParser::GetTitle( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetNumTotalTitle() ) )
	{
		TitleList::const_iterator iter = m_TitleList.begin();
		std::advance( iter, iIdx );
		return iter->first.c_str();
	}

	return "";
}

const char* ioINIParser::GetKey( int iTitle, int iKey ) const
{
	if( COMPARE( iTitle, 0, GetNumTotalTitle() ) )
	{
		TitleList::const_iterator iter = m_TitleList.begin();
		std::advance( iter, iTitle );

		int iNumKey = iter->second->size();
		if( COMPARE( iKey, 0, iNumKey ) )
		{
			KeyList::const_iterator iKeyIter = iter->second->begin();
			std::advance( iKeyIter, iKey );
			return iKeyIter->first.c_str();
		}
	}

	return "";
}

const char* ioINIParser::GetValue( int iTitle, int iKey ) const
{
	const char *pTitle = GetTitle( iTitle );
	const char *pKey = GetKey( iTitle, iKey );

	return GetValue( pTitle, pKey );
}

const char* ioINIParser::GetValue( const char *szTitle, const char *szKeyName ) const
{
	char szLowTitle[MAX_PATH];
	strcpy_s( szLowTitle, szTitle );
	_strlwr_s( szLowTitle, sizeof(szLowTitle) );
	m_szTitleBuf = szLowTitle;

	KeyList *pCurList = NULL;
	if( m_pPreList && m_PreTitle == m_szTitleBuf )
	{
		pCurList = m_pPreList;
	}
	else
	{
		TitleList::const_iterator iter = m_TitleList.find( m_szTitleBuf );
		if( iter != m_TitleList.end() )
		{
			pCurList = iter->second;

			m_PreTitle = m_szTitleBuf;
			m_pPreList = pCurList;
		}
	}

	if( pCurList )
	{
		char szLowKey[MAX_PATH];
		strcpy_s( szLowKey, szKeyName );
		_strlwr_s( szLowKey, sizeof(szLowKey) );
		m_szKeyNameBuf = szLowKey;

		KeyList::const_iterator iKey = pCurList->find( m_szKeyNameBuf );
		if( iKey != pCurList->end() )
			return iKey->second.c_str();
	}

	return NULL;
}
