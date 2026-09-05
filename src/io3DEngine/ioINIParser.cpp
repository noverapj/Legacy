#include "stdafx.h"

#include "ioINIParser.h"

#include "ioStream.h"
#include "ioStringConverter.h"

#include "ioResourceLoader.h"
#include "ioFrameTimer.h"

using namespace std;

template<> ioINIMemoryStatic* Singleton< ioINIMemoryStatic >::ms_Singleton = 0;

ioINIMemoryStatic::ioINIMemoryStatic() : m_StaticSwitch( SATICS_OFF )
{	
}

ioINIMemoryStatic::~ioINIMemoryStatic()
{
}

ioINIMemoryStatic& ioINIMemoryStatic::GetSingleton()
{
	return Singleton<ioINIMemoryStatic>::GetSingleton();
}

ioINIMemoryStatic::INISectionMem* ioINIMemoryStatic::FindSection( INIMem& Mem, const ioHashString& SectionName )
{
	if( m_StaticSwitch == SATICS_OFF )
		return NULL;

	INISectionMemMap::iterator iter = Mem.m_SectionMap.find( SectionName );
	if( iter != Mem.m_SectionMap.end() )
	{
		return &iter->second;
	}

	return NULL;
}

void ioINIMemoryStatic::AddSection( INIMem& Mem, const ioHashString& SectionName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_OFF )
		return;

	INISectionMem Section;
	Section.dwLoadTime		= FRAMEGETTIME();
	Section.dwSectionMem	= dwMem;
	Mem.m_SectionMap.insert( INISectionMemMap::value_type( SectionName, Section ) );
}

void ioINIMemoryStatic::AddMem( const ioHashString& KeyName, const ioHashString& SectionName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_OFF )
		return;

	INIMemMap::iterator iter = m_MemoryMap.find( KeyName );
	if( iter != m_MemoryMap.end() )
	{
		INIMem& Mem = iter->second;
		INISectionMem* pSection = FindSection( Mem, SectionName );
		if( pSection )
		{
			if( pSection->dwLoadTime + STAITC_ACC_REFRESH_TIME > FRAMEGETTIME() )
			{
				pSection->dwSectionMem += dwMem;
			}
			else
			{
				pSection->dwSectionMem = dwMem;
			}
		}
		else
		{
			AddSection( Mem, SectionName, dwMem );
		}
	}
	else
	{
		INIMem Mem;
		AddSection( Mem, SectionName, dwMem );
		m_MemoryMap.insert( INIMemMap::value_type( KeyName, Mem ) );
	}	
}


void ioINIMemoryStatic::GetINIMemStatics( INIMem& Mem, DWORD& dwCount, DWORD& dwSize )
{
	dwSize = 0;
	for( INISectionMemMap::iterator iter = Mem.m_SectionMap.begin(); iter != Mem.m_SectionMap.end(); ++iter )
	{
		dwSize += iter->second.dwSectionMem;		
	}
	if( 0 < dwSize )
		dwCount++;
}

void ioINIMemoryStatic::GetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( INIMemMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		DWORD dwINISize = 0;
		GetINIMemStatics( iter->second, dwCount, dwINISize );
		dwSize += dwINISize;
	}
}

void ioINIMemoryStatic::GetExceptionINIMemStatics( INIMem& Mem, DWORD& dwSize )
{
	for( INISectionMemMap::iterator iter = Mem.m_SectionMap.begin(); iter != Mem.m_SectionMap.end(); ++iter )
	{
		dwSize += iter->first.GetCapacity();
		dwSize += sizeof( iter->second );
	}	
}

void ioINIMemoryStatic::GetExceptionMemStatics( DWORD& dwSize )
{
	dwSize = 0;
	for( INIMemMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize += iter->first.GetCapacity();
		dwSize += sizeof( ioHashString );

		GetExceptionINIMemStatics( iter->second, dwSize );
	}
}

//------------------------------------------------------------------------------------------------------------------------------

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
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioINIParser::Parsing - %s Load Failed", szFileName );
		return false;
	}
	
	ParseINI( kStream );

	return true;
}

void ioINIParser::ParseINI( ioTextStream &rkStream )
{
	string line, lastline;

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
	for( i=tSize-1 ; i>=0 ; i-- )
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
	for( i=0 ; i<tSize ; i++ )
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
	strcpy( szLowTitle, szTitle );
	strlwr( szLowTitle );
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
		strcpy( szLowKey, szKeyName );
		strlwr( szLowKey );
		m_szKeyNameBuf = szLowKey;

		KeyList::const_iterator iKey = pCurList->find( m_szKeyNameBuf );
		if( iKey != pCurList->end() )
			return iKey->second.c_str();
	}

	return NULL;
}
